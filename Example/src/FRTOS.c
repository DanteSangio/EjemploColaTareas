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

SemaphoreHandle_t Semaforo_1;
SemaphoreHandle_t Semaforo_2;

QueueHandle_t Cola_1;

#include <cr_section_macros.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here
#define PORT(x) 	((uint8_t) x)
#define PIN(x)		((uint8_t) x)

#define OUTPUT		((uint8_t) 1)
#define INPUT		((uint8_t) 0)

#define DEBUGOUT(...) printf(__VA_ARGS__)


void uC_StartUp (void)
{
	Chip_GPIO_Init (LPC_GPIO);
	Chip_GPIO_SetDir (LPC_GPIO , PORT(0) , PIN(22) , OUTPUT);
	Chip_IOCON_PinMux (LPC_IOCON , PORT(0) , PIN(22), IOCON_MODE_INACT , IOCON_FUNC0);
}


/*TAREA QUE GUARDA EL DATO*/
static void vTask1(void *pvParameters)
{
	while (1)
	{
		unsigned int Parametro=5;

		DEBUGOUT("Envio 5 datos\n");	//Imprimo en la consola

		vTaskDelay(5000/portTICK_RATE_MS);

		xQueueSendToBack(Cola_1,&Parametro,portMAX_DELAY);

		//DEBUGOUT("Tick: %d\r\n", tickCnt);	//Imprimo en la consola

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

		DEBUGOUT("Recibo 5 datos\nMultiplico x2 para poder encender y apagar en cada dato\n");	//Imprimo en la consola

		while(Receive)
		{
			DEBUGOUT("Recibo: %d\n",Receive);	//Imprimo en la consola
			Chip_GPIO_SetPinToggle(LPC_GPIO,PORT(0),PIN(22));
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

