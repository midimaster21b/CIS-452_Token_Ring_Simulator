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
 *  @param msg The message that will be acknowledged.
 *  @return Void.
 */
void message_acknowledge(message *msg);

/** @brief Clears the message supplied.
 *
 *  Clears the header and body of the supplied message.
 *
 *  @param msg The message that will be cleared.
 *  @return Void.
 */
void message_clear(message *msg);

/** @brief Clears the message from the message queue.
 *
 *  Removes the message from the message queue. This
 *  function is to be used when the message has been
 *  successfully delivered and acknowledged.
 *
 *  @param msg The message to be finalized.
 *  @param head The message queue to be updated.
 *  @return The new head pointer.
 */
message_queue *message_complete(message *msg, message_queue *head);

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

/** @brief Append the message to the message queue.
 *
 *  Append the supplied message to the message queue provided.
 *  If the supplied message queue is NULL, the message will be
 *  put in a newly constructed message_queue struct as the
 *  first element and the pointer will be returned.
 *
 *  @param msg The message to be appended.
 *  @param head The message queue to be appeneded.
 *  @return A pointer to the modified message queue.
 */
message_queue *message_queue_put_message(message *msg, message_queue *head);

/** @brief Print the message provided in a descriptive manner.
 *
 *  Prints out a useful string-based representation of the
 *  message provided to standard output.
 *
 *  @param msg The message to be printed to standard output.
 *  @return Void.
 */
void message_print(message *msg);

/** @brief Print the message queue provided in a descriptive manner.
 *
 *  Prints out a useful string-based representation of the
 *  message queue provided to standard output.
 *
 *  @param head The message queue to be printed to standard output.
 *  @return Void.
 */
void message_queue_print(message_queue *head);

/** @brief Print the message queue element provided in a descriptive manner.
 *
 *  Prints out a useful string-based representation of the
 *  message queue element provided to standard output.
 *
 *  @param head The message queue element to be printed to standard output.
 *  @return Void.
 */
void message_queue_print_element(message_queue *head);

#endif // __MESSAGE_H__
