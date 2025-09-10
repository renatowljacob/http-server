#ifndef ARENA_H
#define ARENA_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "base_core.h"

#define DEFAULT_ALIGNMENT (sizeof(void *))
#define DEFAULT_ALLOC_SIZE KB(10)

typedef struct {
    u8     *buf;
    size_t buf_size;
    size_t prev_offset;
    size_t curr_offset;
} Arena;

typedef struct {
    Arena *arena;
    size_t prev_offset;
    size_t curr_offset;
} ArenaTemp;

struct Arena_Init_Params {
    size_t size;
    void *backing_buffer;
};

Arena _arena_init(struct Arena_Init_Params params);
#define arena_init(...) \
    _arena_init((struct Arena_Init_Params) { .size = DEFAULT_ALLOC_SIZE, .backing_buffer = NULL, __VA_ARGS__ })

void *arena_alloc_align(Arena *a, size_t size, size_t align);
#define arena_alloc(a, size) arena_alloc_align((a), (size), DEFAULT_ALIGNMENT)

void *arena_resize_align(
    Arena *a,
    void *old_memory,
    size_t old_size,
    size_t new_size,
    size_t align
);
#define arena_resize(a, old_memory, new_size) arena_resize_align(  \
    (a),                                                      \
    (old_memory),                                             \
    (a)->curr_offset - (a)->prev_offset,                      \
    (new_size),                                               \
    DEFAULT_ALIGNMENT                                         \
)

void arena_free(Arena *a);

void arena_reset(Arena *a);

ArenaTemp arena_temp_begin(Arena *a);

void arena_temp_end(ArenaTemp *temp);

bool _arena_check_overflow(Arena *a, size_t size, size_t align);
#define arena_check_overflow(a, size) _arena_check_overflow(  \
        (a),                                                  \
        (size),                                               \
        DEFAULT_ALIGNMENT)

size_t arena_get_relative_offset(Arena *a, size_t align);

#endif
