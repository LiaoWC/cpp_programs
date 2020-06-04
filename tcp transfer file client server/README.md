# Simple client and server for transfer a file via TCP socket
The c.cpp is the client code, and the s.cpp is the server code
It can run on my ubuntu host but I haven't test if the client program and the server program runs successfully on differenct host.

### Server Usage
`./s <port number>`
<p>After you got the file, you have to type its name. (After it tranfered, it'll be stored in default name.)


### Client Usage
`./c  <sever host>  <server port>`
<p>After you connect to the server, you have to type the file name you want to transfer to the server.<p>
