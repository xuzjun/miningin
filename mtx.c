#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "mtx.h"
#include "files.h"
#include "log.h"

int mtx_create(cycle_t *cycle, const char *fn)
{
	int fd;
	fd = open(fn, O_WRONLY | O_CREAT, 0644);
	if (fd == -1)
		return -1;
	cycle->mtx_fd = fd;
	return 0;
}

void mtx_destroy(cycle_t *cycle)
{
	if (close(cycle->mtx_fd) < 0)
		mlog("mtx_destroy error");
}

int mtx_try_lock(cycle_t *cycle)
{
	int ret;
	ret = try_lock_fd(cycle->mtx_fd);
	if (ret == 0)
		return 1;
	return 0;
}

void mtx_lock(cycle_t *cycle)
{
	lock_fd(cycle->mtx_fd);
}

void mtx_unlock(cycle_t *cycle)
{
	unlock_fd(cycle->mtx_fd);
}

int mtx_force_lock(cycle_t *cycle)
{
	return 0;
}
