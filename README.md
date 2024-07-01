# Muxtor Project

https://github.com/orgs/muxtor-tech/

## Disclaimer

This project was in large parts generated using ChatGPT 4o model. While the core concept are authored by humans, implementation was largely done by the model to aid and accelerate the process.
More details of the chat usage can be found here: https://chatgpt.com/share/1469f088-6e99-4c57-a886-7cd718e11f79

## Atomic Queue

The `Atomic Queue` is a header-only library that supports both static and dynamic memory allocation. It is optimized for queues with sizes that are powers of 2 and ensures thread/process safety using atomic operations. This makes it suitable for real-time and low-latency applications.

### Features

- **Lock-free**: Utilizes atomic operations to ensure synchronization without locks.
- **Static and Dynamic Allocation**: Supports both static and dynamic memory allocation.
- **Configurable Alignment**: Allows alignment of queue items for optimal performance.
- **Power of 2 Optimization**: Optimized for queue sizes that are powers of 2.
- **Busy-wait Synchronization**: Suitable for real-time systems where busy-wait is acceptable.
- **Interrupt Safe**: Can be used within interrupt service routines (ISRs) due to its lock-free nature.


### Limitations

- Maximum queue length: 65535 items; limitation coming from design decisions.
- Queue lenght must be power of 2.
- Tested and designed for minimum C11 ISO/IEC 9899:2011 standard.
- Tested with GCC 13.2.0 on WSL environment.

### Example

The example code provided can be complied and run on typical Linux development setup using Make and GCC.

```bash
make
./atomq
```

### Usage

#### Include the Header

First, include the `mx_queue.h` header in your project:

```c
#include "mx_queue.h"
```

#### Define a Queue
Use the provided macro to define a queue for your specific data type:

```c
#define QUEUE_SIZE 128

DEFINE_FIFO_QUEUE(int, IntQueue, QUEUE_SIZE, 4)
```

#### Initialize the Queue
You can initialize the queue with either static or dynamic memory allocation.

#### Static Allocation:

```c
IntQueue queue;
IntQueue_StaticBuffer buffer;

IntQueue_init_static(&queue, &buffer);
```

#### Dynamic Allocation:

```c
IntQueue queue;

if (IntQueue_init_dynamic(&queue, QUEUE_SIZE) != 0) {
    // Handle allocation error
}
```

#### Enqueue and Dequeue Operations
Use the enqueue and dequeue functions to add and remove items from the queue.


#### Enqueue
```c
int item = 42;
if (IntQueue_enqueue(&queue, &item) != 0) {
    // Handle queue full error
}
```

#### Dequeue:

```c
int item;
if (IntQueue_dequeue(&queue, &item) != 0) {
    // Handle queue empty error
}
```

#### Example Usage
Here is an example of a producer and consumer using the Atomic Queue:

```c
#include "mx_queue.h"
#include <stdio.h>

#define QUEUE_SIZE 128
#define ITEMS_PER_PRODUCER 100
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2

DEFINE_FIFO_QUEUE(int, IntQueue, QUEUE_SIZE, 4)

IntQueue queue;
IntQueue_StaticBuffer buffer;

void producer(int id) {
    for (int i = 0; i < ITEMS_PER_PRODUCER; ++i) {
        int item = id * ITEMS_PER_PRODUCER + i;
        while (IntQueue_enqueue(&queue, &item) == -1) {
            ; // Busy-wait until there is space in the queue
        }
        printf("Producer %d enqueued %d\n", id, item);
    }
}

void consumer() {
    for (int i = 0; i < (NUM_PRODUCERS * ITEMS_PER_PRODUCER) / NUM_CONSUMERS; ++i) {
        int item;
        while (IntQueue_dequeue(&queue, &item) == -1) {
            ; // Busy-wait until there is an item in the queue
        }
        printf("Consumer dequeued %d\n", item);
    }
}

int main() {
    // Initialize the queue with a static buffer
    IntQueue_init_static(&queue, &buffer);

    // Simulate producers and consumers
    producer(0);
    producer(1);
    consumer();
    consumer();

    // Free the queue resources if dynamically allocated
    // IntQueue_free(&queue);

    return 0;
}
```

#### Integration with Multi-Process Environment
To use the Atomic Queue in a multi-process environment with shared memory:

- Create and map shared memory using shm_open and mmap.
- Initialize the queue in the shared memory region.
- Fork producer and consumer processes.
- Refer to the example in the full documentation for detailed steps.

### License
Muxtor Atomic Queue is released under the MIT License. See LICENSE for details.

This README.md file provides a comprehensive guide on how to use the `Atomic Queue` library in your project. It includes features, usage examples, and initialization steps for both static and dynamic memory allocations.
