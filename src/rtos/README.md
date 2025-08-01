# FreeRTOS

This FreeRTOS library provides a facade tailored to the requirements of
the elastic-AI runtime for the Elastic Note version 5.
This README provides a basic overview of the provided functionality and
their intended usage.  
The chapter [Components](#components) aims to present the provided parts of the library.
Whereas the chapter [Implementation Constraints](#implementation-constraints)
provides basic principles for working with FreeRTOS and the required steps
to properly implement a scenario alongside common errors.

We also use FreeRTOS to support the usage of both cores of the RP2040.
Executing all Tasks on Core 0 disables this option.

## Components

The provided faced makes three core concepts of the
FreeRTOS kernel available for usage:

|                   |                                               |
| ----------------: | :-------------------------------------------- |
|   [Tasks](#tasks) | Core feature for workload orchestration       |
| [Queues](#queues) | Feature to enable communication between tasks |
|   [Mutex](#mutex) | Feature to lock resources                     |

### Tasks

Tasks are the core feature of FreeRTOS where all other functions evolve around.
Tasks in this context offer a way to schedule workloads to FreeRTOS.

The scheduler considers each Task's Stack and priority.

### Queues

Queues are First-In-First-Out (FIFO) buffers
that offer a way to exchange data between tasks,
or send data to a task from an Interrupt Service Routine (ISR).

The system will copy the content provided as input to the queue itself.

If you pass a pointer to the queue,
it is possible to change the content pushed to the queue, even after you pushed it.

> [!IMPORTANT]
>
> The element size must be known on queue initialization.

Initialization defines the number of items a Queue can hold.

### Mutex

A mutex offers a simple way to ensure mutual exclusion of shared resources if necessary.
It is necessary to assure that only one task is able to access the SPI bus and
other tasks have to wait before it is their turn.

## Implementation constraints

### The Interrupt-Service-Routine (ISR) should be as short as possible

Only push the received content into a queue handled by a Task.

### Passing data to or between tasks

To allow the schedular to properly run the tasks, it is necessary to use queues
as the mechanism to exchange data.

### Ensure only one task access a physical resource

Only **one** Task should be able to access a physical resources such as
the I2C, SPI or UART bus at the same time.

This can be assured in two ways:

**Single Task:**  
Combine all access to a physical resource in the same Task

**Mutex:**  
Guard access to physical resources with mutex based lock and unlock operations.

```C title="Pub-Sub-Handling"
#include <FreeRtosTaskWrapper.h>  // include functions for FreeRTOS Tasks
#include <FreeRtosQueueWrapper.h> // include functions for FreeRTOS Queues

queue_t receivedPostings; // reference pointer for the queue with new posts

// Callback to handle newly received posts (equals ISR)
void deliverCallback(Posting post) {
    freeRtosQueueWrapperPushFromInterrupt(receivedPosting, &post); // push to queue
}

_Noreturn void receiveTask(void) {
    // subscribe topic and set callback
    protocolSubscribeForData("enV5", dataTopic, (subscriber_t){.deliver = deliverCallback});

    while (1) {
        posting_t post;
        if (freeRtosQueueWrapperPop(postings, &post)) { // check if new post received
            PRINT("Received Message: '%s' via '%s'", post.data, post.topic); // handle new post
        }
        freeRtosTaskWrapperTaskSleep(1000); // sleep to prevent blockage
    }

}

_Noreturn void sendTask(void) {
    while (1) {
        protocolPublishData(dataTopic, "TEST TEST"); // publish post
        freeRtosTaskWrapperTaskSleep(1000);
    }
}

int main(void)  {
    //...

    receivedPostings = freeRtosQueueWrapperCreate(5, sizeof(Posting)); // create queue for 5 posts

    freeRtosTaskWrapperRegisterTask(receiveTask, "receive", 0, FREERTOS_CORE_0); // register receiver task
    freeRtosTaskWrapperRegisterTask(sendTask, "send", 0, FREERTOS_CORE_1); // register sender task

    freeRtosTaskWrapperStartScheduler(); // start schedular (main loop)
}
```

## Further reading

- [FreeRTOS Developer Docs](https://www.freertos.org/features.html)
- [RP2040 Symmetric Multiprocessing
  Demo](https://www.freertos.org/smp-demos-for-the-raspberry-pi-pico-board.html)
