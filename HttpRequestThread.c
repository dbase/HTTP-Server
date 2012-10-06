#include <sys/socket.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

void sendHttpHeader(int socket, char response[]);
int daytime(char buf[]);
char*substring(char *string, int position, int length);
int  contains(char str1[], char str2[]){
  char* ptr;
  ptr=strstr(str1, str2);
  if(ptr==NULL){
    return 0;
  }
  return 1;
}

 void HttpRequestThread(void * charSocket){
   printf("Thread Created\n");
   int socket = (int)charSocket;
   // Set up the reading from the socket, and then write a success to the socket.
   char buffer[256]; // Buffer is where the read is stored.

   int status=read(socket, buffer, 255); 
   if(status<0) error("There was an error reading the input");
 
   // Process the HTTP Request the user sent to the server.
   printf("The user sent the message: %s \n", buffer);
   if(contains(buffer, "HTTP/1.1")==0 && contains(buffer, "HTTP/1.0")==0){
     printf("The message received was not an HTTP message %s", " ");
     close(socket);
     return;
   }

    if(contains(buffer,"GET")==1){
      // Gets the filename from the sent request.
      char *fileName=(char*) malloc(sizeof(buffer)-13);
      strncpy(fileName, buffer+13, sizeof(buffer)-13);
      fileName=substring(fileName, 0, strlen(fileName)-2);
      FILE * fp;
      char script[999];
      char directory[256];
      char response[9999];
      getcwd(directory, sizeof(directory));
      strcat(script,"cat ");
      strcat(script, directory);
      strcat(script, "/");
      strcat(script,fileName);
      printf("The script will run: %s. \n", script);
      // The script is now generated, just run it.
      
      fp=popen(script, "r");
      char line[999];
      while(fgets(line, sizeof(line)-1, fp)!=NULL){
        strcat(response,line);
      }
      if(strlen(response)==0) strcat(response,"404 error: File not found");
      printf("The response was: %s \n", response);
      sendHttpHeader(socket, response);

    } else if(contains(buffer,"PUT")==1){
      // PUT request in the following Format
      // PUT HTTP/1.1 [filename] <filedata>data</filedata>

      // Gets the filename from the sent request.
      char *temp = strstr(buffer, "<filedata>");
      int filenameOffset = temp - buffer;

      //File name is set
      char *fileName=(char*) malloc(filenameOffset-1);
      strncpy(fileName, buffer+13, filenameOffset - 14 );

      printf("The file name is: %s\n", fileName);

      //Get the file Data
      char *fileData =(char*) malloc( sizeof(buffer)- filenameOffset);
      strncpy(fileData, buffer + filenameOffset, sizeof(buffer) - filenameOffset\
	      - 1);
      printf("The file data is: %s\n", fileData);

      FILE * fileOutput;
      fileOutput = fopen (fileName, "w");
      if (fileOutput != NULL){
        fputs (fileData, fileOutput );
        fclose (fileOutput);
      }
      else {
        printf("Error Writing file\n");
      }
      status=write(socket, "PUT request processed \n", 22);
    } else if(contains(buffer,"DELETE")==1){

      // Gets the filename from the sent request.
      char *fileName=(char*) malloc(sizeof(buffer)-16);
      strncpy(fileName, buffer+16, sizeof(buffer)-16);
      fileName=substring(fileName, 0, strlen(fileName)-2);
      FILE * fp;
      char script[999];
      char directory[256];
      char response[9999];
      getcwd(directory, sizeof(directory));
      strcat(script,"rm ");
      strcat(script, directory);
      strcat(script, "/");
      strcat(script,fileName);
      printf("The script will run: %s. \n", script);
      // The script is now generated, just run it.

      fp=popen(script, "r");
      char line[999];
      while(fgets(line, sizeof(line)-1, fp)!=NULL){
	strcat(response,line);
      }
      status=write(socket, response, strlen(response));
    } else if(contains(buffer,"HEAD")==1){
      sendHttpHeader(socket, "");
    }
    
    close(socket);
  }

char *substring(char *string, int position, int length) 
{
  char *pointer;
  int c;
  pointer = malloc(length+1);
  for (c = 0; c<position-1; c++) 
    string++; 
 
  for (c = 0; c<length; c++)
    {
      *(pointer+c) = *string;      
      string++;   
    }
  *(pointer+c) = '\0';
  return pointer;
}

// Is given the response to give, and the socket to send on.
// Creates a header and concatonates response, then sends the header+response.
// Should really be renamed sendWithHeader since it now also sends response.
void sendHttpHeader(int socket, char response[]){
  char headerResponse[9999];
  char date[256];
  daytime(date);
  strcat(headerResponse, "Content-Type: text/html; ");
  strcat(headerResponse, "charset=UTF-8 \n");
  strcat(headerResponse, "Server: Jeff And Charles CS436 Project \n");
  strcat(headerResponse, "Date: ");
  strcat(headerResponse, date);
  strcat(headerResponse, "Content-Length: ");
  int length=strlen(response)+strlen(headerResponse)+3;
  char cLength[256];
  sprintf(cLength, "%i",length);
  strcat(headerResponse, cLength);
  strcat(headerResponse, "\n");
  strcat(headerResponse, response);
  strcat(headerResponse, "\n");
  write(socket, headerResponse, strlen(headerResponse));
}

int daytime(char buf[])
    {
      char    *ctime();
      time_t  now;

      (void) time(&now);
      sprintf(buf, "%s", ctime(&now));
}
