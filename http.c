#include <string.h>
#include "http.h"

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
