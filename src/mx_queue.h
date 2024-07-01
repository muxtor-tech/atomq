#ifndef MUXTOR_ATOMIC_QUEUE_H
#define MUXTOR_ATOMIC_QUEUE_H

#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Define a type for storing the head and tail in a single 32-bit value
// validate that the basic integral type covers our design requirements
static_assert(sizeof(unsigned int) >= 4);
typedef unsigned int muxtor_internal_head_tail_t;

#define MUXTOR_INTERNAL_GET_HEAD(ht) ((unsigned int)((ht >> 16) & 0xFFFF))
#define MUXTOR_INTERNAL_GET_TAIL(ht) ((unsigned int)(ht & 0xFFFF))
#define MUXTOR_INTERNAL_MAKE_HEAD_TAIL(head, tail) (((head & 0xFFFF) << 16) | (tail & 0xFFFF))
#define MUXTOR_INTERNAL_ADJUST_SIZE_TO_MULTIPLE(size, align) ((size-1) + (align - ((size-1) % align)))

// Macro to define a FIFO queue structure, static buffer type, and functions,
// with static validation of the provided parameters
#define MUXTOR_TOOLKIT_DEFINE_FIFO_QUEUE(T, Q, SIZE, ALIGN) \
    static_assert(SIZE <= 0xFFFF); /* SIZE MUST BE 2^16 OR LESS */ \
    static_assert(SIZE > 1); /* SIZE MUST BE GREATER THAN 1 */ \
    typedef struct { \
        _Atomic muxtor_internal_head_tail_t head_tail; \
        T* buffer; \
    } Q; \
    \
    typedef struct { \
        T data[SIZE] __attribute__((aligned(ALIGN))); \
    } Q##_StaticBuffer; \
    \
    static inline void Q##_init_static(Q* q, Q##_StaticBuffer* buf) { \
        atomic_store(&q->head_tail, 0); \
        q->buffer = buf->data; \
    } \
    \
    static inline int Q##_init_dynamic(Q* q) { \
        atomic_store(&q->head_tail, 0); \
        q->buffer = (T*)aligned_alloc(ALIGN, \
            MUXTOR_INTERNAL_ADJUST_SIZE_TO_MULTIPLE(sizeof(T) * SIZE, ALIGN)); \
        if (q->buffer == NULL) return -1; \
        return 0; \
    } \
    \
    static inline void Q##_free(Q* q) { \
        if (q->buffer) { \
            free(q->buffer); \
            q->buffer = NULL; \
        } \
    } \
    \
    static inline int Q##_is_empty(Q* q) { \
        muxtor_internal_head_tail_t ht = atomic_load(&q->head_tail); \
        return MUXTOR_INTERNAL_GET_HEAD(ht) == MUXTOR_INTERNAL_GET_TAIL(ht); \
    } \
    \
    static inline int Q##_is_full(Q* q) { \
        muxtor_internal_head_tail_t ht = atomic_load(&q->head_tail); \
        return ((MUXTOR_INTERNAL_GET_TAIL(ht) + 1) % SIZE) == MUXTOR_INTERNAL_GET_HEAD(ht); \
    } \
    \
    static inline int Q##_enqueue(Q* q, const T* item) { \
        muxtor_internal_head_tail_t ht, new_ht; \
        size_t head, tail, next_tail; \
        do { \
            ht = atomic_load(&q->head_tail); \
            head = MUXTOR_INTERNAL_GET_HEAD(ht); \
            tail = MUXTOR_INTERNAL_GET_TAIL(ht); \
            next_tail = (tail + 1) % SIZE; \
            if (next_tail == head) { \
                return -1; /* Queue is full */ \
            } \
            new_ht = MUXTOR_INTERNAL_MAKE_HEAD_TAIL(head, next_tail); \
        } while (!atomic_compare_exchange_weak(&q->head_tail, &ht, new_ht)); \
        memcpy(&q->buffer[tail], item, sizeof(T)); \
        return 0; \
    } \
    \
    static inline int Q##_dequeue(Q* q, T* item) { \
        muxtor_internal_head_tail_t ht, new_ht; \
        size_t head, tail, next_head; \
        do { \
            ht = atomic_load(&q->head_tail); \
            head = MUXTOR_INTERNAL_GET_HEAD(ht); \
            tail = MUXTOR_INTERNAL_GET_TAIL(ht); \
            if (head == tail) { \
                return -1; /* Queue is empty */ \
            } \
            next_head = (head + 1) % SIZE; \
            new_ht = MUXTOR_INTERNAL_MAKE_HEAD_TAIL(next_head, tail); \
        } while (!atomic_compare_exchange_weak(&q->head_tail, &ht, new_ht)); \
        memcpy(item, &q->buffer[head], sizeof(T)); \
        return 0; \
    }

#endif
