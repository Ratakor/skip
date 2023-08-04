#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "skip.h"

#define LENGTH(X) (sizeof(X) / sizeof(X[0]))

static void
skip_print(SkipList *list)
{
	SkipNode *node;

	for (node = skip_first(list); node; node = node->next)
		printf("%d[%d]->", node->key, node->val);
	puts("NULL");
}

#ifdef SKIP_DOUBLY
static void
skip_printbw(SkipList *list)
{
	SkipNode *node;

	printf("NULL");
	for (node = skip_last(list); node; node = node->prev)
		printf("<-%d[%d]", node->key, node->val);
	fputc('\n', stdout);
}
#endif /* SKIP_DOUBLY */

static void
skip_prettyprint(SkipList *list)
{
	SkipNode *node;
	size_t i;

	printf("%*s\n", 32, "str");
	for (i = 0; i < skip_size(list); i++)
		printf("------   ");
	printf("--------\n");
	for (node = skip_first(list); node; node = node->next)
		printf("| %02d |-->", node->key);
	printf("| NULL |\n");
	for (i = 0; i < skip_size(list); i++)
		printf("------   ");
	printf("--------\n");
}

int
main(void)
{
	const SkipKey key[] = { 32, 47, 36, 51, 12, 62, 43, 39 };
	const SkipVal val[] = { 14, 10, 11, 29, 2, 18, 16, 38 };
	const SkipKey delkey[] = { 43, 62, 31, 32, 66 };
	const SkipKey searchkey[] = { 32, 51, 8, 0, 100 };
	SkipList *list;
	SkipNode *node;
	size_t i;

	srand(time(NULL));
	list = skip_create();
	if (list == NULL) {
		perror(NULL);
		return 1;
	}

	for (i = 0; i < LENGTH(key); i++) {
		printf("insert key %d with val %d\n", key[i], val[i]);
		if (skip_insert(list, key[i], val[i]) == NULL) {
			perror(NULL);
			return 1;
		}
	}

	printf("\nskip list of size %zu:\n", skip_size(list));
	skip_print(list);
#ifdef SKIP_DOUBLY
	skip_printbw(list);
#endif /* SKIP_DOUBLY */
	skip_prettyprint(list);
	fputc('\n', stdout);

	for (i = 0; i < LENGTH(delkey); i++) {
		if (skip_delete(list, delkey[i]))
			printf("deleted node with key %d\n", delkey[i]);
		else
			printf("no node with key %d in the list\n", delkey[i]);
	}

	printf("\nskip list of size %zu:\n", skip_size(list));
	skip_print(list);
#ifdef SKIP_DOUBLY
	skip_printbw(list);
#endif /* SKIP_DOUBLY */
	skip_prettyprint(list);
	fputc('\n', stdout);

	for (i = 0; i < LENGTH(searchkey); i++) {
		node = skip_search(list, searchkey[i]);
		if (node == NULL)
			printf("key %d is not in the list\n", searchkey[i]);
		else
			printf("key %d found with val = %d\n",
			       searchkey[i], node->val);
	}

	skip_destroy(list);

	return 0;
}
