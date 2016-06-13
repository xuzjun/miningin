#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/epoll.h>
#include <sys/uio.h>
#include <unistd.h>
#include "log.h"
#include "connection.h"
#include "http.h"
#include "file.h"

int create_listen_socket()
{
	int sock;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sock) {
		mlog("socket");
		return -1;
	}
	int opt = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		mlog("setsockopt");
		return -1;
	}
	int ret;
	struct sockaddr_in serv_addr;
	int addr_len;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_family = AF_INET;
	addr_len = sizeof(struct sockaddr_in);
	ret = bind(sock, (struct sockaddr *)&serv_addr, addr_len);
	if (-1 == ret) {
		mlog("bind");
		return -1;
	}

	ret = listen(sock, LISTEN_QUEUE_SIZE);
	if (-1 == ret) {
		mlog("listen");
		return -1;
	}
	return sock;
}

int accept_client(int fd)
{
	int conn_sock;
	struct sockaddr_in cli_addr;
	int addr_len = sizeof(struct sockaddr_in);
	conn_sock = accept(fd, (struct sockaddr *)&cli_addr, (socklen_t *)&addr_len);
	if (-1 == conn_sock) {
		mlog("accept");
		return -1;
	}
	return conn_sock;
}

size_t recv_request(int fd, char *buf)
{
	return read_http_head(fd, buf);
}

size_t read_http_head(int fd, char *buf)
{
	char ch;
	int  i;
	for (i = 0;;++i) {
		read(fd, &ch, 1);
		buf[i] = ch;
		if (i >= 3 && buf[i] == '\n' && buf[i - 1] == '\r'
            && buf[i - 2] == '\n' && buf[i - 3] == '\r')
			break;
	}
	return i;
}

int send_file(int sockfd, const char *path)
{
	int fd;
	fd = open(path, O_RDONLY);
	size_t len;
	len = 0;
	struct stat st;
	fstat(fd, &st);
	sendfile(sockfd, fd, (off_t *)&len, st.st_size);
	mlog("\nsend file length[%ll]\n", len);
	close(fd);
	return 0;
}

size_t send_response(int fd, const char *buf, size_t len)
{
	return write_sock(fd, buf, len);
}

size_t write_sock(int fd, const char *buf, size_t len)
{
	size_t n, left;
	left = len;
	while (left > 0) {
		n = write(fd, buf, left);
		if (n <= 0)
			break;
		left -= n;
	}
	return len - left;
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
				if (make_fd_nonblocking(connfd) == -1) {
					mlog("make fd non-blocking error");
					continue;
				}
				event.data.fd = connfd;
				event.events = EPOLLIN | EPOLLET;
				epoll_ctl(epollfd, EPOLL_CTL_ADD | EPOLLIN, connfd, &event);
				continue;
			} else {
				connfd = events[i].data.fd;
				handle_http_request(connfd);
				epoll_ctl(epollfd, EPOLL_CTL_DEL, connfd, NULL);
				close(connfd);
			}
		}
	}
	return 0;
}
