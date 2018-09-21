# CS 452 Program 2
# Simulation: Token-Ring Communication

## Overview

The purpose of this programming assignment is to reinforce your understanding of the mechanisms used in process management system calls (creation, execution, suspension and termination), and in the use of the fundamental OS-level InterProcess Communication API (pipes, signals).

## Activities

- Write a program that uses communicating multiple processes to emulate a form of network communication.
- This is to be an individual programming assignment.
- Submit
  + your well-documented source code
  + a sample run
  + a Design Document describing how your system works, and how it is implemented
- Be prepared to demonstrate your program.
- Please follow the stylistic guidelines you have learned in previous introductory courses (i.e. project documentation, commenting, formatting, etc.).

## Programming Assignment (Token-Ring Communication Network)
At this point you should have a good understanding of the relationship between the fundamental system calls and library functions involved in process management on a Linux system.  You should also have experience with using processes that can communicate via pipes and/or signals.  Use this knowledge to write a program that uses multiple processes communicating via piped IPC to emulate the communication that occurs in the Token-Ring Network Protocol.

The token-ring protocol was an early communication infrastructure for connecting computers in a Local Area Network (LAN); you may have studied it in a networking course.  Machines are organized into a virtual ring topology, with communication occurring unidirectionally around the ring.  All communications have a very well-structured and deterministic profile (unlike the collision-based mechanism used in Ethernet communication), and for a long time it was a popular networking protocol.  However, the increasing speed and decreasing cost of Ethernet allowed it to eventually prevail in the marketplace.

The basic idea behind token-ring communications:
- the k computers in a network are connected in a ring topology (see diagram below)
- a communication token (essentially an empty frame) is passed around the ring in a continuing circular pattern (P1, P2, ..., Pk, P1, P2,...).  Sending/receiving of data occurs only when a computer possesses the token, a protocol which synchronizes the system.
- when a computer (say, P1) has a message to send, it seizes the token as it is circulating by.  It puts the message into the Information field of the empty frame, and puts the ID of the destination machine (say P4) into the Header field of the frame.  The token continues to circulate.
- as each machine receives the token, it examines the Header to see if the frame contains a message intended for that machine.  If the Header does not match the ID of the machine holding the token, it simply passes it on.  If the Header in the frame matches the ID of the machine holding the token, it copies the message and sets the Header back to 0.
- when the token gets back to the original sender, it sees that the Header has been set to 0, and it knows that the message has successfully been delivered.  It removes the message from the frame.

Your assignment is to write a program that uses multiple processes executing in parallel to simulate the operation of the token-ring protocol.  The processes should be organized in a ring configuration, using pipes for communication and coordination.  Signals may be used (depending on your design) to coordinate the behavior of processes in the ring.

## Use Case

Solution demonstration:

1. your program should spawn processes to initialize a virtual local network of a user-defined size.  It should use pipes to establish the ring communication structure.
1. the original parent process (P1) should accept a message from the user (i.e. a string), and a destination process  (e.g. P3).  Note: you may use the process's virtual ID or the real process PID to identify and specify a destination.
1. P1 should construct an Information token with the required fields filled in, pass it on to P2, and so on...
1. the token should circulate around to the destination process, which copies the message and resets the Header field to 0
1. when the token gets back to the sender, P1 should remove the message from the Information field
1. you only need to transmit one message, but the empty token should continue to circulate around the ring
1. when the user hits ^C, the original process (P1) should use signals to gracefully shutdown the simulation

At each step along the way, your program should output diagnostic information to communicate what is happening in the simulation (you might want to put small "sleeps" in between receiving/sending to enable a user to watch what is going on).  The idea here is to help a user understand the operation of the token-ring protocol by watching your simulation execute.

## Guidelines:
- your solution must spawn one process per machine in the network.
- your solution must be scalable (based on a user-defined k as described above).  You may specify a reasonable maximum value for k.  Note: you can examine the "ancestral tree" parent-child relationship of your processes by using the 'ps f' command.
- you must utilize some combination of the standard Linux process management and IPC system calls (i.e. fork, exec, wait, exit, signal, pipe) as introduced in previous labs. The exact design is up to you, which is one reason you must produce a Design Document.
- your system must produce informative/diagnostic output to communicate the status of your program as it executes.  For example, indicate when you are spawning a new process, or sending/receiving the token.  The exact format of the output display is up to you, but it should clearly demonstrate that your system works as specified.

## Grading Rubric (tentative):
- Functionality
  + 'C' level:  create a 3-process system arranged in a linear configuration.  The original (parent)  process should accept a message (i.e. a string) from the user and pipe it to the second process, which pipes the message to the third process.  The third process prints the message to the display.  In effect, this is a 3-process version of the 'echo' command.
  + 'B' level:  implement the linear configuration of the 'C'-level solution, but with the following additions. Your simulation must be scalable; it should accept the value k from the user and spawn exactly k processes connected linearly.  The original (parent)  process should accept a destination (an ID number) and a message (i.e. a string) from the user and pipe this information to the second process, which pipes it on down the linear array of processes.  The final process prints the information to the display.  In effect, this is a k-process version of the 'echo' command.  The original process should continue to accept and forward ID/message information "packets" until the user quits (for example, by entering a 0 for the destination).
  + 'A' level:  implement the full functionality of the Use Case as described above.

- Software Engineering
  + your code follows good software engineering practices
  + your solution works correctly
  + your solution meets specifications
- Documentation
  + your code must be well-documented and properly formatted
  + you must include a complete Design Document describing your solution
