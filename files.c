#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "files.h"
#include "log.h"

int file_can_exec(const char *path)
{
	struct stat buf;
	if (stat(path, &buf) == -1) {
		perror("stat");
		return 0;
	}
	short s = EXEC_MODE;     //can exec
	return s & buf.st_mode;
}

char *read_file(const char *path, size_t *len)
{
	int fd = open(path, O_RDONLY);
	if (fd < 0) {
		mlog("open [%s] error\n", path);
		return NULL;
	}
	struct stat st;
	if (stat(path, &st) < 0) {
		mlog("stat error.[%s]\n", strerror(errno));
		close(fd);
		return NULL;
	}
	char *buf = calloc(st.st_size, st.st_size);
	size_t rlen;
	rlen = read(fd, buf, st.st_size);
	close(fd);
	if (rlen != st.st_size) {
		free(buf);
		return NULL;
	}
	*len = st.st_size;
	return buf;
}

int make_fd_nonblocking(int fd)
{
	int flags = 0;
	flags = fcntl(fd, F_GETFL, NULL);
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int try_lock_fd(int fd)
{
	struct flock fl;

	memset(&fl, 0x00, sizeof(fl));
	fl.l_type = F_WRLCK; 
	fl.l_whence = SEEK_SET;
	if (fcntl(fd, F_SETLK, &fl) == -1)
		return errno;

	return 0;
}

int lock_fd(int fd)
{
	struct flock fl;

	memset(&fl, 0x00, sizeof(fl));
	fl.l_type = F_WRLCK; 
	fl.l_whence = SEEK_SET;
	if (fcntl(fd, F_SETLKW, &fl) == -1)
		return -1;

	return 0;
}


int unlock_fd(int fd)
{
	struct flock fl;

	memset(&fl, 0x00, sizeof(fl));
	fl.l_type = F_UNLCK; 
	fl.l_whence = SEEK_SET;
	if (fcntl(fd, F_SETLK, &fl) == -1)
		return -1;

	return 0;
}
