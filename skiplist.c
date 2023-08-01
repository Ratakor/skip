/* Copyright © 2023 Ratakor. See LICENSE file for license details. */

#include <stdlib.h>

#include "skiplist.h"

SkipList *
sl_create(unsigned int maxlvl, unsigned int seed)
{
	SkipList *list;

	list = malloc(sizeof(*list));
	if (list == NULL)
		return NULL;

	list->maxlvl = maxlvl;
	list->lvl = 0;
	list->header = calloc(1, sizeof(Node) + (maxlvl + 1) * sizeof(Node *));
	if (list->header == NULL) {
		free(list);
		return NULL;
	}

	srand(seed);

	return list;
}

void
sl_destroy(SkipList *list)
{
	Node *np, *next;

	if (list == NULL)
		return;

	for (np = list->header; np; np = next) {
		next = np->fwd[0];
		free(np);
	}
	free(list);
}

int
sl_insert(SkipList *list, Key key, Val val)
{
	Node **update, *np = list->header;
	unsigned int lvl, i;

	update = malloc((list->maxlvl + 1) * sizeof(Node *));
	if (update == NULL)
		return -1;

	for (i = list->lvl; (int)i >= 0; i--) {
		while (np->fwd[i] && np->fwd[i]->key < key)
			np = np->fwd[i];
		update[i] = np;
	}

	np = np->fwd[0];
	if (np && np->key == key) {
		np->val = val;
		free(update);
		return 0;
	}

	for (lvl = 0; lvl < list->maxlvl && rand() < RAND_MAX / 2; lvl++);

	if (lvl > list->lvl) {
		for (i = list->lvl + 1; i < lvl + 1; i++)
			update[i] = list->header;
		list->lvl = lvl;
	}

	np = malloc(sizeof(*np) + (lvl + 1) * sizeof(Node *));
	if (np == NULL) {
		free(update);
		return -1;
	}
	np->key = key;
	np->val = val;

	for (i = 0; i < lvl + 1; i++) {
		np->fwd[i] = update[i]->fwd[i];
		update[i]->fwd[i] = np;
	}

	free(update);

	return 0;
}

int
sl_remove(SkipList *list, Key key)
{
	Node **update, *np = list->header;
	unsigned int i;

	update = malloc((list->maxlvl + 1) * sizeof(Node *));
	if (update == NULL)
		return -1;

	for (i = list->lvl; (int)i >= 0; i--) {
		while (np->fwd[i] && np->fwd[i]->key < key)
			np = np->fwd[i];
		update[i] = np;
	}

	np = np->fwd[0];
	if (np == NULL || np->key != key) {
		free(update);
		return -1;
	}

	for (i = 0; i < list->lvl + 1; i++) {
		if (update[i]->fwd[i] != np)
			break;
		update[i]->fwd[i] = np->fwd[i];
	}

	free(np);
	free(update);

	while (list->lvl > 0 && list->header->fwd[list->lvl] == NULL)
		list->lvl--;

	return 0;
}

Node *
sl_search(SkipList *list, Key key)
{
	Node *np = list->header;
	unsigned int i;

	for (i = list->lvl; (int)i >= 0; i--) {
		while (np->fwd[i] && np->fwd[i]->key < key)
			np = np->fwd[i];
	}

	if (np->fwd[0] && np->fwd[0]->key == key)
		return np->fwd[0];

	return NULL;
}
