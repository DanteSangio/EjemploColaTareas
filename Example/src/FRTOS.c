/*
 Name        : FRTOS.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/
#include "chip.h"
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include <cr_section_macros.h>

#include "TFT_ILI9163C.h"


// SSP - Variables globales
static SSP_ConfigFormat ssp_format;

static void vTestTFTTask( void *pvParameters );


#define CLM_START1 0
#define CLM_START2 20
#define CLM_START3 40
#define CLM_START4 60
#define CLM_START5 80
#define PAGE_START 0
#define SQUARE_SIZE 50


static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	/*
	Board_LED_Set(LED0_GPIO_PORT_NUM,LED0_GPIO_BIT_NUM, true);
	Board_LED_Set(LED1_GPIO_PORT_NUM,LED1_GPIO_BIT_NUM, true);
	Board_LED_Set(LED2_GPIO_PORT_NUM,LED2_GPIO_BIT_NUM, true);
	Board_LED_Set(LED3_GPIO_PORT_NUM,LED3_GPIO_BIT_NUM, true);
	Board_LED_Set(LED4_GPIO_PORT_NUM,LED4_GPIO_BIT_NUM, true);
	Board_LED_Set(LED5_GPIO_PORT_NUM,LED5_GPIO_BIT_NUM, true);
	Board_LED_Set(LED6_GPIO_PORT_NUM,LED6_GPIO_BIT_NUM, true);
	Board_LED_Set(LED7_GPIO_PORT_NUM,LED7_GPIO_BIT_NUM, true);
*/

	Chip_GPIO_SetPinDIROutput(LPC_GPIO, A0_PORT_NUM, A0_BIT_NUM);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, CS_PORT_NUM, CS_BIT_NUM);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, RST_PORT_NUM, RST_BIT_NUM);


	Chip_GPIO_WritePortBit(LPC_GPIO,A0_PORT_NUM,A0_BIT_NUM,false);
	Chip_GPIO_WritePortBit(LPC_GPIO,CS_PORT_NUM,CS_BIT_NUM,false);
	Chip_GPIO_WritePortBit(LPC_GPIO,RST_PORT_NUM,RST_BIT_NUM,true);

	// SSP initialization
	Board_SSP_Init(LPC_SSP1);
	Chip_SSP_Init(LPC_SSP1);
	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_BITS_8;
	ssp_format.clockMode = SSP_CLOCK_CPHA0_CPOL0;
	Chip_SSP_SetFormat(LPC_SSP1, ssp_format.bits, ssp_format.frameFormat, ssp_format.clockMode);
	Chip_SSP_Enable(LPC_SSP1);

	NVIC_EnableIRQ(SSP1_IRQn);
	Chip_SSP_SetMaster(LPC_SSP1, 1);
}
/*-----------------------------------------------------------*/

int main( void )
{
	prvSetupHardware();

	/* Init the semi-hosting. */
	printf( "\n" );

	xTaskCreate(vTestTFTTask,"Test TFT",1000,NULL,1,NULL);

	/* Start the scheduler so our tasks start executing. */
	vTaskStartScheduler();

	for( ;; );
	return 0;
}
/*-----------------------------------------------------------*/

static void vTestTFTTask( void *pvParameters )
{
//	uint8_t comando,data;
	int i;
	for( ;; ){
		// Reseteo por HW
		Chip_GPIO_WritePortBit(LPC_GPIO,RST_PORT_NUM,RST_BIT_NUM,true);
		vTaskDelay(1/portTICK_RATE_MS);
		Chip_GPIO_WritePortBit(LPC_GPIO,RST_PORT_NUM,RST_BIT_NUM,false);
		vTaskDelay(1/portTICK_RATE_MS);
		Chip_GPIO_WritePortBit(LPC_GPIO,RST_PORT_NUM,RST_BIT_NUM,true);
		vTaskDelay(1/portTICK_RATE_MS);

		TFT_EscribirComando(CMD_SWRESET);
		vTaskDelay(5/portTICK_RATE_MS);

		TFT_EscribirComando(CMD_SLPOUT);
		vTaskDelay(5/portTICK_RATE_MS);

		TFT_EscribirComando(CMD_PIXFMT);
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(0x05);
		vTaskDelay(5/portTICK_RATE_MS);

		TFT_EscribirComando(CMD_GAMMASET);
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(0x01);
		vTaskDelay(5/portTICK_RATE_MS);

		TFT_EscribirComando(CMD_GAMRSEL);
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(0x01);

		TFT_EscribirComando(CMD_NORML);
		vTaskDelay(5/portTICK_RATE_MS);

		TFT_EscribirComando(CMD_DFUNCTR);
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(0b11111111);
		TFT_EscribirDato(0b00000110);
		vTaskDelay(5/portTICK_RATE_MS);

		// Terminar despues
		// Positive Gamma Correction Setting
		//Negative Gamma Correction Setting

		TFT_EscribirComando(CMD_FRMCTR1);
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(0x08);//0x0C//0x08
		TFT_EscribirDato(0x02);//0x14//0x08
		vTaskDelay(5/portTICK_RATE_MS);

		TFT_EscribirComando(CMD_DINVCTR);//display inversion
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(0x07);
		vTaskDelay(5/portTICK_RATE_MS);

		TFT_EscribirComando(CMD_PWCTR1);//Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(0x0A);//4.30 - 0x0A
		TFT_EscribirDato(0x02);//0x05
		vTaskDelay(5/portTICK_RATE_MS);

		TFT_EscribirComando(CMD_PWCTR2);//Set BT[2:0] for AVDD & VCL & VGH & VGL
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(0x02);
		vTaskDelay(5/portTICK_RATE_MS);

		TFT_EscribirComando(CMD_VCOMOFFS);
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(0);//0x40
		vTaskDelay(5/portTICK_RATE_MS);


		TFT_EscribirComando(CMD_CLMADRS);//Set Column Address
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(0>>8);
		TFT_EscribirDato(0);
		TFT_EscribirDato(WIDTH>>8);
		TFT_EscribirDato(WIDTH);
		vTaskDelay(5/portTICK_RATE_MS);

		TFT_EscribirComando(CMD_PGEADRS);//Set Page Address
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(0>>8);
		TFT_EscribirDato(0);
		TFT_EscribirDato(HEIGHT>>8);	//Probar 160
		TFT_EscribirDato(HEIGHT);	//Probar 160
		vTaskDelay(5/portTICK_RATE_MS);
/*		// set scroll area (offset 160 132)
		TFT_EscribirComando(CMD_VSCLLDEF);
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(__OFFSET);
		TFT_EscribirDato
		TFT_EscribirDato(_GRAMHEIGH - __OFFSET);
		TFT_EscribirDato
		TFT_EscribirDato(0);
		TFT_EscribirDato*/

//		colorSpace(_colorspaceData);
//		setRotation(0);

		TFT_EscribirComando(CMD_DISPON);
		vTaskDelay(5/portTICK_RATE_MS);


		TFT_EscribirComando(CMD_RAMWR);//Memory Write
		vTaskDelay(5/portTICK_RATE_MS);



		// Cuadrado rojo
		TFT_EscribirComando(CMD_CLMADRS);
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(CLM_START1>>8);
		TFT_EscribirDato(CLM_START1);
		TFT_EscribirDato((CLM_START1+SQUARE_SIZE)>>8);
		TFT_EscribirDato((CLM_START1+SQUARE_SIZE));
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirComando(CMD_PGEADRS);
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(PAGE_START>>8);
		TFT_EscribirDato(PAGE_START);
		TFT_EscribirDato((PAGE_START+SQUARE_SIZE)>>8);
		TFT_EscribirDato((PAGE_START+SQUARE_SIZE));
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirComando(CMD_RAMWR);
		for(i=0; i<SQUARE_SIZE*SQUARE_SIZE ; i++){
			TFT_EscribirDato(0b00000000);
			TFT_EscribirDato(0b00011111);
		}
		// Cuadrado verde
		TFT_EscribirComando(CMD_CLMADRS);
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(CLM_START2>>8);
		TFT_EscribirDato(CLM_START2);
		TFT_EscribirDato((CLM_START2+SQUARE_SIZE)>>8);
		TFT_EscribirDato((CLM_START2+SQUARE_SIZE));
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirComando(CMD_PGEADRS);
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(PAGE_START>>8);
		TFT_EscribirDato(PAGE_START);
		TFT_EscribirDato((PAGE_START+SQUARE_SIZE)>>8);
		TFT_EscribirDato((PAGE_START+SQUARE_SIZE));
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirComando(CMD_RAMWR);
		for(i=0; i<SQUARE_SIZE*SQUARE_SIZE ; i++){
			TFT_EscribirDato(0b00000111);
			TFT_EscribirDato(0b11100000);
		}
		// Cuadrado azul
		TFT_EscribirComando(CMD_CLMADRS);
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(CLM_START3>>8);
		TFT_EscribirDato(CLM_START3);
		TFT_EscribirDato((CLM_START3+SQUARE_SIZE)>>8);
		TFT_EscribirDato((CLM_START3+SQUARE_SIZE));
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirComando(CMD_PGEADRS);
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(PAGE_START>>8);
		TFT_EscribirDato(PAGE_START);
		TFT_EscribirDato((PAGE_START+SQUARE_SIZE)>>8);
		TFT_EscribirDato((PAGE_START+SQUARE_SIZE));
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirComando(CMD_RAMWR);
		for(i=0; i<SQUARE_SIZE*SQUARE_SIZE ; i++){
			TFT_EscribirDato(0b11111000);
			TFT_EscribirDato(0b00000000);
		}
		// Cuadrado blanco
		TFT_EscribirComando(CMD_CLMADRS);
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(CLM_START4>>8);
		TFT_EscribirDato(CLM_START4);
		TFT_EscribirDato((CLM_START4+SQUARE_SIZE)>>8);
		TFT_EscribirDato((CLM_START4+SQUARE_SIZE));
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirComando(CMD_PGEADRS);
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(PAGE_START>>8);
		TFT_EscribirDato(PAGE_START);
		TFT_EscribirDato((PAGE_START+SQUARE_SIZE)>>8);
		TFT_EscribirDato((PAGE_START+SQUARE_SIZE));
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirComando(CMD_RAMWR);
		for(i=0; i<SQUARE_SIZE*SQUARE_SIZE ; i++){
			TFT_EscribirDato(0b11111111);
			TFT_EscribirDato(0b11111111);
		}
		// Cuadrado negro
		TFT_EscribirComando(CMD_CLMADRS);
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(CLM_START5>>8);
		TFT_EscribirDato(CLM_START5);
		TFT_EscribirDato((CLM_START5+SQUARE_SIZE)>>8);
		TFT_EscribirDato((CLM_START5+SQUARE_SIZE));
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirComando(CMD_PGEADRS);
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirDato(PAGE_START>>8);
		TFT_EscribirDato(PAGE_START);
		TFT_EscribirDato((PAGE_START+SQUARE_SIZE)>>8);
		TFT_EscribirDato((PAGE_START+SQUARE_SIZE));
		vTaskDelay(5/portTICK_RATE_MS);
		TFT_EscribirComando(CMD_RAMWR);
		for(i=0; i<SQUARE_SIZE*SQUARE_SIZE ; i++){
			TFT_EscribirDato(0b00000000);
			TFT_EscribirDato(0b00000000);
		}

//		 TFT_EscribirComando(CMD_DINVON);
		vTaskDelay(10000/portTICK_RATE_MS);
	}

}


/*-----------------------------------------------------------*/
/*void SSP1_IRQHandler(void) {
	int i ;

	i=0;

}*/
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

void TFT_EscribirComando(uint16_t comando)
{
	Chip_GPIO_WritePortBit(LPC_GPIO,A0_PORT_NUM,A0_BIT_NUM,false);
	Chip_SSP_WriteFrames_Blocking(LPC_SSP1,&comando,1);
}
void TFT_EscribirDato(uint16_t dato)
{
	Chip_GPIO_WritePortBit(LPC_GPIO,A0_PORT_NUM,A0_BIT_NUM,true);
	Chip_SSP_WriteFrames_Blocking(LPC_SSP1,&dato,1);
}

