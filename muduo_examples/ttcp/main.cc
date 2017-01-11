#include "lib.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
  if (argc == 1)
  {
    printf("Please specify server or client\n");
    return 0;
  }

  Option option;
  const char* arg = argv[1];
  if(arg[0] == 'c')
  {
    option.host = "localhost";
    option.port = 9999;
    option.length = 8192;
    option.count = 10000;
    transmit(option);
  }
  else
  {
    option.port = 9999;
    receive(option);
  }   

  return EXIT_SUCCESS;
}
