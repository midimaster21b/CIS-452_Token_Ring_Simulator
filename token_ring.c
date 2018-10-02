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

#define MESSAGE_BUFFER_SIZE 1000

void *token_ring_passer(void *endpoint_descriptor);

int child_process = 0;
int pid, parent_pid;
int token_id = -1;
pthread_t admin_thread, token_thread;

int main() {
  int wraparound_fd[2];
  int temp_rd_old;
  int temp_rd;
  int num_endpoints;
  int x;
  endpoint_list *endpoint_list_head = NULL;
  endpoint *temp_endpoint;

  // Admin pipes (parent process)
  int *admin_rd_pipes, *admin_wr_pipes;

  // Endpoint pipes (child process)
  /* int admin_rd_pipe, admin_wr_pipe, token_rd_pipe, token_wr_pipe; */

  // Welcome the user to the program
  printf("Welcome to the CIS 452 Token Ring Simulator\n");
  printf("===========================================\n");

  // Get the desired number of endpoints to create from the user
  num_endpoints = request_num_endpoints();

  // Allocate space for admin control pipes
  admin_rd_pipes = malloc(num_endpoints * sizeof(int));
  admin_wr_pipes = malloc(num_endpoints * sizeof(int));

  // Prompt user
  printf("You have requested %d endpoints. Creating now...\n", num_endpoints);

  if(pipe(wraparound_fd) != 0) {
    printf("ERROR: Couldn't create the wraparound pipe.\n");
  }

  // Create the appropriate endpoints
  for(x=0; x<num_endpoints; x++) {
    printf("Creating endpoint %d...\n", x);

    temp_endpoint = create_endpoint(x);

    // Put read from previous node into current node, but save current node rd
    temp_rd_old = temp_endpoint->token_pipe[PIPE_READ_INDEX];
    temp_endpoint->token_pipe[PIPE_READ_INDEX] = temp_rd;
    temp_rd = temp_rd_old;

    // If first element
    if(x == 0) {
      temp_endpoint->token_pipe[PIPE_READ_INDEX] = wraparound_fd[PIPE_READ_INDEX];
    }

    // Else if last element
    else if(x == num_endpoints - 1) {
      /* wraparound_wr_fd = temp_endpoint->token_pipe[PIPE_WRITE_INDEX]; */
      // ERROR: THIS DOESN'T CLOSE OLD PIPE, JUST REPLACES WITH THE WRAPAROUND!!!
      // TODO: FIX THIS!!!!
      temp_endpoint->token_pipe[PIPE_WRITE_INDEX] = wraparound_fd[PIPE_WRITE_INDEX];
    }

    // Handle error in endpoint creation
    if(temp_endpoint == NULL) {
      // Clean up any and all resources used and report the error to the user
      printf("Error: Unable to create endpoint.\n");
    }

    // Child behavior
    // TODO: FREE UNUSED MEMORY AND RESOURCES
    // TODO: CLOSE ALL PIPES WITH AUTHORITY/OWNERSHIP!
    // TODO: Spawn off thread to handle control/admin interaction (pthread_create)
    else if(temp_endpoint->pid == 0) {
      // Set child process flag
      child_process = 1;

      // Perform and necessary setup and cleanup for child process running
      // Get pipe assignments from endpoint
      /* admin_rd_pipe = temp_endpoint->admin_wr_pipe[PIPE_READ_INDEX]; */
      /* admin_wr_pipe = temp_endpoint->admin_rd_pipe[PIPE_WRITE_INDEX]; */
      /* token_rd_pipe = temp_endpoint->token_pipe[PIPE_READ_INDEX]; */
      /* token_wr_pipe = temp_endpoint->token_pipe[PIPE_WRITE_INDEX]; */

      // Get token id
      token_id = temp_endpoint->token_id;

      // Get child and parent PID
      pid = getpid();
      parent_pid = getppid();

      // Clean up any and all resources used, but unnecessary for child processes
      // Close unused pipe
      /* close(temp_endpoint->admin_wr_pipe[PIPE_WRITE_INDEX]); */
      /* close(temp_endpoint->admin_rd_pipe[PIPE_READ_INDEX]); */

      // Free temp_endpoint space
      /* free(temp_endpoint); */
      /* free(admin_wr_pipes); */
      /* free(admin_rd_pipes); */

      pthread_create(&token_thread, NULL, token_ring_passer, temp_endpoint);

      // Exit the for loop
      break;
    }

    // Parent behavior
    // TODO: Spawn off thread to handle user interaction (pthread_create)
    else {
      // Add the endpoint to the list
      endpoint_list_head = endpoint_list_add(endpoint_list_head, temp_endpoint);

      // Add control pipe endpoints to array
      admin_wr_pipes[x] = temp_endpoint->admin_wr_pipe[PIPE_WRITE_INDEX];
      admin_rd_pipes[x] = temp_endpoint->admin_rd_pipe[PIPE_READ_INDEX];

      // Close unused pipes
      /* close(temp_endpoint->admin_wr_pipe[PIPE_READ_INDEX]); */
      /* close(temp_endpoint->admin_rd_pipe[PIPE_WRITE_INDEX]); */
      /* close(temp_endpoint->token_pipe[PIPE_READ_INDEX]); */
      /* close(temp_endpoint->token_pipe[PIPE_WRITE_INDEX]); */

      // Free temp_endpoint space
      /* free(temp_endpoint); */
    }
  }

  // Child process behavior
  while(child_process) {
    pthread_join(token_thread, NULL);

    printf("Child thread exited...\n");
    while(1);
  }

  // Parent process behavior
  while(!child_process) {

    char *msg_body = malloc(MESSAGE_MAX_BODY_LENGTH);

    // Get the user's input
    printf("Please enter a message for the network: ");
    fgets(msg_body, MESSAGE_MAX_BODY_LENGTH, stdin);

    /* printf("Message received: %s (%p)\n", msg_body, msg_body); */

    // Create the message to be passed to the network
    message *a = message_create(5, msg_body);

    // Print the created message to stdout
    /* message_print(a); */

    // Write a test message to the pipeline
    write(endpoint_list_head->endp->token_pipe[PIPE_WRITE_INDEX], a, sizeof(message));

    // Busy wait for now
    while(1);
  }

  printf("Exiting...\n");
  return 0;
}

// Token passing thread
void *token_ring_passer(void *endpoint_descriptor) {
  endpoint *endpoint_description = endpoint_descriptor;
  message *msg_buffer = malloc(sizeof(message));

  int token_id = endpoint_description->token_id;
  int token_rd_pipe = endpoint_description->token_pipe[PIPE_READ_INDEX];
  int token_wr_pipe = endpoint_description->token_pipe[PIPE_WRITE_INDEX];

  int rd_len = 0;

  while(1) {
    // Read
    rd_len = read(token_rd_pipe, msg_buffer, sizeof(message));

    // Process
    // TODO: This section
    // TODO: Handle incomplete reads (not 100% of bytes in first read)
    printf("\nEndpoint %d (%d) read in %d of %ld bytes\n", token_id, pid, rd_len, sizeof(message));
    printf("Endpoint %d header: %s\n", token_id, msg_buffer->header);
    printf("Endpoint %d body: %s", token_id, msg_buffer->body);
    sleep(1);

    // Write
    write(token_wr_pipe, msg_buffer, sizeof(message));
  }
}

void *admin_thread_handler(void *endpoint_descriptor) {
  /* endpoint *endpoint_description = endpoint_descriptor; */
  return NULL;
}
