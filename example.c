#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "skiplist.h"

#define LENGTH(X) (sizeof(X) / sizeof(X[0]))

static unsigned int
getseed(void)
{
	unsigned int seed;
	int fd;

	if ((fd = open("/dev/urandom", O_RDONLY)) < 0)
		return 0;
	if (read(fd, &seed, sizeof(seed)) < 0)
		return 0;
	if (close(fd) < 0)
		return 0;

	return seed;
}

static void
sl_printlvl(SkipList *list, size_t lvl)
{
	Node *np;

	if (list == NULL)
		return;

	for (np = list->header->fwd[lvl]; np; np = np->fwd[lvl])
		printf("%d[%d]->", np->key, np->val);
	puts("NULL");

}

static void
sl_printall(SkipList *list)
{
	unsigned int i;

	if (list == NULL)
		return;

	for (i = list->lvl; (int)i >= 0; i--) {
		printf("lvl %u: ", i);
		sl_printlvl(list, i);
	}
}

int
main(void)
{
	const Key key[] = { 32, 47, 36, 51, 0, 62, 43, 39 };
	const Val val[] = { 14, 10, 11, 29, 2, 18, 16, 38 };
	const Key delkey[] = { 43, 62, 31, 32, 66 };
	const Key searchkey[] = { 32, 51, 8, 0, 100 };
	SkipList *list;
	Node *np;
	size_t i;

	list = sl_create(10, getseed());
	if (list == NULL) {
		perror(NULL);
		return 1;
	}

	for (i = 0; i < LENGTH(key); i++) {
		printf("insert key %d with val %d\n", key[i], val[i]);
		if (sl_insert(list, key[i], val[i]) < 0) {
			perror(NULL);
			return 1;
		}
	}

	sl_printlvl(list, 0);
	fputc('\n', stdout);

	sl_printall(list);
	fputc('\n', stdout);

	for (i = 0; i < LENGTH(delkey); i++) {
		if (sl_remove(list, delkey[i]) < 0)
			/* not found or malloc failed */
			printf("key %d is not in the list\n", delkey[i]);
		else
			printf("key %d has been deleted\n", delkey[i]);
	}

	sl_printlvl(list, 0);
	fputc('\n', stdout);

	for (i = 0; i < LENGTH(searchkey); i++) {
		np = sl_search(list, searchkey[i]);
		if (np == NULL)
			printf("key %d is not in the list\n", searchkey[i]);
		else
			printf("key %d found with val = %d\n",
			       searchkey[i], np->val);

	}

	sl_destroy(list);

	return 0;
}
