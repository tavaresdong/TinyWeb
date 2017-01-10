################################################################################
# Makefile                                                                     #
#                                                                              #
# Description: This file contains the make rules for Recitation 1.             #
#                                                                              #
# Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                          #
#          Wolf Richter <wolf@cs.cmu.edu>                                      #
#                                                                              #
################################################################################

default: echo_server echo_client

echo_server: echo_server.c
	@gcc echo_server.c -o echo_server -Wall -Werror

echo_client: echo_client.c
	@gcc echo_client.c -o echo_client -Wall -Werror

clean:
	@rm echo_server echo_client
