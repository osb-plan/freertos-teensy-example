/*
  Copyright 	: Monografie Tecnologiche
  Author 	: osbplan
  Date		: 2021-01-24
*/

/* LICENSE */
/*  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

/* Description
 This example is targeted for Teensy 4.1
 In uses FreeRTOS
 4 tasks are created
   - Thread_An  -> [Priority 2] 
     	      reads from a knob an analog value 
   - Thread_Led -> [Priority 1] 
                   Blink a led with a time between [0 - 1]s, mapping
                   the analog voltage in the 0-1 interval
   - Thread_UART-> [Priority 3]
                   Simply sends an alive message every 1 second
   - ThreadBlink-> [Priority 3]
     	      Blink a led every second for 20 cycles then delete 
     	      itself 
*/

#include "Arduino.h"
#include <FreeRTOS_TEENSY4.h>

const int ledPin = 2;
const int anrdPin = 0;
const int ledPinBlink = 3;

TaskHandle_t handlerBlink = NULL;

// Declare a semaphore handle.
SemaphoreHandle_t sem_sensor;
int sensorValue;

/* read analog form trimmer */
static void Thread_An(void *arg)
{
	while (1) {
		xSemaphoreTake(sem_sensor, portMAX_DELAY);

		sensorValue = analogRead(anrdPin);
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}

/* turn on-off led with delay taken from ext analog sensor */
static void Thread_Led(void *arg)
{
	while (1) {
		xSemaphoreGive(sem_sensor);

		digitalWrite(ledPin, LOW);
		vTaskDelay(pdMS_TO_TICKS(sensorValue));
		digitalWrite(ledPin, HIGH);
		vTaskDelay(pdMS_TO_TICKS(sensorValue));
	}
}

static void Thread_UART(void *arg)
{
	while (1) {
		Serial.println("Alive");
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

static void ThreadBlink(void *arg)
{
	unsigned short cnt = 0;
	while (1) {
		digitalWrite(ledPinBlink, HIGH);
		vTaskDelay(pdMS_TO_TICKS(1000));
		digitalWrite(ledPinBlink, LOW);
		vTaskDelay(pdMS_TO_TICKS(1000));

		if (cnt == 20) {
			vTaskDelete(handlerBlink);
		}
		cnt++;
	}
}

void setup()
{
	pinMode(ledPin, OUTPUT);
	pinMode(ledPinBlink, OUTPUT);
	portBASE_TYPE s1, s2, s3, s4;

	Serial.begin(9600);

	// initialize semaphore
	sem_sensor = xSemaphoreCreateCounting(1, 0);

	s1 = xTaskCreate(Thread_An, NULL, configMINIMAL_STACK_SIZE, NULL,
			 tskIDLE_PRIORITY + 2, NULL);

	s2 = xTaskCreate(Thread_Led, NULL, configMINIMAL_STACK_SIZE, NULL,
			 tskIDLE_PRIORITY + 1, NULL);

	s3 = xTaskCreate(Thread_UART, NULL, configMINIMAL_STACK_SIZE, NULL,
			 tskIDLE_PRIORITY + 3, NULL);

	s4 = xTaskCreate(ThreadBlink, NULL, configMINIMAL_STACK_SIZE, NULL,
			 tskIDLE_PRIORITY + 3, &handlerBlink);

	// check for creation errors
	if (sem_sensor == NULL ||
	    s1 != pdPASS || s2 != pdPASS || s3 != pdPASS || s4 != pdPASS) {
		Serial.println("Creation problem");
		while (1) ;
	}

	Serial.println("Starting the scheduler !");

	// start scheduler
	vTaskStartScheduler();
	Serial.println("Insufficient RAM");
	while (1) ;
}

void loop()
{

}
