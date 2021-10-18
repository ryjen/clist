#ifndef CLIST_INTERNAL_H
#define CLIST_INTERNAL_H

#include "list-vtable.h"

struct __clist {
    ClistVtable *vtable;
    void *impl;
};

struct __clist_item {
    void *data;
    size_t size;
    ClistAllocCallback allocator;
    ClistDestroyCallback destructor;
    ClistCopyCallback copier;
    ClistCompareCallback comparer;
};

/**
 * a singly linked list
 */
ClistVtable *__clist_single_vtable();

#endif
