# FreeRtos_Learing-
Gain a foundational understanding of FreeRTOS concepts and apply them hands-on by testing on the PIC32CM JH01 Curiosity Pro board.JH01 Curiosity pro board
1. RTOS_ADC_PIC32CM5164JH01100 project:
   a.Download the the above project zip file by using the git link (https://github.com/PK1AT1114/FreeRtos_Learing-)
   b.open the project in the Keil IDE.(download and install the required DFP's)
   c.add the below lines in the RTE_Components.h file 
     #include "pic32cm5164jh01100.h"
     extern uint32_t SystemCoreClock;
   d. build the project verify there is no error's found. if the build is taking tomch time due to some warnings
   then click on the option od target option, select the c/c++(AC6), select the warings as No Warnings, click on
   Ok at the bottom and build again.

2. Learing Que.pdf.
   This small pdf to understand how the queue is queue created and how to read & write the data inside the queue. 
   


