#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "skip.h"

static size_t
skip_delrange_slow(SkipList *list, SkipKey from, SkipKey to)
{
	SkipNode *node, *next;
	size_t n;

	node = skip_approx(list, from);
	for (n = 0; node && node->key < to; n++) {
		next = node->next;
		skip_delete(list, node->key);
		node = next;
	}

	return n;
}

static void
skip_print(SkipList *list)
{
	SkipNode *node;

	for (node = skip_first(list); node; node = node->next)
		printf("%d[%d]->", node->key, node->val);
	puts("NULL");
}

int
main(void)
{
	SkipList *list;
	SkipNode *node;
	size_t rv;
	int i;

	srand(time(NULL));
	list = skip_create();
	if (list == NULL) {
		perror(NULL);
		return 1;
	}

	for (i = 0; i < 100; i++) {
		if (skip_insert(list, i, i) == NULL) {
			perror(NULL);
			return 1;
		}
	}

	puts("list:");
	skip_print(list);
	fputc('\n', stdout);

	printf("delete node with key from %d to %d\n", 32, 58);
	rv = skip_delrange(list, 32, 58);
	/* rv = skip_delrange_slow(list, 32, 58); */
	printf("removed %zu nodes\nupdated list:\n", rv);
	skip_print(list);
	fputc('\n', stdout);

	/* Normally we should null check all of this */
	node = skip_approx(list, 37);
	printf("node with key ~= 37: %d[%d]\n", node->key, node->val);
#ifdef SKIP_DOUBLY
	node = node->prev;
	printf("prev: %d[%d]\n", node->key, node->val);
#endif
	printf("set val %d to node with key %d\n", 42, node->val);
	node->val = 42;
	skip_print(list);

	skip_destroy(list);

	return 0;
}
