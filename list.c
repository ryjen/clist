#include <assert.h>
#include <clist/list.h>
#include "internal.h"

#define clist_assert_vtable(list, fun) assert((list)->vtable->fun != NULL)

/*
 * call a vtable function with zero arguments
 */
#define clist_vtable0(list, fun) ((list)->vtable->fun)((list))

/*
 * call a vtable function with one argument
 */
#define clist_vtable1(list, fun, arg) ((list)->vtable->fun)((list), (arg))

/*
 * call a vtable function with two arguments
 */
#define clist_vtable2(list, fun, arg1, arg2) ((list)->vtable->fun)((list), (arg1), (arg2))

/**
 * creates a new list
 * @return an allocated list object
 */
Clist *clist_new_single() {
    Clist *list = malloc(sizeof(Clist));

    list->vtable = clist_single_vtable();

    assert(list->vtable != NULL);

    clist_assert_vtable(list, create);

    list->impl = clist_vtable0(list, create);

    return list;
}

/**
 * destroys a created list
 * @param list the list instance
 */
void clist_delete(Clist *list) {
    assert(list != NULL);

    clist_assert_vtable(list, destroy);

    clist_vtable0(list, destroy);

    free(list);
}

/**
 * prepends a list item to the list
 * @param list the list instance
 * @param item the item to add to the list
 * @see rj_list_item_create
 */
void clist_add(Clist *list, ClistItem *item) {
    assert(list != NULL);

    clist_assert_vtable(list, add);

    clist_vtable1(list, add, item);
}

/**
 * adds an item to the list after the specific index
 * @param list  the list instance
 * @param index the index to add after
 * @param item  the item to add
 * @see rj_list_item_create
 */
void clist_add_index(Clist *list, size_t index, ClistItem *item) {
    assert(list != NULL);

    clist_assert_vtable(list, add_index);

    clist_vtable2(list, add_index, index, item);
}

/**
 * adds one list to another
 * if the items in the other list have an allocator and a copier,
 * then a copy will be made.
 * @param list  the list instance
 * @param other the list to add from
 */
void clist_add_all(Clist *list, const Clist *other) {
    assert(list != NULL);

    clist_assert_vtable(list, add_all);

    clist_vtable1(list, add_all, other);
}

/**
 * adds one list to another after the specified index
 * if the items in the other list have an allocator and a copier,
 * then a copy will be made.
 * @param list  the list instance
 * @param index the index to add after
 * @param other the list to add from
 */
void clist_add_all_index(Clist *list, size_t index, const Clist *other) {
    assert(list != NULL);

    clist_assert_vtable(list, add_all_index);

    clist_vtable2(list, add_all_index, index, other);
}

/**
 * removes all items in the list
 * if the items have a destructor set, it will be called
 * @param list the list instance
 */
void clist_clear(Clist *list) {
    assert(list != NULL);

    clist_assert_vtable(list, clear);

    clist_vtable0(list, clear);
}

/**
 * tests if a list contains an item
 * items in the list must have a compare function set
 * @param  list the list instance
 * @param  item the item (memory) to check for
 * @return      zero if not found, positive if found
 */
int clist_contains(const Clist *list, const void *item) {
    assert(list != NULL);

    clist_assert_vtable(list, contains);

    return clist_vtable1(list, contains, item);
}

/**
 * tests if a list contains all items in another list
 * items in the list must have a compare function set
 * @param  list  the list instance
 * @param  other the list to check
 * @return       zero if nothing found, otherwise the number of items found
 */
int clist_contains_all(const Clist *list, const Clist *other) {
    assert(list != NULL);

    clist_assert_vtable(list, contains_all);

    return clist_vtable1(list, contains_all, other);
}

/**
 * gets the data (memory) from a list
 * @param  list  the list instance
 * @param  index the index of the data
 * @return       bytes of the item
 * @see rj_list_get_size to get the size of the data
 */
void *clist_get(const Clist *list, size_t index) {
    assert(list != NULL);

    clist_assert_vtable(list, get);

    return clist_vtable1(list, get, index);
}

/**
 * removes an item from a list
 * items must have a compare function set
 * @param  list the list instance
 * @param  item the item to remove
 * @return      zero if nothing was removed, otherwise a positive value
 */
int clist_remove(Clist *list, const void *item) {
    assert(list != NULL);

    clist_assert_vtable(list, remove);

    return clist_vtable1(list, remove, item);
}

/**
 * removes an index from a list
 * @param  list  the list instance
 * @param  index the index to remove
 * @return       positive value if the index was removed
 */
int clist_remove_index(Clist *list, size_t index) {
    assert(list != NULL);

    clist_assert_vtable(list, remove_index);

    return clist_vtable1(list, remove_index, index);
}

/**
 * removes a list from a list
 * items must have a compare function set
 * @param  list  the list instance
 * @param  other the list to remove
 * @return       zero if nothing removed, otherwise the number of items removed
 */
int clist_remove_all(Clist *list, const Clist *other) {
    assert(list != NULL);

    clist_assert_vtable(list, remove_all);

    return clist_vtable1(list, remove_all, other);
}

/**
 * gets the index of an item in a list
 * @param  list the list instance
 * @param  item the item to find
 * @return      the index of the item in the list
 */
int clist_index_of(const Clist *list, const void *item) {
    assert(list != NULL);

    clist_assert_vtable(list, index_of);

    return clist_vtable1(list, index_of, item);
}

/**
 * sets (replaces) an item in a list.  the existing item will be destroyed.
 * @param list  the list instance
 * @param index the index to set
 * @param item  the item to set
 */
void clist_set(Clist *list, size_t index, ClistItem *item) {
    assert(list != NULL);

    clist_assert_vtable(list, set);

    clist_vtable2(list, set, index, item);
}

/**
 * gets the size of a list
 * @param  list the list instance
 * @return      the size (number of items)
 */
size_t clist_size(const Clist *list) {
    assert(list != NULL);

    clist_assert_vtable(list, size);

    return clist_vtable0(list, size);
}

/**
 * tests if a list is empty
 * @param  list the list instance
 * @return      a positive value if the list is empty, otherwise zero
 */
int clist_is_empty(const Clist *list) {
    assert(list != NULL);

    clist_assert_vtable(list, is_empty);

    return clist_vtable0(list, is_empty);
}

/**
 * sorts the list based on the comparator
 * NOTE: the implementation is subject to change
 * @param  list the list instance
 */
void clist_sort(Clist *list) {
    assert(list != NULL);

    clist_assert_vtable(list, sort);

    clist_vtable0(list, sort);
}
