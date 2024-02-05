# Communication Protocol Documentation

#### *The communication protocol between the client and server in this project is based on TCP/IP and uses the WinSock2 library for socket programming. The protocol is designed to handle file transfer operations such as GET, LIST, PUT, DELETE, and INFO. Here's a detailed description of the protocol:*

## 1 - Connection Establishment
The server listens for incoming connections on a specific port. When a client connects, the server accepts the connection, increments the client number, and creates a new thread to handle the client.
## 2 - Command Processing
The server waits for the client to send a command. The command is an integer value that corresponds to one of the enumerated commands (GET, LIST, PUT, DELETE, INFO, EXIT). The command is received using the recv function.
## 3 - Data transfer
Data is transferred between the client and server in chunks. The size of each chunk is specified when calling the sendChunkedData or receiveChunkedData methods.

The process of sending data is as follows:  
1. The total size of the data to be sent is determined and sent first. This is an integer value and takes up 4 bytes (the size of an int in C++).  
2. For each chunk of data:
* The size of the chunk is determined and sent. This is also an integer value and takes up 4 bytes.
* The chunk of data itself is sent. The size of this data is variable and depends on the chunk size specified when calling the sendChunkedData method.

The process of receiving data is as follows:
1. The total size of the data to be received is read first. This is an integer value and takes up 4 bytes.
2. For each chunk of data:
* The size of the chunk is read. This is also an integer value and takes up 4 bytes.
* The chunk of data itself is read. The size of this data is variable and depends on the chunk size that was sent.

This method of sending the size before the data allows the receiver to know how much data to expect and to correctly assemble the chunks.
## 4 - File Transfer
For file transfer operations (GET, PUT), the filename is sent first, followed by the file data. Both the filename and file data are sent as chunked data, following the process described above. The filename is a string and its size in bytes depends on the length of the string. The file data is also a string (read from the file as binary data) and its size in bytes depends on the size of the file. If the file does not exist, a "File not found" message is sent instead of the file data. This message is a string and takes up 13 bytes (12 characters plus a null terminator).
## 5 - Directory Listing
For the LIST command, the server sends a list of filenames in the specified directory. Each filename is followed by a newline character. The size of this data in bytes depends on the total length of all filenames plus the number of newline characters.  If the directory does not exist, a "Directory not found" message is sent. This message is a string and takes up 18 bytes (17 characters plus a null terminator).
## 6 - File Deletion
For the DELETE command, the server attempts to delete the specified file. If the file does not exist, a "File not found" message is sent. This message is a string and takes up 13 bytes (12 characters plus a null terminator).  If the file is successfully deleted, a "File deleted successfully" message is sent. This message is a string and takes up 22 bytes (21 characters plus a null terminator)
## 7 - File Information
For the INFO command, the server sends information about the specified file, including the filename, size, and last modified time. The size of this data in bytes depends on the length of the information string.  If the file does not exist, a "Not found" message is sent. This message is a string and takes up 10 bytes (9 characters plus a null terminator).
## 8 - Connection Termination
The client can terminate the connection by sending the EXIT command. The server then closes the client's socket and removes the client's connection directory. The EXIT command is an integer value and takes up 4 bytes.
