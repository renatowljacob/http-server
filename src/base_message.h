#ifndef BASE_MESSAGE_H
#define BASE_MESSAGE_H

#include "base_arena.h"
#include "base_core.h"
#include "base_string.h"

#define DEFAULT_MESSAGE(arena, msg, err)                                       \
    (arena), (msg), DEFAULT_CAPACITY, "#%lu in %s() %s:%d. %s. Aborting...",   \
      (msg)->index, __FUNCTION__, __FILE__, __LINE__, strerror((err))

struct MessageNode
{
    u32 index;
    i32 status_code;
    String message;
    SListNode list;
};
typedef struct MessageNode MessageNode;

struct MessageList
{
    MessageNode *head;
    MessageNode *tail;
};
typedef struct MessageList MessageList;

static volatile const MessageNode
  nil_message = { 0, 0, { 0, "" }, { (SListNode *)&nil_message.list } };

MessageNode *message_new(Arena *arena, u32 index, i32 status_code);

char *message_alloc(
  Arena *arena,
  MessageNode *msg,
  size_t buf_capacity,
  char *fmt,
  ...
);

#endif
