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

# Normal Operation

The token ring network simulator works by passing a message between node processes using pipes. When the message being passed has a body and header that are zero length strings the message is considered blank and can be filled by any process which has a message in it's message queue. The message queue is implemented as a singlely linked list that is used as a FIFO message queue. If a message is available on the process's message queue and a blank message is read, the queue fills the message with it's oldest message and writes that message to it's token pipe write end.

## Token Operation

When a token is first read, it is checked to see if it is a blank token. If the token is blank, the node checks the queue for messages. If the node's queue contains any queued messages the token is filled with the oldest queued message and passed to the next node. If the node's queue is empty, then a blank node is passed to the next node. If the token received was not blank, the header is first checked to see if the current node is the destination node. If it is, the contents are read and appended to the output file and the header is changed to a single zero to acknowledge the message before being passed to the next node. If the node had previously sent a message and the header contains a single zero, the token is cleared and passed to the next node. If the token is not blank, doesn't match the current node as a destination, and a message wasn't previously sent from the current node, the token is simply passed to the next node in the ring.

It's worth noting that if a single node has multiple messages in it's message queue and another node also has multiple messages in it's message queue the behavior of the network is to alternate between which node sends data. This behavior is due to the clearing of the token on successful sending and acknowledging a message instead of supplying the next message in the queue. This disallows any node in the network from monopolizing the networks token and prevents starvation of other nodes.

# Major Libraries

## Message

The message library is written to enable easy creation, deletion, and management of messages as they are used within a token ring network.

## Endpoint

The endpoint library is written to enable easy creation, deletion, and management of endpoints as they are used within a token ring network. An endpoint represents a single node in the token ring network.

# Design Decisions

1. Limit message body length to an amount specified by the constant MESSAGE_MAX_BODY_LENGTH and the message header to an amount specified by the constant MESSAGE_MAX_HEADER_LENGTH. These constants are defined in the message library.
1. The user admin interface is separate from the token ring node output. The output for all token ring nodes is an output file, while the main admin process uses standard output and standard input. This prevents the screen from filling up while the user is using the admin interface.
1. Upon recognizing a successfully sent message from the current node, a blank token is passed to the next node. This prevents any one node from monopolizing the networks bandwidth.
1. A quit keyword can be used to exit the program at any time.
1. The admin process sends messages to children processes using a pipe for each specific node. These pipes are stored in the admin_pipes variable.
1. A message structure was used to model the message that is passed between nodes.
1. A message queue object is used to store all messages to be sent by a specific node. The message queue is created as a struct organized as a singlely linked list that is used as a FIFO buffer.
1. Each message has a unique message_id.
1. The admin process is the direct parent of all node processes.
1. Each endpoint has a struct that describes everything about the node.
1. A doubly linked list is used by the admin process to maintain an understanding of the organization of the network. This was intended to be used to diagnose issues with the network and potentially insert and remove nodes from the network in realtime. This structure is not used by the node processes.
1. Each node process consists of two constantly running threads. One is the admin thread which waits for, and handles, information from the admin process and the other is the token ring thread which waits for the token to be read by this process, processes it, and writes the appropriate token to the output. The two threads were separated to allow dynamic reconfiguration of the nodes using the admin process and allow the restarting of the token process when appropriate.


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
