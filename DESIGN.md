# Overall Design

The design is broken down into two main process types, the admin process and the node process.

The admin process is a single-threaded process in charge of receiving input from the user and sending control messages to the appropriate node processes.

The node process is a double-threaded process. One thread handles the token ring network behavior of the node while the second thread handles communication with the admin process. The admin process has a write and read pipe that can be used to communicate with the admin process asynchronously.

# Startup

1. The user is prompted to enter a number of endpoints desired in the token ring.
1. The original process, which remains as the admin process after processes have been created, will fork off 'n' processes where 'n' is the number of endpoints desired by the user. This results in 'n' node processes and 1 admin process resulting in n+1 total processes.
1. Processes are all started and the pipes are all connected.
1. The wraparound pipe is connected.
1. A blank message is written to the base node's write pipe which begins the token traversing the token ring network.
1. The remainder of the message queueing is handled by message queues that are independently managed by each of the nodes/processes' message handler thread.

# Major Libraries

## Message

The message library is written to enable easy creation, deletion, and management of messages as they are used within a token ring network.

## Endpoint

The endpoint library is written to enable easy creation, deletion, and management of endpoints as they are used within a token ring network. An endpoint represents a single node in the token ring network.

# Design Decisions

## Input

1. Limit message body length to an amount specified by the constant MESSAGE_MAX_BODY_LENGTH and the message header to an amount specified by the constant MESSAGE_MAX_HEADER_LENGTH. These constants are defined in the message library.

## Output

1. The output for all token ring nodes is an output file, while the main admin process uses standard output and standard input.

## Child process identification

- As child processes are forked, the return value from fork is used to differentiate between the parent and child processes. After differentiation, the global child_process_flag identifies a process as a child process.

# Object Lifecycles

Proper lifecycle expectations vs. terminated (using a signal)

[1] admin_pipes: A list of write endpoints for the admin process to use.

    1. Creation: Malloc'd using the specified number endpoints requested by the user.
    1. Fork'd: An instance remains in memory for both the parent and child processes.
    1. Child destruction: The child free's the memory prior to admin and token thread creation.
    1. Parent destruction: The parent free's the memory when exiting the application. [TODO!]

[2] temp_endpoint
