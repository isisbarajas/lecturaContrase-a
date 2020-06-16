/*
 * keypad.c
 *
 *  Created on: May 13, 2020
 *      Author: remberto
 */

#include "keypad.h"
#include <stdint.h>
#include "fsl_gpio.h"


void config_input_pin(keypad *k,uint8_t pin_n, input_pin pin)
{
	k->row_pin[pin_n]=pin;
}
void config_output_pin(keypad *k,uint8_t pin_n, output_pin pin)
{
	k->col_pin[pin_n]=pin;
}


/*
 *  function for set keypad config
 */
void set_keypad_config(keypad *k,keypad_config * config){
	GPIO_Type *GPIO_input, *GPIO_output;
	PORT_Type *PORT_input, *PORT_output;

	uint32_t offset;
	char data[4][4]={
			{'1','2','3','A'},
			{'4','5','6','B'},
			{'7','8','9','C'},
			{'*','0','#','D'}
	};
	keypad_state FSM_i[5]= {
				(keypad_state){.name = inactive, .cols = 0},
				(keypad_state){.name = read_col_1, .cols = 0b11110111},
				(keypad_state){.name = read_col_2, .cols = 0b11111011},
				(keypad_state){.name = read_col_3, .cols = 0b11111101},
				(keypad_state){.name = read_col_4, .cols = 0b11111110}
		};
	for(uint8_t i = 0;i<5;i++)
		k->FSM[i]=FSM_i[i];
	k->actual_state = inactive;
	// Set MCU
	for(uint8_t i =0;i<4;i++)
	{
		//Set Structure & FSM
		k->row_pin[i] = config->row_pin[i];
		k->col_pin[i] = config->col_pin[i];

		k->cols=0;
		k->rows=0;
		for(uint8_t j = 0;j<4;j++)
			k->data_matrix[i][j]=data[i][j];

		//Clock gating to GPIO PORT
		SIM->SCGC5 |= 1u<<(config->col_pin[i].port+9u);
		SIM->SCGC5 |= 1u<<(config->row_pin[i].port+9u);

		offset = (config->row_pin[i].port)*(0x40u);
		GPIO_input = (GPIO_Type *) (GPIOA_BASE + offset);

		GPIO_output = (GPIO_Type *) (GPIOA_BASE + (config->col_pin[i].port)*(0x40u));

		//Direction
		GPIO_input->PDDR &= ~(1u<<config->row_pin[i].Bit);	//clear bit
		GPIO_output->PDDR |= ~(1u<<config->col_pin[i].Bit); //set bit

		//Set intial output value
		if(config->col_pin[i].initial)
			GPIO_output->PDOR |= ~(1u<<config->col_pin[i].Bit); //set bit
		else
			GPIO_output->PDOR &=~(1u<<config->col_pin[i].Bit);	//clear bit

		//set multiplexing pin ALT1 = GPIO
		PORT_input = (PORT_Type *) (PORTA_BASE + config->row_pin[i].port*(0x1000u));
		PORT_output = (PORT_Type *) (PORTA_BASE + config->col_pin[i].port*(0x1000u));

		PORT_input->PCR[config->row_pin[i].Bit] = 0x00000100;
		PORT_output->PCR[config->col_pin[i].Bit] = 0x00000100;

		//Set input pull
		if(config->row_pin[i].Pull_Enable)
		{
			if(config->row_pin[i].Pull)
				PORT_input->PCR[config->row_pin[i].Bit] |= 0x3;
			else
				PORT_input->PCR[config->row_pin[i].Bit] |= 0x2;
		}


	}
}
/*!
 * @brief The functions gets a default keypad configuration
 *	col_pin
 *		output
 *		port B
 *		pin 8-11
 *		initial state 1
 *	row_pin
 *		input
 *		port E
 *		pin 2-5
 *		pull up enable
 * @param config empty config struct
 *
*/
void get_default_keypad_config(keypad_config * config){
	for(uint8_t i=0;i<4;i++)
	{
		config->col_pin[i].port = rem_PORTB;
		config->row_pin[i].port = rem_PORTE;

		config->col_pin[i].Bit=11u-i;
		config->row_pin[i].Bit=5u-i;

		config->col_pin[i].Direction = 1u;	// output
		config->row_pin[i].Direction = 0;	// input

		config->col_pin[i].initial=1; //initial state
		config->row_pin[i].Pull_Enable = 1; // Pull enable
		config->row_pin[i].Pull = 1;		// Pull-up
	}

}
void set_cols(keypad *k)
{
	GPIO_Type * GPIO_output;
	uint8_t state=k->actual_state;
	for(uint8_t i =0;i<4;i++)
	{
		GPIO_output = (GPIO_Type *)(GPIOA_BASE + k->col_pin[i].port*(0x40u));

		if(k->FSM[state].cols & 1u<<i)
			GPIO_output->PSOR |= 1u<<k->col_pin[i].Bit;
		else
			GPIO_output->PCOR |= 1u<<k->col_pin[i].Bit;
	}
}
uint8_t get_rows(keypad *k)
{
	GPIO_Type * GPIO_input;
	uint8_t row=0;
	for(uint8_t i =0;i<4;i++)
	{
		GPIO_input = (GPIO_Type *)(GPIOA_BASE + k->row_pin[i].port*(0x40u));

		if(GPIO_input->PDIR & (1u<<k->row_pin[i].Bit))
			row+=1u<<i;
	}
	switch(row)
	{
		default: return(0);
		case 0b1110: return(1);
		case 0b1101: return(2);
		case 0b1011: return(3);
		case 0b0111: return(4);
	}

}
void delay(uint16_t ticks)
{
	for(uint16_t i; i<ticks;i++)
		__asm("nop");
}
/*!
 * @brief read input char from keypad
 *
 */
char read_keypad(keypad *k){
	uint8_t row=0,col=0;
	k->actual_state=read_col_1;
	while(k->actual_state!=inactive)
	{
		set_cols(k); //set cols
		row=get_rows(k);
		if(row)
		{
			switch(k->actual_state)
			{
				case inactive: col=0; break;
				case read_col_1: col=1; break;
				case read_col_2: col=2; break;
				case read_col_3: col=3; break;
				case read_col_4: col=4; break;
			}
			k->actual_state=inactive;
		}
		else
		{
			if(k->actual_state==read_col_4)
				k->actual_state=inactive;
			else
				k->actual_state++;
		}
	}
	if(row>0 && row<5 && col>0 && col<5)
		return(k->data_matrix[row-1][4-col]);
	else
		return(0);
}
