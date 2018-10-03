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

  int admin_pipe[2];
  int token_pipe[2];

  // Create admin write pipe and handle pipe creation errors
  if(pipe(admin_pipe) != 0) {
    return NULL;
  }

  // Create token pipe and handle pipe creation errors
  if(pipe(token_pipe) != 0) {
    return NULL;
  }

  // Create a new process
  int pid = fork();

  if(pid < 0) {
    return NULL;
  }

  // Get space for storing endpoint descriptor
  endpoint *retval = malloc(sizeof(endpoint));

  retval->pid = pid;
  retval->token_id  = id;

  // Copy pipe data into struct
  memcpy(retval->token_pipe, token_pipe, sizeof(retval->token_pipe));
  memcpy(retval->admin_pipe, admin_pipe, sizeof(retval->admin_pipe));

  /* retval->token_pipe = memcpy(retval->token_pipe, token_pipe, sizeof(retval->token_pipe)); */
  /* retval->admin_pipe = memcpy(retval->admin_pipe, admin_pipe, sizeof(retval->admin_pipe)); */
  /* retval->admin_rd_pipe = memcpy(retval->admin_rd_pipe, admin_rd_pipe, sizeof(retval->admin_rd_pipe)); */

  // Return newly created struct
  return retval;
}

endpoint_list *endpoint_list_add(endpoint_list *endpoint_list_head, endpoint *token_endpoint) {
  // Create an endpoint list struct for the supplied endpoint
  endpoint_list *token_endpoint_list_item = malloc(sizeof(endpoint_list));
  token_endpoint_list_item->endp = token_endpoint;
  token_endpoint_list_item->next = NULL;
  token_endpoint_list_item->prev = NULL;

  // Empty list
  if(endpoint_list_head == NULL) {

    // Item loops back to itself when it is the only item present
    token_endpoint_list_item->next = token_endpoint_list_item;
    token_endpoint_list_item->prev = token_endpoint_list_item;

    // Assign the created endpoint to supplied list
    endpoint_list_head = token_endpoint_list_item;

    // Return pointer to first(and only) item in list
    return endpoint_list_head;

  } else {
    // Create a temporary head pointer
    endpoint_list *temp_head = endpoint_list_head;
    endpoint_list *temp = temp_head;

    // If 2 or more items in the list already
    if(temp->prev != temp) {
      // Find lowest token_id endpoint in list
      while(temp->prev->endp->token_id <= temp->endp->token_id) {
	temp = temp->prev;
      }

      // Assign new head to endpoint list item with lowest token id
      temp_head = temp;
    }

    // If the supplied endoint is greater than the current lowest
    // token_id, advance one to set up next iteration loop (so temp != temp_head)
    if(token_endpoint->token_id > temp->endp->token_id) {
      // Advance one item
      temp = temp->next;
    }

    // Iterate through currently available nodes to find appropriate
    // place to insert supplied endpoint. Will end one endpoint list item
    // past the desired location (just after correct insertion location) or
    // at the last item in the list if the item should be appended.
    while(temp->next != temp_head && temp->endp->token_id < token_endpoint->token_id) {
      temp = temp->next;
    }

    // If we end up just past the correct spot, insert the new endpoint
    // NOTE: ASSUMES COHERENT LIST
    // NOTE: List insertion fails if item_a->next = item_b and item_b->prev != item_a
    // NOTE: This conditional must come first for the case of second to last item insertion
    if(temp->endp->token_id >= token_endpoint->token_id) {
      // Set appropriate values for next and previous of item to be inserted
      token_endpoint_list_item->prev = temp->prev;
      token_endpoint_list_item->next = temp;

      // Set the previous item's next attribute to point to inserted endpoint list item
      temp->prev->next = token_endpoint_list_item;

      // Set the temp's previous attribute to point to the inserted endpoint list item
      temp->prev = token_endpoint_list_item;

      // If item was inserted prior to first item positionally,
      // return new token endpoint list item. Otherwise return
      // the found previously found lowest token_id list item.
      if(temp == temp_head) {
	return token_endpoint_list_item;

      } else {
	return temp_head;
      }
    }

    // If this is the last item in the list or the second item added.
    else if(temp->next == temp_head) {

      // Assign appropriate links for new endpoint list item
      token_endpoint_list_item->prev = temp;
      token_endpoint_list_item->next = temp_head;

      // Assign next link for current
      temp->next = token_endpoint_list_item;

      // Assign prev link for temp_head
      temp_head->prev = token_endpoint_list_item;

      // Return new head
      return temp_head;
    }

    // TODO: WHAT IS THIS LOCATION FOR???
    else {
      return temp_head;
    }
  }
}

void endpoint_list_recycle(endpoint_list *endpoint_list_head) {
  endpoint_list *temp = endpoint_list_head;
  endpoint_list *temp_next = temp;

  // If an empty list was passed, return
  if(temp == NULL) {
    return;
  }

  // Otherwise, setup the while loop
  else {
    temp_next = temp->next;
    temp->next = NULL;
    temp = temp_next;
  }

  while(temp != NULL) {
    // Retain a handle to the next element in the list
    temp_next = temp->next;

    // Close token pipe handles
    close(temp->endp->token_pipe[0]);
    close(temp->endp->token_pipe[1]);

    // Close admin write pipe handles
    close(temp->endp->admin_pipe[0]);
    close(temp->endp->admin_pipe[1]);

    // Free the memory used
    free(temp->endp);
    free(temp);

    // Move to the next element
    temp = temp_next;
  }
}

void endpoint_list_print(endpoint_list *head) {
  endpoint_list *temp = head;

  printf("Found token(%p) id: %d (%p)\n", temp, temp->endp->token_id, &(temp->endp->token_id));

  while(temp->next != head) {
    temp = temp->next;

    printf("Found token(%p) id: %d (%p)\n", temp, temp->endp->token_id, &(temp->endp->token_id));
  }
}
