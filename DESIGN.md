# Overall Design

The design is broken down into three main process types, the admin process and the node process.

The admin process is a single-threaded process in charge of receiving input from the user and sending control messages to the appropriate node processes.

The node process is a double-threaded process. One thread handles the token ring network behavior of the node while the second thread handles communication with the admin process. The admin process has a write and read pipe that can be used to communicate with the admin process asynchronously.

# Startup

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

1. Limit message length to

## Output

1. The output for all token ring nodes is an output file, while the main admin process uses standard output and standard input.
