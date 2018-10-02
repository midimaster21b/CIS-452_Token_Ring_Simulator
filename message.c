/** @file message.c
 *  @brief Function definitions for the message library.
 *
 * The token ring message library is developed to
 * allow the easy creation, reading, and management
 * of token ring messages.
 *
 *  @author Joshua Edgcombe (joshedgcombe@gmail.com)
 *  @bug No known bugs.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "message.h"

static int msg_count = 0;

message *message_create(int destination, char *body) {
  // Allocate space for the message to be returned
  message *retval = malloc(sizeof(message));

  // On error, return a null pointer
  if(retval == NULL) {
    return NULL;
  }

  // Copy the body string into the message struct
  strncpy(retval->body, body, MESSAGE_MAX_BODY_LENGTH - 1);

  // Get the header string
  char *header = malloc(sizeof(MESSAGE_MAX_HEADER_LENGTH));
  sprintf(header, "%d", destination);

  // Fill in the header of the message
  strncpy(retval->header, header, MESSAGE_MAX_HEADER_LENGTH - 1);
  free(header);

  // Assign a message id to the message
  retval->message_id = msg_count++;

  // Return the newly created message
  return retval;
}

