#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "base_arena.h"
#include "base_core.h"

Arena _arena_init(struct Arena_Init_Params params)
{
    size_t size = params.size;
    void *backing_buffer = params.backing_buffer or_else malloc(ALIGN_POW2(size, DEFAULT_ALIGNMENT));
    assert(backing_buffer != NULL);

    return (Arena) {
        .buf = (u8 *)backing_buffer,
        .buf_size = size,
        .curr_offset = 0,
        .prev_offset = 0,
    };
}

void *arena_alloc_align(Arena *a, size_t size, size_t align)
{
    size_t offset = arena_get_relative_offset(a, align);

    if (!_arena_check_overflow(a, size, align))
    {
        void *ptr = &a->buf[offset];
        a->prev_offset = offset;
        a->curr_offset = offset + size;
        memset(ptr, 0, size);

        return ptr;
    }
    return NULL;
}

void *arena_resize_align(
    Arena *a,
    void *old_memory,
    size_t old_size,
    size_t new_size,
    size_t align
)
{
    u8 *old_mem = (u8 *)old_memory;

    assert(IS_POW2(align));

    if (old_mem == NULL || old_size == 0)
    {
        return arena_alloc_align(a, new_size, align);
    }

    // Out of bounds
    if (old_mem < a->buf || old_mem >= a->buf + a->buf_size)
    {
        return NULL;
    }

    if (a->buf + a->prev_offset == old_mem)
    {
        size_t new_offset = a->prev_offset + new_size;
        if (new_offset > a->buf_size)
        {
            return NULL;
        }
        a->curr_offset = new_offset;

        if (new_size > old_size)
        {
            memset(&a->buf[a->curr_offset], 0, new_size - old_size);
        }

        return old_memory;
    }

    return arena_alloc_align(a, new_size, align);
}

void arena_free(Arena *a)
{
    free(a->buf);
}

void arena_reset(Arena *a)
{
    a->prev_offset = 0;
    a->curr_offset = 0;
}

ArenaTemp arena_temp_begin(Arena *a)
{
    return (ArenaTemp) {
        .arena = a,
        .curr_offset = a->curr_offset,
        .prev_offset = a->prev_offset
    };
}

void arena_temp_end(ArenaTemp *temp)
{
    temp->arena->curr_offset = temp->curr_offset;
    temp->arena->prev_offset = temp->prev_offset;
}

// Maybe eliminate this
bool _arena_check_overflow(Arena *a, size_t size, size_t align)
{
    size_t offset = arena_get_relative_offset(a, align);
    if (offset + size > a->buf_size)
    {
        return true;
    }
    return false;
}

size_t arena_get_relative_offset(Arena *a, size_t align)
{
    uintptr_t curr_ptr = (uintptr_t) a->buf + (uintptr_t) a->curr_offset;
    uintptr_t offset = ALIGN_POW2(curr_ptr, align);

    offset -= (uintptr_t) a->buf;

    return offset;
}
