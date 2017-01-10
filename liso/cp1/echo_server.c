/******************************************************************************
* echo_server.c                                                               *
*                                                                             *
* Description: This file contains the C source code for an echo server.  The  *
*              server runs on a hard-coded port and simply write back anything*
*              sent to it by connected clients.  It does not support          *
*              concurrent clients.                                            *
*                                                                             *
* Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                         *
*          Wolf Richter <wolf@cs.cmu.edu>                                     *
*                                                                             *
*******************************************************************************/

#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// Default port for our echo server
#define ECHO_PORT 9999

#define BUF_SIZE 4096

int close_socket(int sock)
{
    if (close(sock))
    {
        fprintf(stderr, "Failed closing socket.\n");
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    int maxfd; // Record the max fd to select from
    fd_set master;
    fd_set readfds; // We only check readfds
    int status; 
    struct timeval tv;

    int sock, client_sock;
    ssize_t readret;
    ssize_t sendret;
    struct sockaddr_in addr;
    char buf[BUF_SIZE];

    fprintf(stdout, "----- Echo Server -----\n");
    
    /* all networked programs must create a socket */
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Failed creating socket.\n");
        return EXIT_FAILURE;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(ECHO_PORT); // Bind to port 9999
    addr.sin_addr.s_addr = INADDR_ANY; // Bind to all interface(ip) of current machine

    /* servers bind sockets to ports---notify the OS they accept connections */
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)))
    {
        close_socket(sock);
        fprintf(stderr, "Failed binding socket.\n");
        return EXIT_FAILURE;
    }


    if (listen(sock, SOMAXCONN))
    {
        close_socket(sock);
        fprintf(stderr, "Error listening on socket.\n");
        return EXIT_FAILURE;
    }

    /* Now, we can use select to poll ready io fds_bits */
    FD_ZERO(&master);
    FD_ZERO(&readfds);

    FD_SET(sock, &master);
    maxfd = sock;

    tv.tv_sec = 3;
    tv.tv_usec = 500000;

    for (;;)
    {
        readfds = master; // copy
        status = select(maxfd + 1, &readfds, NULL, NULL, &tv);
        if (status == -1)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }
        else if (status == 0)
        {
            // printf("Still Waiting...\n");
        }
        else
        {
            /* We only check for read fds */
            for (int i = 0; i <= maxfd; i++)
            {
                if (FD_ISSET(i, &readfds))
                {
                    /* The listener， accept new connections */
                    if (i == sock)
                    {
                        client_sock = accept(sock, NULL, NULL);
                        if (client_sock == -1)
                        {
                            close_socket(sock);
                            FD_CLR(sock, &master);
                            fprintf(stderr, "Server Accepting sock error");
                        } 
                        else
                        {
                            /* Add newfd to master set */
                            FD_SET(client_sock, &master);
                            if (client_sock > maxfd)
                            {
                                maxfd = client_sock;
                            }
                        }
                    }
                    else
                    {
                        /*　Read data from a client */
                        readret = recv(i, buf, BUF_SIZE, 0);
                        if (readret == -1)
                        {
                            close_socket(i);
                            FD_CLR(i, &master);
                            fprintf(stderr, "Server Reading data error");
                        }
                        else if (readret == 0)
                        {
                            /* the other end closed */
                            close_socket(i);
                            FD_CLR(i, &master);
                        }
                        else 
                        {
                            sendret = send(i, buf, readret, 0);
                            if (sendret == 0)
                            {
                                close_socket(i);
                                FD_CLR(i, &master);
                            }
                            else if (sendret != readret)
                            {
                                close_socket(i);
                                FD_CLR(i, &master);
                                fprintf(stderr, "Server Error sending to client\n");
                            }
                        }

                    }
                }
            }
        }
    }

    return EXIT_SUCCESS;
}
