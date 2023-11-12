# **IPK-Project01-2022**

### 🖥️ Lightweight HTTP Server

> 🎓 **University**: [VUT FIT](https://www.fit.vut.cz/)
>
> 📚 **Course**: [Computer Communications and Networks (IPK)](https://www.fit.vut.cz/study/course/244865/.en)
>
> 📅 **Academic Year**: 2021/22

This HTTP server facilitates communication via the HTTP protocol and delivers an array of system information. It listens on a user-specified port and returns system details based on the endpoint requested by the user. The server is accessible using standard web browsers and command-line tools such as `wget` and `curl`.

## 📌 Table of Contents

- [🚀 Getting Started](#-getting-started)
  - [📋 Prerequisites](#-prerequisites)
  - [🔧 Installation](#-installation)
  - [🖥️ Usage](#-usage)
- [🤝 Communication Example](#-communication-example)
- [📝 Detailed Description](#-detailed-description)
  - [📧 Supported Requests](#-supported-requests)
  - [📬 Server Responses](#-server-responses)
  - [🚨 Error Codes](#-error-codes)
- [✅ Project Evaluation](#-project-evaluation)

## 🚀 Getting Started

These instructions will get you a copy of the project up and running on your local machine.

### 📋 Prerequisites

Before installing, ensure you have the following software installed:

- GCC compiler
- GNU Make

On a Debian-based system, you can install these with the following command:

```bash
sudo apt-get install build-essential
```

### 🔧 Installation

Clone the repository and compile the server with:

```bash
git clone https://github.com/Jekwwer/IPK-Project01-2022
cd repository
make
```

### 🖥️ Usage

To start the server, run:

```bash
./hinfosvc <port-number>
```

Replace `<port-number>` with the port number where you want the server to listen for incoming requests.

### 🤝 Communication Example

To demonstrate the server's responses, the following GIF showcases interactions with the server:

![communication_example](https://github.com/Jekwwer/IPK-Project01-2022/blob/main/communication-example.gif?raw=true)

If you're having trouble viewing the image above, try accessing it [here](https://i.ibb.co/Xycp0SD/usage-example.gif).

## 📝 Detailed Description

### 📧 Supported Requests

The server recognizes these requests:

| Request    | Description                                        | Example                                |
| ---------- | -------------------------------------------------- | -------------------------------------- |
| `cpu-name` | Retrieves information about the server's CPU.      | `curl http://servername:1234/cpu-name` |
| `hostname` | Obtains the network name and domain of the server. | `GET http://servername:1234/hostname`  |
| `load`     | Provides the current CPU load.                     | `wget http://servername:1234/load`     |

### 📬 Server Responses

Here is what you can expect from the server after a request:

| Scenario                 | Response Example                         |
| ------------------------ | ---------------------------------------- |
| Valid request received   | `AMD Ryzen 5 4500U with Radeon Graphics` |
| Unknown request          | `404 Not Found`                          |
| Request processing error | `500 Internal Server Error`              |

### 🚨 Error Codes

Below are possible error codes and their explanations:

| Error Number | Error Name       | Description                                                                 | Stderr Output Example                                                                                |
| ------------ | ---------------- | --------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------- |
| 1            | `ARG_ERROR`      | Incorrect number or format of arguments provided.                           | `ERROR: Bad argument!`<br>`Usage: ./hinfosvc <port-number>`                                            |
| 2            | `SOCKET_ERROR`   | Failure in socket creation.                                                 | `ERROR: socket() failed!`                                                                            |
| 3            | `BIND_ERROR`     | Failure in assigning a name to the socket.                                  | `ERROR: bind() failed!`<br>`Port may be occupied.`                                                     |
| 4            | `LISTEN_ERROR`   | Failure in setting up the socket to accept connections.                     | `ERROR: listen() failed!`                                                                            |
| 8            | `INTERNAL_ERROR` | General errors such as memory allocation failure. Server continues running. | `ERROR: malloc() failed!`<br>`INTERNAL ERROR!`<br>`GET /cpu-name HTTP/1.1`<br>`Host: localhost:8080`<br>`User-Agent: curl/7.68.0`<br>`Accept: */*` |

## ✅ Project Evaluation

🟢🟢🟢🟢🟢  
**Total Points: 20/20**\
The project successfully fulfilled the stringent requirements and performance benchmarks established by the [Computer Communications and Networks](https://www.fit.vut.cz/study/course/244865) course.

---

**🔙 Back to [Table of Contents](#-table-of-contents)**
