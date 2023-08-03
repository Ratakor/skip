#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "skip.h"

#define LENGTH(X) (sizeof(X) / sizeof(X[0]))

/* This is just to show a dump of a skiplists with all its levels. One should
 * never define SkipNode or SkipList but use given functions instead.
 */

struct SkipNode {
	SkipKey key;
	SkipVal val;
#ifdef SKIP_DOUBLY
	SkipNode *bwd;
#endif /* SKIP_DOUBLY */
	SkipNode *fwd[];
};

struct SkipList {
	SkipNode *hdr;
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
	SkipNode *node;
	int i;

	for (i = list->lvl - 1; i >= 0; i--) {
		printf("lvl %u: ", i);
		for (node = list->hdr->fwd[i]; node; node = node->fwd[i])
			printf("%d[%d]->", skip_key(node), skip_val(node));
		puts("NULL");
	}
}

int
main(void)
{
	const SkipKey key[] = { 32, 47, 36, 51, 0, 62, 43, 39 };
	const SkipVal val[] = { 14, 10, 11, 29, 2, 18, 16, 38 };
	SkipList *list;
	size_t i;

	setrand();
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

	fputc('\n', stdout);
	skip_dump(list);
	skip_destroy(list);

	return 0;
}
