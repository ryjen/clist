#ifndef CLIST_ITEM_H
#define CLIST_ITEM_H

#include <stdlib.h>

/*
 * Callbacks for alloc, destry, compare and copy
 */
typedef void (*ClistDestroyCallback)(void *);

typedef void *(*ClistAllocCallback)(size_t);

typedef int (*ClistCompareCallback)(const void *, const void *, size_t);

typedef void *(*ClistCopyCallback)(void *, const void *, size_t);

/* public type for private implementation */
typedef struct __clist_item ClistItem;

/**
 * creates a list item suitable for adding to a list
 * this implementation will use the stdlib methods for dealing with memory:
 *    destroy = free
 *    copy = memmove
 *    compare = memcmp
 * @param  data the memory to use as a list item
 * @param  size the size of the data in bytes
 * @param  comparator the compare function, can be null
 * @return      a list item
 */
ClistItem *clist_item_new(void *data, size_t size, ClistCompareCallback comparator);

/**
 * creates a list item suitable for adding to a list
 * this implementation will not destroy, copy or compare memory
 * @param  data the memory to use as a list item
 * @param  size the size of the data in bytes
 * @param  comparator the compare function, can be NULL
 * @return      a list item
 */
ClistItem *clist_item_new_static(void *data, size_t size, ClistCompareCallback comparator);

/**
 * creates a list item suitable for adding to a list
 * this implementation you can pass functions to deal with the memory
 * @param  data       the memory to use as a list item
 * @param  size       the size of the data in bytes
 * @param  allocator  the function to allocate new memory (malloc)
 * @param  destructor the function to destroy the memory (free)
 * @param  copier     the function to copy the memory (memmove)
 * @param  comparator the function to compare the memory (memcpy)
 * @return            a list item
 */
ClistItem *clist_item_new_transient(void *data, size_t size, ClistCompareCallback comparator,
                                           ClistAllocCallback allocator, ClistDestroyCallback destructor,
                                           ClistCopyCallback copier);

/**
 * destroy a list item
 * will call the destructor function on the data if set
 * @param item the item instance
 */
void clist_item_delete(ClistItem *item);


/**
 * gets the data associated with a list item
 * @param item the list item with data
 * @return the list item data
 */
void *clist_item_data(const ClistItem *item);


/**
 * gets the size of the data associated with a list item
 * @param item the list item with data
 * @return the size of the item data
 */
size_t clist_item_size(const ClistItem *item);

/**
 * copies and item.  the allocator and copy functions will be called if set, otherwise the pointer will be assigned
 * @param  item the item instance
 * @return      a copy of the item
 */
ClistItem *clist_item_copy(const ClistItem *item);

/**
 * compares two items.  the compare function must be set on the item, otherwise memcmp will be used
 * @param  item the item instance
 * @param  data the data to compare
 * @return      zero if equal
 */
int clist_item_compare(const ClistItem *item, const void *data);

#endif
