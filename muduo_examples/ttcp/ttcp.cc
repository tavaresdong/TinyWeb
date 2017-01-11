#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#include <muduo/base/Timestamp.h>

#include "lib.h"

static int acceptInfo(uint16_t port)
{
  int listenfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (listenfd <= 0)
  {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  int optval = 1;
  if (::setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != 0)
  {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (::bind(listenfd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) != 0)
  {
    perror("bind");
    exit(EXIT_FAILURE);
  } 

  if (::listen(listenfd, 32) != 0)
  {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  int peerFd = ::accept(listenfd, NULL, NULL);
  if (peerFd < 0)
  {
    perror("accept");
    exit(EXIT_FAILURE);
  }
  
  ::close(listenfd);
  return peerFd;
}

static int write_n(int sockfd, const void* buf, int length)
{
  int written = 0;
  while (written < length)
  {
    ssize_t nw = ::write(sockfd, static_cast<const char*>(buf) + written, length - written);
    if (nw > 0)
    {
      written += static_cast<int>(nw);
    }
    else if (nw == 0)
    {
      break;
    }
    else if (errno != EINTR)
    {
      perror("write_n");
      exit(EXIT_FAILURE);
    }
  }
  return written;
}

static int read_n(int sockfd, void* buf, int length)
{
  int numread = 0;
  while (numread < length)
  {
    ssize_t nr = ::read(sockfd, static_cast<char*>(buf) + numread, length - numread);
    if (nr > 0)
    {
      numread += static_cast<int>(nr);
    }
    else if (nr == 0)
    {
      break;
    }
    else if (errno != EINTR)
    {
      perror("read_n");
      exit(EXIT_FAILURE);
    }
  } 
  return numread;
}

void transmit(const Option& op)
{
  struct sockaddr_in addr = resolve(op.host.c_str(), op.port);
  printf("connecting to %s:%d\n", inet_ntoa(addr.sin_addr), op.port); 

  int sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd <= 0)
  {
    perror("creating connecting socket");
    exit(EXIT_FAILURE);
  }

  if (::connect(sockfd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) != 0)
  {
    perror("connect");
    ::close(sockfd);
    return;
  }

  printf("connected");
  muduo::Timestamp start(muduo::Timestamp::now());
  struct SessionMessage session;
  session.count = htonl(op.count);
  session.length = htonl(op.length);
  
  if (write_n(sockfd, &session, sizeof(session)) != sizeof(session))
  {
    perror("send session");
    ::close(sockfd);
    return;
  }

  // Length of one message
  const int msgLength = static_cast<int>(sizeof(int32_t)) + op.length;
  struct PayloadMessage* payload = static_cast<PayloadMessage*>(::malloc(msgLength));

  assert(payload);
  payload->length = htonl(op.length);
  for (int i = 0; i < op.length; ++i)
  {
    payload->data[i] = "123456789ABCDEF"[i % 16];
  }

  double total_mb_sent = 1.0 * op.length * op.count / 1024 / 1024;
  printf("%.3f mb in total\n", total_mb_sent);

  for (int i = 0; i < op.count; ++i)
  {
    if (write_n(sockfd, payload, msgLength) != msgLength)
    {
      perror("send msg");
      ::close(sockfd);
      return;
    }

    int32_t ack = 0;
    if(read_n(sockfd, &ack, sizeof(ack)) != sizeof(ack))
    {
      perror("receive ack");
      ::close(sockfd);
      return;
    }
    ack = ntohl(ack);
    if (ack != op.length)
    {
      printf("received data length: %d\n", ack);
      perror("received data length not match");
      ::close(sockfd);
      return;
    }
  }
  ::free(payload);
  ::close(sockfd);

  double elapsed = timeDifference(muduo::Timestamp::now(), start);
  printf("%.3f seconds\n%.3f MiB/s\n", elapsed, total_mb_sent / elapsed);
}

void receive(const Option& op)
{
  int sockfd = acceptInfo(op.port); 

  struct SessionMessage session { 0, 0 };
  if (read_n(sockfd, &session, sizeof(session)) != sizeof(session))
  {
    perror("Read session info");
    exit(EXIT_FAILURE);
  }

  // The info you read is in network order
  session.count = ntohl(session.count);
  session.length = ntohl(session.length);

  // Length of one message
  const int msgLength = static_cast<int>(sizeof(int32_t)) + session.length;
  struct PayloadMessage* payload = static_cast<PayloadMessage*>(::malloc(msgLength));

  assert(payload);

  for (int i = 0; i < session.count; i++)
  {
    payload->length = 0;
    if (read_n(sockfd, &payload->length, sizeof(payload->length)) != sizeof(payload->length))
    {
      perror("read payload length");
      exit(EXIT_FAILURE);
    }

    payload->length = ntohl(payload->length);
    assert(payload->length == session.length);
    if (read_n(sockfd, payload->data, payload->length) != payload->length)
    {
      perror("read payload data");
      exit(EXIT_FAILURE);
    }

    int32_t ack = htonl(payload->length);
    if (write_n(sockfd, &ack, sizeof(ack)) != sizeof(ack))
    {
      perror("write ack to client");
      exit(EXIT_FAILURE);
    }
  }

  ::free(payload);
  ::close(sockfd);
}


