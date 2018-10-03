/** @file message.h
 *  @brief Function prototypes and structure definitions for the message library.
 *
 * The token ring message library is developed to
 * allow the easy creation, reading, and management
 * of token ring messages.
 *
 *  @author Joshua Edgcombe (joshedgcombe@gmail.com)
 *  @bug No known bugs.
 */

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#define MESSAGE_MAX_HEADER_LENGTH 100
#define MESSAGE_MAX_BODY_LENGTH 1024

// Message definition
typedef struct message {
  int message_id;
  /* char *header; */
  /* char *body; */
  char header[MESSAGE_MAX_HEADER_LENGTH];
  char body[MESSAGE_MAX_BODY_LENGTH];
} message;

// Message queue definition
typedef struct message_queue {
  message *msg;
  struct message_queue *next;
} message_queue;

/** @brief Creates a new message and returns a pointer to a message struct.
 *
 *  Creates a token ring message to be passed around the
 *  token ring network. The function returns a pointer to a
 *  new message on success or a NULL pointer on failure. If
 *  a negative destination is supplied, a blank message will
 *  be created.
 *
 *  @param destination The token ring destination endpoint id.
 *  @param body The body of the message to be sent.
 *  @return The token ring message struct.
 */
message *message_create(int destination, char *body);

/** @brief Acknowledges a messages reception by modifying the header.
 *
 *  Modifies the header field to represent a received message.
 *
 *  @param head The message that will be acknowledged.
 *  @return Void.
 */
void message_acknowledge(message *msg);

/** @brief Get the oldest message on the message queue supplied.
 *
 *  Returns a message pointer for the oldest message on the
 *  message queue supplied. If the supplied message queue is
 *  NULL, a blank message is returned.
 *
 *  @param head The message queue to get a message from.
 *  @return The oldest message on the message queue supplied.
 */
message *message_queue_get_message(message_queue *head);


/** @brief Print the message provided in a descriptive manner.
 *
 *  Prints out a useful string-based representation of the
 *  message provided to standard output.
 *
 *  @param msg The message to be printed to standard output.
 *  @return Void.
 */
void message_print(message *msg);

#endif // __MESSAGE_H__
