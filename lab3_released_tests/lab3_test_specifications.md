### TS310: 3 part general message passing test
** Generalized mailbox message passing scenarios with multiple senders and 1 receiver, 1 receiver and 1 sender

*test0_start: multiple senders, 1 receiver (all of medium priority)
make 1 receiving task and 3 sending tasks
the third sender sends message via a blocking call to the receiver and exits. It also fills up the receiver. The receiver is still not scheduled hence it cannot run.
the second sender sends a message to the receiver but gets blocked. Same for the first sender.
the recevier is finally allowed to run and it receives the message from sender 3, sender 2 and sender 1 in that order.

*test1_start: 1 blocked receiver and 1 sender sending to the blocked receiver (all of medium priority)
make 1 receiver. Create its mailbox and make a blocking receive on the receiver to make it block.
populate a buffer in a sender and make a blocking send on the receiver's mailbox
the receiver unblocks after the sender is done with its task

*test1_start3: 1 receiver (medium priority) and 2 senders (high priority)
Make 1 receiver and make it blocked on the receive on empty mailbox.
1 sender sends message to the receiver's mailbox and then exits
the other sender tries to send a message to the receiver but gets blocked.
the receiver runs and gets the messages from sender 1 and then from sender 2


### TS320: Simple non-blocking send/recv test
Initialized the system with HIGH priority task0.
The task0 creates two HIGH priority tasks: task1 and task2.
The task0 lower its priority to LOW.
The task1 creates a mailbox, allocates receive buffer and then keeps calling non-blocking receive and yields the cpu
The task2 allocates a send buffer and then does a non-blocking send with one char to task1. It then yields the cpu. 
The task1 should receive the char sent by task2 and then the test verifies the execution sequence.


### TS330:
** Filling up a task's mailbox to max capacity

A task (task1) and a mailbox of size 'X' is created
Another task (task0) sends two messages each of size 'X/2' to the former task to essentially fill up its mailbox
The receving task must receive both the messages one after the other. The received messages are checked for their integrity.
task0 again send a message of size X to fill up the mailbox in one single go
task1 should receive the message as a whole


### TS331:
**Creation of multiple message receivers of same priority and one sender.

Create a 4 worker task that will receive message sent by a sender. They will get blocked on a blocking receive.
The sender sends a broadcast message to the all the workers. 
All of the workers send the same messaged back to the sender.


### TS332:
**Checks for passing of message in chain of tasks and correct reception in each and every one of them

A root task first creates a mailbox and a child task.
Each task creates its own mailbox and a new child task until there are total 6 tasks in the system (1 root task and 5 child task)
The root node propagates a '1' as data in the message to its child. This data is received by each child, incremented by one and propagated further to its own child.
The last child sends the incremented data back to the root node for verification. If the root node received the data back as '6' it verifies that the data was propagated and passed correctly in each child.


### TS340:
**Testing the Sending Task Wait List Functionality

Task 2 has a mailbox of 32 bytes
Task 1 sends 30 bytes of data to Task 2 via a blocking call
Task 1 sends 8 bytes of data to Task2 via a blocking call. This must block task 1 and add it to the STWL of Task2.
Task 1 sends 30 bytes of data to Task 2 via a blocking call. This must be received by the task2.


### TS350:
**KCD and Console Display Task Test

Test 1 : Try registering 'H' as a task command identifier for a task. This should be valid.
Test 2 : Try registering 'G' as a task command identifier for a task. This should be valid.
Test 3 : Try to send a message to the console display task if Test 2 passed


### TS360:
**Corner Case Detection around mailboxes for Lab3

Try sending message to a task that does not have a mailbox
Try creating a second mailbox in a task
Try sending a message more than the size of a task's mailbox
Try to create a mailbox with memory more than required.
