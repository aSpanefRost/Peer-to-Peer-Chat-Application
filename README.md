# README
## Compilation Procedure

* Go the directory containing the files
* Run the following command in terminal to compile the code:
    *  ` make `
*  This will create a executable file named 'chat' in the present working directory

## Running Procedure

* Write the following command in terminal to run the application:
    * ` ./chat <port number>`
* Here, port number should be one of the ports specified in users.txt
* In the users.txt file, change the port number according to your preference
* Open a new terminal window and run the above command with another available port number mentioned in users.txt
* Suppose, in one terminal window, ` ./chat 4269 ` was entered and in the other terminal window, ` ./chat 4242 ` was entered.
* Write the following text in the first terminal window:
    * ` Mamata/ Hi! `
* You will see that the "Yogi Hi!" message is displayed in the second terminal window
* Similarly, you can open upto 5 terminal windows to test out the application

## Design Requirements
* The entire chat application runs under a single process.
* Every participating user maintains a user_info table that contains-
  * name of the friend
  * IP address of the machine where the chat application is running
  * port number at which the peer-chat server is running.
* This table is static and shared a priori with all the participating users
* For the chat purpose, a user enters a message using the keyboard. The message format is as follows: `friendname/<msg>` where friendname is the name of the friend to whom the user wants to send a message, and msg is the corresponding message.
* The communication mode is asynchronous, indicating that a user can enter the message anytime. The console should always be ready to accept a new message from the keyboard, unless the user is already typing another message.

## Protocol Primitives
* As we mentioned earlier, every instance of the chat application runs a TCP server which binds itself to a well known port and keeps on listening for the incoming connections. You may assume a maximum of 5 peers (users), so a maximum of 5 connections need to be supported.
* Once the server receives a connection, it accepts the connection, reads the data from that connection, extracts the message, and displays it over the console.
* Once a user enters a message through the keyboard,
   * The message is read from the stdin, the standard input file descriptor.
   * The application checks whether a client socket to the corresponding server already exists. If a client socket does not exist, then a client socket is created to the corresponding server based on the lookup over user_info table
   * The message is sent over the client socket
* If there is no activity over the client socket for a timeout duration, then the client socket is closed.

## Screenshot
![](https://i.imgur.com/s7vKmiN.png)

## Flow Diagram
![](https://i.imgur.com/0SJSruV.png)


