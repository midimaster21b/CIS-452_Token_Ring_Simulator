/******************************************************************
 * Program: Token Ring
 * Author: Joshua Edgcombe <joshedgcombe@gmail.com>
 * Date: 2018-09-21
 *
 * Description: This program simulates token ring operation using
 *              system calls and IPC.
 *
 ******************************************************************/
#include <stdio.h>

#include "endpoint.h"

int main() {
  int num_endpoints;
  int x;
  int token_id = 1;

  // Welcome the user to the program
  printf("Welcome to the CIS 452 Token Ring Simulator\n");
  printf("===========================================\n");

  // Get the desired number of endpoints to create from the user
  num_endpoints = request_num_endpoints();

  // Prompt user
  printf("You have requested %d endpoints. Creating now...\n", num_endpoints);

  // Create the appropriate endpoints
  for(x=0; x<num_endpoints; x++) {
    create_endpoint(x + 2);
  }

  return 0;
}
