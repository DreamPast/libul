#include "ulrb.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct node_t {
    ulrb_node_t base;
    const char* key;
    int val;
} node_t;

static int node_comp(void* opaque, const void* lhs, const void* rhs) {
    (void)opaque; return strcmp(*(const char* const*)lhs, *(const char* const*)rhs);
}
static void node_walk(void* opaque, const ulrb_node_t* x) {
    (void)opaque; printf("[%s] = %d; ", ((const node_t*)x)->key, ((const node_t*)x)->val);
}
static void node_destroy(void* opaque, ulrb_node_t* x) {
    (void)opaque; free(x);
}

static ulrb_node_t* create_node(const char* key, int val) {
    node_t* node = (node_t*)malloc(sizeof(node_t));
    if(!node) exit(1);
    node->key = key;
    node->val = val;
    return (ulrb_node_t*)node;
}

static void insert_node(ulrb_node_t** proot, const char* key, int val) {
    ulrb_node_t* x = create_node(key, val);
    if(ulrb_insert(proot, x, node_comp, NULL) != x)
        node_destroy(NULL, x);
}

static void print_map(ulrb_node_t* root, const char* comment) {
    fputs(comment, stdout);
    ulrb_walk_inorder(root, node_walk, NULL);
    putchar('\n');
}

int main(void) {
    ulrb_node_t* root = NULL;
    const char* str;

    /* Insert */
    insert_node(&root, "A", 10);
    insert_node(&root, "B", 20);
    insert_node(&root, "C", 30);
    insert_node(&root, "D", 40);
    print_map(root, "Insert: ");

    /* Find and update */
    do {
        node_t* node;
        str = "C";
        node = (node_t*)ulrb_find(root, &str, node_comp, NULL);
        if(!node) printf("Find failed\n");
        else {
            node->val = -20;
            print_map(root, "Find and update: ");
        }
    } while(0);

    /* Remove */
    str = "C";
    node_destroy(NULL, ulrb_remove(&root, &str, node_comp, NULL));
    print_map(root, "Remove: ");

    /* Clear */
    ulrb_destroy(&root, node_destroy, NULL);
    print_map(root, "Clear: ");

    /* Count */
    insert_node(&root, "E", 50);
    insert_node(&root, "F", 60);
    insert_node(&root, "G", 70);
    insert_node(&root, "H", 80);
    printf("Count: %d\n", (int)ulrb_count(root));

    /* Walk */
    printf("Walk preorder: "); ulrb_walk_preorder(root, node_walk, NULL); putchar('\n');
    printf("Walk inorder: "); ulrb_walk_inorder(root, node_walk, NULL); putchar('\n');
    printf("Walk postorder: "); ulrb_walk_postorder(root, node_walk, NULL); putchar('\n');

    /* Find next and prev */
    do {
        ulrb_node_t* x;

        printf("Find next: ");
        x = ulrb_leftmost(root);
        do {
            node_walk(NULL, x);
        } while((x = ulrb_find_next(root, x + 1, node_comp, NULL)));
        putchar('\n');

        printf("Find prev: ");
        x = ulrb_rightmost(root);
        do {
            node_walk(NULL, x);
        } while((x = ulrb_find_prev(root, x + 1, node_comp, NULL)));
        putchar('\n');
    } while(0);

    /* Iterate */
    do {
        ulrb_iter_t iter;
        const ulrb_node_t* x;

        ulrb_iter_init(&iter, root);
        printf("Iterate forward: ");
        while((x = ulrb_iter_next(&iter))) node_walk(NULL, x);
        putchar('\n');

        printf("Iterate backward: ");
        while((x = ulrb_iter_prev(&iter))) node_walk(NULL, x);
        putchar('\n');

        printf("Iterator is in the end: %s\n", ulrb_iter_isend(&iter) ? "Yes" : "No");
    } while(0);

    return 0;
}
