# Communication Protocol Documentation

## 1 - Connection
The client establishes a TCP connection to the server using port 12345.
## 2 - Commands
Once the connection is established, the client can send the following commands to the server: 
* GET - retrieve a file from the server; 
* LIST - get a list of files in a specific folder;
* PUT - send a file to the server;
* DELETE - delete a file;
* INFO - request information about the file;
* EXIT - close the connection;
## 3 - Data transfer
Data is exchanged using the sendChunkedData and receiveChunkedData functions. They operate on the data piece by piece, assembling it into one whole, which helps to optimize the use of network and server/client resources. The default chunk size is 1024 bytes. For short strings, a size of 10 bytes is used.
