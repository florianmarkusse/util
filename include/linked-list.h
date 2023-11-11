#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#define FLO_OFFSET_OF(T, x) (unsigned long long int)(&(((T *)0)->x))

struct flo_SingleLink {
    struct flo_SingleLink *next;
};
typedef struct flo_SingleLink flo_SingleLink;

#define FLO_SINGLE_LINKED_LIST_FOR_EACH(pos, head)                             \
    for ((pos) = (head)->next; (pos) != (head); (pos) = (pos)->next)

#ifdef __cplusplus
}
#endif

#endif
