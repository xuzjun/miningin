#ifndef MTX_H
#define MTX_H

#include "myhttpd.h"

int mtx_create(cycle_t *cycle, const char *fn);
void mtx_destroy(cycle_t *cycle);
int mtx_try_lock(cycle_t *cycle);
void mtx_lock(cycle_t *cycle);
void mtx_unlock(cycle_t *cycle);
int mtx_force_lock(cycle_t *cycle);

#endif
