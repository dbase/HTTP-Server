/************************************ 
CS436
HW1 Programing Assignment
Simple HTML Server
NAME: Charles Ringger  and Jeff Rackauckas
*/

#include <sys/socket.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <strings.h>
#include <signal.h>

#define PORT_NUMBER 31429
#define BUFFER_SIZE 1024

//Function tha handles the commands passed from the client.
void *HttpRequestThread(void * connectionFD);

main(int numargs, char  *args[]){
  
  unsigned short portNumber;
  
  //Determine the port number - If not provided, uses default
  if (numargs != 2){
	portNumber = PORT_NUMBER;
  } else if ( numargs == 2){
	portNumber = (unsigned short)atoi(args[1]);
  }

  printf("Port Number is: %d\nServer is running...\n", portNumber);
  // Socket file descriptor
  int socket_fd;
  
  //Create the socket
  struct sockaddr_in socket_address;
  
  char sendBuff[BUFFER_SIZE];

  socket_fd = socket(AF_INET, SOCK_STREAM,0);

  bzero(&socket_address, sizeof(socket_address));
  socket_address.sin_family = AF_INET;
  socket_address.sin_port = htons(portNumber);
  socket_address.sin_addr.s_addr = INADDR_ANY;

  //Bind the socket to the descriptor
  bind(socket_fd, (struct sockaddr*)&socket_address, sizeof(socket_address));  

  listen(socket_fd, 5);
  printf("Listen Socket Created\n");

  int i = 0;
  //Creates an array of threads (Thread pool)
  pthread_t thread_id[10];
  //Used to keep track of which threads have been used.
  int inUse[10] = {0,0,0,0,0,0,0,0,0,0};

  while(i > -1 && i < 10 ){
       
    int client_fd;
    struct sockaddr_in client_address;
    int addresslength = sizeof(client_address);

    printf("Waiting to accept connection (%d)\n",i);
    //take the connection
    client_fd = accept(socket_fd, (struct sockaddr*)&client_address, &addresslength);
    printf("Client Connection (%d) accepted\n", i);

    //convert the client_file_descriptor to a char array
    char *msg = NULL;
   
    //thread the connection
    pthread_create(&thread_id[i], NULL, &HttpRequestThread, (void *) client_fd );
    printf("Thread (%d) Created\n",i);
    inUse[i] = 1;
    i++;

    int iIsSet = 0;
    int j;
    //TODO: CLEAN UP THE THREAD POOL
    for( j = 0 ; j < 10 ; j++ ) {
      
      if (inUse[j] == 1){
	printf("Checking the status of thread (%d)\n",j);
	if(pthread_kill(thread_id[j], 0) == 0 ){
	    printf("Thread (%d) is alive\n", j);
	    //TODO: Count number of alive threads.  
	        //determine global stay alive time
	  }	
	  else{
	    //Kills all inactive threads.
	    int k = pthread_kill(thread_id[j], 1);
	    printf("Thread (%d) has been terminated\n",j);
	    if (iIsSet == 0){
	      //Set i to the lowest thread in the pool
	      i = j;
	      iIsSet = 1;
	    }
	  }
      }
	  if (j == 9 && i > 9){
	      //If here, thread pool is full.  Wait for 10 seconds, 
	      //then clean the pool again
	      sleep(10);
	      j = 0;
	  }
	}
     }
  
  //Close the master socket
  //Should never get here
  close(socket_fd);
  
}

