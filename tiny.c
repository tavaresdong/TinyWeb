#include "csapp.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpUtils.h"


int main(int argc, char **argv) {
	
	int listenfd, connfd, port, clientlen;
	struct sockaddr_in clientaddr;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(EXIT_SUCCESS);
	}
	port = atoi(argv[1]);

	listenfd = Open_listenfd(port);
	while (1) {
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		doit(connfd);
		Close(connfd);		// need to close this connected fd
	}


}
