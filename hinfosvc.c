#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

/** Constants */
#define BUFFER_SIZE 100
#define PROC_STAT_COLUMNS_NUM 10

/** Functions */
double calculateProcessorUsage();
void cleanBuffer(char[], int);
char *getStringFromProcStat();
void getValuesfromString(unsigned long long[], int, char *, int);

// /proc/stat values (columns)
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


int main() {
    printf("%f\n", calculateProcessorUsage());
    return 0;
}

/**
 * @brief Calculating the current CPU usage
 * https://stacdataStringCounteroverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
 * 
 * @return percent of CPU usage
 */
double calculateProcessorUsage() {

    char *prevData = getStringFromProcStat();
    if (prevData == NULL) {
        return -1.0;
    }

    sleep(1);

    char *currData = getStringFromProcStat();
    if (currData == NULL) {
        return -1.0;
    }

    unsigned long long int prevValues[PROC_STAT_COLUMNS_NUM];
    unsigned long long int currValues[PROC_STAT_COLUMNS_NUM];
    getValuesfromString(prevValues, PROC_STAT_COLUMNS_NUM, prevData, BUFFER_SIZE);
    getValuesfromString(currValues, PROC_STAT_COLUMNS_NUM, currData, BUFFER_SIZE);

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
 * @brief Buffer Cleaning
 * 
 * @return nothing
 */
void cleanBuffer(char array[], int length) {
    for (int i = 0; i < length; i++) {
        array[i] = '\0';
    }
}

/**
 * @brief Getting first row from /proc/stat
 * 
 * @return string (char*)
 */
char *getStringFromProcStat() {
    char *firstLine = malloc(BUFFER_SIZE * sizeof(char));

    FILE *proc_stat = fopen("/proc/stat", "r");
    if (proc_stat == NULL) {
        fprintf(stderr, "/proc/stat Open Error\n");
        return NULL;
    }

    fgets(firstLine, BUFFER_SIZE, proc_stat);

    if (fclose(proc_stat) == EOF) {
        fprintf(stderr, "/proc/stat Close Error\n");
        return NULL;
    }

    return firstLine;
}

/**
 * @brief Getting int values from string to array
 * 
 * @return nothing
 */
void getValuesfromString(unsigned long long array[], int arrayLength, char *string, int stringLength) {
    char buffer[BUFFER_SIZE] = "";
    int arrayCounter = 0;
    int bufferCharCounter = 0;
    int stringCharCounter = 0;
    char c;
    int tmp;

    while (stringCharCounter < stringLength) {
        c = string[stringCharCounter];
        if (c == ' ' || c == '\0') {
            tmp = atoi(buffer);
            /** if there is a number int the buffer (atoi returns 0 if converting wasn't successful )*/
            if (tmp != 0 || (tmp == 0 && buffer[0] == '0')) {
                array[arrayCounter] = tmp;
                arrayCounter++;
                if (arrayCounter == arrayLength) {
                    return;
                }
            }
            cleanBuffer(buffer, BUFFER_SIZE);
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