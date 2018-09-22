/****************************************************
 * File: endpoint.c
 * Author: Joshua Edgcombe <joshedgcombe@gmail.com>
 * Date: 2018-09-21
 *
 * Description: The endpoint library is developed to
 * allow the easy creation, deletion, and management
 * of token ring simulation endpoints.
 ****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "endpoint.h"

endpoint_list *endpoint_list_head = NULL;

int request_num_endpoints(void) {
  // Add one to endpoint string length to ensure string is null terminated
  // SEE: fgets documentation for reasoning
  char num_endpoints_in[ENDPOINT_STRING_LENGTH + 1] = {0};
  int num_endpoints;

  // Get the number of endpoints from the end user
  printf("Please enter the desired number of endpoints: ");
  fgets(num_endpoints_in, ENDPOINT_STRING_LENGTH, stdin);

  // Convert string input into integer
  // TODO: ADD APPROPRIATE ERROR CHECKING
  num_endpoints = strtol(num_endpoints_in, NULL, 10);

  return num_endpoints;
}

endpoint *create_endpoint(int id) {
  endpoint retval;

  return NULL;
}
