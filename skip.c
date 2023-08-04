/* Copyright © 2023 Ratakor. ISC License */

#include <stdlib.h>

#include "skip.h"

#define MAXLVL    32
#define THRESHOLD RAND_MAX * 0.25

struct _SkipNode {
	SkipKey key;
	SkipVal val;
#ifdef SKIP_DOUBLY
	struct _SkipNode *bwd;
#endif /* SKIP_DOUBLY */
	struct _SkipNode *fwd[];
};

struct SkipList {
	struct _SkipNode *hdr;
	size_t siz;
	int lvl;
};

SkipList *
skip_create(void)
{
	SkipList *list;

	list = malloc(sizeof(*list));
	if (list == NULL)
		return NULL;

	list->siz = 0;
	list->lvl = 1;
	list->hdr = calloc(1, sizeof(SkipNode) + MAXLVL * sizeof(SkipNode *));
	if (list->hdr == NULL) {
		free(list);
		return NULL;
	}

	return list;
}

void
skip_destroy(SkipList *list)
{
	struct _SkipNode *node, *next;

	for (node = list->hdr; node; node = next) {
		next = node->fwd[0];
		free(node);
	}
	free(list);
}

SkipNode *
skip_insert(SkipList *list, SkipKey key, SkipVal val)
{
	struct _SkipNode *update[MAXLVL], *node = list->hdr;
	int lvl, i;

	for (i = list->lvl - 1; i >= 0; i--) {
		while (node->fwd[i] && node->fwd[i]->key < key)
			node = node->fwd[i];
		update[i] = node;
	}

	node = node->fwd[0];
	if (node && node->key == key) {
		node->val = val;
		return (SkipNode *)node;
	}

	for (lvl = 1; rand() < THRESHOLD; lvl++);
	if (lvl > list->lvl) {
		if (lvl > MAXLVL)
			lvl = MAXLVL;
		for (i = list->lvl; i < lvl; i++)
			update[i] = list->hdr;
		list->lvl = lvl;
	}

	node = malloc(sizeof(*node) + (size_t)lvl * sizeof(SkipNode *));
	if (node == NULL)
		return NULL;
	node->key = key;
	node->val = val;

	for (i = 0; i < lvl; i++) {
		node->fwd[i] = update[i]->fwd[i];
		update[i]->fwd[i] = node;
	}

#ifdef SKIP_DOUBLY
	node->bwd = (update[0] == list->hdr) ? NULL : update[0];

	if (node->fwd[0])
		node->fwd[0]->bwd = node;
	else
		list->hdr->bwd = node;
#endif /* SKIP_DOUBLY */

	list->siz++;

	return (SkipNode *)node;
}

static inline void
skip_delnode(SkipList *list, struct _SkipNode *node, struct _SkipNode **update)
{
	int i;

	for (i = 0; i < list->lvl; i++) {
		if (update[i]->fwd[i] != node)
			break;
		update[i]->fwd[i] = node->fwd[i];
	}

#ifdef SKIP_DOUBLY
	if (node->fwd[0])
		node->fwd[0]->bwd = node->bwd;
	else
		list->hdr->bwd = node->bwd;
#endif /* SKIP_DOUBLY */

	while (list->lvl > 1 && list->hdr->fwd[list->lvl - 1] == NULL)
		list->lvl--;
	list->siz--;
	free(node);
}

bool
skip_delete(SkipList *list, SkipKey key)
{
	struct _SkipNode *update[MAXLVL], *node = list->hdr;
	int i;

	for (i = list->lvl - 1; i >= 0; i--) {
		while (node->fwd[i] && node->fwd[i]->key < key)
			node = node->fwd[i];
		update[i] = node;
	}

	node = node->fwd[0];
	if (node && node->key == key) {
		skip_delnode(list, node, update);
		return true;
	}

	return false;
}

size_t
skip_delrange(SkipList *list, SkipKey from, SkipKey to)
{
	struct _SkipNode *update[MAXLVL], *node = list->hdr, *next;
	size_t n;
	int i;

	for (i = list->lvl - 1; i >= 0; i--) {
		while (node->fwd[i] && node->fwd[i]->key < from)
			node = node->fwd[i];
		update[i] = node;
	}

	node = node->fwd[0];
	for (n = 0; node && node->key < to; n++) {
		next = node->fwd[0];
		skip_delnode(list, node, update);
		node = next;
	}

	return n;
}

SkipNode *
skip_search(SkipList *list, SkipKey key)
{
	struct _SkipNode *node;

	node = (struct _SkipNode *)skip_approx(list, key);
	if (node && node->key == key)
		return (SkipNode *)node;

	return NULL;
}

SkipNode *
skip_approx(SkipList *list, SkipKey key)
{
	struct _SkipNode *node = list->hdr;
	int i;

	for (i = list->lvl - 1; i >= 0; i--) {
		while (node->fwd[i] && node->fwd[i]->key < key)
			node = node->fwd[i];
	}

	return (SkipNode *)node->fwd[0];
}

size_t
skip_size(SkipList *list)
{
	return list->siz;
}

SkipNode *
skip_first(SkipList *list)
{
	return (SkipNode *)list->hdr->fwd[0];
}

#ifdef SKIP_DOUBLY
SkipNode *
skip_last(SkipList *list)
{
	return (SkipNode *)list->hdr->bwd;
}
#endif /* SKIP_DOUBLY */
