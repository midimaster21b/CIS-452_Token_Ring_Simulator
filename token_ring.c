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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "endpoint.h"
#include "message.h"

#define SIMULATION_SLEEP_TIME 1
#define ENDPOINT_BASE_ADDR 1

void *token_ring_passer(void *endpoint_descriptor);
void *admin_thread_handler(void *endpoint_descriptor);

int child_process_flag = 0;
int admin_running = 1;
pthread_t admin_thread, token_thread;
message_queue *msg_queue = NULL;

int main() {
  int wraparound_fd[2];
  int temp_pipe_fd[2];
  int num_endpoints;
  int endpoint_iterator;
  endpoint_list *endpoint_list_head = NULL;
  char *output_filename = "output.txt";
  FILE *output_file;

  // A temporary endpoint used to describe the currently operating node [2]
  endpoint *temp_endpoint;

  // Admin pipe descriptors for communicating with nodes [1]
  int *admin_pipes;

  // Welcome the user to the program
  printf("Welcome to the CIS 452 Token Ring Simulator\n");
  printf("===========================================\n");

  // Get the desired number of endpoints to create from the user
  num_endpoints = request_num_endpoints();

  // Allocate space for admin control pipes [1]
  admin_pipes = malloc(num_endpoints * sizeof(int));

  // Prompt user
  printf("You have requested %d endpoints. Creating now...\n", num_endpoints);

  if(pipe(wraparound_fd) != 0) {
    printf("ERROR: Couldn't create the wraparound pipe.\n");
    exit(1);
  }

  // Open handle to output file
  output_file = fopen(output_filename, "a");

  /////////////////////////////////////
  // Create the appropriate endpoints
  /////////////////////////////////////
  for(endpoint_iterator=ENDPOINT_BASE_ADDR; endpoint_iterator<num_endpoints+ENDPOINT_BASE_ADDR; endpoint_iterator++) {

    // Create the endpoint
    printf("Creating endpoint %d...\n", endpoint_iterator);
    temp_endpoint = create_endpoint(endpoint_iterator);

    // Handle error in endpoint creation
    if(temp_endpoint == NULL) {
      // TODO: Clean up any and all resources used and report the error to the user
      printf("Error: Unable to create endpoint %d.\n", endpoint_iterator);
      exit(1);
    }

    // Connect read node from previous node, save current read node for next connection
    temp_pipe_fd[1] = temp_endpoint->token_pipe[PIPE_READ_INDEX];
    temp_endpoint->token_pipe[PIPE_READ_INDEX] = temp_pipe_fd[0];
    temp_pipe_fd[0] = temp_pipe_fd[1];

    // If first element
    if(endpoint_iterator == ENDPOINT_BASE_ADDR) {
      // Use read endpoint of wraparound pipe
      temp_endpoint->token_pipe[PIPE_READ_INDEX] = wraparound_fd[PIPE_READ_INDEX];
    }

    // Else if last element
    else if(endpoint_iterator >= num_endpoints + ENDPOINT_BASE_ADDR - 1) {
      // Close original pipe associated with last element
      close(temp_pipe_fd[0]);                             // Read end from previous node
      close(temp_endpoint->token_pipe[PIPE_WRITE_INDEX]); // Write end

      // Use write end of wraparound pipe
      temp_endpoint->token_pipe[PIPE_WRITE_INDEX] = wraparound_fd[PIPE_WRITE_INDEX];
    }

    // Child behavior
    // TODO: FREE UNUSED MEMORY AND RESOURCES
    // TODO: CLOSE ALL PIPES WITH AUTHORITY/OWNERSHIP!
    // TODO: Spawn off thread to handle control/admin interaction (pthread_create)
    if(temp_endpoint->pid == 0) {
      // Set child process flag
      child_process_flag = 1;

      // Redirect stdout to output file
      dup2(fileno(output_file), STDOUT_FILENO);

      // Get child and parent PID
      temp_endpoint->pid = getpid();

      // Clean up any and all resources used, but unnecessary for child processes
      // Close unused admin end of admin pipe
      close(temp_endpoint->admin_pipe[PIPE_WRITE_INDEX]);

      // Close wraparound pipe (if able)
      // If first element
      if(endpoint_iterator == ENDPOINT_BASE_ADDR) {
	close(wraparound_fd[PIPE_WRITE_INDEX]);
      }

      // Else if last element
      else if(endpoint_iterator >= num_endpoints + ENDPOINT_BASE_ADDR - 1) {
	close(wraparound_fd[PIPE_READ_INDEX]);
      }

      // If not first or last
      else {
	// Close the wraparound pipe
	close(wraparound_fd[PIPE_WRITE_INDEX]);
	close(wraparound_fd[PIPE_READ_INDEX]);
      }

      // Close all open admin pipes
      int pipe_iterator;
      for(pipe_iterator=0; pipe_iterator<num_endpoints; pipe_iterator++) {
	close(admin_pipes[pipe_iterator]);
      }

      // Free temp_endpoint space
      free(admin_pipes); // Child free [1]

      // Create threads for the admin and token handlers
      pthread_create(&admin_thread, NULL, admin_thread_handler, temp_endpoint);
      pthread_create(&token_thread, NULL, token_ring_passer, temp_endpoint);

      // Exit the for loop
      break;
    }

    // Parent behavior
    // TODO: Spawn off thread to handle user interaction (pthread_create)
    else {
      // Add control pipe endpoints to array ([1] assignment)
      admin_pipes[endpoint_iterator-ENDPOINT_BASE_ADDR] = temp_endpoint->admin_pipe[PIPE_WRITE_INDEX];

      // Close unused pipes
      // Close read end of admin pipe (only used by child)
      close(temp_endpoint->admin_pipe[PIPE_READ_INDEX]);

      // Close token pipe (only used by child)
      /* close(temp_endpoint->token_pipe[PIPE_READ_INDEX]); */
      /* close(temp_endpoint->token_pipe[PIPE_WRITE_INDEX]); */

      // Add the endpoint to the list
      endpoint_list_head = endpoint_list_add(endpoint_list_head, temp_endpoint);

      // Free temp_endpoint space (already copied into the endpoint_list)
      free(temp_endpoint);
    }
  }

  // Child process behavior
  if(child_process_flag) {
    pthread_join(admin_thread, NULL);
    pthread_join(token_thread, NULL);

    printf("Child thread exited...\n");
    while(1);
  }

  // Parent process behavior
  else {
    printf("Admin process: %d\n", getpid());

    // Admin variables
    int destination_id, source_id;

    // Start the token ring sequence (bootstrap)
    // Create a blank message to directly start the token ring
    message *msg = message_create(-1, NULL);

    // Write the first message to the pipeline
    write(endpoint_list_head->endp->token_pipe[PIPE_WRITE_INDEX], msg, sizeof(message));

    const char *quit_text = "quit";

    // Allocate space for the message body and header
    char *msg_body = malloc(MESSAGE_MAX_BODY_LENGTH);
    char *msg_header_from = malloc(MESSAGE_MAX_HEADER_LENGTH);
    char *msg_header_to = malloc(MESSAGE_MAX_HEADER_LENGTH);

    // Main admin loop
    while(admin_running) {
      // Get the user's input
      // TODO: Validate user input
      printf("Please enter a node to send a message from: ");
      fgets(msg_header_from, MESSAGE_MAX_HEADER_LENGTH, stdin);

      // if the user attempted to exit the program using exit keyword
      if(strncmp(msg_header_from, quit_text, 4) == 0) {
	admin_running = 0;
	break;
      }

      printf("Please enter a node to send a message to: ");
      fgets(msg_header_to, MESSAGE_MAX_HEADER_LENGTH, stdin);

      // if the user attempted to exit the program using exit keyword
      if(strncmp(msg_header_to, quit_text, 4) == 0) {
	admin_running = 0;
	break;
      }

      printf("Please enter a message for the network: ");
      fgets(msg_body, MESSAGE_MAX_BODY_LENGTH, stdin);

      // if the user attempted to exit the program using exit keyword
      if(strncmp(msg_body, quit_text, 4) == 0) {
	admin_running = 0;
	break;
      }

      // Get the destination node id from the string provided by the user
      destination_id = strtol(msg_header_to, NULL, 10);

      // Get the source node id from the string provided by the user
      source_id = strtol(msg_header_from, NULL, 10);

      // Create the message to be sent
      msg = message_create(destination_id, msg_body);

      // Write the message to the admin pipe
      write(admin_pipes[source_id - 1], msg, sizeof(message));
    }

    /*******************************
     * Parent synchronous cleanup.
     *******************************/
    // Perform initial synchronous cleanup
    // Free current scope memory
    free(msg_body);
    free(msg_header_from);
    free(msg_header_to);

    // Free parent specific pipes
    for(endpoint_iterator=0; endpoint_iterator<num_endpoints; endpoint_iterator++) {
      close(admin_pipes[endpoint_iterator]);
    }

    // Free parent specific memory
    // Free admin pipes parent [1]
    free(admin_pipes);
  }

  // Perform synchronous exit cleanup
  printf("Process %d: Exiting...\n", getpid());

  return 0;
}

// Token passing thread
void *token_ring_passer(void *endpoint_descriptor) {
  endpoint *endpoint_description = endpoint_descriptor;

  // Thread descriptor variables
  int token_id = endpoint_description->token_id;
  int token_rd_pipe = endpoint_description->token_pipe[PIPE_READ_INDEX];
  int token_wr_pipe = endpoint_description->token_pipe[PIPE_WRITE_INDEX];

  // Message variables
  message *msg_buffer = malloc(sizeof(message));
  int msg_sent_flag = 0;
  int msg_dest = 0;
  int rd_len = 0;

  while(1) {
    // Read
    rd_len = read(token_rd_pipe, msg_buffer, sizeof(message));

    // Process
    // TODO: Handle incomplete reads (not 100% of bytes in first read)
    printf("\nEndpoint %d (%d) read in %d of %ld bytes\n", token_id, endpoint_description->pid, rd_len, sizeof(message));

    // Non-blank message received
    if(strlen(msg_buffer->header) > 0) {

      // Get message destination from string
      msg_dest = strtol(msg_buffer->header, NULL, 10);

      // Handle message reception for this node
      if(msg_dest == token_id) {
	printf("Endpoint %d: Received message: %s", token_id, msg_buffer->body);

	// Acknowledge reception of message
	message_acknowledge(msg_buffer);
      }

      // Handle message successfully sent
      else if(msg_sent_flag) {
	if(msg_dest == 0) {
	  printf("Endpoint %d: Message successfully sent and acknowledged.\n", token_id);

	  // Clear the message sent flag
	  msg_sent_flag = 0;

	  // Finalize message
	  msg_queue = message_complete(msg_buffer, msg_queue);

	  // Fill the message buffer with a new blank message
	  msg_buffer = message_create(-1, NULL);
	}

	// Handle message not acknowledged
	else {
	  printf("Endpoint %d: Message failed to be received.\n", token_id);
	}
      }

      // Not intended destination, nor did this node send anything
      else {
	printf("Endpoint %d: Passing token ahead...\n", token_id);
      }
    }

    // Blank message received
    else {
      // If a message is available
      if(msg_queue != NULL) {
	printf("Endpoint %d: Putting new message on blank token.\n", token_id);

	// set the message sent flag
	msg_sent_flag = 1;

	// Get rid of the old message
	free(msg_buffer);

	// Retrieve it from the message queue
	msg_buffer = message_queue_get_message(msg_queue);
      }

      // Pass the message that was received
      else {
	// do nothing.
	printf("Endpoint %d: Blank token found.\n", token_id);
      }
    }

    // Wait for predefined seconds (allows progress to be tracked by humans)
    sleep(SIMULATION_SLEEP_TIME);

    // Write
    write(token_wr_pipe, msg_buffer, sizeof(message));
  }
}

void *admin_thread_handler(void *endpoint_descriptor) {
  endpoint *endpoint_description = endpoint_descriptor;

  // Thread descriptor variables
  int admin_rd_pipe = endpoint_description->admin_pipe[PIPE_READ_INDEX];

  // Message variables
  message *msg_buffer = malloc(sizeof(message));

  while(1) {
    // Read
    read(admin_rd_pipe, msg_buffer, sizeof(message));

    // Process
    msg_queue = message_queue_put_message(msg_buffer, msg_queue);

    // Write (if necessary)
  }
}
