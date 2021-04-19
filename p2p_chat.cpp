// Networks Assignment 8: Peer to Peer Chat Application

// Members:
// Name: Aayush Prasad, 18CS30002
// Name: Rajdeep Das, 18CS30034

#include <sys/select.h>
#include <iomanip>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <bits/stdc++.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
using namespace std;

#define MAX_BUFFER_SIZE 1024
#define BUFSIZE 1024

// Prints the error message
void error(string str)
{
  char *msg = new char[str.length() + 1];
  strcpy(msg, str.c_str());
  perror(msg);
  exit(1);
}

// Structure to store user details
struct detail
{
  string name;
  char *ipadr;
  char *portn;
};

// Returns the person name and the message sent to that person
pair<string, string> getPersonAndMessage(char *buffer)
{
  string person, message;
  int slashes = 0;
  for (int i = 0; i < strlen(buffer); i++)
  {
    if (buffer[i] == '/') slashes++;
    else if (!slashes)
      person += buffer[i];
    else message += buffer[i];
  }
  return make_pair(person, message);
}

// Returns the concatenated char* to be sent
char *getSendbuf(char *buf, string name)
{
  char output[BUFSIZE];
  int len = strlen(buf);
  bool done = false;
  string person, message;
  pair<string, string> temp = getPersonAndMessage(buf);
  person = temp.first;
  message = temp.second;
  strcpy(buf, name.c_str());
  strcat(buf, message.c_str());
  buf[strlen(buf)] = '\0';
  return buf;
}

// returns the list of users given filename
vector<detail> getUserList(string filename)
{
  ifstream myfile;
  myfile.open(filename);

  vector<detail> mp;
  string name, iip, iport;
  while (myfile >> name)
  {
    myfile >> iip >> iport;
    detail det;
    det.ipadr = new char[iip.length() + 1];
    strcpy(det.ipadr, iip.c_str());

    det.portn = new char[iport.length() + 1];
    strcpy(det.portn, iport.c_str());
    det.name = name;
    mp.push_back(det);
  }
  myfile.close();
  return mp;
}

// Displays the Users Table
void displayUserList(vector<detail> &mp)
{
  printf("~~~~~~~~~~~~~~~~~Connected Users~~~~~~~~~~~~~~~\n");
  cout << "-----------------------------------------------\n";
  cout << "|" << setw(15) << "Name" << setw(15) << "IP" << setw(15) << "Port" << "|" << endl;
  cout << "-----------------------------------------------\n";
  for (int i = 0; i < mp.size(); i++)
  {
    cout << "|" << setw(15) << mp[i].name << setw(15) << mp[i].ipadr << setw(15) << mp[i].portn << "|" << endl;
  }
  cout << "-----------------------------------------------\n";
}

// creates a socket
void createSocket(int & srvsock)
{
  // Creating the parent server socket for the user
    /*
        socket creates an endpoint for communication
        domain = AF_INET, IPv4 Internet Protocols
        type = SOCK_STREAM, TCP communication (reliable, two-way connection-based byte streams)
        protocol = 0, Single protocol exists for a protocol type
    */
  srvsock = socket(AF_INET, SOCK_STREAM, 0);
  if (srvsock != -1)
  {
    int optval = 1;
    setsockopt(srvsock, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof(int));
  }
  /*
        sockfd = server_socket
        level = SOL_SOCKET, sockets API level
        optname = SO_REUSEADDR, to reuse the ip address on another run, avoid binding error
    */
  else
    error("ERROR opening socket");

}

// Builds the server address
void buildServerAddress(struct sockaddr_in &serveraddr, int portno)
{
  bzero((char*) &serveraddr, sizeof(serveraddr));
  /*let the system figure out our IP address */
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

  /*this is the port we will listen on */
  serveraddr.sin_port = htons((unsigned short) portno);
  /*this is an Internet address */
  serveraddr.sin_family = AF_INET;
}

// Returns the name of the user running the server
string getName(int portno, vector<detail> &mp)
{
  string myName;
  int checker = 0;
  for (int i = 0; i < mp.size(); i++)
  {
    detail d = mp[i];
    if (atoi(d.portn) != portno || strcmp(d.ipadr, "127.0.0.1"))
      continue;
    else
    {
      cout << "Hello " << d.name << "." << endl << "Start chatting..." << endl;
      myName = d.name;
      checker = 1;
      break;
    }
  }
  if (checker != 0)
  {
    return myName;
  }
  else
  {
    printf("User list does not contain your name.\n");
    exit(1);
  }
  return myName;
}

// Sends the message to a peer
void sendMessage(char *buf, vector<detail> &mp, string myName)
{
  if (read(0, buf, MAX_BUFFER_SIZE) == -1)
  {
    cout << "Error reading from terminal" << endl;
  }
  int sockfd, portno2, n;
  struct sockaddr_in serveraddr2;
  struct hostent * server;
  char hostname[20];
  pair<string, string> personMessage2 = getPersonAndMessage(buf);

  bool isPersonAvailable = false;

  for (detail d: mp)
  {
    if (d.name != personMessage2.first)
      continue;
    else
    {
      strcpy(hostname, d.ipadr);
      portno2 = atoi(d.portn);
      cout << "Sending to : (" << hostname << ":" << portno2 << ")" << endl;
      isPersonAvailable = true;
      break;
    }
  }

  if (isPersonAvailable)
  {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
      error("ERROR opening socket");
    }

    /*gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server)
    { /*build the server's Internet address */
      bzero((char*) &serveraddr2, sizeof(serveraddr2));
      serveraddr2.sin_family = AF_INET;
      bcopy((char*) server->h_addr, (char*) &serveraddr2.sin_addr.s_addr, server->h_length);
      serveraddr2.sin_port = htons(portno2);

      /*connect: create a connection with the server */
      if (connect(sockfd, (struct sockaddr *) &serveraddr2, sizeof(serveraddr2)) == -1)
      {
        error("ERROR connecting");
      }

      /*send the message line to the server */
      strcpy(buf, getSendbuf(buf, myName));
      n = write(sockfd, buf, strlen(buf));
      if (n < 0)
      {
        error("ERROR writing to socket");
      }
      close(sockfd);
    }
    else
    {
      fprintf(stderr, "ERROR, no such host as %s\n", hostname);
      exit(0);
    }
  }
}

// Main function
int main(int argc, char **argv)
{
  // Get the list of users from users.txt
  vector<detail> mp = getUserList("users.txt");

  //Displays the user table
  displayUserList(mp);

  char buf[BUFSIZE];
  fd_set readset, tempset;
  int portno;
  int maxfd, flags;
  struct timeval tv;
  char buffer[MAX_BUFFER_SIZE + 1];
  struct sockaddr_in serveraddr;
  int srvsock, peersock, j, result, result1, sent;
  socklen_t len;
  char iname[50];

  if (argc == 2)
  {
    portno = atoi(argv[1]);
    /*
     *socket: create the parent socket
     */
    createSocket(srvsock);
    /*
     *build the server's Internet address
     */
    buildServerAddress(serveraddr, portno);

    /*
     *bind: associate the parent socket with a port
     */
    if (bind(srvsock, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) != -1)
    { /*
       *listen: make this socket ready to accept connection requests
       */
      /*allow 5 requests to queue up */
      int listen_ret = listen(srvsock, 5);
      if (listen_ret < 0) error("ERROR on listen");
      printf("\nServer is running!\n\n");

      string myName = getName(portno, mp);

      // Initializing and testing fd_set masks for select()
      FD_ZERO(&readset);
      FD_SET(srvsock, &readset);
      FD_SET(0, &readset);
      maxfd = srvsock;

      // Initializing the timeout for select()
      tv.tv_sec = 3;
      tv.tv_usec = 0;
      while (1)
      {
        memcpy(&tempset, &readset, sizeof(tempset));

        // Selects ready fd from set of fds
        result = select(maxfd + 1, &tempset, NULL, NULL, &tv);

        if (!(result >= 0 || errno == EINTR))
          cout << "Error in select (): " << strerror(errno) << endl;
        else if (result > 0)
        {
          // Accepting connections
          if (FD_ISSET(srvsock, &tempset))
          {
            len = sizeof(serveraddr);
            peersock = accept(srvsock, (struct sockaddr *) &serveraddr, &len);
            if (peersock == -1)
            {
              cout << "Error in accept (): " << strerror(errno) << endl;
            }
            else
            {
              FD_SET(peersock, &readset);
              maxfd = (maxfd < peersock) ? peersock : maxfd;
            }
            FD_CLR(srvsock, &tempset);
          }
          // Sending messages
          if (FD_ISSET(0, &tempset))
          {
            sendMessage(buf, mp, myName);
            continue;
          }
          // Receiving messages
          for (j = 1; j <= maxfd; j++)
          {
            if (FD_ISSET(j, &tempset))
            {
              while (1)
              {
                result = recv(j, buffer, MAX_BUFFER_SIZE, 0);
                if (!(result == -1 && errno == EINTR)) break;
              }

              if (result == 0)
              {
                close(j);
                FD_CLR(j, &readset);
              }
              else if (result > 0)
              {

                buffer[result] = '\0';
                pair<string, string> personMessage1 = getPersonAndMessage(buffer);
                string recv_msg = personMessage1.first + ": " + personMessage1.second;
                cout << recv_msg << endl;
              }
              else
              {
                cout << "Error in recv () : " << strerror(errno) << endl;
              }
            }
          }
        }
      }
    }
    else
    {
      error("ERROR on binding");
    }
  }
  else
  {
    fprintf(stderr, "usage: %s<port>\n", argv[0]);
    exit(1);
  }
  return 0;
}
