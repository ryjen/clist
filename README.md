
clist
======

a linked list implementation

Features:

- virtual table for implementations (or custom)
- allocated, static or transient memory capable
- designed with public/private api
- single linked (double, ordered, circular or dynamic array) implementation(s)

examples:

### creating a list
```c
Clist *list = clist_new_single();
// list = clist_create_double();
```

### create a list item
```c
size_t data_size = ...
void *data = ...

/* create an item using stdlib memory function */
ClistItem *item = clist_item_new(data, data_size, memcmp);

/* create an item with no memory functions, data will not be destroyed or copied */
item = clist_item_new_static(data, data_size, memcmp);

/* create an item with custom memory functions */
item = clist_item_new_transient(data, data_size, memcmp, malloc, free, memmove);
```

### add some data:
```c
clist_add(list, item);

clist_add_index(list, 1, item);

clist_set(list, 1, item);

clist_add_all(list, other_list);

clist_add_all_index(list, 1, other_list);
```

### remove some data
```c
clist_remove(list, data);

clist_remove_index(list, 1);

clist_remove_all(list, other_list);

clist_clear(list);
```

### some data exist?
```c
bool val = clist_contains(list, data);

val = clist_contains_all(list, other_list);
```

### get some data
```c
void *data = clist_get(list, 1);

size_t size = clist_get_size(list, 1);
```

### properties
```c
size_t list_size = clist_size(list);

int index = clist_index_of(list, data);

bool is_empty = clist_is_empty(list);
```

### sorting (mutable)
```c
clist_sort(list);
```


## TODO

- [x] unit tests
- [x] single linked list implementation
- [ ] double linked list implementation
- [ ] dynamic array list implementation
- [ ] ordered list implementation
- [ ] circular list implementation

