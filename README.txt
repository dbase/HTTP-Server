This is the program for Charles Ringger and Jeffrey Rackauckus


To Run - Execute the shell script

sh compile.sh



then run 
./server.exe [portnumb]


for example 
./server.exe 49984 


Supports telnet commands 
GET HTTP/1.0 index.html
//For put command put the file data inside filedata tags
PUT HTTP/1.0 filename <filedata></filedata>
DELETE HTTP/1.0 filename
HEAD HTTP/1.0

Server is designed as a recycling threading application pool. 
