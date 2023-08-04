#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "skip.h"

#define LENGTH(X) (sizeof(X) / sizeof(X[0]))

/* This is just to show a dump of a skiplists with all its levels. One should
 * never define _SkipNode or SkipList but use given functions instead.
 */

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

static void
setrand(void)
{
	unsigned int seed;
	int fd;

	if ((fd = open("/dev/urandom", O_RDONLY)) < 0)
		return;
	read(fd, &seed, sizeof(seed));
	close(fd);
	srand(seed);
}

static void
skip_dump(SkipList *list)
{
	struct _SkipNode *node;
	int i;

	for (i = list->lvl - 1; i >= 0; i--) {
		printf("lvl %u: ", i);
		for (node = list->hdr->fwd[i]; node; node = node->fwd[i])
			printf("%d[%d]->", node->key, node->val);
		puts("NULL");
	}
}

/*  ----------                     ------   --------
 *  | Header |-------------------->| 40 |-->| NULL |
 *  ----------                     ------   --------
 *  ----------   ------            ------   --------
 *  | Header |-->| 32 |----------->| 40 |-->| NULL |
 *  ----------   ------            ------   --------
 *  ----------   ------   ------   ------   --------
 *  | Header |-->| 32 |-->| 33 |-->| 40 |-->| NULL |
 *  ----------   ------   ------   ------   --------
 */

static void
skip_prettydump(SkipList *list)
{
	struct _SkipNode *node, **nodes, **lvlnodes;
	size_t j;
	int i;

	nodes = calloc(list->siz, sizeof(struct _SkipNode *));
	lvlnodes = calloc(list->siz, sizeof(struct _SkipNode *));
	for (node = list->hdr->fwd[0], i = 0; node; node = node->fwd[0], i++)
		nodes[i] = node;

	for (i = list->lvl - 1; i >= 0; i--) {
		memset(lvlnodes, 0, list->siz * sizeof(struct _SkipNode *));
		printf("----------   ");
		j = 0;
		for (node = list->hdr->fwd[i]; node; node = node->fwd[i]) {
			for (; nodes[j]->key < node->key; j++)
				printf("         ");
			printf("------   ");
			lvlnodes[j] = node;
			j++;
		}
		for (; j < list->siz; j++)
			printf("         ");
		printf("--------\n| Header |--");
		for (j = 0; j < list->siz; j++) {
			if (lvlnodes[j])
				printf(">| %02d |--", lvlnodes[j]->key);
			else
				printf("---------");
		}
		printf(">| NULL |\n----------   ");
		for (j = 0; j < list->siz; j++) {
			if (lvlnodes[j])
				printf("------   ");
			else
				printf("         ");
		}
		printf("--------\n");
	}

	free(nodes);
	free(lvlnodes);
}

int
main(void)
{
	const SkipKey key[] = { 32, 47, 36, 51, 13, 62, 43, 39 };
	const SkipVal val[] = { 14, 10, 11, 29, 22, 18, 16, 38 };
	SkipList *list;
	size_t i;

	setrand();
	list = skip_create();
	if (list == NULL) {
		perror(NULL);
		return 1;
	}

	for (i = 0; i < LENGTH(key); i++) {
		if (skip_insert(list, key[i], val[i]) == NULL) {
			perror(NULL);
			return 1;
		}
	}

	/* skip_dump(list); */
	skip_prettydump(list);
	skip_destroy(list);

	return 0;
}
