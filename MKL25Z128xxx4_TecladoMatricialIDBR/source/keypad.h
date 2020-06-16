/*
 * keypad.h
 *
 *  Created on: May 13, 2020
 *      Author: Remberto Sandoval Arechiga
 *      		Universidad Autónoma de Zacatecas
 *      		Todos los derechos reservados
 *      		Se prohibe cualquier uso comercial o no comercial sin autorización del autor
 *
 *      		driver for a 4x4 matrix keypad
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_
#include <stdint.h>
/*
 * Definition of ports for driver
 */
typedef enum  port_enum{
		rem_PORTA = 0U,
		rem_PORTB,
		rem_PORTC,
		rem_PORTD,
		rem_PORTE,
} Port;
/*
 * Definition of the state names
 */
typedef enum state_enum{
		inactive =0u,
		read_col_1,
		read_col_2,
		read_col_3,
		read_col_4,
} state_x;

/*
 * struct definition for input pins
 */
typedef struct input_pin_struct{
	uint8_t port;
	uint8_t Bit;
	uint8_t Direction;
	uint8_t Pull_Enable;
	uint8_t Pull;
}input_pin;

/*
 * struct definition for output pins
 */
typedef struct output_pin_struct{
	uint8_t port;
	uint8_t Bit;
	uint8_t Direction;
	uint8_t initial;
}output_pin;


/*
 * struct definition for FSM state
 */
typedef struct fsm_state{
	state_x name; 	// enum states
	uint8_t cols;	// cols states
} keypad_state;

/*
 * keypad abstraction
 */
typedef struct keypad_struct{
	input_pin row_pin[4];
	output_pin col_pin[4];
	uint8_t rows;
	uint8_t cols;
	keypad_state FSM[5];
//	= {
//			(keypad_state){.name = uninitialized, .cols = 0},
//			(keypad_state){.name = inactive, .cols = 0},
//			(keypad_state){.name = read_col_1, .cols = 0b0111},
//			(keypad_state){.name = read_col_2, .cols = 0b1011},
//			(keypad_state){.name = read_col_3, .cols = 0b1101},
//			(keypad_state){.name = read_col_4, .cols = 0b1110}
//	};
	state_x actual_state;
	char data_matrix[4][4];
//	{
//			{'1','2','3','A'},
//			{'4','5','6','B'},
//			{'7','8','9','C'},
//			{'*','0','#','D'}
//
//	};
} keypad;

typedef struct keypad_config_struct{
	input_pin row_pin[4];
	output_pin col_pin[4];
}keypad_config;

/*
 *  function for config input pins
 */
void config_input_pin(keypad *k,uint8_t pin_n, input_pin pin);
/*
 *  function for config output pins
 */
void config_output_pin(keypad *k,uint8_t pin_n, output_pin pin);
/*
 *  function for set keypad config
 */
void set_keypad_config(keypad * k, keypad_config * config);
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
void get_default_keypad_config(keypad_config * config);
void set_cols(keypad *k);
uint8_t get_rows(keypad *k);

/*!
 * @brief read input char from keypad
 *
 */
char read_keypad(keypad *k);

#endif /* KEYPAD_H_ */
