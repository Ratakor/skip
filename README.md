skiplist.c
==========
A C99 implementation of [skiplists](https://wikipedia.org/wiki/Skip_list)

Usage
-----
Change Key and Val type in skiplist.h according to your key/val types and
that's it.

Notes
-----
sl_create() and sl_search() return NULL on error.
sl_insert() and sl_remove() return -1 on error.
Only the .val field of a Node could be modified manually otherwise use
provided functions.
