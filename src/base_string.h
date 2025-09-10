#ifndef BASE_H
#define BASE_H

#include "base_arena.h"
#include "base_core.h"

typedef struct {
    String buf;
    size_t capacity;
} StringBuilder;

String cstring_copy(Arena *arena, const char *s);

String string_copy(Arena *arena, const String *s);

struct String_Slice_Params {
    i64 from;
    i64 to;
};

StringSlice _string_slice(const String *s, struct String_Slice_Params params);
#define string_slice(s, ...)                                                          \
    _string_slice((s),                                                                \
        (struct String_Slice_Params) {.from = 0, .to = (i64) (s)->len, __VA_ARGS__})

void string_builder_init(Arena *arena, StringBuilder *sb, size_t capacity);
#define string_builder_reinit(arena, sb) \
    string_builder_init((arena), (sb), (sb)->capacity)

String string_builder_build(Arena *arena, StringBuilder *sb, char *s, ...);

#endif
