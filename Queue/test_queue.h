#ifndef TEST_QUEUE_H      // Guards against multiple inclusion
#define TEST_QUEUE_H

#include <stdbool.h>
#include <stddef.h>
#include "pic32cm5164jh01100.h"
#include "FreeRTOS.h"
#include "task.h"
#include <semphr.h>
#include <stdio.h>
#include <string.h>


typedef uint32_t UART_STATUS;

typedef struct {
    uint8_t *queueStorage;
    unsigned int itemSize;
    unsigned int queueLength;
    unsigned int queueReadIndex;
    unsigned int queueWriteIndex;
    unsigned int queueWaitData;
    SemaphoreHandle_t queueMutex;
}TestQueue_t;

TestQueue_t CreateQueue(unsigned int itemsTotal , unsigned int ItemSize);

uint32_t QueueWrite(TestQueue_t *queue,void *dataItem,TickType_t TicksToWait);

uint32_t QueueRead(TestQueue_t *queue,void *dataItem,TickType_t TicksToWait);

#endif