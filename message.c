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

  // if non-blank message desired (zero or positive destination supplied)
  if(destination >= 0) {
    // Get the header string
    sprintf(retval->header, "%d", destination);

    // Copy the body string into the message struct
    strncpy(retval->body, body, MESSAGE_MAX_BODY_LENGTH - 1);
  }

  // Else create a blank message
  else {
    retval->header[0] = '\0';
    retval->body[0] = '\0';
  }

  // Assign a message id to the message
  retval->message_id = msg_count++;

  // Return the newly created message
  return retval;
}

void message_print(message *msg) {
  printf("Message provided: (%p)\n", msg);
  printf("ID: %d (%p)\n", msg->message_id, &(msg->message_id));
  printf("Header: %s (%p)\n", msg->header, msg->header);
  printf("Body: %s (%p)\n", msg->body, msg->body);
}
