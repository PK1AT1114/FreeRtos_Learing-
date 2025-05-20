#include "pic32cm5164jh01100.h"
#include "FreeRTOS.h"
#include "task.h"
#include <semphr.h>
#include "test_adc.h"
#include "Clock_init.h"
#include "test_uart.h"
#include "test_queue.h"
#include <stdio.h>
#include <string.h>

#define LED_Pin 5U
#define VREF_ADC             (3.3f)
#define ADC_MAX_COUNT        (4095U)

float sharedAdcVoltage = 0.0f;  // Shared memory for ADC voltage
char adcString[30] = {0};
SemaphoreHandle_t xMutex;       // Mutex to protect access to the shared variable
extern uint32_t SystemCoreClock;
volatile uint16_t adcResult = 0; // ADC result
TaskHandle_t xAdcTaskHandle = NULL;
TaskHandle_t xUartTaskHandle = NULL;

void vLEDTask(void *pvParameters)
{
    
    while(1)
    {
			  
        PORT_REGS->GROUP[2].PORT_OUTCLR |= (1 << LED_Pin);
        vTaskDelay(pdMS_TO_TICKS(500));

        PORT_REGS->GROUP[2].PORT_OUTSET |= (1 << LED_Pin);
        vTaskDelay(pdMS_TO_TICKS(500)); 
    }
}

void vADCTask(void *pvParameters)
{
	  xTaskNotifyGive(xAdcTaskHandle);
    while(1)
    {
			  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        adcResult = APP_ADC_ADCMeasurment(); 
        float adcVoltage = (float)adcResult * VREF_ADC / ADC_MAX_COUNT;
        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            sharedAdcVoltage = adcVoltage;
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(500)); 
				xTaskNotifyGive(xUartTaskHandle);
    }
}

void vUARTTask(void *pvParameters)
{
    while(1)
    {
			  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            float localAdcVoltage = sharedAdcVoltage;
            xSemaphoreGive(xMutex);
            sprintf(adcString, "ADC Voltage: %.3f V\r\n", localAdcVoltage);
            UART_TRANSFER(adcString, strlen(adcString));
        }
        vTaskDelay(pdMS_TO_TICKS(50)); 				
			  xTaskNotifyGive(xAdcTaskHandle);
    }
		
}

void System_hardware_init(void)
{
	 CLOCK_Initialize();
	
   PORT_REGS->GROUP[2].PORT_DIR |= (1 << LED_Pin); // Configure LED pin as output
	 
	 PORT_REGS->GROUP[1].PORT_PINCFG[10] = 0x1U;
   PORT_REGS->GROUP[1].PORT_PINCFG[11] = 0x1U;
   PORT_REGS->GROUP[1].PORT_PMUX[5] = 0x33U;
}

int main(void)
{
	  System_hardware_init();
	   
    UART_Initialize(115200); 
    ADC0_Initialize();       

    ADC0_REGS->ADC_INPUTCTRL = (uint16_t)ADC_INPUTCTRL_MUXPOS_AIN11; 
    while((ADC0_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_INPUTCTRL_Msk));   

    ADC0_REGS->ADC_SWTRIG |= ADC_SWTRIG_START_Msk | ADC_SWTRIG_FLUSH_Msk; 
    while(ADC0_REGS->ADC_SYNCBUSY);  
	
    ADC0_ConversionResultGet();  
	
    xMutex = xSemaphoreCreateMutex();

    if (xMutex != NULL)
    {
        xTaskCreate(vLEDTask, "LEDTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        xTaskCreate(vADCTask, "ADCTask", configMINIMAL_STACK_SIZE, NULL, 2, &xAdcTaskHandle);
        xTaskCreate(vUARTTask, "UARTTask", configMINIMAL_STACK_SIZE, NULL, 1, &xUartTaskHandle);

        vTaskStartScheduler();
    }

    while (1);
}
