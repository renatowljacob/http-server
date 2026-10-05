/*
 *  Acknowledgements: this library is hugely inspired by handmade hero, gb.h,
 *  odin, the raddebugger codebase, vkrajacic's own library excerpt at BSC and
 *  Tsoding.
 *
 *  The functionality revolves around Linux because that's what I use :P
 *  although I intend to extend it to Windows and BSD as well.
 *
 *  Wouldn't recommend using it because it's for my own personal use (and the
 *  code probably sucks too), so use it at your own risk.
 */

#ifndef RWLJ_H
#define RWLJ_H

#include <stdio.h>
#ifdef __SSE2__
#include <emmintrin.h>
#endif

#ifdef __linux__

#include <bits/time.h>
#include <limits.h>
#include <memory.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#define _GNU_SOURCE

#elif defined(_WIN64)

#endif

#if defined(_WIN64) || defined(__x86_64__) || defined(_M_X64) ||               \
    defined(__64BIT__) || defined(__powerpc64__) || defined(__ppc64__)

#define RWLJ_ARCH_64_BIT 1

#else

#define RWLJ_ARCH_32_BIT 1

#endif

// TODO: pthreads

/*
 *
 *  DECLARATION
 *
 */

/*
 *  Types, Utility Functions and Macros
 */

#if defined(__clang__) || defined(__GNUC__)
#define RWLJ_PRINTF_ARGS(num) __attribute__((format(printf, num, ((num) + 1))))
#endif

#define global     static
#define internal   static
#define persistent static

#define rwlj_concat(x, y)  x##y
#define rwlj_concat_(x, y) x##_##y

#define RWLJ_STDIN  STDIN_FILENO
#define RWLJ_STDOUT STDOUT_FILENO
#define RWLJ_STDERR STDERR_FILENO

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#ifdef __SIZEOF_INT128__
typedef __int128_t i128;
typedef __uint128_t u128;
#endif

typedef intptr_t intptr;
typedef uintptr_t uintptr;
typedef ptrdiff_t ptrdiff;
typedef ptrdiff isize;
typedef size_t usize;

typedef bool b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;
typedef isize bsize;

#ifdef __SSE__
typedef _Float16 f16;
#endif
typedef float f32;
typedef double f64;

#define RWLJ_U8_MIN 0u
#define RWLJ_U8_MAX 0xffu
#define RWLJ_I8_MIN (-0x7f - 1)
#define RWLJ_I8_MAX 0x7f

#define RWLJ_U16_MIN 0u
#define RWLJ_U16_MAX 0xffffu
#define RWLJ_I16_MIN (-0x7fff - 1)
#define RWLJ_I16_MAX 0x7fff

#define RWLJ_U32_MIN 0u
#define RWLJ_U32_MAX 0xffffffffu
#define RWLJ_I32_MIN (-0x7fffffff - 1)
#define RWLJ_I32_MAX 0x7fffffff

#define RWLJ_U64_MIN 0ull
#define RWLJ_U64_MAX 0xffffffffffffffffull
#define RWLJ_I64_MIN (-0x7fffffffffffffffll - 1)
#define RWLJ_I64_MAX 0x7fffffffffffffffll

#define RWLJ_F16_MAX 65504.0
#define RWLJ_F16_MIN 6.10351562e-5

#define RWLJ_F32_MAX 3.40282347e+38F
#define RWLJ_F32_MIN 1.17549435e-38F

#define RWLJ_F64_MIN 2.2250738585072014e-308
#define RWLJ_F64_MAX 1.7976931348623158e+308

#define cast(T) (T)

// Can't have shit in Detroit
// #define transmute(x, T) *(T *)&x

#define transmute(x, from_T, to_T)                                             \
    ((union {                                                                  \
        from_T a;                                                              \
        to_T b;                                                                \
    }){ x })                                                                   \
        .b

#define RWLJ_TRAP() __builtin_trap()

#define rwlj_no_op()     ((void)0)
#define rwlj_unused(var) ((void)var)

#ifdef RWLJ_DISABLE_ASSERT
#define rwlj_debug_print(...) rwlj_no_op()

#define rwlj_debug_printf(...) rwlj_no_op()

#define rwlj_assert(cond) rwlj_no_op()

#define rwlj_assert_msg(cond, ...) rwlj_no_op()

#define rwlj_assert_fail(...) rwlj_no_op()

#define rwlj_not_implemented() rwlj_no_op()
#else // RWLJ_DISABLE_ASSERT
#define rwlj_debug_print(...) rwlj_eprintln(__VA_ARGS__)

#define rwlj_debug_printf(...) rwlj_eprintfln(__VA_ARGS__)

#define rwlj_assert(cond)                                                      \
    do {                                                                       \
        if (cond) {                                                            \
            break;                                                             \
        }                                                                      \
                                                                               \
        rwlj_debug_printf(                                                     \
            "Assertion failure at %s:%d:%s()",                                 \
            RWLJ_FILE,                                                         \
            RWLJ_LINE,                                                         \
            RWLJ_FUNCTION                                                      \
        );                                                                     \
        RWLJ_TRAP();                                                           \
    } while (false)

#define rwlj_assert_msg(cond, ...)                                             \
    do {                                                                       \
        if (cond) {                                                            \
            break;                                                             \
        }                                                                      \
                                                                               \
        rwlj_debug_printf(__VA_ARGS__);                                        \
        RWLJ_TRAP();                                                           \
    } while (false)

#define rwlj_assert_fail(...) rwlj_assert_msg(false, __VA_ARGS__)

#define rwlj_not_implemented()                                                 \
    rwlj_assert_fail(                                                          \
        "NOT IMPLEMENTED: %s:%d:%s()", RWLJ_FILE, RWLJ_LINE, RWLJ_FUNCTION     \
    )
#endif // RWLJ_DISABLE_ASSERT

#define RWLJ_LINE     __LINE__
#define RWLJ_FUNCTION __func__
#define RWLJ_FILE     __FILE__
#define RWLJ_COUNTER  (__COUNTER__ + 1)

#define __rwlj_static_assert(cond, msg)                                        \
    global u8 rwlj_concat(msg, RWLJ_LINE)[!!(cond) ? 1 : -1]

#define rwlj_static_assert(cond)                                               \
    __rwlj_static_assert(cond, static_assertion_at_line_)

#define rwlj_panic(msg) __rwlj_static_assert(false, msg)

#define rwlj_size_of(x) (isize)(sizeof(x))

#define RWLJ_WORD_SIZE rwlj_size_of(void *)

#define rwlj_size_of_bits(x)      (rwlj_size_of(x) * RWLJ_WORD_SIZE)
#define rwlj_count_of(a)          (rwlj_size_of(a) / rwlj_size_of(0 [a]))
#define rwlj_offset_of(T, member) ((isize) & (((T *)0)->member))
#define rwlj_align_of(T)                                                       \
    ((isize)rwlj_offset_of(                                                    \
        struct {                                                               \
            char c;                                                            \
            T member;                                                          \
        },                                                                     \
        member                                                                 \
    ))
#define rwlj_container_of(T, object, member)                                   \
    (void *)((intptr)(object) - rwlj_offset_of(T, member))

#define rwlj_bit(n) (1ull << n)
#define rwlj_kb(n)  (n << 10)
#define rwlj_mb(n)  (n << 20)
#define rwlj_gb(n)  (n << 30)

#define rwlj_swap(T, x, y)                                                     \
    do {                                                                       \
        T __temp = (x);                                                        \
        (x) = (y);                                                             \
        (y) = __temp;                                                          \
    } while (false)

#define rwlj_max(x, y)          (x > y ? x : y)
#define rwlj_min(x, y)          (x < y ? x : y)
#define rwlj_clamp(x, min, max) (rwlj_max(rwlj_min(x, max), min))

#define rwlj_align_pow2(x, align) (((x) + (align) - 1) & (~((align) - 1)))
#define rwlj_is_pow2(x)           (((x) & ((x) - 1)) == 0)

#define rwlj_is_space(c)                                                       \
    (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v')
#define rwlj_is_upper(c)        (c >= 'A' && c <= 'Z')
#define rwlj_is_lower(c)        (c >= 'a' && c <= 'z')
#define rwlj_to_upper(c)        (rwlj_is_lower(c) ? (c) - 32 : c)
#define rwlj_to_lower(c)        (rwlj_is_upper(c) ? (c) + 32 : c)
#define rwlj_is_alpha(c)        (rwlj_is_upper(c) || rwlj_is_lower(c))
#define rwlj_is_digit(c)        (c >= '0' && c <= '9')
#define rwlj_is_alphanumeric(c) (rwlj_is_alpha(c) || rwlj_is_digit(c))
#define rwlj_is_digit_hex(c)                                                   \
    (rwlj_is_digit(c) || (rwlj_to_upper(c) >= 'A' && rwlj_to_upper(c) <= 'F'))

/*
 *  Memory
 */

#define rwlj_memory_compare(a, b, size)  memcmp(a, b, size)
#define rwlj_memory_copy(dst, src, size) memcpy(dst, src, size)
#define rwlj_memory_move(dst, src, size) memmove(dst, src, size)
#define rwlj_memory_set(mem, byte, size) memset(mem, byte, size)
void *rwlj_memory_swap(void *a, void *b, usize size);
#define rwlj_memory_zero(mem, size) rwlj_memory_set(mem, 0, size)

/*
 *  Math
 */

#define rwlj_abs(x) (x >= 0 ? x : -(x))

#define rwlj_truncate(x) (cast(i64) x)
#define rwlj_floor(x)    (rwlj_truncate(x > 0.0f ? x : (x) - 1.0f))
#define rwlj_ceil(x)     (rwlj_truncate(x < 0.0f ? x : (x) + 1.0f))
#define rwlj_round(x)                                                          \
    (x >= 0.0f ? rwlj_ceil((x) - 0.5f) : rwlj_floor((x) + 0.5f))

#define RWLJ_F16_SHIFT (16 - 6)
#define RWLJ_F16_MASK  0x1fll
#define RWLJ_F16_BIAS  0xfll

#define RWLJ_F32_SHIFT (32 - 9)
#define RWLJ_F32_MASK  0xffll
#define RWLJ_F32_BIAS  0x7fll

#define RWLJ_F64_SHIFT (64 - 12)
#define RWLJ_F64_MASK  0x7ffll
#define RWLJ_F64_BIAS  0x3ffll

#define RWLJ_NAN      (.0 / .0)
#define RWLJ_INFINITY (.1 / .0)

enum rwljFloat_Class {
    RWLJ_FLOAT_CLASS_NORMAL,
    RWLJ_FLOAT_CLASS_SUBNORMAL,
    RWLJ_FLOAT_CLASS_INFINITY,
    RWLJ_FLOAT_CLASS_NAN,
};
typedef isize rwljFloat_Class;

bsize rwlj_is_inf(f64 f);
bool rwlj_is_nan(f64 f);
bool rwlj_is_subnormal(f64 f);
rwljFloat_Class rwlj_classify(f64 f);

/*
 *  Memory Allocators
 */

/* arena */

#define RWLJ_DEFAULT_ALIGNMENT RWLJ_WORD_SIZE

enum rwljArena_Kind {
    RWLJ_ARENA_STATIC,
    RWLJ_ARENA_GROWING,
    RWLJ_ARENA_BUFFER,
};
typedef isize rwljArena_Kind;

typedef struct rwljArena {
    u8 *backing_buf;
    usize total_size;
    usize allocated_size;
    rwljArena_Kind kind;
} rwljArena;

void
rwlj_arena_init(rwljArena *arena, void *mem, usize size, rwljArena_Kind kind);

#define rwlj_arena_init_static(arena, size)                                    \
    rwlj_arena_init(arena, NULL, size, RWLJ_ARENA_STATIC)

#define rwlj_arena_init_growing(arena, size)                                   \
    rwlj_arena_init(arena, NULL, size, RWLJ_ARENA_GROWING)

#define rwlj_arena_init_from_buffer(arena, backing_buf, size)                  \
    rwlj_arena_init(arena, backing_buf, size, RWLJ_ARENA_BUFFER)

void *rwlj_arena_alloc_aligned(rwljArena *arena, usize size, usize alignment);
#define rwlj_arena_alloc(arena, size)                                          \
    rwlj_arena_alloc_aligned(arena, size, RWLJ_DEFAULT_ALIGNMENT)

void *rwlj_arena_resize_aligned(
    rwljArena *arena,
    void *old_mem,
    usize old_size,
    usize new_size,
    usize alignment
);
#define rwlj_arena_resize(arena, old_mem, old_size, new_size)                  \
    rwlj_arena_resize_aligned(                                                 \
        arena, old_mem, old_size, new_size, RWLJ_DEFAULT_ALIGNMENT             \
    )

void rwlj_arena_free_all(rwljArena *arena);
void rwlj_arena_destroy(rwljArena *arena);

typedef struct rwljArena_Temp {
    rwljArena *arena;
    usize allocated_size;
} rwljArena_Temp;

void rwlj_arena_temp_init(rwljArena_Temp *arena_temp, rwljArena *arena);
void rwlj_arena_temp_free_all(rwljArena_Temp *arena_temp);

/*
 *  Data Structures
 */

/* slices and dynamic arrays */

enum rwljArray_Kind {
    RWLJ_ARRAY_GROWING,
    RWLJ_ARRAY_FIXED,
};
typedef isize rwljArray_Kind;

#define GENERIC_ARRAY(T, name)                                                 \
    typedef struct rwlj_concat_(rwljArray, name) {                             \
        T *data;                                                               \
        isize len;                                                             \
        isize capacity;                                                        \
        rwljArray_Kind kind;                                                   \
        rwljArena *arena;                                                      \
        usize item_size;                                                       \
    } rwlj_concat_(rwljArray, name);                                           \
    typedef struct rwlj_concat_(rwljSlice, name) {                             \
        T *data;                                                               \
        isize len;                                                             \
    } rwlj_concat_(rwljSlice, name)

GENERIC_ARRAY(char, Char);
GENERIC_ARRAY(i8, I8);
GENERIC_ARRAY(i16, I16);
GENERIC_ARRAY(i32, I32);
GENERIC_ARRAY(i64, I64);
GENERIC_ARRAY(isize, Isize);
GENERIC_ARRAY(i128, I128);
GENERIC_ARRAY(u8, U8);
GENERIC_ARRAY(u16, U16);
GENERIC_ARRAY(u32, U32);
GENERIC_ARRAY(u64, U64);
GENERIC_ARRAY(usize, Usize);
GENERIC_ARRAY(u128, U128);
GENERIC_ARRAY(f16, F16);
GENERIC_ARRAY(f32, F32);
GENERIC_ARRAY(f64, F64);
GENERIC_ARRAY(bool, Bool);
GENERIC_ARRAY(b8, B8);
GENERIC_ARRAY(b16, B16);
GENERIC_ARRAY(b32, B32);
GENERIC_ARRAY(b64, B64);
GENERIC_ARRAY(bsize, Bsize);
GENERIC_ARRAY(void, Void);

#define rwlj_slice(ptr, start, end)                                            \
    { &(ptr)[start], rwlj_max((end) - (start), 0) }
#define rwlj_slice_from_array(array) rwlj_slice(array, 0, rwlj_count_of(array))
#define rwlj_slice_from_buf(buf)     rwlj_slice_from_array(buf)

#define rwlj_slice_unpack(slice) (slice)->data, (slice)->len

#define rwlj_slice_get(slice, index)                                           \
    (index >= 0 && index < (slice)->len ? (slice)->data[index] : 0)
#define rwlj_slice_get_struct(item_T, slice, index)                            \
    (index >= 0 && index < (slice)->len ? (slice)->data[index] : (item_T){ 0 })

#define rwlj_slice_set(slice, index, value)                                    \
    (index >= 0 && index < (slice)->len                                        \
         ? ((slice)->data[index] = (value), true)                              \
         : false)

#define rwlj_slice_reverse(slice, item_T)                                      \
    do {                                                                       \
        for (isize __i = 0; __i < (slice)->len / 2; __i += 1) {                \
            rwlj_swap(                                                         \
                item_T,                                                        \
                (slice)->data[__i],                                            \
                (slice)->data[((slice)->len - __i - 1)]                        \
            );                                                                 \
        }                                                                      \
    } while (false)

#define rwlj_slice_clear(slice)                                                \
    do {                                                                       \
        rwlj_memory_zero(                                                      \
            &(slice)->data[0],                                                 \
            cast(usize)((slice)->len * rwlj_size_of((slice)->data[0]))         \
        );                                                                     \
    } while (false)

#define RWLJ_GROW_FORMULA(capacity) (8 + (capacity) * 2)

#define rwlj_array_init_reserve_with_kind(array, capacity_, kind_, arena_)     \
    do {                                                                       \
        (array)->arena = (arena_);                                             \
        (array)->len = 0;                                                      \
        (array)->capacity = (capacity_);                                       \
        (array)->kind = (kind_);                                               \
        (array)->item_size = rwlj_size_of((array)->data[0]);                   \
        (array)->data = rwlj_arena_alloc(                                      \
            (array)->arena, cast(usize)(array)->capacity * (array)->item_size  \
        );                                                                     \
    } while (false)

#define rwlj_array_init_dynamic(array, arena)                                  \
    rwlj_array_init_reserve_with_kind(                                         \
        array, RWLJ_GROW_FORMULA(0), RWLJ_ARRAY_GROWING, arena                 \
    )

#define rwlj_array_init_dynamic_reserve(array, capacity, arena)                \
    rwlj_array_init_reserve_with_kind(                                         \
        array, capacity, RWLJ_ARRAY_GROWING, arena                             \
    )

#define rwlj_array_init_fixed(array, fixed_array)                              \
    do {                                                                       \
        (array)->arena = NULL;                                                 \
        (array)->len = 0;                                                      \
        (array)->capacity = rwlj_count_of(fixed_array);                        \
        (array)->kind = RWLJ_ARRAY_FIXED;                                      \
        (array)->item_size = rwlj_size_of((array)->data[0]);                   \
        (array)->data = (fixed_array);                                         \
    } while (false)

#define rwlj_array_get(array, index) rwlj_slice_get(array, index)
#define rwlj_array_get_struct(item_T, array, index)                            \
    rwlj_slice_get_struct(item_T, array, index)

#define rwlj_array_set(array, index, value) rwlj_slice_set(array, index, value)

bool __rwlj_array_resize(rwljArray_I64 *array, isize new_capacity);

#define rwlj_array_resize(array, new_capacity)                                 \
    __rwlj_array_resize((rwljArray_I64 *)array, new_capacity)

#define rwlj_array_grow(array)                                                 \
    rwlj_array_resize(array, RWLJ_GROW_FORMULA((array)->capacity))

#define rwlj_array_reserve(array, new_capacity)                                \
    rwlj_array_resize(                                                         \
        array, new_capacity > (array)->capacity ? new_capacity : 0             \
    )

#define rwlj_array_shrink(array) rwlj_array_resize(array, (array)->len)

// Abusing short circuit to resize array (while also being an expression,
// that's the important part)
// Checks to see if the array is a growing one and has enough capacity or can be
// grown
#define rwlj_array_append(array, value)                                        \
    ((((array)->kind == RWLJ_ARRAY_GROWING &&                                  \
       (array)->len < (array)->capacity) ||                                    \
      rwlj_array_grow(array)) ||                                               \
             ((array)->kind == RWLJ_ARRAY_FIXED &&                             \
              (array)->len < (array)->capacity)                                \
         ? ((array)->data[((array)->len)] = (value), (array)->len += 1, true)  \
         : false)

#define rwlj_array_insert(array, index, value)                                 \
    ((((array)->kind == RWLJ_ARRAY_GROWING &&                                  \
       (array)->len < (array)->capacity) ||                                    \
      rwlj_array_grow(array)) ||                                               \
             ((array)->kind == RWLJ_ARRAY_FIXED &&                             \
              (array)->len < (array)->capacity)                                \
         ? (rwlj_memory_move(                                                  \
                &(array)->data[(index) + 1],                                   \
                &(array)->data[index],                                         \
                (cast(usize)(array)->len * (array)->item_size)                 \
            ),                                                                 \
            (array)->data[index] = (value),                                    \
            ((array)->len += 1),                                               \
            true)                                                              \
         : false)

#define rwlj_array_pop_nil(array, nil)                                         \
    ((array)->len > 0 ? ((array)->len -= 1, (array)->data[(array)->len]) : nil)

#define rwlj_array_pop(array) rwlj_array_pop_nil(array, 0)

#define rwlj_array_remove_unordered(array, index)                              \
    do {                                                                       \
        if (index == (array)->len - 1) {                                       \
            (array)->len -= 1;                                                 \
        }                                                                      \
                                                                               \
        if (index < (array)->len && index >= 0 && (array)->len > 0) {          \
            (array)->len -= 1;                                                 \
            (array)->data[index] = (array)->data[(array)->len];                \
        }                                                                      \
    } while (false)

#define rwlj_array_remove_ordered(array, index)                                \
    do {                                                                       \
        if (index == (array)->len - 1) {                                       \
            (array)->len -= 1;                                                 \
        }                                                                      \
                                                                               \
        if (index < (array)->len && index >= 0 && (array)->len > 0) {          \
            (array)->data[index] = (array)->data[(index) + 1];                 \
            rwlj_memory_move(                                                  \
                &(array)->data[index],                                         \
                &(array)->data[(index) + 1],                                   \
                cast(usize)((array)->len - (index)) * (array)->item_size       \
                                                                               \
            );                                                                 \
            (array)->len -= 1;                                                 \
        }                                                                      \
    } while (false)

/* hashmaps */

// TODO: Make a hashmap

/*
 *  Algorithms
 */

#define RWLJ_COMPARE_PROC(proc) isize proc(void *a, void *b)
typedef RWLJ_COMPARE_PROC(rwljCompare_Proc);

inline bsize rwlj_sort_compare_isize(void *a, void *b);
inline bsize rwlj_sort_compare_usize(void *a, void *b);
void rwlj_sort(void *data, isize len, isize elem_size, rwljCompare_Proc proc);
void
rwlj_smooth_sort(void *data, isize len, isize elem_size, rwljCompare_Proc proc);
void rwlj_insertion_sort(
    void *data,
    isize len,
    isize elem_size,
    rwljCompare_Proc proc
);

/*
 *  Time
 */

typedef long rwljDuration;
typedef long rwljTime;

#define RWLJ_TIME_NANOSECOND  (1ll)
#define RWLJ_TIME_MICROSECOND (1ll * 1000ll)
#define RWLJ_TIME_MILLISECOND (1000ll * 1000ll)
#define RWLJ_TIME_SECOND      (1000000ll * 1000ll)
#define RWLJ_TIME_MINUTE      (1000000000ll * 60ll)
#define RWLJ_TIME_HOUR        (60000000000ll * 60ll)
#define RWLJ_TIME_DAY         (3600000000000ll * 24ll)

rwljTime rwlj_time_now(void);
rwljDuration rwlj_time_diff(rwljTime t1, rwljTime t2);
rwljDuration rwlj_time_since(rwljTime time);
i64 rwlj_time_unix_now(void);
i64 rwlj_time_unix_diff(i64 t1, i64 t2);
i64 rwlj_time_unix_since(i64 time);
rwljDuration rwlj_time_sleep(rwljDuration time);

/*
 *  Strings
 */

typedef struct rwljString {
    char *data;
    isize len;
} rwljString;

GENERIC_ARRAY(rwljString, String);

#define STRING(string) cast(rwljString){ string, rwlj_size_of(string) - 1 }

#define rwlj_string_from_ptr(ptr, start, end)                                  \
    cast(rwljString)                                                           \
    {                                                                          \
        cast(char *) & (ptr)[start], rwlj_max((end) - (start), 0)              \
    }

#define rwlj_string(str, start, end) rwlj_string_from_ptr(str.data, start, end)

#define rwlj_string_from_slice(slice) rwlj_string(slice, 0, (slice)->len)

// rwljString procedures
isize __rwlj_string_compare(rwljString a, rwljString b, bool sensitive);
#define rwlj_string_compare(a, b)             __rwlj_string_compare(a, b, true)
#define rwlj_string_compare_insensitive(a, b) __rwlj_string_compare(a, b, false)

bool __rwlj_string_are_equal(rwljString a, rwljString b, bool sensitive);
#define rwlj_string_are_equal(a, b) __rwlj_string_are_equal(a, b, true)
#define rwlj_string_are_equal_insensitive(a, b)                                \
    __rwlj_string_are_equal(a, b, false)

bool __rwlj_string_contains(rwljString s, rwljString substr, bool sensitive);
#define rwlj_string_contains(s, substr) __rwlj_string_contains(s, substr, true)
#define rwlj_string_contains_insensitive(s, substr)                            \
    __rwlj_string_contains(s, substr, false)

isize __rwlj_string_index(rwljString s, rwljString substr, bool sensitive);
#define rwlj_string_index(s, substr) __rwlj_string_index(s, substr, true)
#define rwlj_string_index_insensitive(s, substr)                               \
    __rwlj_string_index(s, substr, false)

isize __rwlj_string_count(rwljString s, rwljString substr, bool sensitive);
#define rwlj_string_count(s, substr) __rwlj_string_count(s, substr, true)
#define rwlj_string_count_insensitive(s, substr)                               \
    __rwlj_string_count(s, substr, false)

rwljSlice_String __rwlj_string_split(
    rwljString s,
    rwljString sep,
    rwljArena *arena,
    bool sensitive
);
#define rwlj_string_split(s, substr, arena)                                    \
    __rwlj_string_split(s, substr, arena, true)
#define rwlj_string_split_insensitive(s, substr, arena)                        \
    __rwlj_string_split(s, substr, arena, false)

rwljString rwlj_string_clone(rwljString s, rwljArena *arena);
rwljString
rwlj_string_concatenate(rwljString a, rwljString b, rwljArena *arena);
rwljString rwlj_string_reverse(rwljString s, rwljArena *arena);

// C string procedures

char *rwlj_cstring_from_string(rwljString s, rwljArena *arena);
isize rwlj_cstring_len(const char *string, isize max_len);

// Formatting procudures

// TODO: Implement time formatting

#define rwlj_string_from_fmt(slice, ...)                                       \
    cast(rwljString)                                                           \
    {                                                                          \
        cast(char *)(slice)->data, rwlj_bprintf(*(slice), __VA_ARGS__)         \
    }

isize rwlj_write_i64(rwljSlice_U8 buf, i64 number);
isize rwlj_write_u64(rwljSlice_U8 buf, u64 number, u8 fmt);
isize rwlj_write_f64(rwljSlice_U8 buf, f64 number, u8 fmt, i64 precision);
isize rwlj_write_string(rwljSlice_U8 buf, rwljString s);

internal isize __rwlj_bprintf_va(
    rwljSlice_U8 buf,
    bool has_new_line,
    const char *fmt,
    va_list args
);
isize rwlj_bprintf(rwljSlice_U8 buf, const char *fmt, ...);
isize rwlj_bprintfln(rwljSlice_U8 buf, const char *fmt, ...);

typedef i32 rwljFile_Descriptor;

internal isize __rwlj_fprintf_va(
    rwljFile_Descriptor fd,
    bool has_new_line,
    char const *fmt,
    va_list ap
);
isize rwlj_fprintf(rwljFile_Descriptor fd, char const *fmt, ...);
isize rwlj_fprintfln(rwljFile_Descriptor fd, char const *fmt, ...);
isize rwlj_printf(char const *fmt, ...);
isize rwlj_printfln(char const *fmt, ...);
isize rwlj_eprintf(char const *fmt, ...);
isize rwlj_eprintfln(char const *fmt, ...);

isize rwlj_bprint(rwljSlice_U8 buf, rwljString s);
isize rwlj_bprintln(rwljSlice_U8 buf, rwljString s);
isize rwlj_fprint(rwljFile_Descriptor fd, rwljString s);
isize rwlj_fprintln(rwljFile_Descriptor fd, rwljString s);
isize rwlj_print(rwljString s);
isize rwlj_println(rwljString s);
isize rwlj_eprint(rwljString s);
isize rwlj_eprintln(rwljString s);

typedef struct rwljString_Builder {
    char *buf;
    isize len;
    isize capacity;
} rwljString_Builder;

void rwlj_string_builder_init(
    rwljString_Builder *sb,
    isize capacity,
    rwljArena *arena
);
rwljString rwlj_string_builder_write_i64(rwljString_Builder *sb, i64 number);
rwljString
rwlj_string_builder_write_u64(rwljString_Builder *sb, u64 number, u8 fmt);
rwljString rwlj_string_builder_write_f64(
    rwljString_Builder *sb,
    f64 number,
    u8 fmt,
    i64 precision
);
rwljString
rwlj_string_builder_write_string(rwljString_Builder *sb, rwljString s);
rwljString rwlj_string_builder_to_string(rwljString_Builder *sb);
rwljString rwlj_string_builder_clone(rwljString_Builder *sb, rwljArena *arena);
void rwlj_string_builder_clear(rwljString_Builder *sb);

rwljString rwlj_sbprintf(rwljString_Builder *sb, char const *fmt, ...);
rwljString rwlj_sbprintfln(rwljString_Builder *sb, char const *fmt, ...);
rwljString rwlj_sbprint(rwljString_Builder *sb, rwljString s);
rwljString rwlj_sbprintln(rwljString_Builder *sb, rwljString s);

#endif // RWLJ_H

#define RWLJ_IMPLEMENTATION
#if defined(RWLJ_IMPLEMENTATION) && !defined(RWLJ_IMPLEMENTAITON_DONE)
#define RWLJ_IMPLEMENTATION_DONE

/*
 *
 *  IMPLEMENTATION
 *
 */

/*
 *  Memory
 */

void *
rwlj_memory_swap(void *a, void *b, usize size)
{
    if (a == b || size == 0) {
        return a;
    }

    if (size == 1) {
        rwlj_swap(u8, *cast(u8 *) a, *cast(u8 *) b);
    } else if (size == 2) {
        rwlj_swap(u16, *cast(u16 *) a, *cast(u16 *) b);
    } else if (size == 4) {
        rwlj_swap(u32, *cast(u32 *) a, *cast(u32 *) b);
    } else if (size == 8) {
        rwlj_swap(u64, *cast(u64 *) a, *cast(u64 *) b);
    } else if (size < rwlj_size_of(u64)) {
        u8 tmp[8] = { 0 };
        rwlj_memory_copy(tmp, a, size);
        rwlj_memory_copy(a, b, size);
        rwlj_memory_copy(b, tmp, size);
    } else {
#define BUF_SIZE 256
        u8 tmp[BUF_SIZE] = { 0 };
        u8 *a2 = a;
        u8 *b2 = b;
        while (size > 0) {
            usize sz = rwlj_min(size, BUF_SIZE);

            rwlj_memory_copy(tmp, a2, sz);
            rwlj_memory_copy(a2, b2, sz);
            rwlj_memory_copy(b2, tmp, sz);
            a2 += sz;
            b2 += sz;
            size -= sz;
        }
#undef BUF_SIZE
    }

    return b;
}

/*
 *  Math
 */

bsize
rwlj_is_inf(f64 f)
{
    if (f * 0.5 + 1 == f) {
        return (f > 0) ? 1 : -1;
    }

    return false;
}

bool
rwlj_is_nan(f64 f)
{
    return f != f;
}

bool
rwlj_is_subnormal(f64 f)
{
    return !(transmute(f, f64, i64) & (RWLJ_F64_MASK << RWLJ_F64_SHIFT));
}

rwljFloat_Class
rwlj_classify(f64 f)
{
    rwljFloat_Class class = 0;
    if (rwlj_is_inf(f)) {
        class = RWLJ_FLOAT_CLASS_INFINITY;
    } else if (rwlj_is_nan(f)) {
        class = RWLJ_FLOAT_CLASS_NAN;
    } else if (rwlj_is_subnormal(f)) {
        class = RWLJ_FLOAT_CLASS_SUBNORMAL;
    }

    return class;
}

/*
 *  Algorithms
 */

inline RWLJ_COMPARE_PROC(rwlj_sort_compare_isize)
{
    return *cast(isize *) a - *cast(isize *) b;
}

inline RWLJ_COMPARE_PROC(rwlj_sort_compare_usize)
{
    return cast(isize)(*cast(usize *) a - *cast(usize *) b);
}

// TODO: Finish sorting functions

// void
// rwlj_sort(void *data, isize len, isize size, rwljCompare_Proc proc)
// {
//     if (data == NULL || len <= 1 || size == 0 || proc == NULL) {
//         return;
//     }
//     if (len <= 8) {
//         return rwlj_insertion_sort(data, len, size, proc);
//     }
//
//     return rwlj_smooth_sort(data, len, size, proc);
// }
//
// #define RWLJ_TREES_LEN RWLJ_WORD_SIZE * 12
//
// void
// __rwlj_leonardo_heap_rectify(
//     void *data,
//     isize len,
//     rwljSlice_Isize shape,
//     rwljCompare_Proc proc
// )
// {
// }
//
// void
// __rwlj_leonardo_heap_remove(
//     void *data,
//     isize len,
//     rwljSlice_Isize shape,
//     rwljCompare_Proc proc
// )
// {
// }
//
// void
// __rwlj_leonardo_heap_add(
//     void *data,
//     isize len,
//     rwljSlice_Isize shape,
//     rwljCompare_Proc proc
// )
// {
// }
//
// void
// rwlj_smooth_sort(void *data, isize len, isize size, rwljCompare_Proc proc)
// {
//     if (data == NULL || len <= 1 || size == 0 || proc == NULL) {
//         return;
//     }
//
//     u8 *buf = data;
//
//     usize trees_buf[RWLJ_TREES_LEN];
//     rwljSlice_Usize trees = { trees_buf, rwlj_count_of(trees_buf) };
//
//     // Heap add
//     for (isize i = 0; i < len; i += 1) {
//     }
//
//     // Heap remove
//     for (isize i = len - 1; i >= 0; i -= 1) {
//     }
// }

void
rwlj_insertion_sort(void *data, isize len, isize size, rwljCompare_Proc proc)
{
    if (data == NULL || len <= 1 || size == 0 || proc == NULL) {
        return;
    }

    u8 *buf = data;
    isize buf_size = len * size;

    for (isize i = size; i < buf_size; i += size) {
        for (isize j = i - size, key = i;
             j >= 0 && proc(&buf[j], &buf[key]) > 0;
             j -= size, key -= size) {
            rwlj_memory_swap(&buf[j], &buf[key], cast(usize) size);
        }
    }
}

/*
 *  Memory Allocators
 */

/* arena */

void
rwlj_arena_init(
    rwljArena *arena,
    void *backing_buf,
    usize size,
    rwljArena_Kind kind
)
{
    arena->allocated_size = 0;
    arena->kind = kind;

    switch (cast(enum rwljArena_Kind) arena->kind) {
    case RWLJ_ARENA_STATIC:
        arena->total_size = size != 0 ? size : rwlj_kb(1);
        arena->backing_buf = malloc(arena->total_size);
        break;
    case RWLJ_ARENA_GROWING:
        arena->total_size = size != 0 ? size : rwlj_kb(8);
        arena->backing_buf = mmap(
            NULL,
            arena->total_size,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANON,
            -1,
            0
        );
        break;
    case RWLJ_ARENA_BUFFER:
        arena->total_size = size;
        arena->backing_buf = backing_buf;
        break;
    }
    rwlj_assert(arena->backing_buf != NULL);
}

void *
rwlj_arena_alloc_aligned(rwljArena *arena, usize size, usize alignment)
{
    if (size == 0) {
        return NULL;
    }

    usize allocation_size = rwlj_align_pow2(size, alignment);
    usize allocation_offset = arena->allocated_size + allocation_size;
    if (allocation_offset > arena->total_size) {
        return NULL;
    }

    void *allocation = &arena->backing_buf[arena->allocated_size];
    rwlj_memory_zero(allocation, allocation_size);
    arena->allocated_size = allocation_offset;

    return allocation;
}

void *
rwlj_arena_resize_aligned(
    rwljArena *arena,
    void *old_memory,
    usize old_size,
    usize new_size,
    usize alignment
)
{
    u8 *old_mem = old_memory;
    if (old_mem == NULL || old_size == 0) {
        return rwlj_arena_alloc_aligned(arena, new_size, alignment);
    }

    // Out of bounds
    if (old_mem < arena->backing_buf ||
        old_mem >= &arena->backing_buf[arena->total_size] ||
        old_size > arena->allocated_size) {
        return NULL;
    }

    usize offset = arena->allocated_size - old_size;
    void *mem = &arena->backing_buf[offset];
    if (new_size > 0 && new_size <= arena->total_size && mem == old_mem) {
        arena->allocated_size = arena->allocated_size + new_size - old_size;
    }

    return old_mem;
}

void
rwlj_arena_free_all(rwljArena *arena)
{
    arena->allocated_size = 0;
}

void
rwlj_arena_destroy(rwljArena *arena)
{
    switch (cast(enum rwljArena_Kind) arena->kind) {
    case RWLJ_ARENA_STATIC:
        free(arena->backing_buf);
        break;
    case RWLJ_ARENA_GROWING:
        munmap(arena->backing_buf, arena->total_size);
        break;
    case RWLJ_ARENA_BUFFER:
        rwlj_no_op();
        return;
    }
}

void
rwlj_arena_temp_init(rwljArena_Temp *temp_arena, rwljArena *arena)
{
    temp_arena->arena = arena;
    temp_arena->allocated_size = temp_arena->arena->allocated_size;
}

void
rwlj_arena_temp_free_all(rwljArena_Temp *temp_arena)
{
    temp_arena->arena->allocated_size = temp_arena->allocated_size;
}

/*
 *  Array Resizing
 */

bool
__rwlj_array_resize(rwljArray_I64 *array, isize new_capacity)
{
    if (new_capacity == 0) {
        return false;
    }

    if (array->kind == RWLJ_ARRAY_FIXED) {
        // Reserve is a no op
        if (new_capacity > array->capacity || new_capacity < 0) {
            return false;
        }
        array->capacity = new_capacity;
        return true;
    }

    void *curr_data = array->data;
    usize array_curr_size = cast(usize) array->capacity * array->item_size;
    isize array_new_capacity = new_capacity;
    usize array_new_size = cast(usize) array_new_capacity * array->item_size;

    usize arena_new_allocated_size =
        array->arena->allocated_size - array_curr_size + array_new_size;
    if (arena_new_allocated_size > array->arena->total_size) {
        usize rest = array->arena->total_size - array->arena->allocated_size;
        array_new_capacity =
            array->capacity + cast(isize)(rest / array->item_size);

        // Array doesn't get resized
        if (array_new_capacity == array->capacity) {
            return false;
        }
        array_new_size = cast(usize) array_new_capacity * array->item_size;
    }

    array->data = rwlj_arena_resize(
        array->arena, array->data, array_curr_size, array_new_size
    );
    rwlj_assert(array->data != NULL);

    array->capacity = array_new_capacity;
    if (curr_data != NULL && array->data != curr_data) {
        rwlj_memory_move(
            array->data, curr_data, cast(usize) array->len * array->item_size
        );
    }

    return true;
}

/*
 *  Time
 */

rwljTime
rwlj_time_now(void)
{
    i32 id = 0;

#if defined(CLOCK_MONOTONIC_RAW)
    id = CLOCK_MONOTONIC_RAW;
#elif defined(CLOCK_MONOTONIC)
    id = CLOCK_MONOTONIC;
#else
    rwlj_panic(no_monotonic_clock);
#endif

    struct timespec timespec = { 0 };
    if (clock_gettime(id, &timespec) == 0) {
        return timespec.tv_sec * RWLJ_TIME_SECOND + timespec.tv_nsec;
    }

    return 0;
}

rwljDuration
rwlj_time_diff(rwljTime t1, rwljTime t2)
{
    return t1 - t2;
}

rwljDuration
rwlj_time_since(rwljTime t)
{
    return rwlj_time_diff(rwlj_time_now(), t);
}

i64
rwlj_time_unix_now(void)
{
    struct timespec timespec = { 0 };
    if (clock_gettime(CLOCK_REALTIME, &timespec) == 0) {
        return timespec.tv_sec;
    }

    return 0;
}

rwljDuration
rwlj_time_unix_diff(i64 t1, i64 t2)
{
    return t1 - t2;
}

rwljDuration
rwlj_time_unix_since(i64 t)
{
    return rwlj_time_unix_diff(rwlj_time_unix_now(), t);
}

rwljDuration
rwlj_time_sleep(rwljDuration time)
{
    if (time <= 0) {
        return 0;
    }

    rwljDuration seconds = time / RWLJ_TIME_SECOND;
    rwljDuration nano = time % RWLJ_TIME_SECOND;

    struct timespec timespec = { seconds, nano };
    struct timespec remaining = { 0 };
    if (clock_nanosleep(CLOCK_MONOTONIC, 0, &timespec, &remaining) == 0) {
        return 0;
    }

    return remaining.tv_sec * RWLJ_TIME_SECOND + remaining.tv_nsec;
}

/*
 *  Strings
 */

#define RWLJ_STRING_COMPARE_SENTINEL cast(isize) rwlj_bit(8)
isize
__rwlj_string_compare(rwljString a, rwljString b, bool sensitive)
{
    if (a.len <= 0 || b.len <= 0) {
        isize ret = 0;
        if (a.len <= 0) {
            ret += RWLJ_STRING_COMPARE_SENTINEL;
        }
        if (b.len <= 0) {
            ret -= RWLJ_STRING_COMPARE_SENTINEL;
        }
        return ret;
    }

    if (sensitive) {
        return rwlj_memory_compare(
            a.data, b.data, cast(usize) rwlj_min(a.len, b.len)
        );
    }

    for (isize i = 0; i < a.len && i < b.len; i += 1) {
        char x = rwlj_to_lower(a.data[i]);
        char y = rwlj_to_lower(b.data[i]);
        if (x != y) {
            return x - y;
        }
    }

    return 0;
}

bool
__rwlj_string_are_equal(rwljString a, rwljString b, bool sensitive)
{
    if (a.len != b.len || (__rwlj_string_compare(a, b, sensitive))) {
        return false;
    }

    return true;
}

bool
__rwlj_string_contains(rwljString s, rwljString substr, bool sensitive)
{
    if (substr.len == 0) {
        return true;
    }

    for (isize i = 0; i < s.len; i += 1) {
        if (!__rwlj_string_compare(
                rwlj_string(s, i, s.len), substr, sensitive
            )) {
            return true;
        }
    }
    return false;
}

isize
__rwlj_string_index(rwljString s, rwljString substr, bool sensitive)
{
    if (substr.len == 0) {
        return 0;
    }

    for (isize i = 0; i < s.len; i += 1) {
        if (!__rwlj_string_compare(
                rwlj_string(s, i, s.len), substr, sensitive
            )) {
            return i;
        }
    }
    return -1;
}

isize
__rwlj_string_count(rwljString s, rwljString substr, bool sensitive)
{
    if (substr.len == 0) {
        return s.len + 1;
    }

    isize count = 0;
    for (isize i = 0; i < s.len; i += 1) {
        if (!__rwlj_string_compare(
                rwlj_string(s, i, s.len), substr, sensitive
            )) {
            count += 1;
            i += (substr.len - 1);
        }
    }

    return count;
}

rwljSlice_String
__rwlj_string_split(
    rwljString s,
    rwljString separator,
    rwljArena *arena,
    bool sensitive
)
{
    rwljArray_String parts = { 0 };

    if (s.len == 0 && s.data[0] == '\0') {
        return (rwljSlice_String){ parts.data, parts.len };
    }

    if (separator.len == 0 && separator.data[0] == '\0') {
        rwlj_array_init_dynamic_reserve(&parts, s.len, arena);
        for (isize i = 0; i < s.len; i += 1) {
            rwlj_array_append(&parts, rwlj_string(s, i, i + 1));
        }

        return (rwljSlice_String){ parts.data, parts.len };
    }

    isize n = __rwlj_string_count(s, separator, sensitive) + 1;
    rwlj_array_init_dynamic_reserve(&parts, n, arena);
    if (n == 1) {
        rwlj_array_append(&parts, s);
        return (rwljSlice_String){ parts.data, parts.len };
    }

    isize index = 0;
    isize start = 0;
    for (isize i = 0; i < n; i += 1) {
        isize res = __rwlj_string_index(
            rwlj_string(s, index, s.len), separator, sensitive
        );
        index = res == -1 ? s.len : index + res;
        rwljString part = rwlj_string(s, start, index);
        rwlj_array_append(&parts, part);
        start = index = rwlj_min(index + separator.len, s.len);
    }

    return (rwljSlice_String){ parts.data, parts.len };
}

rwljString
rwlj_string_clone(rwljString s, rwljArena *arena)
{
    rwljString cloned = { 0 };
    cloned.data = rwlj_arena_alloc(arena, cast(usize) s.len);
    if (cloned.data == NULL || s.data == NULL) {
        return cloned;
    }
    cloned.len = s.len;
    rwlj_memory_copy(cloned.data, s.data, cast(usize) s.len);

    return cloned;
}

rwljString
rwlj_string_concatenate(rwljString a, rwljString b, rwljArena *arena)
{
    isize buf_len = a.len + b.len;
    u8 *buf = rwlj_arena_alloc(arena, cast(usize)(buf_len));
    if (buf == NULL) {
        return STRING("");
    }

    isize i = 0;
    for (; i < a.len; i += 1) {
        buf[i] = cast(u8) a.data[i];
    }
    for (isize j = 0; j < b.len && i < buf_len; j += 1, i += 1) {
        buf[i] = cast(u8) b.data[j];
    }

    return rwlj_string_from_ptr(buf, 0, buf_len);
}

rwljString
rwlj_string_reverse(rwljString s, rwljArena *arena)
{
    rwlj_unused(s);
    rwlj_unused(arena);
    rwlj_not_implemented();
}

// C string procedures

char *
rwlj_cstring_from_string(rwljString s, rwljArena *arena)
{
    usize buf_size = cast(usize) s.len + 1;

    // NOTE: Alloc already memzeroes the buffer
    u8 *buf = rwlj_arena_alloc(arena, buf_size);
    rwlj_memory_copy(buf, s.data, buf_size - 1);

    return cast(char *) buf;
}

isize
rwlj_cstring_len(char const *string, isize max_len)
{
    char *s = cast(char *) string;
    isize len = 0;

    while (s[0] && len < max_len) {
        len += 1;
        s = &s[1];
    }

    return len;
}

// Formatting procudures

isize
rwlj_write_i64(rwljSlice_U8 buf, i64 number)
{
    i64 base = 10;
    isize bytes_written = 0;

    isize start = 0;
    if (number < 0) {
        if (rwlj_slice_set(&buf, bytes_written, '-')) {
            bytes_written += 1;
            start = bytes_written;
        }

        // Sanitizer complains of overflow
        if (number == RWLJ_I64_MIN) {
            buf.data[bytes_written] = cast(u8)(rwlj_abs(number % base) + '0');
            number /= base;
            bytes_written += 1;
        }
    }

    i64 n = rwlj_abs(number);
    do {
        buf.data[bytes_written] = cast(u8)(n % base + '0');
        n /= base;
        bytes_written += 1;
    } while (bytes_written < buf.len && n > 0);

    rwljSlice_U8 slice = rwlj_slice(buf.data, start, bytes_written);
    rwlj_slice_reverse(&slice, u8);

    return bytes_written;
}

isize
rwlj_write_u64(rwljSlice_U8 buf, u64 number, u8 fmt)
{
    isize bytes_written = 0;
    usize base = 10;
    switch (fmt) {
    case 'b':
        base = 2;
        break;
    case 'o':
        base = 8;
        break;
    case 'u':
        break;
    case 'X':
    case 'x':
        base = 16;
        break;
    default:
        rwlj_not_implemented();
    }
    isize start = bytes_written;

    if (base == 10) {
        do {
            buf.data[bytes_written] = cast(u8)(number % 10 + '0');
            number /= 10;
            bytes_written += 1;
        } while (bytes_written < buf.len && number > 0);
    } else if (base == 16) {
        char *digit_table =
            fmt != 'X' ? "0123456789abcdef" : "0123456789ABCDEF";
        usize mask = base - 1;

        do {
            buf.data[bytes_written] = cast(u8) digit_table[number & mask];
            number >>= 4;
            bytes_written += 1;
        } while (bytes_written < buf.len && number > 0);
    } else {
        usize mask = base - 1;
        usize shift = base == 8 ? 3 : 1;

        do {
            buf.data[bytes_written] = cast(u8)((number & mask) + '0');
            number >>= shift;
            bytes_written += 1;
        } while (bytes_written < buf.len && number > 0);
    }

    rwljSlice_U8 slice = rwlj_slice(buf.data, start, bytes_written);
    rwlj_slice_reverse(&slice, u8);

    return bytes_written;
}

// This whole floating-point section was adapted from
// https://github.com/renatowljacob/ftoa.h which is also an adaptation of stb's
// sprintf float code (https://github.com/nothings/stb)

#define __RWLJ_LEFTJUST       1
#define __RWLJ_LEADINGPLUS    2
#define __RWLJ_LEADINGSPACE   4
#define __RWLJ_LEADINGZERO    16
#define __RWLJ_TRIPLET_COMMA  64
#define __RWLJ_NEGATIVE       128
#define __RWLJ_METRIC_SUFFIX  256
#define __RWLJ_METRIC_NOSPACE 1024
#define __RWLJ_METRIC_1024    2048
#define __RWLJ_METRIC_JEDEC   4096

// define this before inclusion to force stbsp_sprintf to always use aligned
// accesses
#ifdef __RWLJ_SPRINTF_NOUNALIGNED
#define __RWLJ_UNALIGNED(code)
#else
#define __RWLJ_UNALIGNED(code) code
#endif

#define __RWLJ_SPECIAL 0x7000

struct {
    i16 temp; // force next field to be 2-byte aligned
    char pair[201];
} __rwlj_digitpair = { 0,
                       "00010203040506070809101112131415161718192021222324"
                       "25262728293031323334353637383940414243444546474849"
                       "50515253545556575859606162636465666768697071727374"
                       "75767778798081828384858687888990919293949596979899" };

f64 const __rwlj_bot[23] = { 1e+000, 1e+001, 1e+002, 1e+003, 1e+004, 1e+005,
                             1e+006, 1e+007, 1e+008, 1e+009, 1e+010, 1e+011,
                             1e+012, 1e+013, 1e+014, 1e+015, 1e+016, 1e+017,
                             1e+018, 1e+019, 1e+020, 1e+021, 1e+022 };
f64 const __rwlj_negbot[22] = { 1e-001, 1e-002, 1e-003, 1e-004, 1e-005, 1e-006,
                                1e-007, 1e-008, 1e-009, 1e-010, 1e-011, 1e-012,
                                1e-013, 1e-014, 1e-015, 1e-016, 1e-017, 1e-018,
                                1e-019, 1e-020, 1e-021, 1e-022 };
f64 const __rwlj_negboterr[22] = {
    -5.551115123125783e-018,  -2.0816681711721684e-019,
    -2.0816681711721686e-020, -4.7921736023859299e-021,
    -8.1803053914031305e-022, 4.5251888174113741e-023,
    4.5251888174113739e-024,  -2.0922560830128471e-025,
    -6.2281591457779853e-026, -3.6432197315497743e-027,
    6.0503030718060191e-028,  2.0113352370744385e-029,
    -3.0373745563400371e-030, 1.1806906454401013e-032,
    -7.7705399876661076e-032, 2.0902213275965398e-033,
    -7.1542424054621921e-034, -7.1542424054621926e-035,
    2.4754073164739869e-036,  5.4846728545790429e-037,
    9.2462547772103625e-038,  -4.8596774326570872e-039
};
f64 const __rwlj_top[13] = { 1e+023, 1e+046, 1e+069, 1e+092, 1e+115,
                             1e+138, 1e+161, 1e+184, 1e+207, 1e+230,
                             1e+253, 1e+276, 1e+299 };
f64 const __rwlj_negtop[13] = { 1e-023, 1e-046, 1e-069, 1e-092, 1e-115,
                                1e-138, 1e-161, 1e-184, 1e-207, 1e-230,
                                1e-253, 1e-276, 1e-299 };
f64 const __rwlj_toperr[13] = { 8388608,
                                6.8601809640529717e+028,
                                -7.253143638152921e+052,
                                -4.3377296974619174e+075,
                                -1.5559416129466825e+098,
                                -3.2841562489204913e+121,
                                -3.7745893248228135e+144,
                                -1.7356668416969134e+167,
                                -3.8893577551088374e+190,
                                -9.9566444326005119e+213,
                                6.3641293062232429e+236,
                                -5.2069140800249813e+259,
                                -5.2504760255204387e+282 };
f64 const __rwlj_negtoperr[13] = {
    3.9565301985100693e-040,  -2.299904345391321e-063,
    3.6506201437945798e-086,  1.1875228833981544e-109,
    -5.0644902316928607e-132, -6.7156837247865426e-155,
    -2.812077463003139e-178,  -5.7778912386589953e-201,
    7.4997100559334532e-224,  -4.6439668915134491e-247,
    -6.3691100762962136e-270, -9.436808465446358e-293,
    8.0970921678014997e-317
};

#if defined(_MSC_VER) && (_MSC_VER <= 1200)
u64 const __rwlj_powten[20] = { 1,
                                10,
                                100,
                                1000,
                                10000,
                                100000,
                                1000000,
                                10000000,
                                100000000,
                                1000000000,
                                10000000000,
                                100000000000,
                                1000000000000,
                                10000000000000,
                                100000000000000,
                                1000000000000000,
                                10000000000000000,
                                100000000000000000,
                                1000000000000000000,
                                10000000000000000000u };
#define __rwlj_tento19th (cast(u64) 1000000000000000000)
#else
u64 const __rwlj_powten[20] = { 1,
                                10,
                                100,
                                1000,
                                10000,
                                100000,
                                1000000,
                                10000000,
                                100000000,
                                1000000000,
                                10000000000ull,
                                100000000000ull,
                                1000000000000ull,
                                10000000000000ull,
                                100000000000000ull,
                                1000000000000000ull,
                                10000000000000000ull,
                                100000000000000000ull,
                                1000000000000000000ull,
                                10000000000000000000ull };
#define __rwlj_tento19th (1000000000000000000ull)
#endif

#define __rwlj_ddmulthi(oh, ol, xh, yh)                                        \
    {                                                                          \
        oh = xh * yh;                                                          \
        i64 bt = transmute(xh, f64, i64);                                      \
        bt &= cast(i64)((~0llu) << 27);                                        \
        f64 ahi = transmute(bt, i64, f64);                                     \
        f64 alo = (xh) - ahi;                                                  \
        bt = transmute(yh, f64, i64);                                          \
        bt &= cast(i64)((~0llu) << 27);                                        \
        f64 bhi = transmute(bt, i64, f64);                                     \
        f64 blo = (yh) - bhi;                                                  \
        ol = ((ahi * bhi - (oh)) + ahi * blo + alo * bhi) + alo * blo;         \
    }

#define __rwlj_ddtoS64(ob, xh, xl)                                             \
    {                                                                          \
        ob = cast(i64) xh;                                                     \
        f64 vh = cast(f64) ob;                                                 \
        f64 ahi = ((xh) - vh);                                                 \
        f64 t = (ahi - (xh));                                                  \
        f64 alo = ((xh) - (ahi - t)) - (vh + t);                               \
        ob += cast(i64)(ahi + alo + (xl));                                     \
    }

#define __rwlj_ddrenorm(oh, ol)                                                \
    {                                                                          \
        f64 s = (oh) + (ol);                                                   \
        ol = (ol) - (s - (oh));                                                \
        oh = s;                                                                \
    }

#define __rwlj_ddmultlo(oh, ol, xh, xl, yh, yl)                                \
    ol = (ol) + ((xh) * (yl) + (xl) * (yh));

#define __rwlj_ddmultlos(oh, ol, xh, yl) ol = (ol) + ((xh) * (yl));

internal void
__rwlj_raise_to_power10(
    f64 *ohi,
    f64 *olo,
    f64 d,
    i64 power
) // power can be -323 to +350
{
    f64 ph = 0;
    f64 pl = 0;

    if ((power >= 0) && (power <= 22)) {
        __rwlj_ddmulthi(ph, pl, d, __rwlj_bot[power]);
    } else {
        f64 p2h = 0;
        f64 p2l = 0;

        i64 e = power;
        if (power < 0) {
            e = -e;
        }
        i64 et = (e * 0x2c9) >> 14; /* %23 */
        if (et > 13) {
            et = 13;
        }
        i64 eb = e - (et * 23);

        ph = d;
        pl = 0.0;
        if (power < 0) {
            if (eb) {
                eb -= 1;
                __rwlj_ddmulthi(ph, pl, d, __rwlj_negbot[eb]);
                __rwlj_ddmultlos(ph, pl, d, __rwlj_negboterr[eb]);
            }
            if (et) {
                __rwlj_ddrenorm(ph, pl);
                et -= 1;
                __rwlj_ddmulthi(p2h, p2l, ph, __rwlj_negtop[et]);
                __rwlj_ddmultlo(
                    p2h, p2l, ph, pl, __rwlj_negtop[et], __rwlj_negtoperr[et]
                );
                ph = p2h;
                pl = p2l;
            }
        } else {
            if (eb) {
                e = eb;
                if (eb > 22) {
                    eb = 22;
                }
                e -= eb;
                __rwlj_ddmulthi(ph, pl, d, __rwlj_bot[eb]);
                if (e) {
                    __rwlj_ddrenorm(ph, pl);
                    __rwlj_ddmulthi(p2h, p2l, ph, __rwlj_bot[e]);
                    __rwlj_ddmultlos(p2h, p2l, __rwlj_bot[e], pl);
                    ph = p2h;
                    pl = p2l;
                }
            }
            if (et) {
                __rwlj_ddrenorm(ph, pl);
                et -= 1;
                __rwlj_ddmulthi(p2h, p2l, ph, __rwlj_top[et]);
                __rwlj_ddmultlo(
                    p2h, p2l, ph, pl, __rwlj_top[et], __rwlj_toperr[et]
                );
                ph = p2h;
                pl = p2l;
            }
        }
    }
    __rwlj_ddrenorm(ph, pl);
    *ohi = ph;
    *olo = pl;
}

internal i64
__rwlj_real_to_parts(i64 *bits, i64 *expo, f64 value)
{
    // load value and round at the frac_digits
    f64 d = value;
    i64 b = transmute(d, f64, i64);

    *bits = b & cast(i64)((1llu << 52) - 1);
    *expo = ((b >> 52) & 2047) - 1023;

    return b >> 63;
}

internal i32
__rwlj_real_to_str(
    char const **start,
    i64 *len,
    char *out,
    i64 *decimal_pos,
    f64 value,
    u32 frac_digits
)
{
    f64 d = value;
    i64 bits = transmute(d, f64, i64);
    i64 expo = (bits >> 52) & 2047;
    i32 is_negative = cast(i32)(cast(u64) bits >> 63);

    if (is_negative) {
        d = -d;
    }

    if (expo == 2047) // is nan or inf?
    {
        *start = (cast(u64) bits & (((1llu) << 52) - 1)) ? "NaN" : "Inf";
        *decimal_pos = __RWLJ_SPECIAL;
        *len = 3;

        return is_negative;
    }

    if (expo == 0) // is zero or denormal
    {
        if ((cast(u64) bits << 1) == 0) // do zero
        {
            *decimal_pos = 1;
            *start = out;
            out[0] = '0';
            *len = 1;

            return is_negative;
        }
        // find the right expo for denormals
        {
            i64 v = (1llu) << 51;
            while ((bits & v) == 0) {
                --expo;
                v >>= 1;
            }
        }
    }

    // find the decimal exponent as well as the decimal bits of the value
    i64 tens = 0;
    {
        // log10 estimate - very specifically tweaked to hit or undershoot by no
        // more than 1 of log10 of all expos 1..2046
        tens = expo - 1023;
        tens =
            (tens < 0) ? ((tens * 617) / 2048) : (((tens * 1233) / 4096) + 1);

        // move the significant bits into position and stick them into an int
        f64 ph = 0;
        f64 pl = 0;
        __rwlj_raise_to_power10(&ph, &pl, d, 18 - tens);

        // get full as much precision from double-double as possible
        __rwlj_ddtoS64(bits, ph, pl);

        // check if we undershot
        if ((cast(u64) bits) >= __rwlj_tento19th) {
            tens += 1;
        }
    }

    // now do the rounding in integer land
    i64 e = 0;
    frac_digits = (frac_digits & 0x80000000) ? ((frac_digits & 0x7ffffff) + 1)
                                             : (cast(u32) tens + frac_digits);
    if ((frac_digits < 24)) {
        u32 dg = 1;
        if (cast(u64) bits >= __rwlj_powten[9]) {
            dg = 10;
        }

        while (cast(u64) bits >= __rwlj_powten[dg]) {
            dg += 1;
            if (dg == 20) {
                goto noround;
            }
        }

        if (frac_digits < dg) {
            // add 0.5 at the right position and round
            e = cast(i32)(dg - frac_digits);
            if (cast(u32) e >= 24) {
                goto noround;
            }
            u64 r = __rwlj_powten[e];
            bits = bits + cast(i64)(r / 2);
            if (cast(u64) bits >= __rwlj_powten[dg]) {
                tens += 1;
            }
            bits = cast(i64)(cast(u64) bits / r);
        }
    noround:;
    }

    // kill long trailing runs of zeros
    if (bits) {
        while (true) {
            if (bits <= 0xffffffff) {
                break;
            }
            if (bits % 1000) {
                goto donez;
            }
            bits /= 1000;
        }
        u32 n = cast(u32) bits;
        while ((n % 1000) == 0) {
            n /= 1000;
        }
        bits = n;
    donez:;
    }

    // convert to string
    out += 64;
    e = 0;
    while (true) {
        u32 n = 0;
        char *o = out - 8;
        // do the conversion in chunks of U32s (avoid most 64-bit divides, worth
        // it, constant denomiators be damned)
        if (bits >= 100000000) {
            n = cast(u32)(bits % 100000000);
            bits /= 100000000;
        } else {
            n = cast(u32) bits;
            bits = 0;
        }
        while (n) {
            out -= 2;
            *(u16 *)out = *(u16 *)&__rwlj_digitpair.pair[(n % 100) * 2];
            n /= 100;
            e += 2;
        }
        if (bits == 0) {
            if (e && out[0] == '0') {
                out += 1;
                e -= 1;
            }
            break;
        }
        while (out != o) {
            out -= 1;
            *out = '0';
            e += 1;
        }
    }

    *decimal_pos = tens;
    *start = out;
    *len = cast(u32) e;
    return is_negative;
}

internal void
__rwlj_lead_sign(u32 fl, char *sign)
{
    sign[0] = 0;
    if (fl & __RWLJ_NEGATIVE) {
        sign[0] = 1;
        sign[1] = '-';
    } else if (fl & __RWLJ_LEADINGSPACE) {
        sign[0] = 1;
        sign[1] = ' ';
    } else if (fl & __RWLJ_LEADINGPLUS) {
        sign[0] = 1;
        sign[1] = '+';
    }
}

// TODO: suppress ASAN diagnostic

isize
rwlj_write_f64(rwljSlice_U8 buf, f64 number, u8 fmt, i64 precision)
{
    persistent char hex[] = "0123456789abcdefxp";
    persistent char hexu[] = "0123456789ABCDEFXP";

    char *bf = cast(char *) buf.data;

    isize remaining_whitespace = 0;
    isize prec = precision != -1 ? precision : 6;
    u32 flags = 0;
    isize trailing_zeros = 0;

    switch (cast(enum rwljFloat_Class) rwlj_classify(number)) {
    case RWLJ_FLOAT_CLASS_INFINITY: {
        rwljString s = STRING("nan");
        return rwlj_write_string(buf, s);
    }
    case RWLJ_FLOAT_CLASS_NAN: {
        rwljString s = STRING("inf");
        return rwlj_write_string(buf, s);
    }
    case RWLJ_FLOAT_CLASS_SUBNORMAL:
    case RWLJ_FLOAT_CLASS_NORMAL:
        break;
    }

    // Use our own buffer
#define __RWLJ_NUMSZ 512 // big enough for e308 (with commas) or e-307
    char num[__RWLJ_NUMSZ];
    char lead[8] = { 0 };
    char tail[8] = { 0 };

    char *s = NULL;
    char const *h = NULL;
    char const *sn = NULL;

    isize len = 0;
    isize n = 0;
    u32 cs = 0;
    u64 n64 = 0;

    isize decimal_point = 0;
    // handle each replacement
    switch (fmt) {

    case 'A': // hex float
    case 'a': // hex float
        h = (fmt == 'A') ? hexu : hex;
        // read the double into a string
        if (__rwlj_real_to_parts(cast(i64 *) & n64, &decimal_point, number))
            flags |= __RWLJ_NEGATIVE;

        s = num + 64;

        __rwlj_lead_sign(flags, lead);

        if (decimal_point == -1023) {
            decimal_point = (n64) ? -1022 : 0;
        } else {
            n64 |= ((1lu) << 52);
        }
        n64 <<= (64 - 56);
        if (prec < 15) {
            n64 += ((8lu << 56) >> (prec * 4));
        }
        // add leading chars

        // Take these things off, we do it outside ftoa
#ifdef STB_SPRINTF_MSVC_MODE
        *s++ = '0';
        *s++ = 'x';
#else
        lead[1 + lead[0]] = '0';
        lead[2 + lead[0]] = 'x';
        lead[0] += 2;
#endif
        *s = h[(n64 >> 60) & 15];
        s += 1;
        n64 <<= 4;
        if (prec) {
            *s = '.';
            s += 1;
        }
        sn = s;

        // print the bits
        n = prec;
        if (n > 13) {
            n = 13;
        }
        if (prec > n) {
            trailing_zeros = prec - n;
        }
        prec = 0;
        while (n) {
            n -= 1;
            *s = h[(n64 >> 60) & 15];
            s += 1;
            n64 <<= 4;
        }

        // print the expo
        tail[1] = h[17];
        if (decimal_point < 0) {
            tail[2] = '-';
            decimal_point = -decimal_point;
        } else {
            tail[2] = '+';
        }
        n = decimal_point >= 1000
                ? 6
                : (decimal_point >= 100 ? 5 : (decimal_point >= 10 ? 4 : 3));
        tail[0] = cast(char) n;
        while (true) {
            tail[n] = cast(char)('0' + decimal_point % 10);
            if (n <= 3) {
                break;
            }
            n -= 1;
            decimal_point /= 10;
        }

        decimal_point = cast(isize)(s - sn);
        len = cast(isize)(s - (num + 64));
        s = num + 64;
        cs = 1 + (3 << 24);
        goto scopy;

    case 'G': // float
    case 'g': // float
        h = (fmt == 'G') ? hexu : hex;

        // default is 6
        if (prec == 0) {
            prec = 1;
        }
        // read the double into a string
        if (__rwlj_real_to_str(
                &sn,
                &len,
                num,
                &decimal_point,
                number,
                cast(u32)((prec - 1) | 0x80000000)
            ))
            flags |= __RWLJ_NEGATIVE;

        // clamp the precision and delete extra zeros after clamp
        n = prec;
        if (len > prec) {
            len = prec;
        }
        while (len > 1 && prec && sn[len - 1] == '0') {
            prec -= 1;
            len -= 1;
        }

        // should we use %e
        if (decimal_point <= -4 || decimal_point > n) {
            if (prec > len) {
                prec = len - 1;
            } else if (prec) {
                prec -=
                    1; // when using %e, there is one digit before the decimal
            }
            goto doexpfromg;
        }
        // this is the insane action to get the pr to match %g semantics for %f
        if (decimal_point > 0) {
            prec = (decimal_point < len) ? len - decimal_point : 0;
        } else {
            prec = -decimal_point + ((prec > len) ? len : prec);
        }
        goto dofloatfromg;

    case 'E': // float
    case 'e': // float
        h = (fmt == 'E') ? hexu : hex;
        // read the double into a string
        if (__rwlj_real_to_str(
                &sn,
                &len,
                num,
                &decimal_point,
                number,
                cast(u32)(prec | 0x80000000)
            )) {
            flags |= __RWLJ_NEGATIVE;
        }
    doexpfromg:
        tail[0] = 0;
        __rwlj_lead_sign(flags, lead);
        if (decimal_point == __RWLJ_SPECIAL) {
            s = cast(char *) sn;
            cs = 0;
            prec = 0;
            goto scopy;
        }
        s = num + 64;
        // handle leading chars
        *s = sn[0];
        s += 1;

        if (prec) {
            *s = '.';
            s += 1;
        }

        // handle after decimal
        if (len - 1 > prec) {
            len = prec + 1;
        }
        for (n = 1; n < len; n++) {
            *s = sn[n];
            s += 1;
        }
        trailing_zeros = prec - (len - 1);
        prec = 0;
        // dump expo
        tail[1] = h[0xe];
        decimal_point -= 1;
        if (decimal_point < 0) {
            tail[2] = '-';
            decimal_point = -decimal_point;
        } else {
            tail[2] = '+';
        }
#ifdef STB_SPRINTF_MSVC_MODE
        n = 5;
#else
        n = (decimal_point >= 100) ? 5 : 4;
#endif
        tail[0] = cast(char) n;
        while (true) {
            tail[n] = cast(char)('0' + decimal_point % 10);
            if (n <= 3) {
                break;
            }
            n -= 1;
            decimal_point /= 10;
        }
        cs = 1 + (3 << 24); // how many tens
        goto flt_lead;

    case 'f': // float
    doafloat:
        // do kilos
        if (flags & __RWLJ_METRIC_SUFFIX) {
            f64 divisor = 1000.0;
            if (flags & __RWLJ_METRIC_1024) {
                divisor = 1024.0;
            }
            while (flags < 0x4000000) {
                if ((number < divisor) && (number > -divisor)) {
                    break;
                }
                number /= divisor;
                flags += 0x1000000;
            }
        }
        // read the double into a string
        if (__rwlj_real_to_str(
                &sn, &len, num, &decimal_point, number, cast(u32) prec
            )) {
            flags |= __RWLJ_NEGATIVE;
        }
    dofloatfromg:
        tail[0] = 0;
        __rwlj_lead_sign(flags, lead);
        if (decimal_point == __RWLJ_SPECIAL) {
            s = cast(char *) sn;
            cs = 0;
            prec = 0;
            goto scopy;
        }
        s = num + 64;

        // handle the three decimal varieties
        if (decimal_point <= 0) {
            // handle 0.000*000xxxx
            *s = '0';
            s += 1;
            if (prec) {
                *s = '.';
                s += 1;
            }

            n = -decimal_point;
            if (n > prec) {
                n = prec;
            }
            isize i = n;
            while (i) {
                if (((cast(uintptr) s) & 3) == 0) {
                    break;
                }
                *s = '0';
                s += 1;
                i -= 1;
            }
            while (i >= 4) {
                *cast(u32 *) s = 0x30303030;
                s += 4;
                i -= 4;
            }
            while (i) {
                *s = '0';
                s += 1;
                i -= 1;
            }
            if (len + n > prec) {
                len = prec - n;
            }
            i = len;
            while (i) {
                *s = *sn;
                s += 1;
                sn += 1;
                i -= 1;
            }
            trailing_zeros = prec - (n + len);
            cs = 1 + (3 << 24); // how many tens did we write (for commas below)
        } else {
            cs = (flags & __RWLJ_TRIPLET_COMMA)
                     ? cast(u32)((600 - decimal_point) % 3)
                     : 0;
            if (decimal_point >= len) {
                // handle xxxx000*000.0
                n = 0;
                while (true) {
                    cs += 1;
                    if ((flags & __RWLJ_TRIPLET_COMMA) && (cs == 4)) {
                        cs = 0;
                        *s = ',';
                        s += 1;
                    } else {
                        *s = sn[n];
                        s += 1;
                        n += 1;
                        if (n >= len) {
                            break;
                        }
                    }
                }
                if (n < decimal_point) {
                    n = decimal_point - n;
                    if ((flags & __RWLJ_TRIPLET_COMMA) == 0) {
                        while (n) {
                            if (((cast(uintptr) s) & 3) == 0) {
                                break;
                            }

                            *s = '0';
                            s += 1;
                            n -= 1;
                        }
                        while (n >= 4) {
                            *cast(u32 *) s = 0x30303030;
                            s += 4;
                            n -= 4;
                        }
                    }
                    while (n) {
                        cs += 1;
                        if ((flags & __RWLJ_TRIPLET_COMMA) && (cs == 4)) {
                            cs = 0;
                            *s = ',';
                            s += 1;
                        } else {
                            *s = '0';
                            s += 1;
                            n -= 1;
                        }
                    }
                }
                // cs is how many tens
                cs = cast(u32)(s - (num + 64)) + (3 << 24);
                if (prec) {
                    *s = '.';
                    s += 1;
                    trailing_zeros = prec;
                }
            } else {
                // handle xxxxx.xxxx000*000
                n = 0;
                while (true) {
                    cs += 1;
                    if ((flags & __RWLJ_TRIPLET_COMMA) && (cs == 4)) {
                        cs = 0;
                        *s = ',';
                        s += 1;
                    } else {
                        *s = sn[n];
                        s += 1;
                        n += 1;
                        if (n >= decimal_point) {
                            break;
                        }
                    }
                }
                // cs is how many tens
                cs = cast(u32)(s - (num + 64)) + (3 << 24);
                if (prec) {
                    *s = '.';
                    s += 1;
                }
                if ((len - decimal_point) > prec) {
                    len = prec + decimal_point;
                }
                while (n < len) {
                    *s = sn[n];
                    s += 1;
                    n += 1;
                }
                trailing_zeros = prec - (len - decimal_point);
            }
        }
        prec = 0;

        // handle k,m,g,t
        if (flags & __RWLJ_METRIC_SUFFIX) {
            char idx = 1;
            if (flags & __RWLJ_METRIC_NOSPACE) {
                idx = 0;
            }
            tail[0] = idx;
            tail[1] = ' ';
            {
                // SI kilo is 'k', JEDEC and SI kibits are 'K'.
                if (flags >> 24) {
                    if (flags & __RWLJ_METRIC_1024) {
                        tail[idx + 1] = "_KMGT"[flags >> 24];
                    } else {
                        tail[idx + 1] = "_kMGT"[flags >> 24];
                    }
                    idx += 1;
                    // If printing kibits and not in jedec, add the 'i'.
                    if (flags & __RWLJ_METRIC_1024 &&
                        !(flags & __RWLJ_METRIC_JEDEC)) {
                        tail[idx + 1] = 'i';
                        idx += 1;
                    }
                    tail[0] = idx;
                }
            }
        };

    flt_lead:
        // get the length that we copied
        len = s - (num + 64);
        s = num + 64;
        goto scopy;

        if (flags & __RWLJ_METRIC_SUFFIX) {
            if (n64 < 1024) {
                prec = 0;
            } else if (prec == -1) {
                prec = 1;
            }
            number = cast(f64) cast(i64) n64;
            goto doafloat;
        }

        // convert to string
        s = num + __RWLJ_NUMSZ;
        len = 0;

        while (true) {
            // do in 32-bit chunks (avoid lots of 64-bit divides even with
            // constant denominators)
            char *o = s - 8;
            if (n64 >= 100000000) {
                n = cast(isize)(n64 % 100000000);
                n64 /= 100000000;
            } else {
                n = cast(isize) n64;
                n64 = 0;
            }
            if ((flags & __RWLJ_TRIPLET_COMMA) == 0) {
                do {
                    s -= 2;
                    *cast(u16 *) s =
                        *cast(u16 *) & __rwlj_digitpair.pair[(n % 100) * 2];
                    n /= 100;
                } while (n);
            }
            while (n) {
                if ((flags & __RWLJ_TRIPLET_COMMA) && (len++ == 3)) {
                    len = 0;
                    s -= 1;
                    *s = ',';
                    o -= 1;
                } else {
                    s -= 1;
                    *s = cast(char)(n % 10) + '0';
                    n /= 10;
                }
            }
            if (n64 == 0) {
                if ((s[0] == '0') && (s != (num + __RWLJ_NUMSZ))) {
                    s += 1;
                }
                break;
            }
            while (s != o) {
                if ((flags & __RWLJ_TRIPLET_COMMA) && (len++ == 3)) {
                    len = 0;
                    s -= 1;
                    *s = ',';
                    o -= 1;
                } else {
                    s -= 1;
                    *s = '0';
                }
            }
        }

        tail[0] = 0;
        __rwlj_lead_sign(flags, lead);

        // get the length that we copied
        len = (num + __RWLJ_NUMSZ) - s;
        if (len == 0) {
            s -= 1;
            *s = '0';
            len = 1;
        }
        cs = cast(u32)(len + (3 << 24));
        if (prec < 0) {
            prec = 0;
        }

    scopy:
        // get remaining_whitespace=leading/trailing space, prec=leading zeros
        if (prec < len) {
            prec = len;
        }
        n = prec + lead[0] + tail[0] + trailing_zeros;
        if (remaining_whitespace < n) {
            remaining_whitespace = n;
        }
        remaining_whitespace -= n;
        prec -= len;

        // handle right justify and leading zeros
        if ((flags & __RWLJ_LEFTJUST) == 0) {
            // if leading zeros, everything is in pr
            if (flags & __RWLJ_LEADINGZERO) {
                prec =
                    (remaining_whitespace > prec) ? remaining_whitespace : prec;
                remaining_whitespace = 0;
            } else {
                // if no leading zeros, then no commas
                flags &= cast(u32) ~__RWLJ_TRIPLET_COMMA;
            }
        }

        // copy the spaces and/or zeros
        if (remaining_whitespace + prec) {
            isize i = 0;
            u32 c = 0;

            // copy leading spaces (or when doing %8.4d stuff)
            if ((flags & __RWLJ_LEFTJUST) == 0) {
                while (remaining_whitespace > 0) {
                    i = remaining_whitespace;
                    remaining_whitespace -= i;
                    while (i) {
                        if (((cast(uintptr) bf) & 3) == 0) {
                            break;
                        }

                        *bf = ' ';
                        bf += 1;
                        i -= 1;
                    }
                    while (i >= 4) {
                        *cast(u32 *) bf = 0x20202020;
                        bf += 4;
                        i -= 4;
                    }
                    while (i) {
                        *bf = ' ';
                        bf += 1;
                        i -= 1;
                    }
                }
            }

            // copy leader
            sn = lead + 1;
            while (lead[0]) {
                i = lead[0];
                lead[0] -= cast(char) i;
                while (i) {
                    *bf = *sn;
                    bf += 1;
                    sn += 1;
                    i -= 1;
                }
            }

            // copy leading zeros
            c = cs >> 24;
            cs &= 0xffffff;
            cs = (flags & __RWLJ_TRIPLET_COMMA)
                     ? ((c - ((cast(u32) prec + cs) % (c + 1))))
                     : 0;
            while (prec > 0) {
                i = prec;
                prec -= i;
                if ((flags & __RWLJ_TRIPLET_COMMA) == 0) {
                    while (i) {
                        if ((cast(uintptr) bf & 3) == 0) {
                            break;
                        }
                        *bf = '0';
                        bf += 1;
                        i -= 1;
                    }
                    while (i >= 4) {
                        *cast(u32 *) bf = 0x30303030;
                        bf += 4;
                        i -= 4;
                    }
                }
                while (i) {
                    if ((flags & __RWLJ_TRIPLET_COMMA) && (cs++ == c)) {
                        cs = 0;
                        *bf = ',';
                        bf += 1;
                    } else {
                        *bf = '0';
                        bf += 1;
                    }
                    i -= 1;
                }
            }
        }

        // copy leader if there is still one
        sn = lead + 1;
        while (lead[0]) {
            i32 i = lead[0];
            lead[0] -= cast(char) i;
            while (i) {
                *bf = *sn;
                bf += 1;
                sn += 1;
                i -= 1;
            }
        }

        // copy the string
        n = len;
        while (n) {
            isize i = n;
            n -= i;
            __RWLJ_UNALIGNED(while (i >= 4) {
                *cast(u32 volatile *) bf = *cast(u32 volatile *) s;
                bf += 4;
                s += 4;
                i -= 4;
            })
            while (i) {
                *bf = *s;
                bf += 1;
                s += 1;
                i -= 1;
            }
        }

        // copy trailing zeros
        while (trailing_zeros) {
            isize i = trailing_zeros;
            trailing_zeros -= i;
            while (i) {
                if ((cast(uintptr) bf & 3) == 0) {
                    break;
                }
                *bf = '0';
                bf += 1;
                i -= 1;
            }
            while (i >= 4) {
                *cast(u32 *) bf = 0x30303030;
                bf += 4;
                i -= 4;
            }
            while (i) {
                *bf = '0';
                bf += 1;
                i -= 1;
            }
        }

        // copy tail if there is one
        sn = tail + 1;
        while (tail[0]) {
            i32 i = tail[0];
            tail[0] -= (char)i;
            while (i) {
                *bf = *sn;
                bf += 1;
                sn += 1;
                i -= 1;
            }
        }

        // handle the left justify
        if (flags & __RWLJ_LEFTJUST)
            if (remaining_whitespace > 0) {
                while (remaining_whitespace) {
                    isize i = remaining_whitespace;
                    remaining_whitespace -= i;
                    while (i) {
                        if ((cast(uintptr) bf & 3) == 0) break;
                        *bf = ' ';
                        bf += 1;
                        i -= 1;
                    }
                    while (i >= 4) {
                        *cast(u32 *) bf = 0x20202020;
                        bf += 4;
                        i -= 4;
                    }
                    while (i) {
                        i -= 1;
                        *bf = ' ';
                        bf += 1;
                    }
                }
            }
    }

    return cast(isize)(bf - cast(char *) buf.data);
}

isize
rwlj_write_string(rwljSlice_U8 buf, rwljString str)
{
    isize len = rwlj_min(buf.len, str.len);
    if (len == 0 || buf.data == NULL || str.data == NULL) {
        return 0;
    }
    rwlj_memory_copy(buf.data, str.data, cast(usize) len);

    return len;
}

// TODO: document each conversion specifier
internal isize
__rwlj_bprintf_va(
    rwljSlice_U8 buf,
    bool has_new_line,
    char const *fmt,
    va_list ap
)
{
    enum rwljLength_Modifier { RWLJ_INT, RWLJ_CHAR, RWLJ_SHORT, RWLJ_LONG };
    typedef i8 rwljLength_Modifier;
    typedef struct rwljConv_State {
        isize field_width;
        isize precision;
        isize base;
        struct {
            bool alternate;
            bool zero;
            bool minus;
            bool blank;
            bool sign;
        } flags;
        rwljLength_Modifier length_modifier;
    } rwljConv_State;
    rwljConv_State state = { .base = 10, .precision = 6 };

    if (fmt == NULL || buf.data == NULL) {
        return 0;
    }

    char *string = cast(char *) fmt;
    isize bytes_written = 0;
    for (isize i = 0; i < buf.len && bytes_written < buf.len && string[i];
         i += 1) {
        if (string[i] != '%') {
            buf.data[bytes_written] = cast(u8) string[i];
            bytes_written += 1;
            continue;
        }
        if (string[i + 1] == '%') {
            i += 1;
            buf.data[bytes_written] = cast(u8) string[i];
            bytes_written += 1;
            continue;
        }
        i += 1;

        // Parse flags
        for (; i < buf.len; i += 1) {
            switch (string[i]) {
            case '#':
                state.flags.alternate = true;
                continue;
            case '0':
                state.flags.zero = !state.flags.minus;
                continue;
            case '-':
                state.flags.minus = true;
                state.flags.zero = false;
                continue;
            case ' ':
                state.flags.blank = !state.flags.sign;
                continue;
            case '+':
                state.flags.sign = true;
                state.flags.blank = false;
                continue;
            }
            break;
        }

        // Parse field width
        for (; i < buf.len && rwlj_is_digit(string[i]); i += 1) {
            if (string[i] == '0' && state.field_width <= 0) {
                continue;
            }
            // Also skip extra zeros before non-zero value
            state.field_width *= 10;
            state.field_width += string[i] - 48;
        }

        // Parse precision
        if (string[i] == '.') {
            i += 1;
            state.precision = 0;

            bool precision_ignored = false;
            if (string[i] == '-') {
                precision_ignored = true;
                i += 1;
            }

            while (precision_ignored && i < buf.len &&
                   rwlj_is_digit(string[i])) {
                i += 1;
            }
            for (; i < buf.len && rwlj_is_digit(string[i]); i += 1) {
                state.precision *= 10;
                state.precision += string[i] - 48;
            }
        }

        // Parse length modifiers
        for (; i < buf.len && rwlj_is_alpha(string[i]); i += 1) {
            switch (string[i]) {
            case 'h':
                state.length_modifier =
                    state.length_modifier == RWLJ_CHAR ? RWLJ_SHORT : RWLJ_CHAR;
                continue;
            case 'l':
                state.length_modifier = RWLJ_LONG;
                continue;
            }
            break;
        }

        // Parse conversion specifiers
        u8 conv_buf[512] = { 0 };
        rwljSlice_U8 conv = rwlj_slice_from_buf(conv_buf);
        isize conv_bytes_written = 0;

        struct {
            i64 i;
            u64 u;
            f64 f;
        } value = { 0 };

        switch (string[i]) {
        case 'd':
        case 'i':
            switch (cast(enum rwljLength_Modifier) state.length_modifier) {
            case RWLJ_INT:
            case RWLJ_CHAR:
            case RWLJ_SHORT:
                value.i = cast(i64) va_arg(ap, int);
                break;
            case RWLJ_LONG:
                value.i = cast(i64) va_arg(ap, long);
                break;
            }

            if (value.i == 0 && state.precision == 0) {
                continue;
            }

            conv_bytes_written = rwlj_write_i64(conv, value.i);

            // Discard the sign, we'll put it ourselves
            if (value.i < 0) {
                conv_bytes_written -= 1;
                conv.data = &conv.data[1];
            }

            break;
        case 'b':
        case 'o':
        case 'u':
        case 'x':
        case 'X':
            switch (cast(enum rwljLength_Modifier) state.length_modifier) {
            case RWLJ_INT:
            case RWLJ_CHAR:
            case RWLJ_SHORT:
                value.u = cast(u64) va_arg(ap, int);
                break;
            case RWLJ_LONG:
                value.u = cast(u64) va_arg(ap, long);
                break;
            }

            if (value.u == 0 && state.precision == 0) {
                continue;
            }

            if (state.flags.alternate && string[i] != 'u') {
                conv.data[0] = '0';
                conv.data[1] = cast(u8) string[i];
                conv_bytes_written += 2;
            }

            conv_bytes_written += rwlj_write_u64(
                cast(rwljSlice_U8)
                    rwlj_slice(conv.data, conv_bytes_written, conv.len),
                value.u,
                cast(u8) string[i]
            );

            break;
        case 'A':
        case 'a':
        case 'G':
        case 'g':
        case 'E':
        case 'e':
        case 'f':
            value.f = va_arg(ap, f64);
            conv_bytes_written = rwlj_write_f64(
                conv, value.f, cast(u8) string[i], state.precision
            );

            // Discard the sign, we'll put it ourselves
            if (value.f < 0) {
                conv_bytes_written -= 1;
                conv.data = &conv.data[1];
            }

            break;
        case 'p': {
            void *addr = va_arg(ap, void *);
            if (addr == NULL) {
                bytes_written += rwlj_write_string(
                    cast(rwljSlice_U8)
                        rwlj_slice(buf.data, bytes_written, buf.len),
                    STRING("<nil>")
                );

                continue;
            }
            value.u = cast(uintptr) addr;

            u8 ptr_fmt = 'x';
            conv.data[0] = '0';
            conv.data[1] = ptr_fmt;
            conv_bytes_written += 2;
            conv_bytes_written += rwlj_write_u64(
                (rwljSlice_U8)
                    rwlj_slice(conv.data, conv_bytes_written, conv.len),
                value.u,
                ptr_fmt
            );

            break;
        }
        case 's': {
            char *s = va_arg(ap, char *);
            if (s == NULL) {
                continue;
            }

            rwljString str = rwlj_string_from_ptr(
                s, 0, rwlj_cstring_len(s, buf.len - bytes_written)
            );
            bytes_written += rwlj_write_string(
                (rwljSlice_U8)rwlj_slice(buf.data, bytes_written, buf.len), str
            );

            continue;
        }
        case 'S': {
            rwljString s = va_arg(ap, rwljString);
            bytes_written += rwlj_write_string(
                (rwljSlice_U8)rwlj_slice(buf.data, bytes_written, buf.len), s
            );

            continue;
        }
        case 't': {
            rwljString boolean =
                va_arg(ap, int) ? STRING("true") : STRING("false");
            bytes_written += rwlj_write_string(
                (rwljSlice_U8)rwlj_slice(buf.data, bytes_written, buf.len),
                boolean
            );

            continue;
        }
        case 'c':
            value.i = va_arg(ap, int);
            if (rwlj_slice_set(&buf, bytes_written, cast(u8) value.i)) {
                bytes_written += 1;
            }

            continue;
        }

        // Leading chars
        u8 leading = (value.i < 0 || value.f < 0) ? '-'
                     : state.flags.sign           ? '+'
                     : state.flags.blank          ? ' '
                                                  : 0;
        if (leading) {
            if (rwlj_slice_set(&buf, bytes_written, leading)) {
                bytes_written += 1;
                state.field_width -= 1;
            }
        }

        // 0/space padding
        if (state.flags.zero) {
            isize diff = state.field_width - conv_bytes_written;
            if (diff > 0) {
                usize bytes_to_copy =
                    cast(usize) rwlj_min(diff, buf.len - bytes_written);
                rwlj_memory_set(&buf.data[bytes_written], '0', bytes_to_copy);
                bytes_written += cast(isize) bytes_to_copy;
            }
        }

        usize bytes_to_copy =
            cast(usize) rwlj_min(conv_bytes_written, buf.len - bytes_written);
        rwlj_memory_copy(&buf.data[bytes_written], conv.data, bytes_to_copy);
        bytes_written += cast(isize) bytes_to_copy;

        if (state.flags.minus) {
            isize diff = state.field_width - conv_bytes_written;
            if (diff > 0) {
                usize bytes_to_copy =
                    cast(usize) rwlj_min(diff, buf.len - bytes_written);
                rwlj_memory_set(&buf.data[bytes_written], ' ', bytes_to_copy);
                bytes_written += cast(isize) bytes_to_copy;
            }
        }
    }

    if (has_new_line && rwlj_slice_set(&buf, bytes_written, '\n')) {
        bytes_written += 1;
    }

    return bytes_written;
}

isize
rwlj_bprintf(rwljSlice_U8 buf, char const *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    isize len = __rwlj_bprintf_va(buf, false, fmt, ap);
    va_end(ap);

    return len;
}

isize
rwlj_bprintfln(rwljSlice_U8 buf, char const *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    isize len = __rwlj_bprintf_va(buf, true, fmt, ap);
    va_end(ap);

    return len;
}

internal isize
__rwlj_fprintf_va(
    rwljFile_Descriptor fd,
    bool has_new_line,
    char const *fmt,
    va_list ap
)
{
    u8 buf[4096] = { 0 };
    rwljSlice_U8 slice = rwlj_slice_from_buf(buf);
    isize len = __rwlj_bprintf_va(slice, has_new_line, fmt, ap);

    return write(fd, slice.data, cast(usize) len);
}

isize
rwlj_fprintf(rwljFile_Descriptor fd, char const *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    isize len = __rwlj_fprintf_va(fd, false, fmt, ap);
    va_end(ap);

    return len;
}

isize
rwlj_fprintfln(rwljFile_Descriptor fd, char const *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    isize len = __rwlj_fprintf_va(fd, true, fmt, ap);
    va_end(ap);

    return len;
}

isize
rwlj_eprintf(char const *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    isize len = __rwlj_fprintf_va(RWLJ_STDERR, false, fmt, ap);
    va_end(ap);

    return len;
}

isize
rwlj_eprintfln(char const *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    isize len = __rwlj_fprintf_va(RWLJ_STDERR, true, fmt, ap);
    va_end(ap);

    return len;
}

isize
rwlj_printf(char const *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    isize len = __rwlj_fprintf_va(RWLJ_STDOUT, false, fmt, ap);
    va_end(ap);

    return len;
}

isize
rwlj_printfln(char const *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    isize len = __rwlj_fprintf_va(RWLJ_STDOUT, true, fmt, ap);
    va_end(ap);

    return len;
}

isize
rwlj_bprint(rwljSlice_U8 buf, rwljString s)
{
    return rwlj_write_string(buf, s);
}

isize
rwlj_bprintln(rwljSlice_U8 buf, rwljString s)
{
    isize bytes_written = rwlj_write_string(buf, s);
    if (rwlj_slice_set(&buf, bytes_written, '\n')) {
        bytes_written += 1;
    }

    return bytes_written;
}

isize
rwlj_fprint(rwljFile_Descriptor fd, rwljString s)
{
    if (write(fd, s.data, cast(usize) s.len) == -1) {
        return -1;
    }

    return s.len;
}

isize
rwlj_fprintln(rwljFile_Descriptor fd, rwljString s)
{
    if (rwlj_fprint(fd, s) == -1) {
        return -1;
    }

    if (write(fd, "\n", 1) == -1) {
        return -1;
    }

    return s.len + 1;
}

isize
rwlj_print(rwljString s)
{
    return rwlj_fprint(RWLJ_STDOUT, s);
}

isize
rwlj_println(rwljString s)
{
    return rwlj_fprintln(RWLJ_STDOUT, s);
}

isize
rwlj_eprint(rwljString s)
{
    return rwlj_fprint(RWLJ_STDERR, s);
}

isize
rwlj_eprintln(rwljString s)
{
    return rwlj_fprintln(RWLJ_STDERR, s);
}

// TODO: Give more init options to String_Builder (from buffer, arena or
// dynamic array)
void
rwlj_string_builder_init(
    rwljString_Builder *sb,
    isize capacity,
    rwljArena *arena
)
{
    sb->buf = rwlj_arena_alloc(arena, cast(usize) capacity);
    sb->capacity = sb->buf != NULL ? capacity : 0;
    sb->len = 0;
}

rwljString
rwlj_string_builder_write_i64(rwljString_Builder *sb, i64 number)
{
    isize start = sb->len;
    sb->len += rwlj_write_i64(
        cast(rwljSlice_U8) rwlj_slice(cast(u8 *) sb->buf, start, sb->capacity),
        number
    );

    return rwlj_string_from_ptr(sb->buf, start, sb->len);
}

rwljString
rwlj_string_builder_write_u64(rwljString_Builder *sb, u64 number, u8 fmt)
{
    isize start = sb->len;
    sb->len += rwlj_write_u64(
        cast(rwljSlice_U8) rwlj_slice(cast(u8 *) sb->buf, start, sb->capacity),
        number,
        fmt
    );

    return rwlj_string_from_ptr(sb->buf, start, sb->len);
}

rwljString
rwlj_string_builder_write_f64(
    rwljString_Builder *sb,
    f64 number,
    u8 fmt,
    i64 precision
)
{
    isize start = sb->len;
    sb->len += rwlj_write_f64(
        cast(rwljSlice_U8) rwlj_slice(cast(u8 *) sb->buf, start, sb->capacity),
        number,
        fmt,
        precision
    );

    return rwlj_string_from_ptr(sb->buf, start, sb->len);
}

rwljString
rwlj_string_builder_write_string(rwljString_Builder *sb, rwljString s)
{
    isize start = sb->len;
    sb->len += rwlj_write_string(
        cast(rwljSlice_U8) rwlj_slice(cast(u8 *) sb->buf, start, sb->capacity),
        s
    );

    return rwlj_string_from_ptr(sb->buf, start, sb->len);
}

rwljString
rwlj_string_builder_to_string(rwljString_Builder *sb)
{
    return rwlj_string_from_ptr(sb->buf, 0, sb->len);
}

rwljString
rwlj_string_builder_clone(rwljString_Builder *sb, rwljArena *arena)
{
    return rwlj_string_clone(rwlj_string_builder_to_string(sb), arena);
}

void
rwlj_string_builder_clear(rwljString_Builder *sb)
{
    sb->capacity = 0;
    sb->len = 0;
}

rwljString
rwlj_sbprintf(rwljString_Builder *sb, char const *fmt, ...)
{
    isize start = sb->len;
    va_list ap;

    va_start(ap, fmt);
    sb->len += __rwlj_bprintf_va(
        cast(rwljSlice_U8) rwlj_slice(cast(u8 *) sb->buf, start, sb->capacity),
        false,
        fmt,
        ap
    );
    va_end(ap);

    return rwlj_string_from_ptr(sb->buf, start, sb->len);
}

rwljString
rwlj_sbprintfln(rwljString_Builder *sb, char const *fmt, ...)
{
    isize start = sb->len;
    va_list ap;

    va_start(ap, fmt);
    sb->len += __rwlj_bprintf_va(
        cast(rwljSlice_U8) rwlj_slice(cast(u8 *) sb->buf, start, sb->capacity),
        true,
        fmt,
        ap
    );
    va_end(ap);

    return rwlj_string_from_ptr(sb->buf, start, sb->len);
}

rwljString
rwlj_sbprint(rwljString_Builder *sb, rwljString s)
{
    rwljSlice_U8 buf = rwlj_slice(cast(u8 *) sb->buf, sb->len, sb->capacity);
    isize bytes_written = rwlj_bprint(buf, s);

    return rwlj_string_from_ptr(sb->buf, 0, bytes_written);
}

rwljString
rwlj_sbprintln(rwljString_Builder *sb, rwljString s)
{
    rwljSlice_U8 buf = rwlj_slice(cast(u8 *) sb->buf, sb->len, sb->capacity);
    isize bytes_written = rwlj_bprint(buf, s);

    if (rwlj_slice_set(&buf, bytes_written, '\n')) {
        bytes_written += 1;
    }

    return rwlj_string_from_ptr(sb->buf, sb->len, bytes_written);
}

/*
 *  Testing
 */

#define DESCRIBE(string)                                                       \
    do {                                                                       \
        rwlj_println(STRING(                                                   \
            "----------------------------------------------------------------" \
            "----------------"                                                 \
        ));                                                                    \
        rwlj_println(STRING(string));                                          \
        rwlj_println(STRING(                                                   \
            "----------------------------------------------------------------" \
            "----------------"                                                 \
        ));                                                                    \
    } while (false)

#define TEST(string)                                                           \
    for (bool loop = true, success = true;                                     \
         loop && rwlj_printfln("[%d] - " string "...", RWLJ_COUNTER);          \
         loop = false,                                                         \
              success ? rwlj_no_op() : rwlj_println(STRING("FAILED")))

#define rwlj_testing_printf_info()                                             \
    rwlj_printf("\t[%s:%d:%s()]: ", RWLJ_FILE, RWLJ_LINE, RWLJ_FUNCTION)

#define rwlj_testing_printf(...)                                               \
    do {                                                                       \
        rwlj_testing_printf_info();                                            \
        rwlj_printfln(__VA_ARGS__);                                            \
    } while (false)

#define rwlj_testing_expect(condition)                                         \
    do {                                                                       \
        if (!(condition)) {                                                    \
            rwlj_testing_printf("Expected %s to be true", #condition);         \
            success = false;                                                   \
        }                                                                      \
    } while (false)

// A lot of this mess is due to the fact that _Generic only accepts functions
// and not macros (rightfully so, although it doesn't expand those that are
// aliases)
bool
__rwlj_testing_expect_value_i64(
    i64 a,
    i64 b,
    char const *file,
    int line,
    char const *proc
)
{
    if (a != b) {
        rwlj_printf("\t[%s:%d:%s()]: ", file, line, proc);
        rwlj_printfln("Expected %ld, got %ld", b, a);
        return false;
    }

    return true;
}

#define rwlj_testing_expect_value_i64(a, b)                                    \
    __rwlj_testing_expect_value_i64(a, b, RWLJ_FILE, RWLJ_LINE, RWLJ_FUNCTION)

bool
__rwlj_testing_expect_value_u64(
    u64 a,
    u64 b,
    char const *file,
    int line,
    char const *proc
)
{
    if (a != b) {
        rwlj_printf("\t[%s:%d:%s()]: ", file, line, proc);
        rwlj_printfln("Expected %lu, got %lu", b, a);
        return false;
    }

    return true;
}

#define rwlj_testing_expect_value_u64(a, b)                                    \
    __rwlj_testing_expect_value_u64(a, b, RWLJ_FILE, RWLJ_LINE, RWLJ_FUNCTION)

bool
__rwlj_testing_expect_value_f64(
    f64 a,
    f64 b,
    char const *file,
    int line,
    char const *proc
)
{
    if (a != b) {
        rwlj_printf("\t[%s:%d:%s()]: ", file, line, proc);
        rwlj_printfln("Expected %f, got %f", b, a);
        return false;
    }

    return true;
}

#define rwlj_testing_expect_value_f64(a, b)                                    \
    __rwlj_testing_expect_value_f64(a, b, RWLJ_FILE, RWLJ_LINE, RWLJ_FUNCTION)

bool
__rwlj_testing_expect_value_string(
    rwljString a,
    rwljString b,
    char const *file,
    int line,
    char const *proc
)
{
    if (!rwlj_string_are_equal(a, b)) {
        rwlj_printf("\t[%s:%d:%s()]: ", file, line, proc);
        rwlj_printfln("Expected %S, got %S", b, a);
        return false;
    }

    return true;
}

#define rwlj_testing_expect_value_string(a, b)                                 \
    __rwlj_testing_expect_value_string(                                        \
        a, b, RWLJ_FILE, RWLJ_LINE, RWLJ_FUNCTION                              \
    )

#define rwlj_testing_expect_value_symbol(a, b)                                 \
    do {                                                                       \
        if (a != b) {                                                          \
            rwlj_testing_printf("Expected %s, got %s", #b, #a);                \
            success = false;                                                   \
        }                                                                      \
    } while (false)

// clang-format off
#define __rwlj_testing_expect_value(a, b, file, line, proc)                    \
    _Generic((a),                                                              \
    i64: __rwlj_testing_expect_value_i64,                                      \
    u64: __rwlj_testing_expect_value_u64,                                      \
    f64: __rwlj_testing_expect_value_f64,                                      \
    rwljString: __rwlj_testing_expect_value_string                             \
)(a, b, file, line, proc)
// clang-format on

#define rwlj_testing_expect_value(a, b)                                        \
    do {                                                                       \
        if (!__rwlj_testing_expect_value(                                      \
                a, b, RWLJ_FILE, RWLJ_LINE, RWLJ_FUNCTION                      \
            )) {                                                               \
            success = false;                                                   \
        }                                                                      \
    } while (false)

#endif // RWLJ_IMPLEMENTATION
