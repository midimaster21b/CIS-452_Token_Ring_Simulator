/** @file endpoint.h
 *  @brief Function prototypes and structure definitions for the endpoint library.
 *
 * The endpoint library is developed to
 * allow the easy creation, deletion, and management
 * of token ring simulation endpoints.
 *
 *  @author Joshua Edgcombe (joshedgcombe@gmail.com)
 *  @bug No known bugs.
 */

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

// Doubly linked list for management purposes
typedef struct endpoint_list {
  endpoint *endp;
  struct endpoint_list *next;
  struct endpoint_list *prev;
} endpoint_list;

/** @brief Requests and returns the desired number of endpoints specified by the user.
 *
 *  Requests the user's desired number of endpoints for the
 *  token ring simulator and returns this number as an int.
 *
 *  @return The number of token ring endpoints the user desires.
 */
int request_num_endpoints(void);

/** @brief Creates a new endpoint and process and returns the endpoints descriptor struct.
 *
 *  Creates a token ring endpoint to be associated with
 *  token ring specified as the id parameter. This function
 *  starts a process for the endpoint using the fork command
 *  and returns the resulting token ring endpoint descriptor
 *  struct is returned. The endpoint pid value will be zero
 *  if the process the program is currently in is the child
 *  process(return value of fork). The function will return
 *  a NULL pointer on process creation failure.
 *
 *  @param id The token ring endpoint id.
 *  @return The token ring endpoint descriptor struct.
 */
endpoint *create_endpoint(int id);

/** @brief Adds token endpoint supplied to the supplied endpoint list.
 *
 *  Creates an endpoint_list struct for the supplied endpoint
 *  and inserts it into the appropriate place in the supplied
 *  endpoint list. REQUIRES A COHERENT LIST BE SUPPLIED!!!
 *
 *  @param endpoint_list_head A pointer to an element in the list to be modified.
 *  @param token_endpoint The endpoint you'd like added to the supplied list.
 *  @return The endpoint_list struct containing the endpoint with the lowest token_id in the list
 */
endpoint_list *endpoint_list_add(endpoint_list *endpoint_list_head, endpoint *token_endpoint);

#endif // __ENDPOINT_H__
