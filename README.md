##Communication Protocol Documentation

#1 - Connection
The client establishes a TCP connection to the server using port 12345.
#2 - Commands
Once the connection is established, the client can send the following commands to the server:
GET (1) - retrieve a file from the server
LIST (2) - get a list of files in a specific folder
PUT (3) - send a file to the server
DELETE (4) - delete a file
INFO (5) - request information about the file
EXIT (6) - close the connection
#3 - Data transfer
Data is exchanged using the sendChunkedData and receiveChunkedData functions. 
They operate on the data piece by piece, assembling it into one whole, which helps to optimize the use of network and server/client resources. 
The default chunk size is 1024 bytes. For short strings, a size of 10 bytes is used.
