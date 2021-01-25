# freertos-teensy-example
The aim of this guide is to introduce the reader into the Teensy board program-
ming using FreeRTOS, a real time operating system, widely used in the embedded
world. Teensy is a board that has a long life, it was produced in various releases
from the oldest 2.0 to the last one 4.1.
In this repo, the version 4.1 will be used. This version mounts a 600 MHz
Cortex-M7 MCU. Teensy is fully programmable with arduino IDE and it shares
the same libraries. This allows a programmer to have both high performance
(with 600 Mhz) and platform abstration, avoiding to study the internals of the MCU.

Following the list of examples (up to now only one) with a little exaplaination.

## example_01.cpp

In this example we use the some basic function of FreeRTOS:
    `xTaskCreate`
to create a task, it allow us to assign a priority to the tasks, 
the priority starts from 0 (i.e. the idle task, the task with lowest
level of priority) and then increasing the number corresponds to
an higher priority. In the example there are 4 task:
1. Thread_An, read from an analog input a value
2. Thread_Led, blink a led with period proportional to the analog valuea read 
   by Thread_An
3. Thread_UART, send an "Alive" message over the system UART every 1 second
4. Thread_Blink, blink a led with 1 second period

The most of the time Thread_An and Thread_Led hold the execution time, 
Thread_An read the data and then lock itself by mean of a semaphore with
`xSemaphoreTake` function, at this point Thread_Led take the execution and the
unlock the Thread_An releasing the semaphore with `xSemaphoreGive` function.

The 2 tasks Thread_UART and Thread_Blink have higher priority (3).
Just for exercise Thread_Blink will stop to be executed after 20 cycles. 
To stop itself it uses the `vTaskDelete` function. In order to benefit from 
its usage and handler needs to be declared by mean of `TaskHandle_t` datatype.
               
### Golden Rules for Priority               
**Rule 1**<br>
If you have 2 tasks with a consequence priority, let's say T1 and T2, if the T2 is started
for first, T1 (the task with lower priority) will never be executed, in fact as soon it 
finishes it will be he task ampong the others with the highest priority so it will take 
the execution gaing. To be sure the execution will pass to T1 we need to lock 
T2. At this point T1 starts, at the end of its execution it will release the lock
as to T2 will be able to execute itself again.
      
So remember lower priority tasks will never be executed if you don't lock the higher 
explictly.
      
This is the relation between hthe tasks `Thread_An` and `Thread_Led` in the example proposed.

**Rule 2**<br>
If you have a task with a periodic execution and you want to be sure it will be executed,
assign to it the highest priority. It the example we want to blink a led every 1 second,
if you have two tasks T1 and T2 resplectively woth priority 2 and 1 with a lock between them
if you put another task to priority 2, it will be executed only in the case T1 holds the lock
of T2, otherwise a Round-robin algorithm will decide the run between T2 and the new task.
To be sure it will be executed at all costs, you have to assign a priority 3.

Note to Rules 2<br>
When start programming FreeRTOS you may be prone to assign a lower priority to tasks which need
to be "rarely" executed. Don't fall in this misconcept! Higher priority means "you are sure it 
will happens"!

     
