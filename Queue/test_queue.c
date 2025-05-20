#include "test_queue.h"

#define Pass  0xAAAA
#define QueueError  0x5555
#define MutexError  0x3333



TestQueue_t CreateQueue(unsigned int itemsTotal , unsigned int ItemSize)
{
    TestQueue_t *queue = (TestQueue_t *)malloc(sizeof(TestQueue_t));
    queue -> itemSize = ItemSize;
    queue -> queueLength = itemsTotal;
    queue -> queueStorage = (uint8_t *)malloc(itemsTotal * ItemSize);
    queue -> queueReadIndex= 0x00;
    queue -> queueWriteIndex = 0x00;
    queue -> queueWaitData= 0x00;
    queue -> queueMutex =xSemaphoreCreateMutex();

    if (queue->queueMutex  == NULL)
    {
      free(queue->queueStorage );			
      free(queue);
    }

    return *queue;
}

uint32_t QueueWrite(TestQueue_t *queue,void *dataItem,TickType_t TicksToWait)
{

    if (xSemaphoreTake(queue->queueMutex, TicksToWait) == pdTRUE)
    {
        if (queue->queueWaitData < queue->queueLength)
        {
					  memcpy(&(queue->queueStorage[(queue-> queueWriteIndex)* (queue-> itemSize)]), dataItem, queue-> itemSize);

            queue -> queueWriteIndex= (queue -> queueWriteIndex+ 1) % queue -> queueLength;

            queue -> queueWaitData++;

            xSemaphoreGive(queue->queueMutex);

            return Pass;
        }

        xSemaphoreGive(queue->queueMutex);

        return QueueError;

    }

    return MutexError;
}

uint32_t QueueRead(TestQueue_t *queue,void *dataItem,TickType_t TicksToWait)
{

    if (xSemaphoreTake(queue->queueMutex, TicksToWait) == pdTRUE)
    {
        if (queue->queueWaitData > 0)
        {
            memcpy(dataItem, &(queue->queueStorage[(queue-> queueWriteIndex)* (queue-> itemSize)]), queue-> itemSize);

            queue -> queueReadIndex = (queue -> queueReadIndex + 1) % queue-> queueLength;

            queue -> queueWaitData--;

            xSemaphoreGive(queue->queueMutex);

            return Pass;
        }

        xSemaphoreGive(queue->queueMutex);

        return QueueError;

    }

    return MutexError;
}