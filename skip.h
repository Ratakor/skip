/* Copyright © 2023 Ratakor. ISC License */

#ifndef SKIP_H
#define SKIP_H

#include <sys/types.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef int SkipKey;
typedef int SkipVal;
typedef struct SkipNode SkipNode;
typedef struct SkipList SkipList;

SkipList *skip_create(void);
void skip_destroy(SkipList *list);
SkipNode *skip_insert(SkipList *list, SkipKey key, SkipVal val);
bool skip_delete(SkipList *list, SkipKey key);
size_t skip_delrange(SkipList *list, SkipKey from, SkipKey to);
SkipNode *skip_search(SkipList *list, SkipKey key);
SkipNode *skip_approx(SkipList *list, SkipKey key);
size_t skip_size(SkipList *list);
SkipNode *skip_first(SkipList *list);
SkipNode *skip_next(SkipNode *node);
#ifdef SKIP_DOUBLY
SkipNode *skip_last(SkipList *list);
SkipNode *skip_prev(SkipNode *node);
#endif /* SKIP_DOUBLY */
SkipKey skip_key(SkipNode *node);
SkipVal skip_val(SkipNode *node);
void skip_setval(SkipNode *node, SkipVal val);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SKIP_H */
