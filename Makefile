tiny: tiny.c HttpUtils.c HttpUtils.h HttpResponse.h HttpRequest.h csapp.c csapp.h
	gcc -o tiny tiny.c HttpUtils.c csapp.c -lpthread
