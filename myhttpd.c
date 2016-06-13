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

int socket_handle(int listenfd)
{
	int epollfd;
	epollfd = epoll_create1(0);

	struct epoll_event event;
	event.data.fd = listenfd;
	event.events = EPOLLIN | EPOLLET;
	epoll_ctl(epollfd, EPOLL_CTL_ADD | EPOLLIN, listenfd, &event);

	int ready;
	int i;
	struct epoll_event *events;
	events = calloc(32, sizeof(struct epoll_event));
	int connfd;
	while (1) {
		ready = epoll_wait(epollfd, events, 32, -1);
		for (i = 0; i < ready; ++i) {
			if (events[i].data.fd == listenfd) {
				connfd = accept_client(listenfd);
				if (-1 == connfd) {
					mlog("accept_client error");
					return -1;
				}
				event.data.fd = connfd;
				event.events = EPOLLIN | EPOLLET;
				epoll_ctl(epollfd, EPOLL_CTL_ADD | EPOLLIN, connfd, &event);
				continue;
			} else {
				size_t len;
				char   buf[MAXLEN] = {0};
				connfd = events[i].data.fd;
				len = recv_request(connfd, buf);
				mlog("head: [%d]%s", len, buf);
				char path[128] = {0};
				pase_request_path(buf, path);
				mlog("path[%s]\n", path);
				char new_path[128] = {0};
				if (path[0] == '/' && path[1] == '\0')
					strcpy(new_path, "index.html");
				else
					strcpy(new_path, path + 1);
				mlog("new path[%s]\n", new_path);
				memset(buf, 0x00, sizeof(buf));

				if (strstr(path, "jpg")) {
					mlog("jpg");
					send_response(connfd, HTTP_IMG_HEAD, strlen(HTTP_IMG_HEAD));
				} else {
					mlog("text");
					send_response(connfd, HTTP_TEXT_HEAD, strlen(HTTP_TEXT_HEAD));
				}
				len = send_file(connfd, new_path);
				mlog("write len: [%d]\n", len);
				epoll_ctl(epollfd, EPOLL_CTL_DEL, connfd, NULL);
				close(connfd);
			}
		}
	}
	return 0;
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
