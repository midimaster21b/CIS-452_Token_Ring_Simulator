/****************************************************
 * File: endpoint.h
 * Author: Joshua Edgcombe <joshedgcombe@gmail.com>
 * Date: 2018-09-21
 *
 * Description: The endpoint library is developed to
 * allow the easy creation, deletion, and management
 * of token ring simulation endpoints.
 ****************************************************/

#ifndef __ENDPOINT_H__
#define __ENDPOINT_H__

#define ENDPOINT_STRING_LENGTH 10

// Single endpoint
typedef struct endpoint {
  int pid;
  int token_id;
  int read_endpoint;
  int write_endpoint;
} endpoint;

// Singly linked list for management purposes
typedef struct endpoint_list {
  endpoint endp;
  struct endpoint_list *next;
} endpoint_list;

// User interaction
int request_num_endpoints(void);

// Endpoint creation and deletion
endpoint create_endpoint(int id);

#endif // __ENDPOINT_H__
