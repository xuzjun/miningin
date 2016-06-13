#include <string.h>
#include "http.h"
#include "log.h"
#include "connection.h"


int pase_request_path(const char *buf, char *path)
{
	const char *p = buf;
	char buf1[32] = {0};
	char buf2[128] = {0};
	int  i;
	for (i = 0; *p != ' '; ++i) {
		buf1[i] = *p;
		p++;
	}
	while (*p == ' ' ) p++;
	for (i = 0; *p != ' ' && *p != '\r'; ++i) {
		buf2[i] = *p;
		p++;
	}

	if (strcmp("HTTP/", buf2))
		strcpy(path, buf2);
	else
		strcpy(path, "/");
	return 0;
}

int handle_http_request(int connfd)
{
	size_t len;
	char   buf[MAXLEN] = {0};
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
	return 0;
}
