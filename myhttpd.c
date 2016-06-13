#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>
#include "myhttpd.h"
#include "http.h"
#include "connection.h"
#include "log.h"
#include "file.h"

void init()
{
	return;
}

int main(int argc, char *argv[])
{
	init();
	int listenfd = create_listen_socket();
	if (-1 == listenfd) {
		mlog("create_listen_fd error");
		return -1;
	}
	socket_handle(listenfd);
	return 0;
}
