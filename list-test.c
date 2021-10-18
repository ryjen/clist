#include <assert.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include <clist/list.h>

int run_list_tests();

int main(int argc, char *argv[])
{
  return run_list_tests();
}

static int create_test_list(void **state)
{
    Clist *list = clist_new_single();
    *state = list;

    return 0;
}

static int test_int_compare(const void *a, const void *b, size_t size)
{
    int *i1 = (int *)a;
    int *i2 = (int *)b;

    return *i1 - *i2;
}

static ClistItem *random_list_item()
{
    size_t size = sizeof(int);
    int *data = (int *)malloc(size);
    assert(data != NULL);
    *data = rand() % 1000;
    return clist_item_new(data, size, test_int_compare);
}

static int create_and_populate_test_list(void **state)
{
    Clist *list = clist_new_single();

    clist_add(list, random_list_item());

    clist_add(list, random_list_item());

    clist_add(list, random_list_item());

    *state = list;

    return 0;
}

static int destroy_test_list(void **state)
{
    Clist *list = (Clist *)*state;

    clist_delete(list);

    return 0;
}

static void test_list_add_valid(void **state)
{
    Clist *list = (Clist *)*state;

    clist_add(list, random_list_item());

    assert_int_equal(clist_size(list), 1);
}

static void test_list_add_invalid(void **state)
{
    Clist *list = (Clist *)*state;
    ClistItem *item = random_list_item();

    clist_add(NULL, NULL);

    clist_add(NULL, item);

    assert_int_equal(clist_size(list), 0);

    clist_item_delete(item);
}

static void test_list_add_index_valid(void **state)
{
    Clist *list = (Clist *)*state;

    size_t size = clist_size(list);

    ClistItem *item = random_list_item();

    void *data = NULL;

    assert_int_equal(size, 3);

    clist_add_index(list, 1, item);

    assert_int_equal(clist_size(list), 4);

    data = clist_get(list, 2);

    assert_non_null(data);

    assert_int_equal(clist_item_compare(item, data), 0);
}

static void test_list_add_index_invalid(void **state)
{
    Clist *list = (Clist *)*state;

    size_t size = clist_size(list);

    ClistItem *item = random_list_item();

    void *data = NULL;

    assert_int_equal(size, 3);

    clist_add_index(list, 100, item);

    clist_add_index(NULL, 1, item);

    assert_int_equal(clist_size(list), 3);

    clist_item_delete(item);
}

static void test_list_add_all_valid(void **state)
{
    Clist *list = (Clist *)*state;

    void *data = clist_get(list, 1);

    Clist *other = clist_new_single();

    clist_add(other, random_list_item());

    assert_int_equal(clist_size(other), 1);

    clist_add_all(other, list);

    assert_int_equal(clist_size(other), clist_size(list) + 1);

    assert_int_not_equal(clist_contains(other, data), 0);

    clist_delete(other);
}

static void test_list_add_all_invalid(void **state)
{
    Clist *list = (Clist *)*state;

    clist_add_all(list, NULL);

    clist_add_all(NULL, list);

    assert_int_equal(clist_size(list), 3);
}

static void test_list_add_all_index_valid(void **state)
{
    Clist *list = (Clist *)*state;

    Clist *other = clist_new_single();

    ClistItem *item = random_list_item();

    size_t size = clist_size(list);

    clist_add(other, item);

    clist_add(other, random_list_item());

    assert_int_equal(clist_size(other), 2);

    clist_add_all_index(list, 1, other);

    assert_int_equal(clist_size(list), clist_size(other) + size);

    assert_int_not_equal(clist_contains(list, clist_item_data(item)), 0);

    clist_delete(other);
}
static void test_list_add_all_index_invalid(void **state)
{
    Clist *list = (Clist *)*state;

    Clist *other = clist_new_single();

    clist_add(other, random_list_item());

    clist_add_all_index(list, 100, other);

    clist_add_all_index(NULL, 1, other);

    assert_int_equal(clist_size(list), 3);

    clist_delete(other);
}

static void test_list_clear_valid(void **state)
{
    Clist *list = (Clist *)*state;

    assert_int_equal(clist_size(list), 3);

    clist_clear(list);

    assert_int_equal(clist_size(list), 0);
}
static void test_list_clear_invalid(void **state)
{
    Clist *list = (Clist *)*state;

    clist_clear(NULL);
}

static void test_list_contains_valid(void **state)
{
    Clist *list = (Clist *)*state;

    ClistItem *item = random_list_item();

    clist_add(list, item);

    assert_int_equal(clist_size(list), 4);

    assert_int_not_equal(clist_contains(list, clist_item_data(item)), 0);
}

static void test_list_contains_invalid(void **state)
{
    Clist *list = (Clist *)*state;

    ClistItem *item = random_list_item();

    assert_int_equal(clist_contains(list, clist_item_data(item)), 0);

    assert_int_equal(clist_contains(list, NULL), 0);

    assert_int_equal(clist_contains(NULL, clist_item_data(item)), 0);

    assert_int_equal(clist_contains(NULL, NULL), 0);
}

static void test_list_contains_all_valid(void **state)
{
    Clist *list = (Clist *)*state;

    size_t size = clist_size(list);

    Clist *other = clist_new_single();

    clist_add(other, random_list_item());
    clist_add(other, random_list_item());

    clist_add_all(list, other);

    assert_int_equal(clist_size(list), size + clist_size(other));

    assert_int_not_equal(clist_contains_all(list, other), 0);

    clist_delete(other);
}

static void test_list_contains_all_invalid(void **state)
{
    Clist *list = (Clist *)*state;

    size_t size = clist_size(list);

    Clist *other = clist_new_single();

    clist_add(other, random_list_item());
    clist_add(other, random_list_item());

    assert_int_equal(clist_contains_all(list, other), 0);

    assert_int_equal(clist_contains_all(list, NULL), 0);

    assert_int_equal(clist_contains_all(NULL, other), 0);

    assert_int_equal(clist_contains_all(NULL, NULL), 0);
}

static void test_list_get_valid(void **state)
{
    Clist *list = (Clist *)*state;

    void *data = clist_get(list, 1);

    assert_non_null(data);
}
static void test_list_get_invalid(void **state)
{
    Clist *list = (Clist *)*state;

    void *data = clist_get(list, 100);

    assert_null(data);

    data = clist_get(NULL, 1);

    assert_null(data);
}

static void test_list_remove_valid(void **state)
{
    Clist *list = (Clist *)*state;

    void *data = NULL;

    assert_int_equal(clist_size(list), 3);

    data = clist_get(list, 1);

    assert_int_not_equal(clist_remove(list, data), 0);

    assert_int_equal(clist_size(list), 2);
}
static void test_list_remove_invalid(void **state)
{
    Clist *list = (Clist *)*state;

    ClistItem *item = random_list_item();

    assert_int_equal(clist_remove(list, clist_item_data(item)), 0);

    assert_int_equal(clist_size(list), 3);

    assert_int_equal(clist_remove(NULL, clist_item_data(item)), 0);

    clist_item_delete(item);
}

static void test_list_remove_index_valid(void **state)
{
    Clist *list = (Clist *)*state;

    assert_int_equal(clist_size(list), 3);

    assert_int_not_equal(clist_remove_index(list, 1), 0);

    assert_int_equal(clist_size(list), 2);
}

static void test_list_remove_index_invalid(void **state)
{
    Clist *list = (Clist *)*state;

    assert_int_equal(clist_remove_index(list, 100), 0);

    assert_int_equal(clist_remove_index(NULL, 1), 0);
}

static void test_list_remove_all_valid(void **state)
{
    Clist *list = (Clist *)*state;

    Clist *other = clist_new_single();

    clist_add(other, random_list_item());
    clist_add(other, random_list_item());

    assert_int_equal(clist_size(other), 2);

    clist_add_all(list, other);

    assert_int_equal(clist_size(list), 5);

    assert_int_equal(clist_remove_all(list, other), 2);

    assert_int_equal(clist_size(list), 3);

    clist_delete(other);
}
static void test_list_remove_all_invalid(void **state)
{
    Clist *list = (Clist *)*state;

    Clist *other = clist_new_single();

    assert_int_equal(clist_remove_all(list, other), 0);

    assert_int_equal(clist_size(list), 3);

    assert_int_equal(clist_remove_all(list, NULL), 0);

    assert_int_equal(clist_remove_all(NULL, other), 0);

    clist_delete(other);
}

static void test_list_index_of_valid(void **state)
{
    Clist *list = (Clist *)*state;

    void *data = clist_get(list, 1);

    assert_non_null(data);

    assert_int_equal(clist_index_of(list, data), 1);
}
static void test_list_index_of_invalid(void **state)
{
    Clist *list = (Clist *)*state;

    ClistItem *item = random_list_item();

    assert_int_equal(clist_index_of(list, clist_item_data(item)), -1);

    assert_int_equal(clist_index_of(NULL, clist_item_data(item)), -1);
}

static void test_list_set_valid(void **state)
{
    Clist *list = (Clist *)*state;

    ClistItem *item = random_list_item();

    void *data = NULL;

    assert_int_equal(clist_size(list), 3);

    clist_set(list, 1, item);

    assert_int_equal(clist_size(list), 3);

    data = clist_get(list, 1);

    assert_int_equal(clist_item_compare(item, data), 0);
}
static void test_list_set_invalid(void **state)
{
    Clist *list = (Clist *)*state;

    ClistItem *item = random_list_item();

    void *data = NULL;

    assert_int_equal(clist_size(list), 3);

    clist_set(list, 100, item);

    assert_int_equal(clist_size(list), 3);

    data = clist_get(list, 1);

    assert_int_not_equal(clist_item_compare(item, data), 0);

    clist_item_delete(item);
}

static void test_list_size_valid(void **state)
{
    Clist *list = (Clist *)*state;

    assert_int_equal(clist_size(list), 3);

    clist_add(list, random_list_item());

    assert_int_equal(clist_size(list), 4);
}
static void test_list_size_invalid(void **state)
{
    Clist *list = (Clist *)*state;

    assert_int_equal(clist_size(NULL), 0);
}

static void test_list_is_empty_valid(void **state)
{
    Clist *list = (Clist *)*state;

    assert_int_equal(clist_is_empty(list), 0);

    clist_clear(list);

    assert_int_not_equal(clist_is_empty(list), 0);
}
static void test_list_is_empty_invalid(void **state)
{
    Clist *list = (Clist *)*state;

    assert_int_not_equal(clist_is_empty(NULL), 0);
}

static void test_list_merge_sort_valid(void **state)
{
    Clist *list = (Clist *)*state;

    int index = 0;

    int values[] = {5, 7, 10, 3, 20, 4, 18, -1};

    int sorted_values[] = {-1, 3, 4, 5, 7, 10, 18, 20};

    size_t num_values = sizeof(values) / sizeof(values[0]);

    for (index = 0; index < num_values; index++) {
        clist_add(list, clist_item_new_static(&values[index], sizeof(int), test_int_compare));
    }

    assert_int_equal(clist_size(list), num_values);

    clist_sort(list);

    assert_int_equal(clist_size(list), num_values);

    for (index = 0; index < num_values; index++) {
        int *item = (int *)clist_get(list, index);
        assert_int_equal(*item, sorted_values[index]);
    }
}
static void test_list_sort_invalid(void **state)
{
    Clist *list = (Clist *)*state;

    clist_sort(NULL);
}

static void test_list_partition(void **state)
{
    Clist *list = (Clist *)*state;

    int partition = 5;

    int values[] = {3, 5, 8, 5, 10, 2, 1};

    int expected[] = {3, 1, 2, 10, 5, 5, 8};

    size_t num_values = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < num_values; i++) {
        clist_add(list, clist_item_new_static(&values[i], sizeof(int), test_int_compare));
    }

    assert_int_equal(clist_size(list), num_values);

    ClistItem *head;
}

int run_list_tests()
{
    const struct CMUnitTest valid_tests[] = {
        cmocka_unit_test_setup_teardown(test_list_add_valid, create_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_add_index_valid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_add_all_valid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_add_all_index_valid, create_and_populate_test_list,
                                        destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_clear_valid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_contains_valid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_contains_all_valid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_get_valid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_remove_valid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_remove_index_valid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_remove_all_valid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_index_of_valid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_set_valid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_size_valid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_is_empty_valid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_merge_sort_valid, create_test_list, destroy_test_list)};

    const struct CMUnitTest invalid_tests[] = {
        cmocka_unit_test_setup_teardown(test_list_add_invalid, create_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_add_index_invalid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_add_all_invalid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_add_all_index_invalid, create_and_populate_test_list,
                                        destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_clear_invalid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_contains_invalid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_contains_all_invalid, create_and_populate_test_list,
                                        destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_get_invalid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_remove_invalid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_remove_index_invalid, create_and_populate_test_list,
                                        destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_remove_all_invalid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_index_of_invalid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_set_invalid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_size_invalid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_is_empty_invalid, create_and_populate_test_list, destroy_test_list),
        cmocka_unit_test_setup_teardown(test_list_sort_invalid, create_test_list, destroy_test_list)};

    int rval = cmocka_run_group_tests_name("list valid tests", valid_tests, NULL, NULL);

    if (rval) {
        return rval;
    }

    return cmocka_run_group_tests_name("list invalid tests", invalid_tests, NULL, NULL);
}
