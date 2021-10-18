
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include <clist/list-item.h>

ClistItem *clist_item_new(void *data, size_t size, ClistCompareCallback comparer) {
    return clist_item_new_transient(data, size, comparer, malloc, free, memmove);
}

ClistItem *clist_item_new_static(void *data, size_t size, ClistCompareCallback comparer) {
    return clist_item_new_transient(data, size, comparer, NULL, NULL, NULL);
}

ClistItem *clist_item_new_transient(void *data, size_t size,
                                           ClistCompareCallback comparer,
                                           ClistAllocCallback allocator, ClistDestroyCallback destructor,
                                           ClistCopyCallback copier) {
    ClistItem *item = malloc(sizeof(*item));
    assert(item != NULL);
    item->data = data;
    item->size = size;
    item->allocator = allocator;
    item->destructor = destructor;
    item->copier = copier;
    item->comparer = comparer;
    return item;
}

void clist_item_delete(struct __clist_item *item) {
    if (item == NULL) {
        return;
    }

    if (item->data && item->destructor) {
        (*item->destructor)(item->data);
    }

    free(item);
}

void *clist_item_data(const ClistItem *item) {
    return item->data;
}

size_t clist_item_size(const ClistItem *item) {
    return item->size;
}

ClistItem *clist_item_copy(const ClistItem *orig) {
    ClistItem *item = NULL;

    if (orig == NULL) {
        return NULL;
    }

    item = malloc(sizeof(ClistItem));
    assert(item != NULL);
    if (orig->copier && orig->allocator) {
        item->data = (*orig->allocator)(orig->size);
        assert(item->data != NULL);
        (*orig->copier)(item->data, orig->data, orig->size);
    } else {
        item->data = orig->data;
    }
    item->size = orig->size;
    item->allocator = orig->allocator;
    item->destructor = orig->destructor;
    item->copier = orig->copier;
    item->comparer = orig->comparer;

    return item;
}

int clist_item_compare(const ClistItem *item, const void *data) {
    assert(item != NULL);

    if (item->data == NULL && data == NULL) {
        return 0;
    }

    if (item->data == NULL || data == NULL) {
        return -1;
    }

    if (item->comparer) {
        return (*item->comparer)(item->data, data, item->size);
    } else {
        return memcmp(item->data, data, item->size);
    }
}
