/* Copyright © 2023 Ratakor. See LICENSE file for license details. */

#ifndef SKIPLIST_H
#define SKIPLIST_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef int Key;
typedef int Val;

typedef struct Node Node;
struct Node {
	Key key;
	Val val;
	Node *fwd[];
};

typedef struct {
	unsigned int maxlvl;
	unsigned int lvl;
	Node *header;
} SkipList;

SkipList *sl_create(unsigned int maxlvl, unsigned int seed);
void sl_destroy(SkipList *list);
int sl_insert(SkipList *list, Key key, Val val);
int sl_remove(SkipList *list, Key key);
Node *sl_search(SkipList *list, Key key);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SKIPLIST_H */
