#include "mx_queue.h"
#include <stdio.h>

#define QUEUE_SIZE 256
#define ITEMS_PER_PRODUCER 100
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2

MUXTOR_TOOLKIT_DEFINE_FIFO_QUEUE(int, IntQueue, QUEUE_SIZE, 4)

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