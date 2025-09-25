#include "base_core.h"

bool
is_nil(const void *ptr, const void *nil)
{
    return (ptr == NULL || ptr == nil);
}

SListNode *
_slist_append_params(
  SListNode *node,
  SListNode *tail,
  struct SList_Append_Params params
)
{
    node->next = params.next;
    tail->next = node;

    return node;
}

SListNode *
slist_preppend(SListNode *node, SListNode *head)
{
    node->next = head;

    return node;
}

SListNode *
slist_insert(SListNode *prev_node, SListNode *next_node, SListNode *node)
{
    node->next = next_node;
    prev_node->next = node;

    return node;
}

DListNode *
_dlist_append_params(
  DListNode *node,
  DListNode *tail,
  struct DList_Append_Params params
)
{
    node->next = params.next;
    node->prev = tail;
    tail->next = node;

    return node;
}

DListNode *
_dlist_preppend_params(
  DListNode *node,
  DListNode *head,
  struct DList_Preppend_Params params
)
{
    node->next = head;
    node->prev = params.prev;
    head->prev = node;

    return node;
}

DListNode *
dlist_insert(DListNode *node, DListNode *prev_node, DListNode *next_node)
{
    node->next = next_node;
    node->prev = prev_node;
    next_node->prev = node;
    prev_node->next = node;

    return node;
}
