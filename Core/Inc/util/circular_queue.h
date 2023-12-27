#ifndef __CIRCULAR_QUEUE_H__
#define __CIRCULAR_QUEUE_H__

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint16_t len;
  uint8_t *buffer;
  uint16_t head;
  uint16_t tail;
} circular_queue_t;

circular_queue_t *circular_queue_alloc(uint16_t size);

inline static int circular_queue_size(circular_queue_t *q)
{
  return (q->tail - q->head + q->len) % q->len;
}

inline static bool circular_queue_empty(circular_queue_t *q)
{
  return circular_queue_size(q) == 0;
}

int circular_queue_push(circular_queue_t *q, uint16_t size, const void *src);

void circular_queue_get(circular_queue_t *q, uint16_t size, void *dst);

void circular_queue_get_off(circular_queue_t *q, uint16_t offset, uint16_t size,
                            void *dst);

void circular_queue_pop(circular_queue_t *q, uint16_t size);

void circular_queue_free(circular_queue_t *q);

#endif /* __CIRCULAR_QUEUE_H__ */
