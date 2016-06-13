#ifndef SERVER_H
#define SERVER_H

#define MAXLINE 80
#define LISTEN_QUEUE_SIZE 1024
#define PORT  9998

int create_listen_socket();
int accept_client(int fd);
int send_file(int fd, const char *path);
size_t recv_request(int fd, char *buf);
size_t read_http_head(int fd, char *buf);
size_t send_response(int fd, const char *buf, size_t len);
size_t write_sock(int fd, const char *buf, size_t len);
int socket_handle(int listenfd);

#endif
