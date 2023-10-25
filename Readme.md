# Lightweight HTTP Server

The server communicates via HTTP and provides various system information. It listens on the specified port and returns the required information according to the user URL. Communication with the server is possible using both a web browser and tools such as `wget` and `curl`.

The server was made as the first project of the [Computer Communications and Networks](https://www.fit.vut.cz/study/course/244865/.en) course at [VUT FIT](https://www.fit.vut.cz/.en) by **Evgenii Shiliaev (xshili00)**.

## Getting Started

### Installation

```bash
make
```

### Usage

```bash
./hinfosvc <port-number>
```

### Communication Example

![communication_example](https://github.com/Jekwwer/IPK-Project01-2022/blob/main/communication-example.gif?raw=true)
<!--
![communication_example](https://i.ibb.co/Xycp0SD/usage-example.gif)
-->

## Detailed Description

### Supported Requests

The server can respond to the following requests:
| Request  | Description                                              | Example                                |
| :------: | :------------------------------------------------------- | :------------------------------------- |
| cpu-name | Returns server CPU information                           | `curl http://servername:1234/cpu-name` |
| hostname | Returns the network name of the computer with the domain | `GET http://servername:1234/hostname`  |
|   load   | Returns current CPU load information                     | `wget http://servername:1234/load`     |

### Server Responses

| Description                                                                                                  | Response Example                         |
| :----------------------------------------------------------------------------------------------------------- | :--------------------------------------- |
| The server has received one of the supported requests, for example,<br>`curl http://localhost:1234/cpu-name` | `AMD Ryzen 5 4500U with Radeon Graphics` |
| The server has received an unknown request, for example,<br>`GET http://localhost:1234/hostname/cpu-name`    | `404 Not Found`                          |
| Something went wrong during the request processing                                                           | `500 Internal Server Error`              |

### Error Codes

| Error Number | Error Name     | Description                                                                                                                                                                                                           | Stderr Output Example                                                                                                                                  |
| :----------: | :------------- | :-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------- |
|      1       | ARG_ERROR      | Trying to run a server with an invalid number or format of the arguments                                                                                                                                              | `ERROR: Bad argument!`<br>`Usage: /path/hinfosvc <port-number>`                                                                                        |
|      2       | SOCKET_ERROR   | Welcoming socket creation has failed                                                                                                                                                                                  | `ERROR: socket() has failed!`                                                                                                                          |
|      3       | BIND_ERROR     | Assigning a name to the socket has failed                                                                                                                                                                             | `ERROR: bind() has failed!`<br>`Port can be already occupied.`                                                                                         |
|      4       | LISTEN_ERROR   | Marking the socket to accept incoming connection requests has failed                                                                                                                                                  | `ERROR: listen() has failed!`                                                                                                                          |
|      8       | INTERNAL_ERROR | This error may appear due to, for example, memory allocation, file opening, or file closing errors.<br>HTTP Request message prints together with the detailed error description.<br>The server does not stop running. | `ERROR: malloc() has failed!`<br>`INTERNAL ERROR!`<br>`GET /cpu-name HTTP/1.1`<br>`Host: localhost:8080`<br>`User-Agent: curl/7.68.0`<br>`Accept: */*` |

## Project Evaluation
Total points: **20/20**
