# Communication Protocol Documentation

#### *The communication protocol between the client and server in this project is based on TCP/IP and uses the WinSock2 library for socket programming. The protocol is designed to handle file transfer operations such as GET, LIST, PUT, DELETE, and INFO. Here's a detailed description of the protocol:*

## 1 - Connection Establishment
The server listens for incoming connections on a specific port. When a client connects, the server accepts the connection, increments the client number, and creates a new thread to handle the client.
## 2 - Command Processing
The server waits for the client to send a command. The command is an integer value that corresponds to one of the enumerated commands (GET, LIST, PUT, DELETE, INFO, EXIT). The command is received using the recv function.
## 3 - Data transfer
Data is transferred between the client and server in chunks. The size of each chunk is specified when calling the sendChunkedData or receiveChunkedData methods. Before sending the actual data, the total size of the data is sent first. Then, for each chunk, the size of the chunk is sent before the chunk itself. This allows the receiver to know how much data to expect and to correctly assemble the chunks.
## 4 - File Transfer
For file transfer operations (GET, PUT), the filename is sent first, followed by the file data. The filename and file data are both sent as chunked data. If the file does not exist, a "File not found" message is sent instead of the file data.
## 5 - Directory Listing
For the LIST command, the server sends a list of filenames in the specified directory. Each filename is followed by a newline character. If the directory does not exist, a "Directory not found" message is sent.
## 6 - File Deletion
For the DELETE command, the server attempts to delete the specified file. If the file does not exist, a "File not found" message is sent. If the file is successfully deleted, a "File deleted successfully" message is sent.
## 7 - File Information
For the INFO command, the server sends information about the specified file, including the filename, size, and last modified time. If the file does not exist, a "Not found" message is sent.
## 8 - Connection Termination
The client can terminate the connection by sending the EXIT command. The server then closes the client's socket and removes the client's connection directory.
