#include "lib.h"

#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>

struct sockaddr_in resolve(const char* host, uint16_t port)
{
  struct hostent* he = ::gethostbyname(host);
  if (!he) {
    perror("gethostbyname");
    exit(EXIT_FAILURE);
  }
  struct sockaddr_in addr;
  bzero(&addr, sizeof(addr)); 
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
  return addr;
}
