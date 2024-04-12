/*
Generic Linked List


# License
  The MIT License (MIT)

  Copyright (C) 2023-2024 Jin Cai

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef ULLIST_H
#define ULLIST_H

#ifdef __has_builtin
  #if __has_builtin(__builtin_expect)
    #ifndef ul_likely
      #define ul_likely(x) __builtin_expect(!!(x), 1)
    #endif
    #ifndef ul_unlikely
      #define ul_unlikely(x) __builtin_expect(!!(x), 0)
    #endif
  #endif
#endif
#ifndef ul_likely
  #define ul_likely(x) (x)
#endif /* ul_likely */
#ifndef ul_unlikely
  #define ul_unlikely(x) (x)
#endif /* ul_unlikely */

#ifndef ul_unused
  #if (defined(__GNUC__) && __GNUC__ >= 3) || defined(__clang__)
    #define ul_unused __attribute__((unused))
  #elif defined(__cplusplus) && defined(__has_cpp_attribute)
    #if __has_cpp_attribute(maybe_unused)
      #define ul_unused [[maybe_unused]]
    #endif
  #endif
  #ifndef ul_unused
    #define ul_unused
  #endif
#endif /* ul_unused */

#ifndef ul_inline
  #if defined(__cplusplus) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
    #define ul_inline inline
  #else
    #define ul_inline
  #endif
#endif /* ul_inline */

#ifndef ul_hapi
  #define ul_hapi ul_unused static ul_inline
#endif /* ul_hapi */

#include <stddef.h>

typedef struct ullist_t {
  struct ullist_t* next;
  struct ullist_t* prev;
} ullist_t;

#define ULLIST_INIT(var) { &(var), &(var) }
#define ULLIST_ENTRY(node, T) (T*)((unsigned char*)(node) + sizeof(ullist_t))

ul_hapi void ullist_init(ullist_t* head) {
  head->prev = head; head->next = head;
}

ul_hapi void ullist_insert_after(ullist_t* iter, ullist_t* ins) {
  /* iter => ins => iter->next */
  ins->next = iter->next;
  iter->next = ins;
  ins->prev = iter;
  ins->next->prev = ins;
}
ul_hapi void ullist_insert_before(ullist_t* iter, ullist_t* ins) {
  /* iter->prev => ins => iter */
  ins->prev = iter->prev;
  iter->prev = ins;
  ins->next = iter;
  ins->prev->next = ins;
}
ul_hapi ullist_t* ullist_remove(ullist_t* iter) {
  if(iter->prev == iter) return NULL;
  /* iter->prev => iter->next */
  iter->prev->next = iter->next;
  iter->next->prev = iter->prev;
  iter->prev = 0; iter->next = 0;
  return iter;
}

ul_hapi void ullist_push_back(ullist_t* head, ullist_t* ins) {
  ullist_insert_before(head->prev, ins);
}
ul_hapi void ullist_push_front(ullist_t* head, ullist_t* ins) {
  ullist_insert_after(head, ins);
}
ul_hapi ullist_t* ullist_pop_back(ullist_t* head) {
  return ullist_remove(head->prev);
}
ul_hapi ullist_t* ullist_pop_front(ullist_t* head) {
  return ullist_remove(head->next);
}

#define ULLIST_FOREACH(node, head) \
  for((node) = (head)->next; (node) != (head); (node) = (node)->next)
#define ULLIST_FOREACH_SAFE(node, node2, head) \
  for((node) = (head)->next, (node2) = (node)->next; \
      (node) != (head); \
      (node) = (node2), (node2) = (node2)->next)
#define ULLIST_FOREACH_PREV(node, head) \
  for((node) = (head)->prev; (node) != (head); (node) = (node)->prev)
#define ULLIST_FOREACH_PREV_SAFE(node, node2, head) \
  for((node) = (head)->prev, (node2) = (node2)->prev; \
      (node) != (head); \
      (node) = (node2), (node2) = (node2)->prev)

ul_hapi int ullist_empty(ullist_t* head) { return head->next == head; }
ul_hapi size_t ullist_count(ullist_t* head) {
  size_t count = 0;
  ullist_t* node;
  ULLIST_FOREACH(node, head) ++count;
  return count;
}

ul_hapi void ullist_merge_after(ullist_t* iter, ullist_t* ins_head) {
  if(ins_head->next == ins_head) return;
  /* iter => ins_head->next => ... => ins_head->prev => iter->next */
  ins_head->prev->next = iter->next;
  iter->next->prev = ins_head->prev;
  iter->next = ins_head->next;
  ins_head->next->prev = iter;
  ins_head->next = ins_head->prev = ins_head;
}
ul_hapi void ullist_merge_before(ullist_t* iter, ullist_t* ins_head) {
  if(ins_head->next == ins_head) return;
  /* iter->prev => ins_head->next => ... => ins_head->prev => iter */
  iter->prev->next = ins_head->next;
  ins_head->next->prev = iter->prev;
  ins_head->prev->next = iter;
  iter->prev = ins_head->prev;
  ins_head->next = ins_head->prev = ins_head;
}
ul_hapi void ullist_split_after(ullist_t* head, ullist_t* iter, ullist_t* new_head) {
  /* new_head => iter->next => ... => head->prev => new_head */
  new_head->next = iter->next;
  iter->next->prev = new_head;
  new_head->prev = head->prev;
  head->prev->next = new_head;
  /* ... => iter => head => ... */
  iter->next = head;
  head->prev = iter;
}
ul_hapi void ullist_split_before(ullist_t* head, ullist_t* iter, ullist_t* new_head) {
  ullist_split_after(head, iter->prev, new_head);
}

ul_hapi void ullist_rotate(ullist_t* head, ullist_t* iter) {
  if(iter == head) return;
  /* head => iter => ... => head->prev => head->next => ... => iter->prev => head */
  head->prev->next = head->next;
  head->next->prev = head->prev;
  head->prev = iter->prev;
  iter->prev->next = head;
  head->next = iter;
  iter->prev = head;
}
ul_hapi void ullist_reverse(ullist_t* head) {
  ullist_t* node = head;
  ullist_t* tmp;
  do {
    tmp = node->next;
    node->next = node->prev;
    node->prev = tmp;
    node = tmp;
  } while(node != head);
}

typedef int (*ullist_less_comp_fn_t)(void* opaque, const ullist_t* lhs, const ullist_t* rhs);
ul_hapi void ullist_sort(ullist_t* head, ullist_less_comp_fn_t comp, void* opaque) {
  ullist_t* fast = head->next;
  ullist_t* slow = head->next;
  ullist_t* pos;
  ullist_t head2 = ULLIST_INIT(head2);

  /* spilt linked list */
  if(fast->next == head) return; /* no element or only 1 element */
  do {
    fast = fast->next->next;
    slow = slow->next;
  } while(fast != head && fast->next != head);
  ullist_split_before(head, slow, &head2);

  /* sort two parts */
  ullist_sort(head, comp, opaque);
  ullist_sort(&head2, comp, opaque);

  /* merge two parts */
  fast = head->next; slow = head2.next;
  pos = head;
  while(fast != head && slow != &head2) {
    if(!comp(opaque, slow, fast)) { /* (*fast <= *slow) */
      pos->next = fast;
      fast->prev = pos;
      pos = pos->next;
      fast = fast->next;
    } else {
      pos->next = slow;
      slow->prev = pos;
      pos = pos->next;
      slow = slow->next;
    }
  }
  if(fast != head) {
    pos->next = fast;
    fast->prev = pos;
    /* `head->prev` is already `fast`, don't need to handle */
  } else {
    pos->next = slow;
    slow->prev = pos;
    head->prev = head2.prev;
    head->prev->next = head;
  }
}

#endif /* ULLIST_H */
