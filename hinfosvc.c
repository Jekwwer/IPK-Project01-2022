/**
 * @file hinfosvc.c
 * @brief Implementation of a lightweight HTTP server for IPK Project 1.
 *
 * This server handles simple HTTP GET requests to provide system information such as
 * CPU name, hostname, and current CPU load.
 *
 * @see https://github.com/Jekwwer/IPK-Project01-2022
 * @author Evgenii Shiliaev
 */

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


/** Constants */
#define SMALL_BUFFER_SIZE 100
#define MSG_BUFFER_SIZE 1024
#define PROC_STAT_COLUMNS_NUM 10
#define ERRORS 12 // number of errors' codes
#define PROC_STAT "/proc/stat"
#define PROC_CPUINFO "/proc/cpuinfo"
#define CPU_NAME_REQUEST "GET /cpu-name "
#define HOSTNAME_REQUEST "GET /hostname "
#define LOAD_REQUEST "GET /load "

/** Functions */
double calculateProcessorUsage();
void cleanBuffer(char[], int);
char *getCPUname();
char *getStringFromProcStat();
void getValuesfromString(unsigned long long[], int, char *, int);
void printError(int, const char *);

/** /proc/stat values (columns) */
enum CPUdata { USER,
               NICE,
               SYSTEM,
               IDLE,
               IOWAIT,
               IRQ,
               SOFTIRQ,
               STEAL,
               GUEST,
               GUEST_NICE };

/** errors' codes */
enum errors {
    NO_ERROR,
    ARG_ERROR,
    SOCKET_ERROR,
    BIND_ERROR,
    LISTEN_ERROR,
    ACCEPT_ERROR,
    FILE_OPEN_ERROR,
    FILE_CLOSE_ERROR,
    FILE_ERROR,
    MALLOC_ERROR,
    GETHOSTNAME_ERROR,
    INTERNAL_ERROR
};

/**
 * @brief Entry point for the Lightweight HTTP Server.
 *
 * This server accepts a single command-line argument for the port number on which to listen.
 * It then enters a loop where it waits for incoming connections, processes HTTP requests,
 * and sends appropriate responses.
 *
 * @param argc The count of command-line arguments.
 * @param argv The command-line arguments array. argv[1] is expected to be the port number.
 *
 * @return int Returns an error code if an error occurs, otherwise it will loop indefinitely.
 */
int main(int argc, const char *argv[]) {
    int rc;
    int welcomingSocket;
    struct sockaddr_in socketAdr;
    struct sockaddr_in socketAdrClient;
    int portNum;

    /** port getting */
    if (argc != 2) {
        printError(ARG_ERROR, argv[0]);
        return ARG_ERROR;
    }
    portNum = atoi(argv[1]);

    /** welcoming socket creating */
    socklen_t socketAdrClientLen = sizeof(socketAdrClient);
    if ((welcomingSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printError(SOCKET_ERROR, "");
        return SOCKET_ERROR;
    }

    memset(&socketAdr, 0, sizeof(socketAdr));
    socketAdr.sin_family = AF_INET;
    socketAdr.sin_addr.s_addr = INADDR_ANY;
    socketAdr.sin_port = htons(portNum);

    if ((rc = bind(welcomingSocket, (struct sockaddr *) &socketAdr, sizeof(socketAdr))) < 0) {
        printError(BIND_ERROR, "");
        return BIND_ERROR;
    }

    if ((listen(welcomingSocket, 1)) < 0) {
        printError(LISTEN_ERROR, "");
        return LISTEN_ERROR;
    }

    while (1) {
        int communicationSocket = accept(welcomingSocket, (struct sockaddr *) &socketAdrClient, &socketAdrClientLen);
        if (communicationSocket > 0) {

            int internalServerErrorFlag = 0;
            char buffer[MSG_BUFFER_SIZE] = "";
            char response[MSG_BUFFER_SIZE] = "";
            int request = 0;
            while (internalServerErrorFlag == 0) {
                request = recv(communicationSocket, buffer, 1024, 0);
                if (request <= 0) {
                    break;
                }

                if (strncmp(buffer, CPU_NAME_REQUEST, strlen(CPU_NAME_REQUEST)) == 0) {
                    char *cpuName = getCPUname();
                    if (cpuName == NULL) {
                        internalServerErrorFlag = INTERNAL_ERROR;
                    }
                    else {
                        sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%s", cpuName);
                        free(cpuName);
                    }
                }
                else if (strncmp(buffer, HOSTNAME_REQUEST, strlen(HOSTNAME_REQUEST)) == 0) {
                    char hostname[SMALL_BUFFER_SIZE];
                    if (gethostname(hostname, SMALL_BUFFER_SIZE) != 0) {
                        internalServerErrorFlag = INTERNAL_ERROR;
                    }
                    else {
                        struct hostent *host = gethostbyname(hostname);
                        if (host == NULL) {
                            internalServerErrorFlag = INTERNAL_ERROR;
                        }
                        else {
                            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%s\n", host->h_name);
                        }
                    }
                }
                else if (strncmp(buffer, LOAD_REQUEST, strlen(LOAD_REQUEST)) == 0) {
                    double cpuUsage = calculateProcessorUsage();
                    if (cpuUsage < 0) {
                        internalServerErrorFlag = INTERNAL_ERROR;
                    }
                    else {
                        sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%.2f%%\n", cpuUsage);
                    }
                }
                else {
                    sprintf(response, "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found\n");
                }

                if (internalServerErrorFlag != 0) {
                    printError(internalServerErrorFlag, buffer);
                    sprintf(response, "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\n500 Internal Server Error\n");
                }
                send(communicationSocket, response, strlen(response), 0);
                close(communicationSocket);
            }
        }
        else {
            printError(ACCEPT_ERROR, "");
        }

        close(communicationSocket);
    }
    return NO_ERROR;
}


/**
 * @brief Calculate the processor's usage based on previous and current /proc/stat readings.
 * 
 * Uses a one second sleep interval to gather previous and current CPU times, then calculates
 * the usage percentage based on the difference.
 *
 * https://stacdataStringCounteroverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
 * 
 * @return The CPU usage percentage, or a negative error code on failure.
 */
double calculateProcessorUsage() {

    char *prevData = getStringFromProcStat();
    if (prevData == NULL) {
        return -INTERNAL_ERROR;
    }

    sleep(1);

    char *currData = getStringFromProcStat();
    if (currData == NULL) {
        return -INTERNAL_ERROR;
    }

    unsigned long long int prevValues[PROC_STAT_COLUMNS_NUM];
    unsigned long long int currValues[PROC_STAT_COLUMNS_NUM];
    getValuesfromString(prevValues, PROC_STAT_COLUMNS_NUM, prevData, SMALL_BUFFER_SIZE);
    getValuesfromString(currValues, PROC_STAT_COLUMNS_NUM, currData, SMALL_BUFFER_SIZE);

    unsigned long long int PrevIdle = prevValues[IDLE] + prevValues[IOWAIT];
    unsigned long long int CurrIdle = currValues[IDLE] + currValues[IOWAIT];

    unsigned long long int PrevNonIdle = prevValues[USER] + prevValues[NICE] + prevValues[SYSTEM] + prevValues[IRQ] + prevValues[SOFTIRQ] + prevValues[STEAL];
    unsigned long long int NonIdle = currValues[USER] + currValues[NICE] + currValues[SYSTEM] + currValues[IRQ] + currValues[SOFTIRQ] + currValues[STEAL];

    unsigned long long int PrevTotal = PrevIdle + PrevNonIdle;
    unsigned long long int Total = CurrIdle + NonIdle;

    unsigned long long int totald = Total - PrevTotal;
    unsigned long long int idled = CurrIdle - PrevIdle;

    double CPUpercentage = (totald - idled) / (float) totald * 100;
    free(prevData);
    free(currData);

    return CPUpercentage;
}

/**
 * @brief Clean the given character buffer by setting all its elements to '\0'.
 * 
 * @param array The character array to clean.
 * @param length The length of the array.
 */
void cleanBuffer(char array[], int length) {
    for (int i = 0; i < length; i++) {
        array[i] = '\0';
    }
}

/**
 * @brief Get the first line from /proc/stat which contains the CPU time information.
 * 
 * @return A pointer to a dynamically allocated string with the contents of the first line, or NULL on error.
 */
char *getStringFromProcStat() {
    char *firstLine = malloc(SMALL_BUFFER_SIZE * sizeof(char));
    if (firstLine == NULL) {
        printError(MALLOC_ERROR, "");
        return NULL;
    }

    FILE *proc_stat = fopen(PROC_STAT, "r");
    if (proc_stat == NULL) {
        printError(FILE_OPEN_ERROR, PROC_STAT);
        return NULL;
    }

    fgets(firstLine, SMALL_BUFFER_SIZE, proc_stat);

    if (fclose(proc_stat) == EOF) {
        printError(FILE_CLOSE_ERROR, PROC_STAT);
        return NULL;
    }

    return firstLine;
}

/**
 * @brief Parse a string containing space-separated numbers and fill an array with the numeric values.
 * 
 * @param array The array to be filled with the parsed numbers.
 * @param arrayLength The maximum number of elements to store in the array.
 * @param string The string containing the numbers to parse.
 * @param stringLength The length of the string.
 */
void getValuesfromString(unsigned long long array[], int arrayLength, char *string, int stringLength) {
    char buffer[SMALL_BUFFER_SIZE] = "";
    int arrayCounter = 0;
    int bufferCharCounter = 0;
    int stringCharCounter = 0;
    char c;
    int value;

    while (stringCharCounter < stringLength) {
        c = string[stringCharCounter];
        if (c == ' ' || c == '\0') {
            value = atoi(buffer);
            /** if there is a number int the buffer (atoi returns 0 if converting wasn't successful )*/
            if (value != 0 || (value == 0 && buffer[0] == '0')) {
                array[arrayCounter] = value;
                arrayCounter++;
                if (arrayCounter == arrayLength) {
                    return;
                }
            }
            cleanBuffer(buffer, SMALL_BUFFER_SIZE);
            bufferCharCounter = 0;
        }
        else {
            buffer[bufferCharCounter] = c;
            bufferCharCounter++;
        }
        stringCharCounter++;

        if (c == '\0') {
            break;
        }
    }
}

/**
 * @brief Retrieve the CPU name from /proc/cpuinfo.
 * 
 * Searches /proc/cpuinfo for the model name entry and extracts the CPU name from it.
 *
 * @return A pointer to a dynamically allocated string containing the CPU name, or NULL on error.
 */
char *getCPUname() {
    char *cpuName = malloc(SMALL_BUFFER_SIZE * sizeof(char));
    if (cpuName == NULL) {
        printError(MALLOC_ERROR, "");
        return NULL;
    }

    FILE *proc_cpuinfo = fopen(PROC_CPUINFO, "r");
    if (proc_cpuinfo == NULL) {
        printError(FILE_OPEN_ERROR, PROC_CPUINFO);
        return NULL;
    }

    /** Looking for "model name" row*/
    while (fgets(cpuName, SMALL_BUFFER_SIZE, proc_cpuinfo)) {
        if (strncmp(cpuName, "model name", strlen("model name")) == 0) {
            break;
        }
    }

    /** Looking for separator*/
    int i = 0;
    while (cpuName[i++] != ':') { ; }

    if (fclose(proc_cpuinfo) == EOF) {
        printError(FILE_CLOSE_ERROR, PROC_CPUINFO);
        return NULL;
    }
    strcpy(cpuName, &(cpuName[i + 1]));
    return cpuName;
}

/**
 * @brief Print an error message corresponding to an error code with additional context.
 * 
 * The function uses stderr for printing.
 *
 * @param errNum The error code number which corresponds to a predefined error message.
 * @param arg A context argument that can be inserted into the error message if applicable.
 */
void printError(int errNum, const char *arg) {
    char errors[ERRORS][SMALL_BUFFER_SIZE] =
            {"Success\n",
             "ERROR: Bad argument!\nUsage: %s <port-number>\n",
             "ERROR: socket() has failed!\n",
             "ERROR: bind() has failed!\nPort can be already occupied.\n",
             "ERROR: listen() has failed!\n",
             "ERROR: accept() has failed!\n",
             "ERROR: fopen() of %s has failed!\n",
             "ERROR: fclose() of %s has failed!\n",
             "",
             "ERROR: malloc() has failed!\n",
             "ERROR: gethostname() has failed!\n",
             "INTERNAL ERROR!\n%s\n"};
    fprintf(stderr, errors[errNum], arg);
}

/** End of hinfosvc.c file */
