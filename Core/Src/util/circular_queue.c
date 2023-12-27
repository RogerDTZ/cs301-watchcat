#include "util/circular_queue.h"

#include <stdlib.h>

circular_queue_t *circular_queue_alloc(uint16_t size)
{
  circular_queue_t *q = malloc(sizeof(circular_queue_t));
  q->len = size;
  q->buffer = malloc(size);
  q->head = 0;
  q->tail = 0;
  return q;
}

int circular_queue_push(circular_queue_t *q, uint16_t size, const void *src)
{
  uint16_t remain = q->len - 1 - circular_queue_size(q);
  if (size > remain) {
    return 0;
  }

  uint16_t res = size;
  const uint8_t *src_ptr = src;
  while (size > 0) {
    q->buffer[q->tail++] = *src_ptr++;
    if (q->tail >= q->len) {
      q->tail = 0;
    }
    size--;
  }
  return res;
}

void circular_queue_get(circular_queue_t *q, uint16_t size, void *dst)
{
  assert(size <= circular_queue_size(q) && "No enough data in the buffer");

  uint16_t src_i = q->head;
  uint8_t *dst_ptr = dst;
  while (size > 0) {
    *dst_ptr++ = q->buffer[src_i++];
    if (src_i >= q->len) {
      src_i = 0;
    }
    size--;
  }
}

void circular_queue_get_off(circular_queue_t *q, uint16_t offset, uint16_t size,
                            void *dst)
{
  assert(offset + size <= circular_queue_size(q) &&
         "No enough data in the buffer");

  uint16_t src_i = (q->head + offset) % q->len;
  uint8_t *dst_ptr = dst;
  while (size > 0) {
    *dst_ptr++ = q->buffer[src_i++];
    if (src_i >= q->len) {
      src_i = 0;
    }
    size--;
  }
}

void circular_queue_pop(circular_queue_t *q, uint16_t size)
{
  while (size > 0) {
    q->head = (q->head + 1) % q->len;
    size--;
  }
}

void circular_queue_free(circular_queue_t *q)
{
  free(q->buffer);
  free(q);
}
