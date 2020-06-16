#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "fsl_tpm.h"
#include "keypad.h"
#include "string.h"

void DelayTPM();
#define DELAY 100000

int main(void) {

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    BOARD_InitDebugConsole();
    tpm_config_t config;
    TPM_GetDefaultConfig(&config);
    config.prescale= kTPM_Prescale_Divide_128;
    TPM_Init(TPM0, &config);
    TPM_Init(TPM1, &config);
    keypad k;
    keypad_config config1;
    get_default_keypad_config(&config1);
    set_keypad_config(&k, &config1);

    char key=0, anterior;
    char clave[12]={'A','B','C','C','B','A','1','9','8','0','7',0};
    char Dato[12]={'0','0','0','0','0','0','0','0','0','0','0',0};
    char reset[12]={'0','0','0','0','0','0','0','0','0','0','0',0};
    int j=0;

    uint8_t estado=0;
    uint8_t texto=0;
        while(1){
           	anterior=key;
           	key=read_keypad(&k);
           	switch (estado){
           	case 0: /*Ingresar la clave*/
           		if(texto==0){
           		 printf("Ingresar clave, al terminar presionar # \n");
           		   texto=1;
           		   }
        	   		if(key==0 && anterior!=0){
        			key=anterior;
        			if(key!='#'){
        				Dato[j]=key;
               		    j++;
        		    	DelayTPM();
        			}
        			else if (key=='#'){
        				printf("\n");
        				j=0;
        				texto=0;
        				estado=1;
        				}
        		key=0;
        			}
           	break;
        	case 1: /*verifica la clave*/
        		if(strcmp(clave,Dato)==0){ /*Compara lo que tiene clave con Dato (0 cuando coincide)*/
            		strcpy(Dato,reset); /*Regresa al varlor original a Dato*/
            		key=0;
            		estado=2;
        		}
        		else{
           		    key=0;
            		strcpy(Dato,reset);
        			estado=3;
        		}
        	break;
        	case 2:
        		if(texto==0){
        		   printf("Clave correcta\n");
        		   texto=1;
        	   }

       	   break;

           	case 3:
           		if(texto==0){
           			printf("Clave incorrecta, para volver a intentarlo ingresar #\n");
           		     texto=1;
           	    }

           	  if(key==0 && anterior!=0){
           	       	    	key=anterior;
           	       	    		if(key=='#'){
           	       	    		j=0;
           	       	    		texto=0;
           	       	    		estado=0;
           	       	    		key=0;
           	       	    		}
           	       	    }
           	break;
           	}
           	}
        }
