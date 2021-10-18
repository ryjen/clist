#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <clist/list-item.h>
#include <clist/list-vtable.h>
#include "internal.h"

typedef struct __clist_slist ClistSList;

typedef struct __clist_slist_node ClistSListNode;

struct __clist_slist_node {
    ClistSListNode *next;
    ClistItem *item;
};

struct __clist_slist {
    ClistSListNode *first;
    size_t size;
};

extern void clist_single_clear(Clist *list);

static inline ClistSList *__clist_slist_impl(const Clist *arg) {
    assert(arg->impl != NULL);
    return (ClistSList *) arg->impl;
}

static ClistSListNode *__clist_slist_node_create(ClistItem *item) {
    ClistSListNode *node = NULL;
    assert(item != NULL);
    node = malloc(sizeof(ClistSListNode));
    assert(node != NULL);
    node->next = NULL;
    node->item = item;
    return node;
}

static void __clist_slist_node_destroy(ClistSListNode *node) {
    assert(node != NULL);

    if (node->item != NULL) {
        clist_item_delete(node->item);
    }

    free(node);
}

void *clist_single_new() {
    ClistSList *list = malloc(sizeof(ClistSList));
    assert(list != NULL);
    list->first = NULL;
    list->size = 0;
    return list;
}

void clist_single_delete(Clist *list) {
    assert(list != NULL);

    clist_single_clear(list);

    free(__clist_slist_impl(list));
}

static void __clist_slist_node_insert_after(ClistSListNode *node, ClistItem *item) {
    ClistSListNode *other = NULL;

    assert(node != NULL);
    assert(item != NULL);

    other = __clist_slist_node_create(item);

    other->next = node->next;
    node->next = other;
}

static ClistSListNode *__clist_slist_get_node(const ClistSList *list, size_t index) {
    ClistSListNode *node = NULL;
    size_t pos = 0;

    assert(list != NULL);

    for (node = list->first; node; node = node->next, pos++) {
        if (index == pos) {
            return node;
        }
    }
    return NULL;
}

static void __clist_slist_add_node(ClistSList *list, ClistSListNode *node) {
    assert(list != NULL);
    assert(node != NULL);

    node->next = list->first;
    list->first = node;
    list->size++;
}

void clist_single_add(Clist *list, ClistItem *item) {
    ClistSListNode *node = NULL;

    assert(list != NULL);
    assert(item != NULL);

    node = __clist_slist_node_create(item);

    __clist_slist_add_node(__clist_slist_impl(list), node);
}

void clist_single_add_index(Clist *list, size_t index, ClistItem *item) {
    ClistSList *impl = NULL;
    ClistSListNode *node = NULL;

    assert(list != NULL);
    assert(item != NULL);

    impl = __clist_slist_impl(list);

    node = __clist_slist_get_node(impl, index);

    if (node != NULL) {
        __clist_slist_node_insert_after(node, item);
        impl->size++;
    }
}

void clist_single_add_all(Clist *list, const Clist *other) {
    ClistSList *impl = NULL;
    ClistSListNode *node = NULL;

    assert(list != NULL);
    assert(other != NULL);

    impl = __clist_slist_impl(other);

    for (node = impl->first; node; node = node->next) {
        clist_single_add(list, clist_item_copy(node->item));
    }
}

void clist_single_add_all_index(Clist *list, size_t index, const Clist *other) {
    ClistSList *impl = NULL;
    ClistSListNode *node = NULL, *other_node = NULL;

    assert(list != NULL);
    assert(other != NULL);

    node = __clist_slist_get_node(__clist_slist_impl(list), index);

    if (node == NULL) {
        return;
    }

    impl = __clist_slist_impl(other);

    for (other_node = impl->first; other_node; other_node = other_node->next) {
        clist_single_add_index(list, index, clist_item_copy(other_node->item));
    }
}

void clist_single_clear(Clist *list) {
    ClistSList *impl = NULL;
    ClistSListNode *node = NULL, *next_node = NULL;

    assert(list != NULL);

    impl = __clist_slist_impl(list);

    for (node = impl->first; node; node = next_node) {
        next_node = node->next;
        __clist_slist_node_destroy(node);
    }
    impl->first = NULL;
    impl->size = 0;
}

static ClistSListNode *__clist_slist_find_node_data(const ClistSList *list, const void *data) {
    ClistSListNode *node = NULL;

    assert(list != NULL);

    for (node = list->first; node; node = node->next) {
        ClistItem *item = node->item;

        if (item == NULL) {
            continue;
        }

        if (data == NULL && item->data == NULL) {
            return node;
        }

        if (clist_item_compare(item, data) == 0) {
            return node;
        }
    }
    return NULL;
}

int clist_single_contains(const Clist *list, const void *data) {
    ClistSList *impl = NULL;
    ClistSListNode *node = NULL;
    ClistItem *item = NULL;

    if (list == NULL) {
        return 0;
    }

    node = __clist_slist_find_node_data(__clist_slist_impl(list), data);

    if (node == NULL) {
        return 0;
    }

    return 1;
}

int clist_single_contains_all(const Clist *list, const Clist *other) {
    ClistSList *impl = NULL;
    ClistSListNode *node = NULL;

    if (list == NULL || other == NULL) {
        return 0;
    }

    impl = __clist_slist_impl(other);

    if (impl->first == NULL) {
        return 0;
    }

    for (node = impl->first; node; node = node->next) {
        if (clist_contains(list, node->item->data)) {
            return 1;
        }
    }

    return 0;
}

void *clist_single_get(const Clist *list, size_t index) {
    ClistSListNode *node = NULL;

    if (list == NULL) {
        return NULL;
    }
    node = __clist_slist_get_node(__clist_slist_impl(list), index);

    if (node == NULL || node->item == NULL) {
        return NULL;
    }

    return node->item->data;
}

static void __clist_slist_node_unlink(ClistSList *list, ClistSListNode *node, ClistSListNode *prev) {
    if (list == NULL || node == NULL) {
        return;
    }

    if (list->first == node) {
        list->first = node->next;
        node->next = NULL;
        list->size--;
        return;
    }

    if (prev == NULL) {
        for (prev = list->first; prev; prev = prev->next) {
            if (prev->next == node) {
                break;
            }
        }
    }

    if (prev != NULL) {
        prev->next = node->next;
        node->next = NULL;
        list->size--;
    }
}

int clist_single_remove(Clist *list, const void *item) {
    ClistSList *impl = NULL;
    ClistSListNode *node = NULL;

    if (list == NULL) {
        return 0;
    }

    impl = __clist_slist_impl(list);

    node = __clist_slist_find_node_data(impl, item);

    if (node == NULL) {
        return 0;
    }

    __clist_slist_node_unlink(impl, node, NULL);

    __clist_slist_node_destroy(node);

    return 1;
}

int clist_single_remove_index(Clist *list, size_t index) {
    ClistSList *impl = NULL;
    ClistSListNode *node = NULL;

    if (list == NULL) {
        return 0;
    }

    impl = __clist_slist_impl(list);

    node = __clist_slist_get_node(impl, index);

    if (node == NULL) {
        return 0;
    }

    __clist_slist_node_unlink(impl, node, NULL);

    __clist_slist_node_destroy(node);

    return 1;
}

int clist_single_remove_all(Clist *list, const Clist *other) {
    ClistSList *impl = NULL, *oimpl = NULL;
    ClistSListNode *node = NULL, *found = NULL, *prev = NULL;
    int result = 0;

    if (list == NULL || other == NULL) {
        return 0;
    }

    impl = __clist_slist_impl(list);
    oimpl = __clist_slist_impl(other);

    for (node = oimpl->first; node; prev = node, node = node->next) {
        if (node->item == NULL) {
            continue;
        }

        found = __clist_slist_find_node_data(impl, node->item->data);

        if (found) {
            __clist_slist_node_unlink(impl, found, prev);

            __clist_slist_node_destroy(found);

            result++;
        }
    }

    return result;
}

int clist_single_index_of(const Clist *list, const void *data) {
    ClistSList *impl = NULL;
    ClistSListNode *node = NULL;
    int pos = 0;

    if (list == NULL) {
        return -1;
    }

    impl = __clist_slist_impl(list);

    for (node = impl->first; node; node = node->next, pos++) {
        ClistItem *item = node->item;

        if (item == NULL) {
            continue;
        }

        if (clist_item_compare(item, data) == 0) {
            return pos;
        }
    }

    return -1;
}

void clist_single_set(Clist *list, size_t index, ClistItem *item) {
    ClistSListNode *node = NULL;

    if (list == NULL) {
        return;
    }

    node = __clist_slist_get_node(__clist_slist_impl(list), index);

    if (node == NULL) {
        return;
    }

    clist_item_delete(node->item);

    node->item = item;
}

size_t clist_single_size(const Clist *list) {
    ClistSList *impl = NULL;

    if (list == NULL) {
        return 0;
    }

    impl = __clist_slist_impl(list);

    return impl->size;
}

int clist_single_is_empty(const Clist *list) {
    ClistSList *impl = NULL;
    assert(list != NULL);
    impl = __clist_slist_impl(list);
    return impl->size == 0 || impl->first == NULL;
}

static ClistSListNode *__clist_slist_merge_nodes(ClistSListNode *left, ClistSListNode *right) {
    ClistSListNode *result = NULL, *last = NULL;
    ClistSListNode *next = NULL;

    while (left && right) {
        // compare the two nodes
        if (clist_item_compare(left->item, right->item->data) <= 0) {
            // move the left up
            next = left->next;
            if (last == NULL) {
                left->next = result;
                result = left;
            } else {
                left->next = NULL;
                last->next = left;
            }
            last = left;
            left = next;
        } else {
            // move the right up
            next = right->next;
            if (last == NULL) {
                right->next = result;
                result = right;
            } else {
                right->next = NULL;
                last->next = right;
            }
            last = right;
            right = next;
        }
    }

    if (left) {
        // ensure left ends meet
        if (last == NULL) {
            result = left;
        } else {
            last->next = left;
        }
        // last might be needed in the next comparison
        last = left;
    }

    if (right) {
        // ensure right ends meet
        if (last == NULL) {
            result = left;
        } else {
            last->next = right;
        }
        last = right;
    }

    return result;
}

/**
 * standard merge sort, O(n log n)
 * TODO: more efficient algorithm
 */
static ClistSListNode *__clist_slist_merge_sort(ClistSListNode *first) {
    ClistSListNode *left = NULL;
    ClistSListNode *right = NULL;
    ClistSListNode *node = NULL, *node_next = NULL;
    size_t pos = 0;

    /* make sure we have 2 items at least */
    if (first == NULL || first->next == NULL) {
        return first;
    }

    /* this divides the list into two seperate lists.
     * just alternating each consecutive item
     */
    for (node = first; node; node = node_next, pos++) {
        node_next = node->next;
        if (pos % 2 != 0) {
            node->next = left;
            left = node;
        } else {
            node->next = right;
            right = node;
        }
    }

    left = __clist_slist_merge_sort(left);
    right = __clist_slist_merge_sort(right);

    return __clist_slist_merge_nodes(left, right);
}

void clist_single_sort(Clist *list) {
    ClistSList *impl = NULL;

    // Base case. A list of zero or one elements is sorted, by definition.
    if (clist_size(list) <= 1) {
        return;
    }

    impl = __clist_slist_impl(list);

    impl->first = __clist_slist_merge_sort(impl->first);
}

void clist_single_for_each(Clist *list, ClistCallback callback) {
    ClistSListNode *node = NULL;
    ClistSListNode *prev = NULL;
    ClistSList *impl = NULL;
    size_t index = 0;

    assert(list != NULL);
    assert(callback != NULL);

    impl = __clist_slist_impl(list);

    for (node = impl->first; node; prev = node, node = node->next) {
        if (callback(list, index++, node->item) == ClistIteratorDelete) {
            __clist_slist_node_unlink(impl, node, prev);

            __clist_slist_node_destroy(node);
        }
    }
}

static ClistVtable __clist_slist_vtable = {.create = clist_single_new,
        .destroy = clist_single_delete,
        .add = clist_single_add,
        .add_all = clist_single_add_all,
        .add_index = clist_single_add_index,
        .add_all_index = clist_single_add_all_index,
        .clear = clist_single_clear,
        .contains = clist_single_contains,
        .contains_all = clist_single_contains_all,
        .get = clist_single_get,
        .remove = clist_single_remove,
        .remove_index = clist_single_remove_index,
        .remove_all = clist_single_remove_all,
        .index_of = clist_single_index_of,
        .set = clist_single_set,
        .size = clist_single_size,
        .is_empty = clist_single_is_empty,
        .sort = clist_single_sort,
        .for_each = clist_single_for_each};

ClistVtable *clist_single_vtable() {
    return &__clist_slist_vtable;
}
