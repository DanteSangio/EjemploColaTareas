/*
 Name        : FRTOS.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/
#include "chip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include <cr_section_macros.h>


SemaphoreHandle_t Semaforo_1;
SemaphoreHandle_t Semaforo_2;

QueueHandle_t Cola_1;

#define PORT(x) 	((uint8_t) x)
#define PIN(x)		((uint8_t) x)
#define OUTPUT		((uint8_t) 1)
#define INPUT		((uint8_t) 0)

//Placa Infotronic
#define LED_STICK	PORT(0),PIN(22)
#define	BUZZER		PORT(0),PIN(28)
#define	SW1			PORT(2),PIN(10)
#define SW2			PORT(0),PIN(18)
#define	SW3			PORT(0),PIN(11)
#define SW4			PORT(2),PIN(13)
#define SW5			PORT(1),PIN(26)
#define	LED1		PORT(2),PIN(0)
#define	LED2		PORT(0),PIN(23)
#define	LED3		PORT(0),PIN(21)
#define	LED4		PORT(0),PIN(27)
#define	RGBB		PORT(2),PIN(1)
#define	RGBG		PORT(2),PIN(2)
#define	RGBR		PORT(2),PIN(3)

#define DEBUGOUT(...) printf(__VA_ARGS__)


void uC_StartUp (void)
{
	Chip_GPIO_Init (LPC_GPIO);
	Chip_GPIO_SetDir (LPC_GPIO, LED_STICK, OUTPUT);
	Chip_IOCON_PinMux (LPC_IOCON, LED_STICK, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_GPIO_SetDir (LPC_GPIO, BUZZER, OUTPUT);
	Chip_IOCON_PinMux (LPC_IOCON, BUZZER, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_GPIO_SetDir (LPC_GPIO, RGBB, OUTPUT);
	Chip_IOCON_PinMux (LPC_IOCON, RGBB, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_GPIO_SetDir (LPC_GPIO, RGBG, OUTPUT);
	Chip_IOCON_PinMux (LPC_IOCON, RGBG, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_GPIO_SetDir (LPC_GPIO, RGBR, OUTPUT);
	Chip_IOCON_PinMux (LPC_IOCON, RGBR, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_GPIO_SetDir (LPC_GPIO, LED1, OUTPUT);
	Chip_IOCON_PinMux (LPC_IOCON, LED1, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_GPIO_SetDir (LPC_GPIO, LED2, OUTPUT);
	Chip_IOCON_PinMux (LPC_IOCON, LED2, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_GPIO_SetDir (LPC_GPIO, LED3, OUTPUT);
	Chip_IOCON_PinMux (LPC_IOCON, LED3, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_GPIO_SetDir (LPC_GPIO, LED4, OUTPUT);
	Chip_IOCON_PinMux (LPC_IOCON, LED4, IOCON_MODE_INACT, IOCON_FUNC0);

	//Salidas apagadas
	Chip_GPIO_SetPinOutLow(LPC_GPIO, LED_STICK);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, BUZZER);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, RGBR);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, RGBG);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, RGBB);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, LED1);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, LED2);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, LED3);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, LED4);
}


/*TAREA QUE GUARDA EL DATO*/
static void vTask1(void *pvParameters)
{
	while (1)
	{
		unsigned int Parametro=5;

		DEBUGOUT("Envio 5 datos\n");	//Imprimo en la consola

		vTaskDelay(2000/portTICK_RATE_MS);

		xQueueSendToBack(Cola_1,&Parametro,portMAX_DELAY);

		vTaskDelete(NULL);	//Borra la tarea, no necesitaria el while(1)
	}
}

/*TAREA QUE RECIBE EL DATO*/
static void xTask2(void *pvParameters)
{
	unsigned int Receive;

	while (1)
	{
		xQueueReceive(Cola_1,&Receive,portMAX_DELAY);

		Receive*=2;

		DEBUGOUT("Recibo 5 datos\n"
				"Multiplico x2 para poder encender y apagar en cada dato\n");	//Imprimo en la consola

		while(Receive)
		{
			DEBUGOUT("Recibo: %d\n",Receive);	//Imprimo en la consola
			Chip_GPIO_SetPinToggle(LPC_GPIO,BUZZER);
			Chip_GPIO_SetPinToggle(LPC_GPIO,LED_STICK);
			Receive--;
			vTaskDelay(500/portTICK_RATE_MS);
		}
	}
}

/*
 * PROGRAMA QUE TRABAJA CON DOS TAREAS Y UNA COLA
 * Una tarea envia 5 datos a una cola y la otra tarea recive esos 5 datos encendiendo el led
 * del stick cada vez que recibe un dato.
 *
*/
int main(void)
{
	uC_StartUp (); // Config
	SystemCoreClockUpdate();

	DEBUGOUT("Inicializando..\n");

	vSemaphoreCreateBinary(Semaforo_1);
	vSemaphoreCreateBinary(Semaforo_2);

	Cola_1 = xQueueCreate(1, sizeof(uint32_t));	//Creamos una cola

	xSemaphoreTake(Semaforo_1 , portMAX_DELAY );

	xTaskCreate(vTask1, (char *) "vTaskLed1",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	xTaskCreate(xTask2, (char *) "vTaskLed2",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Nunca debería arribar aquí */

    return 0;
}

