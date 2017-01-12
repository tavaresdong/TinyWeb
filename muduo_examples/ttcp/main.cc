#include "lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
  if (argc == 1)
  {
    printf("Please specify server or client\n");
    return 0;
  }

  Option option;
  if(strncmp(argv[1], "-c", 2) == 0)
  {
    if (argc != 4)
    {
      printf("Client must specify the length and count of messages\n");
      return 0;
    }

    option.host = "localhost";
    option.port = 9999;
    option.length = atoi(argv[2]);
    option.count = atoi(argv[3]);
    transmit(option);
  }
  else
  {
    option.port = 9999;
    receive(option);
  }   

  return EXIT_SUCCESS;
}
