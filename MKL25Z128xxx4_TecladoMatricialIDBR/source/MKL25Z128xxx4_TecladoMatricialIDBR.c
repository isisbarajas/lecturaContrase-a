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

    uint8_t texto=0;
        while(1){
           	anterior=key;
           	key=read_keypad(&k);
           	switch (estado){
           	case 0: /*Ingresar la clave*/
           		printf("Ingresar contraseña, al terminar presionar # \n");
        	   		if(key==0 && anterior!=0){
        			key=anterior;
        			if(key!='#'){
        				Dato[j]=key;
               		    j++;
        		    	DelayTPM();
        			}
        			else if (key=='#'){
        				printf("\n");
        				texto=0;
        				j=0;
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
                 printf("Contraseña correcta\n");
       	   break;

           	case 3:
           		printf("Contraseña incorrecta, para volver a intentarlo ingresar #\n");
           	  if(key==0 && anterior!=0){
           	       	    	key=anterior;
           	       	    		if(key=='#'){
           	       	    		texto=0;
           	       	    		j=0;
           	       	    		estado=0;
           	       	    		key=0;
           	       	    		}
           	       	    }
           	break;
           	}
           	}
        }
