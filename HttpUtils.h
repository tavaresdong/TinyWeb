#ifndef _HTTP_UTILS_H_
#define _HTTP_UTILS_H_
#include "csapp.h"

extern void doit(int fd);
extern int parse_uri(char *uri, char *filename, char *cgiargs);
extern void clienterror(int fd, char *cause, char *errnum, 
						char *shortmsg, char *longmsg);

extern void get_filetype(char *filename, char *cgiargs);
extern void serve_static(int fd, char *filename, int filesize);
extern void serve_dynamic(int fd, char *filename, char *cgiargs);
extern void read_requesthdrs(rio_t *rp);

#endif
