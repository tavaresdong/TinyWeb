#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <string>

struct Option
{
  std::string host;
  uint16_t port;
  int32_t length;
  int32_t count;
  
  Option()
  : port(0), length(0), count(0)
  {}
};

// packed attribute is used so there is no padding
// and we can directly read into the message address
struct __attribute__((packed)) SessionMessage
{
  int32_t count; // How many messages
  int32_t length; // How long for each message
};

struct PayloadMessage
{
  int32_t length;
  char data[0];  // variable length trick
};

struct sockaddr_in resolve(const char* host, uint16_t port);

void transmit(const Option& op);
void receive(const Option& op);

#endif
