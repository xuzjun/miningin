#ifndef HTTP_H
#define HTTP_H

#define HTTP_TEXT_HEAD "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
#define HTTP_IMG_HEAD "HTTP/1.1 200 OK\r\nContent-Type: image/jpg\r\n\r\n"
#define HTTP_TEST_BODY "<html><body>It works<img src=\"1.jpg\"/></body></html>"

#define MAXLEN 4096

int pase_request_path(const char *buf, char *path);
int handle_http_request(int fd);

#endif
