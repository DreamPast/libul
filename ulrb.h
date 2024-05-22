/*
Red-Black Tree (fast but restricted version)


# Dependence
  C89 (if we cannot detect it correctly, we assume `size_t` is big enough to hold a pointer)


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
#ifndef ULRB_H
#define ULRB_H

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

#ifndef ul_nodiscard
  #if defined(__has_cpp_attribute)
    #if __has_cpp_attribute(nodiscard)
      #define ul_nodiscard [[nodiscard]]
    #endif
  #endif
  #if !defined(ul_nodiscard) && defined(__has_attribute)
    #if __has_attribute(warn_unused_result)
      #define ul_nodiscard __attribute__((warn_unused_result))
    #endif
  #endif
  #if !defined(ul_nodiscard) && defined(_HAS_NODISCARD)
    #define ul_nodiscard _NODISCARD
  #endif
  #if !defined(ul_nodiscard) && defined(_MSC_VER) && _MSC_VER >= 1700
    #define ul_nodiscard _Check_return_
  #endif
  #ifndef ul_nodiscard
    #define ul_nodiscard
  #endif
#endif /* ul_nodiscard */

#ifndef ul_reinterpret_cast
  #ifdef __cplusplus
    #define ul_reinterpret_cast(T, val) reinterpret_cast<T>(val)
  #else
    #define ul_reinterpret_cast(T, val) ((T)(val))
  #endif
#endif /* ul_reinterpret_cast */

#ifndef ul_static_cast
  #ifdef __cplusplus
    #define ul_static_cast(T, val) static_cast<T>(val)
  #else
    #define ul_static_cast(T, val) ((T)(val))
  #endif
#endif /* ul_static_cast */

#include <assert.h>
#include <stdlib.h>
#include <limits.h>

#if defined(UINTPTR_MAX)
  typedef uintptr_t ulrb_uptr_t;
#elif defined(_WIN32) || defined(_WIN64)
  #if defined(_WIN64)
    typedef unsigned __int64 ulrb_uptr_t;
  #else
    typedef unsigned ulrb_uptr_t;
  #endif
#elif defined(__SIZEOF_POINTER__)
  #if __SIZEOF_POINTER__ == 8
    #define _ULRB_UPTR_MAX 0xFFFFFFFFFFFFFFFF
  #elif __SIZEOF_POINTER__ == 4
    #define _ULRB_UPTR_MAX 0xFFFFFFFF
  #elif __SIZEOF_POINTER__ == 2
    #define _ULRB_UPTR_MAX 0xFFFF
  #elif __SIZEOF_POINTER__ == 1
    #define _ULRB_UPTR_MAX 0xFF
  #endif
  #if defined(ULLONG_MAX) && ULLONG_MAX == _ULRB_UPTR_MAX
    typedef unsigned long long ulrb_uptr_t;
  #elif defined(ULONG_MAX) && ULONG_MAX == _ULRB_UPTR_MAX
    typedef unsigned long ulrb_uptr_t;
  #elif defined(UINT_MAX) && UINT_MAX == _ULRB_UPTR_MAX
    typedef unsigned ulrb_uptr_t;
  #elif defined(USHRT_MAX) && USHRT_MAX == _ULRB_UPTR_MAX
    typedef unsigned short ulrb_uptr_t;
  #elif defined(UCHAR_MAX) && UCHAR_MAX == _ULRB_UPTR_MAX
    typedef unsigned char ulrb_uptr_t;
  #else
    typedef size_t ulrb_uptr_t; /* we assume `size_t` is big enough to hold a pointer */
  #endif
  #undef _ULRB_UPTR_MAX
#else
  typedef size_t ulrb_uptr_t; /* we assume `size_t` is big enough to hold a pointer */
#endif

/**
 * The node of Red-Black tree.
 * The type doesn't include any key and value, so you need to define it.
 * The struct needn't be inited, when you insert the node into the tree, the function will do it.
 *
 * For example:
 * ```c
 * typedef struct mynode_t {
 *   ulrb_node_t base;
 *   const char* key;
 *   void* value;
 * } mynode_t;
 *
 * int mynode_comp(void* opaque, const void* lp, const void* rp) {
 *   const char* lhs = *(const char**)lp;
 *   const char* rhs = *(const char**)rp;
 *   (void)opaque;
 *   return strcmp(lhs, rhs);
 * }
 * ```
 *
 * In C++, you can use inherit. For example(C++11 and above):
 * ```cpp
 * template<class Key, class Value>
 * class Node : public ulrb_node_t {
 * public:
 *   Node() = default;
 *   Node(const Node&) = default;
 *   Node(Node&&) = default;
 *   Node(Key&& k, Value&& v) : key(std::move(k)), value(std::move(v)) { }
 *   Node(const Key& k, const Value& v) : key(k), value(v) { }
 *
 *   Node& operator=(const Node&) = default;
 *   Node& operator=(Node&&) = default;
 *
 *   static int comp(void* opaque, const void* lp, const void* rp) {
 *     auto lhs = *reinterpret_cast<const Key*>(lp);
 *     auto rhs = *reinterpret_cast<const Key*>(rp);
 *     (void)opaque;
 *     if(lhs < rhs) return -1;
 *     else return lhs > rhs;
 *   }
 *   static void destructor(void* opaque, ulrb_node_t* p) {
 *     (void)opaque;
 *     delete reinterpret_cast<Node<Key, Value>*>(p);
 *   }
 *   static ulrb_node_t* copy(void* opaque, const ulrb_node_t* x) {
 *     return new Node(*reinterpret_cast<const Node*>(x));
 *   }
 *
 *   Key key;
 *   Value value;
 * };
 * ```
 *
*/
typedef struct ulrb_node_t {
  struct ulrb_node_t* left;
  struct ulrb_node_t* right;
} ulrb_node_t;

/* returns negative value if less, posstive value if greater, 0 if equal */
typedef int (*ulrb_comp_t)(void* opaque, const void* lhs, const void* rhs);
#define ulrb_node_get_key(node) ul_reinterpret_cast(const void*, (node) + 1)

#define ulrb_node_get_left(node) ((node)->left)
#define ulrb_node_set_left(node, child) ((node)->left = (child))
#define ulrb_node_get_right(node) \
  ul_reinterpret_cast(ulrb_node_t*, ul_reinterpret_cast(ulrb_uptr_t, (node)->right) & ul_static_cast(ulrb_uptr_t, ~1))
#define ulrb_node_set_right(node, child) ((node)->right = ul_reinterpret_cast(ulrb_node_t*, \
  ul_reinterpret_cast(ulrb_uptr_t, ul_reinterpret_cast(ulrb_node_t*, child)) | \
    (ul_reinterpret_cast(ulrb_uptr_t, (node)->right) & 1)))
#define ulrb_node_get_color(node) (ul_reinterpret_cast(ulrb_uptr_t, (node)->right) & 1)
#define ulrb_node_set_color(node, color) ((node)->right = \
  ul_reinterpret_cast(ulrb_node_t*, (ul_reinterpret_cast(ulrb_uptr_t, (node)->right) & \
    ul_static_cast(ulrb_uptr_t, ~1)) | ul_static_cast(unsigned, color)))
#define ulrb_node_set_red(node) \
  ((node)->right = ul_reinterpret_cast(ulrb_node_t*, ul_reinterpret_cast(ulrb_uptr_t, (node)->right) | 1))
#define ulrb_node_set_black(node) ((node)->right = \
  ul_reinterpret_cast(ulrb_node_t*, ul_reinterpret_cast(ulrb_uptr_t, (node)->right) & \
    ul_static_cast(ulrb_uptr_t, ~1)))

ul_hapi void ulrb_node_init(ulrb_node_t* node) {
  assert((ul_reinterpret_cast(ulrb_uptr_t, node) & 1) == 0);
  node->left = 0;
  node->right = ul_reinterpret_cast(ulrb_node_t*, 1);
}

/*
 *         |                         |
 *       x_node                    r_node
 *      /      \                  /      \
 *     L     r_node    =>      x_node     rR
 *          /      \          /      \
 *         rL      rR        L        rL
 */
#define ulrb_node_rotate_left(x_node, r_node) \
  do { \
    (r_node) = ulrb_node_get_right(x_node); \
    ulrb_node_set_right((x_node), ulrb_node_get_left(r_node)); \
    ulrb_node_set_left((r_node), (x_node)); \
  } while(0)

/*
 *         |                   |
 *       x_node              r_node
 *      /      \            /      \
 *   r_node     R    =>    rL     x_node
 *  /      \                     /      \
 * rL      rR                   rR       R
 */
#define ulrb_node_rotate_right(x_node, r_node) \
  do { \
    (r_node) = ulrb_node_get_left(x_node); \
    ulrb_node_set_left((x_node), ulrb_node_get_right(r_node)); \
    ulrb_node_set_right((r_node), (x_node)); \
  } while(0)

/*
 * Red-Black Tree's height is <= 2log2(n + 1).
 */
#define ULRB_MAX_DEPTH (sizeof(void*) * (CHAR_BIT))

ul_hapi ulrb_node_t* ulrb_leftmost(ulrb_node_t* x) {
  ulrb_node_t* y = NULL;
  while(x) { y = x; x = ulrb_node_get_left(x); }
  return y;
}
ul_hapi ulrb_node_t* ulrb_rightmost(ulrb_node_t* x) {
  ulrb_node_t* y = NULL;
  while(x) { y = x; x = ulrb_node_get_right(x); }
  return y;
}

ul_hapi ulrb_node_t* ulrb_lower_bound(ulrb_node_t* root, const void* key, ulrb_comp_t comp, void* opaque) {
  ulrb_node_t* x = root;
  ulrb_node_t* y = NULL;
  while(x)
    if(comp(opaque, key, ulrb_node_get_key(x)) <= 0) { y = x; x = ulrb_node_get_left(x); }
    else x = ulrb_node_get_right(x);
  return y;
}
ul_hapi ulrb_node_t* ulrb_upper_bound(ulrb_node_t* root, const void* key, ulrb_comp_t comp, void* opaque) {
  ulrb_node_t* x = root;
  ulrb_node_t* y = NULL;
  while(x)
    if(comp(opaque, key, ulrb_node_get_key(x)) < 0) { y = x; x = ulrb_node_get_left(x); }
    else x = ulrb_node_get_right(x);
  return y;
}
ul_hapi ulrb_node_t* ulrb_reverse_lower_bound(ulrb_node_t* root, const void* key, ulrb_comp_t comp, void* opaque) {
  ulrb_node_t* x = root;
  ulrb_node_t* y = NULL;
  while(x)
    if(comp(opaque, key, ulrb_node_get_key(x)) >= 0) { y = x; x = ulrb_node_get_right(x); }
    else x = ulrb_node_get_left(x);
  return y;
}
ul_hapi ulrb_node_t* ulrb_reverse_upper_bound(ulrb_node_t* root, const void* key, ulrb_comp_t comp, void* opaque) {
  ulrb_node_t* x = root;
  ulrb_node_t* y = NULL;
  while(x)
    if(comp(opaque, key, ulrb_node_get_key(x)) > 0) { y = x; x = ulrb_node_get_right(x); }
    else x = ulrb_node_get_left(x);
  return y;
}
ul_hapi ulrb_node_t* ulrb_equal_range(ulrb_node_t** upper, ulrb_node_t* root, const void* key, ulrb_comp_t comp, void* opaque) {
  ulrb_node_t* x = root;
  ulrb_node_t* y = NULL;
  int cmp;
  while(x) {
    cmp = comp(opaque, key, ulrb_node_get_key(x));
    if(cmp > 0) x = ulrb_node_get_right(x);
    else if(cmp < 0) { y = x; x = ulrb_node_get_left(x); }
    else {
      ulrb_node_t* xu = x;
      ulrb_node_t* yu = y;
      y = x;
      x = ulrb_node_get_left(x);
      xu = ulrb_node_get_right(xu);
      while(x)
        if(comp(opaque, key, ulrb_node_get_key(x)) <= 0) { y = x; x = ulrb_node_get_left(x); }
        else x = ulrb_node_get_right(x);
      while(xu)
        if(comp(opaque, key, ulrb_node_get_key(xu)) < 0) { yu = xu; xu = ulrb_node_get_left(xu); }
        else xu = ulrb_node_get_right(xu);
      *upper = yu;
      return y;
    }
  }
  *upper = y;
  return y;
}

ul_hapi ulrb_node_t* ulrb_find(ulrb_node_t* root, const void* key, ulrb_comp_t comp, void* opaque) {
  int cmp;
  while(root) {
    cmp = comp(opaque, key, ulrb_node_get_key(root));
    if(cmp == 0) break;
    if(cmp < 0) root = ulrb_node_get_left(root);
    else root = ulrb_node_get_right(root);
  }
  return root;
}
ul_hapi ulrb_node_t* ulrb_find_next(ulrb_node_t* root, const void* key, ulrb_comp_t comp, void* opaque) {
  /* same with `ulrb_upper_bound` */
  ulrb_node_t* x = root;
  ulrb_node_t* y = NULL;
  while(x)
    if(comp(opaque, key, ulrb_node_get_key(x)) < 0) { y = x; x = ulrb_node_get_left(x); }
    else x = ulrb_node_get_right(x);
  return y;
}
ul_hapi ulrb_node_t* ulrb_find_prev(ulrb_node_t* root, const void* key, ulrb_comp_t comp, void* opaque) {
  /* same with `ulrb_reverse_upper_bound` */
  ulrb_node_t* x = root;
  ulrb_node_t* y = NULL;
  while(x)
    if(comp(opaque, key, ulrb_node_get_key(x)) > 0) { y = x; x = ulrb_node_get_right(x); }
    else x = ulrb_node_get_left(x);
  return y;
}

typedef struct _ulrb_path_t {
  ulrb_node_t* node;
  int cmp;
} _ulrb_path_t;

ul_hapi void ulrb_insert_unsafe(ulrb_node_t** proot, ulrb_node_t* ins, ulrb_comp_t comp, void* opaque) {
  _ulrb_path_t path[ULRB_MAX_DEPTH];
  _ulrb_path_t* pathp;
  ulrb_node_init(ins);

  /* find target node */
  path->node = *proot;
  for(pathp = path; pathp->node; ++pathp) {
    int cmp = (pathp->cmp = comp(opaque, ulrb_node_get_key(ins), ulrb_node_get_key(pathp->node)));
    if(cmp < 0) pathp[1].node = ulrb_node_get_left(pathp->node);
    else pathp[1].node = ulrb_node_get_right(pathp->node);
  }
  pathp->node = ins;

  /* go back to root and fix color */
  for(--pathp; pathp >= path; --pathp) {
    ulrb_node_t* cnode = pathp->node;
    if(pathp->cmp < 0) {
      ulrb_node_t* left = pathp[1].node;
      ulrb_node_t* leftleft;

      ulrb_node_set_left(cnode, left);
      if(ulrb_node_get_color(left) == 0) return;
      leftleft = ulrb_node_get_left(left);
      if(leftleft && ulrb_node_get_color(leftleft) == 1) {
        ulrb_node_t* tnode;
        ulrb_node_set_black(leftleft);
        ulrb_node_rotate_right(cnode, tnode);
        cnode = tnode;
      }
    } else {
      ulrb_node_t* right = pathp[1].node;
      ulrb_node_t* left;

      ulrb_node_set_right(cnode, right);
      if(ulrb_node_get_color(right) == 0) return;
      left = ulrb_node_get_left(cnode);
      if(left && ulrb_node_get_color(left) == 1) {
        ulrb_node_set_black(left);
        ulrb_node_set_black(right);
        ulrb_node_set_red(cnode);
      } else {
        ulrb_node_t* tnode;
        const int tcolor = ul_static_cast(int, ulrb_node_get_color(cnode));
        ulrb_node_rotate_left(cnode, tnode);
        ulrb_node_set_color(tnode, tcolor);
        ulrb_node_set_red(cnode);
        cnode = tnode;
      }
    }
    pathp->node = cnode;
  }

  /* set root, and make it black */
  *proot = path->node;
  ulrb_node_set_black(*proot);
}
ul_nodiscard ul_hapi ulrb_node_t* ulrb_insert(ulrb_node_t** proot, ulrb_node_t* ins, ulrb_comp_t comp, void* opaque) {
  _ulrb_path_t path[ULRB_MAX_DEPTH];
  _ulrb_path_t* pathp;
  ulrb_node_init(ins);

  /* find target node */
  path->node = *proot;
  for(pathp = path; pathp->node; ++pathp) {
    int cmp = (pathp->cmp = comp(opaque, ulrb_node_get_key(ins), ulrb_node_get_key(pathp->node)));
    if(cmp < 0) pathp[1].node = ulrb_node_get_left(pathp->node);
    else if(cmp > 0) pathp[1].node = ulrb_node_get_right(pathp->node);
    else return pathp->node;
  }
  pathp->node = ins;

  /* go back to root and fix color */
  for(--pathp; pathp >= path; --pathp) {
    ulrb_node_t* cnode = pathp->node;
    if(pathp->cmp < 0) {
      ulrb_node_t* left = pathp[1].node;
      ulrb_node_t* leftleft;

      ulrb_node_set_left(cnode, left);
      if(ulrb_node_get_color(left) == 0) return ins;
      leftleft = ulrb_node_get_left(left);
      if(leftleft && ulrb_node_get_color(leftleft) == 1) {
        ulrb_node_t* tnode;
        ulrb_node_set_black(leftleft);
        ulrb_node_rotate_right(cnode, tnode);
        cnode = tnode;
      }
    } else {
      ulrb_node_t* right = pathp[1].node;
      ulrb_node_t* left;

      ulrb_node_set_right(cnode, right);
      if(ulrb_node_get_color(right) == 0) return ins;
      left = ulrb_node_get_left(cnode);
      if(left && ulrb_node_get_color(left) == 1) {
        ulrb_node_set_black(left);
        ulrb_node_set_black(right);
        ulrb_node_set_red(cnode);
      } else {
        ulrb_node_t* tnode;
        const int tcolor = ul_static_cast(int, ulrb_node_get_color(cnode));
        ulrb_node_rotate_left(cnode, tnode);
        ulrb_node_set_color(tnode, tcolor);
        ulrb_node_set_red(cnode);
        cnode = tnode;
      }
    }
    pathp->node = cnode;
  }

  /* set root, and make it black */
  *proot = path->node;
  ulrb_node_set_black(*proot);
  return ins;
}

ul_nodiscard ul_hapi ulrb_node_t* ulrb_remove(ulrb_node_t** proot, const void* key, ulrb_comp_t comp, void* opaque) {
  _ulrb_path_t path[ULRB_MAX_DEPTH];
  _ulrb_path_t* pathp = NULL, * nodep = NULL;
  ulrb_node_t* del;

  /* find target node */
  path->node = *proot;
  for(pathp = path; pathp->node; ++pathp) {
    int cmp = (pathp->cmp = comp(opaque, key, ulrb_node_get_key(pathp->node)));
    if(cmp < 0) pathp[1].node = ulrb_node_get_left(pathp->node);
    else {
      pathp[1].node = ulrb_node_get_right(pathp->node);
      if(cmp == 0) {
        /* find node's successor */
        pathp->cmp = 1;
        nodep = pathp;
        for(++pathp; pathp->node; ++pathp) {
          pathp->cmp = -1;
          pathp[1].node = ulrb_node_get_left(pathp->node);
        }
        goto node_exists;
      }
    }
  }
  return NULL; /* cannot find node */

node_exists:
  del = nodep->node;
  --pathp;
  if(pathp->node != del) {
    /* swap node with it's successor */
    const int tcolor = ul_static_cast(int, ulrb_node_get_color(pathp->node));
    ulrb_node_set_color(pathp->node, ulrb_node_get_color(del));
    ulrb_node_set_left(pathp->node, ulrb_node_get_left(del));

    ulrb_node_set_right(pathp->node, ulrb_node_get_right(del));
    ulrb_node_set_color(del, tcolor);

    nodep->node = pathp->node;
    pathp->node = del;
    if(nodep == path) *proot = nodep->node; /* node is root */
    else
      if(nodep[-1].cmp < 0) ulrb_node_set_left(nodep[-1].node, nodep->node);
      else ulrb_node_set_right(nodep[-1].node, nodep->node);
  } else { /* node has no successor */
    ulrb_node_t* left = ulrb_node_get_left(del);
    if(left) {
      /* node has a left child */
      assert(ulrb_node_get_color(del) == 0);
      assert(ulrb_node_get_color(left) == 1);
      ulrb_node_set_black(left);
      if(pathp == path) *proot = left;
      else
        if(pathp[-1].cmp < 0) ulrb_node_set_left(pathp[-1].node, left);
        else ulrb_node_set_right(pathp[-1].node, left);
      return del;
    }
    if(pathp == path) {
      /* the tree only has one nodes */
      *proot = NULL;
      return del;
    }
  }

  if(ulrb_node_get_color(pathp->node) == 1) {
    assert(pathp[-1].cmp < 0);
    ulrb_node_set_left(pathp[-1].node, NULL);
    return del;
  }

  pathp->node = NULL;
  for(--pathp; pathp >= path; --pathp) {
    assert(pathp->cmp != 0);
    if(pathp->cmp < 0) {
      ulrb_node_t* right = ulrb_node_get_right(pathp->node);
      ulrb_node_t* rightleft = ulrb_node_get_left(right);
      ulrb_node_t* tnode;

      ulrb_node_set_left(pathp->node, pathp[1].node);
      if(ulrb_node_get_color(pathp->node) == 1) {
        if(rightleft && ulrb_node_get_color(rightleft) == 1) {
          /*
           * In following diagrams, ||, // and \\
           * indicate the path to the removed node.
           *
           *        ||
           *     pathp(r)
           *   //        \
           *  (b)        (b)
           *             /
           *           (r)
           */
          ulrb_node_set_black(pathp->node);
          ulrb_node_rotate_right(right, tnode);
          ulrb_node_set_right(pathp->node, tnode);
          ulrb_node_rotate_left(pathp->node, tnode);
        } else {
          /*
           *        ||
           *     pathp(r)
           *   //        \
           *  (b)        (b)
           *             /
           *           (b)
           */
          ulrb_node_rotate_left(pathp->node, tnode);
        }

        assert(pathp > path);
        if(pathp[-1].cmp < 0) ulrb_node_set_left(pathp[-1].node, tnode);
        else ulrb_node_set_right(pathp[-1].node, tnode);
        return del;
      } else {
        if(rightleft && ulrb_node_get_color(rightleft) == 1) {
          /*
           *        ||
           *     pathp(b)
           *   //        \
           *  (b)        (b)
           *             /
           *           (r)
           */
          ulrb_node_set_black(rightleft);
          ulrb_node_rotate_right(right, tnode);
          ulrb_node_set_right(pathp->node, tnode);
          ulrb_node_rotate_left(pathp->node, tnode);
          if(pathp == path) *proot = tnode;
          else
            if(pathp[-1].cmp < 0) ulrb_node_set_left(pathp[-1].node, tnode);
            else ulrb_node_set_right(pathp[-1].node, tnode);
          return del;
        } else {
          /*
           *        ||
           *     pathp(b)
           *   //        \
           *  (b)        (b)
           *             /
           *           (b)
           */
          ulrb_node_set_red(pathp->node);
          ulrb_node_rotate_left(pathp->node, tnode);
          pathp->node = tnode;
        }
      }
    } else {
      ulrb_node_t* left = ulrb_node_get_left(pathp->node);
      ulrb_node_t* tnode;

      ulrb_node_set_right(pathp->node, pathp[1].node);
      if(ulrb_node_get_color(left) == 1) {
        ulrb_node_t* leftright = ulrb_node_get_right(left);
        ulrb_node_t* leftrightleft = ulrb_node_get_left(leftright);
        if(leftrightleft && ulrb_node_get_color(leftrightleft) == 1) {
          /*
           *        ||
           *     pathp(b)
           *    /        \\
           *  (r)        (b)
           *    \
           *    (b)
           *    /
           *  (r)
           */
          ulrb_node_t* unode;
          ulrb_node_set_black(leftrightleft);
          ulrb_node_rotate_right(pathp->node, unode);
          ulrb_node_rotate_right(pathp->node, tnode);
          ulrb_node_set_right(unode, tnode);
          ulrb_node_rotate_left(unode, tnode);
        } else {
          /*
           *        ||
           *     pathp(b)
           *    /        \\
           *  (r)        (b)
           *    \
           *    (b)
           *    /
           *  (b)
           */
          assert(leftright);
          ulrb_node_set_red(leftright);
          ulrb_node_rotate_right(pathp->node, tnode);
          ulrb_node_set_black(tnode);
        }

        if(pathp == path) *proot = tnode;
        else
          if(pathp[-1].cmp < 0) ulrb_node_set_left(pathp[-1].node, tnode);
          else ulrb_node_set_right(pathp[-1].node, tnode);
        return del;
      } else if(ulrb_node_get_color(pathp->node) == 1) {
        ulrb_node_t* leftleft = ulrb_node_get_left(left);
        if(leftleft && ulrb_node_get_color(leftleft) == 1) {
          /*
           *          ||
           *       pathp(r)
           *      /        \\
           *    (b)        (b)
           *    /
           *  (r)
           */
          ulrb_node_set_black(pathp->node);
          ulrb_node_set_red(left);
          ulrb_node_set_black(leftleft);
          ulrb_node_rotate_right(pathp->node, tnode);
          assert(pathp > path);
          if(pathp[-1].cmp < 0) ulrb_node_set_left(pathp[-1].node, tnode);
          else ulrb_node_set_right(pathp[-1].node, tnode);
          return del;
        } else {
          /*
           *          ||
           *       pathp(r)
           *      /        \\
           *    (b)        (b)
           *    /
           *  (b)
           */
          ulrb_node_set_red(left);
          ulrb_node_set_black(pathp->node);
          return del;
        }
      } else {
        ulrb_node_t* leftleft = ulrb_node_get_left(left);
        if(leftleft && ulrb_node_get_color(leftleft) == 1) {
          /*
           *          ||
           *       pathp(b)
           *      /        \\
           *    (b)        (b)
           *    /
           *  (r)
           */
          ulrb_node_set_black(leftleft);
          ulrb_node_rotate_right(pathp->node, tnode);
          if(pathp == path) *proot = tnode;
          else
            if(pathp[-1].cmp < 0) ulrb_node_set_left(pathp[-1].node, tnode);
            else ulrb_node_set_right(pathp[-1].node, tnode);
          return del;
        } else {
          /*
           *          ||
           *       pathp(b)
           *      /        \\
           *    (b)        (b)
           *    /
           *  (b)
           */
          ulrb_node_set_red(left);
        }
      }
    }
  }

  *proot = path->node;
  assert(ulrb_node_get_color(path->node) == 0);
  return del;
}

ul_hapi void ulrb_destroy_node(ulrb_node_t* x, void (*destructor)(void* opaque, ulrb_node_t* x), void* opaque) {
  ulrb_node_t* y;
  while(x) {
    y = ulrb_node_get_left(x);
    ulrb_destroy_node(ulrb_node_get_right(x), destructor, opaque);
    destructor(opaque, x);
    x = y;
  }
}
ul_hapi void ulrb_destroy(ulrb_node_t** proot, void (*destructor)(void* opaque, ulrb_node_t* x), void* opaque) {
  ulrb_destroy_node(*proot, destructor, opaque);
  *proot = NULL;
}

ul_hapi size_t ulrb_count(const ulrb_node_t* x) {
  const ulrb_node_t* path[ULRB_MAX_DEPTH];
  const ulrb_node_t** pathp = path;
  size_t cnt = 0;

  while(x) {
    *pathp++ = x;
    x = ulrb_node_get_left(x);
  }
  while(pathp != path) {
    ++cnt;
    x = *--pathp;
    x = ulrb_node_get_right(x);
    while(x) {
      *pathp++ = x;
      x = ulrb_node_get_left(x);
    }
  }
  return cnt;
}

typedef void (*ulrb_walk_func_t)(void* opaque, const ulrb_node_t* x);
ul_hapi void ulrb_walk_preorder_iteration(const ulrb_node_t* x, ulrb_walk_func_t func, void* opaque) {
  const ulrb_node_t* path[ULRB_MAX_DEPTH];
  const ulrb_node_t** pathp = path;

  while(x) {
    *pathp++ = x;
    func(opaque, x);
    x = ulrb_node_get_left(x);
  }
  while(pathp != path) {
    x = *--pathp;
    x = ulrb_node_get_right(x);
    while(x) {
      *pathp++ = x;
      func(opaque, x);
      x = ulrb_node_get_left(x);
    }
  }
}
ul_hapi void ulrb_walk_preorder_recursive(const ulrb_node_t* x, ulrb_walk_func_t func, void* opaque) {
  while(x) {
    func(opaque, x);
    ulrb_walk_preorder_recursive(ulrb_node_get_left(x), func, opaque);
    x = ulrb_node_get_right(x);
  }
}
ul_hapi void ulrb_walk_preorder_morris(ulrb_node_t* x, ulrb_walk_func_t func, void* opaque) {
  ulrb_node_t* y = NULL;
  ulrb_node_t* z;
  while(x) {
    y = ulrb_node_get_left(x);
    if(y) {
      while((z = ulrb_node_get_right(y)) && z != x) y = z;
      if(z == NULL) {
        func(opaque, x);
        ulrb_node_set_right(y, x);
        x = ulrb_node_get_left(x);
        continue;
      } else ulrb_node_set_right(y, NULL);
    } else func(opaque, x);
    x = ulrb_node_get_right(x);
  }
}

ul_hapi void ulrb_walk_inorder_iteration(const ulrb_node_t* x, ulrb_walk_func_t func, void* opaque) {
  const ulrb_node_t* path[ULRB_MAX_DEPTH];
  const ulrb_node_t** pathp = path;

  while(x) {
    *pathp++ = x;
    x = ulrb_node_get_left(x);
  }
  while(pathp != path) {
    x = *--pathp;
    func(opaque, x);
    x = ulrb_node_get_right(x);
    while(x) {
      *pathp++ = x;
      x = ulrb_node_get_left(x);
    }
  }
}
ul_hapi void ulrb_walk_inorder_recursive(const ulrb_node_t* x, ulrb_walk_func_t func, void* opaque) {
  while(x) {
    ulrb_walk_inorder_recursive(ulrb_node_get_left(x), func, opaque);
    func(opaque, x);
    x = ulrb_node_get_right(x);
  }
}
ul_hapi void ulrb_walk_inorder_morris(ulrb_node_t* x, ulrb_walk_func_t func, void* opaque) {
  ulrb_node_t* y = NULL;
  ulrb_node_t* z;
  while(x) {
    y = ulrb_node_get_left(x);
    if(y) {
      while((z = ulrb_node_get_right(y)) && z != x) y = z;
      if(z == NULL) {
        ulrb_node_set_right(y, x);
        x = ulrb_node_get_left(x);
        continue;
      } else ulrb_node_set_right(y, NULL);
    }
    func(opaque, x);
    x = ulrb_node_get_right(x);
  }
}

ul_hapi void ulrb_walk_postorder_iteration(const ulrb_node_t* x, ulrb_walk_func_t func, void* opaque) {
  const ulrb_node_t* path[ULRB_MAX_DEPTH];
  const ulrb_node_t** pathp = path;
  const ulrb_node_t* prev = NULL;
  const ulrb_node_t* right;

  while(x) {
    *pathp++ = x;
    x = ulrb_node_get_left(x);
  }
  while(pathp != path) {
    x = pathp[-1];
    right = ulrb_node_get_right(x);
    if(!right || right == prev) {
      --pathp;
      func(opaque, x);
      prev = x;
      x = NULL;
    } else {
      x = right;
      while(x) {
        *pathp++ = x;
        x = ulrb_node_get_left(x);
      }
    }
  }
}
ul_hapi void ulrb_walk_postorder_recursive(const ulrb_node_t* x, ulrb_walk_func_t func, void* opaque) {
  if(x) {
    ulrb_walk_postorder_recursive(ulrb_node_get_left(x), func, opaque);
    ulrb_walk_postorder_recursive(ulrb_node_get_right(x), func, opaque);
    func(opaque, x);
  }
}
ul_hapi void _ulrb_walk_postorder_morris_post(ulrb_node_t* x, ulrb_walk_func_t func, void* opaque) {
  ulrb_node_t* rev;

  /* reverse */
  rev = NULL;
  while(x) {
    ulrb_node_t* next = ulrb_node_get_right(x);
    ulrb_node_set_right(x, rev);
    rev = x;
    x = next;
  }

  /* travel */
  x = rev;
  while(x) {
    func(opaque, x);
    x = ulrb_node_get_right(x);
  }

  /* reverse back */
  x = rev;
  rev = NULL;
  while(x) {
    ulrb_node_t* next = ulrb_node_get_right(x);
    ulrb_node_set_right(x, rev);
    rev = x;
    x = next;
  }
}
ul_hapi void ulrb_walk_postorder_morris(ulrb_node_t* x, ulrb_walk_func_t func, void* opaque) {
  ulrb_node_t* y = NULL;
  ulrb_node_t* const root = x;
  ulrb_node_t* z;
  while(x) {
    y = ulrb_node_get_left(x);
    if(y) {
      while((z = ulrb_node_get_right(y)) && z != x) y = z;
      if(!z) {
        ulrb_node_set_right(y, x);
        x = ulrb_node_get_left(x);
        continue;
      } else {
        ulrb_node_set_right(y, NULL);
        _ulrb_walk_postorder_morris_post(ulrb_node_get_left(x), func, opaque);
      }
    }
    x = ulrb_node_get_right(x);
  }
  _ulrb_walk_postorder_morris_post(root, func, opaque);
}

ul_hapi void ulrb_walk_preorder(const ulrb_node_t* x, ulrb_walk_func_t func, void* opaque) {
  ulrb_walk_preorder_iteration(x, func, opaque);
}
ul_hapi void ulrb_walk_inorder(const ulrb_node_t* x, ulrb_walk_func_t func, void* opaque) {
  ulrb_walk_inorder_iteration(x, func, opaque);
}
ul_hapi void ulrb_walk_postorder(const ulrb_node_t* x, ulrb_walk_func_t func, void* opaque) {
  ulrb_walk_postorder_iteration(x, func, opaque);
}

typedef ulrb_node_t* (*ulrb_copy_func_t)(void* opaque, const ulrb_node_t* node);
ul_hapi ulrb_node_t* ulrb_copy(const ulrb_node_t* x, ulrb_copy_func_t func, void* opaque) {
  ulrb_node_t* ret;
  ulrb_node_t* y;
  if(!x) return NULL;
  y = ret = func(opaque, x);
  ret->right = ulrb_copy(ulrb_node_get_right(x), func, opaque);
  ulrb_node_set_color(ret, ulrb_node_get_color(x));
  while((x = ulrb_node_get_left(x))) {
    y = (y->left = func(opaque, x));
    y->right = ulrb_copy(ulrb_node_get_right(x), func, opaque);
    ulrb_node_set_color(ret, ulrb_node_get_color(x));
  }
  return ret;
}

typedef struct ulrb_iter_t {
  const ulrb_node_t* path[ULRB_MAX_DEPTH];
  const ulrb_node_t** pathp;
  const ulrb_node_t* todo;
  const ulrb_node_t* root;
} ulrb_iter_t;

/* Note: iterator isn't safe when Red-Black tree is modified */
ul_hapi void ulrb_iter_init(ulrb_iter_t* iter, const ulrb_node_t* x) {
  iter->pathp = iter->path;
  iter->root = iter->todo = x;
}
ul_hapi const ulrb_node_t* ulrb_iter_next(ulrb_iter_t* iter) {
  const ulrb_node_t* x;
  const ulrb_node_t* ret = NULL;
  const ulrb_node_t** pathp = iter->pathp;

  if(pathp == iter->path) {
    if(iter->todo) {
      x = iter->todo; iter->todo = NULL;
      while(x) {
        *pathp++ = x;
        x = ulrb_node_get_left(x);
      }
    } else iter->todo = iter->root;
  }
  if(ul_likely(pathp != iter->path)) {
    x = *--pathp;
    ret = x;
    x = ulrb_node_get_right(x);
    while(x) {
      *pathp++ = x;
      x = ulrb_node_get_left(x);
    }
  }
  iter->pathp = pathp;
  return ret;
}
ul_hapi const ulrb_node_t* ulrb_iter_prev(ulrb_iter_t* iter) {
  const ulrb_node_t* x;
  const ulrb_node_t* ret = NULL;
  const ulrb_node_t** pathp = iter->pathp;

  if(pathp == iter->path) {
    if(iter->todo) {
      x = iter->todo; iter->todo = NULL;
      while(x) {
        *pathp++ = x;
        x = ulrb_node_get_right(x);
      }
    } else iter->todo = iter->root;
  }
  if(ul_likely(pathp != iter->path)) {
    x = *--pathp;
    ret = x;
    x = ulrb_node_get_left(x);
    while(x) {
      *pathp++ = x;
      x = ulrb_node_get_right(x);
    }
  }
  iter->pathp = pathp;
  return ret;
}
ul_hapi int ulrb_iter_isend(const ulrb_iter_t* iter) {
  return iter->pathp == iter->path && iter->todo != NULL;
}

#endif /* ULRB_H */
