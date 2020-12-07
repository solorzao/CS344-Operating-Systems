#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// define size for buffers
#define SIZE 10000

// declare function prototypes
void* inputThreadFunc();
void* lineSeparatorThreadFunc();
void* signThreadFunc();
void* outputThreadFunc();

// create global buffers for use among threads
char *inputBuffer;
char outputBuffer[SIZE];
char buffer1[SIZE];
char buffer2[SIZE];
char buffer3[SIZE];



// initialize the mutex
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;

// Initialize the condition variables for all buffers, don't need empty because theoretically unbounded
pthread_cond_t buffer1Full = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer1Empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer2Full = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer2Empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer3Full = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer3Empty = PTHREAD_COND_INITIALIZER;

int main(int argc, const char* argv[])
{
    // intialize buffers to 0
    memset(buffer1, '\0', sizeof(buffer1));
    memset(buffer2, '\0', sizeof(buffer2));
    memset(buffer3, '\0', sizeof(buffer3));
    
    // declare threads for each process
    pthread_t inputThread;
    pthread_t lineSepThread;
    pthread_t signThread;
    pthread_t outputThread;

    // start threads for each process
    pthread_create(&inputThread, NULL, inputThreadFunc, NULL);
    pthread_create(&lineSepThread, NULL, lineSeparatorThreadFunc, NULL);
    pthread_create(&signThread, NULL, signThreadFunc, NULL);
    pthread_create(&outputThread, NULL, outputThreadFunc, NULL);

    // wait for each thread to finish
    pthread_join(inputThread, NULL);
    pthread_join(lineSepThread, NULL);
    pthread_join(signThread, NULL);
    pthread_join(outputThread, NULL);

    // once all threads are done, exit
    return 0;
}

void* inputThreadFunc() {

    // create input buffer and allocate memory
    size_t buffSize = 10000;
    inputBuffer = (char*)malloc(buffSize + 1);

    // keep reading input until "DONE\n" is found
    while (strcmp(inputBuffer, "DONE\n") != 0) {

        // clear input buffer to read next line
        memset(inputBuffer, '\0', sizeof(inputBuffer));
        getline(&inputBuffer, &buffSize, stdin);

        // lock mutex before performing work
        pthread_mutex_lock(&mutex1);

        while (strlen(buffer1) > 1) {
            pthread_cond_wait(&buffer1Empty, &mutex1);
        }

        // perform work, transfer input to buffer, signal that it has data
        strcat(buffer1, inputBuffer);
        pthread_cond_signal(&buffer1Full);

        // unlock mutex
        pthread_mutex_unlock(&mutex1);        
        

    }

    // free allocated memory
    free(inputBuffer);
    return NULL;
}

void* lineSeparatorThreadFunc() {

    int i;

    // keep looping until terminator is found
    while (strcmp(buffer2, "DONE ") != 0) {

        // lock mutex, check if unprocessed data exists in buffer
        pthread_mutex_lock(&mutex1);

        while (strlen(buffer1) < 1) {
            pthread_cond_wait(&buffer1Full, &mutex1);
        }

        // loop through char array for replacement of target char
        for (i = 0; i < strlen(buffer1); i++) {
            if (buffer1[i] == '\n') {
                buffer1[i] = ' ';
            }
        }


        // lock mutex to write data to next buffer
        pthread_mutex_lock(&mutex2);

        while (strlen(buffer2) > 1) {
            pthread_cond_wait(&buffer2Empty, &mutex2);
        }

        //perform work
        strcat(buffer2, buffer1);
        // reset buffer 1
        memset(buffer1, '\0', sizeof(buffer1));
        pthread_cond_signal(&buffer1Empty);

        // unlock mutex1
        pthread_mutex_unlock(&mutex1);

        // signal that buffer has data
        pthread_cond_signal(&buffer2Full);

        // unlock mutex2
        pthread_mutex_unlock(&mutex2);
    }

    return NULL;
}

void* signThreadFunc() {

    int i, j;

    while (strcmp(buffer3, "DONE ") != 0) {

        // lock mutex, check if unprocessed data exists in buffer
        pthread_mutex_lock(&mutex2);

        // wait until buffer has data
        while (strlen(buffer2) < 1) {
            pthread_cond_wait(&buffer2Full, &mutex2);
        }
       
        // perform char replacement
        for (i = 0; i < strlen(buffer2); i++) {
            if ((buffer2[i] == '+') && (buffer2[i + 1] == '+')) {

                buffer2[i] = '^';

                for (j = i + 1; j < strlen(buffer2); j++) {
                    buffer2[j] = buffer2[j + 1];
                }

            }
        }

        // lock mutex to write data to next buffer
        pthread_mutex_lock(&mutex3);

        while (strlen(buffer3) > 1) {
            pthread_cond_wait(&buffer3Empty, &mutex3);
        }

        // perform work
        strcat(buffer3, buffer2);

        // reset buffer 2
        memset(buffer2, '\0', sizeof(buffer2));
        pthread_cond_signal(&buffer2Empty);

        // unlock mutex2
        pthread_mutex_unlock(&mutex2);
      
        // signal that buffer has data
        pthread_cond_signal(&buffer3Full);
        
        // unlock mutex3
        pthread_mutex_unlock(&mutex3);
    }

    return NULL;
}

void* outputThreadFunc() {

    // declare and initialize counters
    int i, charCount = 0, outputIndex = 0;

    // clear buffer for use
    memset(outputBuffer, '\0', sizeof(outputBuffer));

    while (1) {

        // lock mutex, check if unprocessed data exists in buffer
        pthread_mutex_lock(&mutex3);

        while (strlen(buffer3) < 1) {
            // wait until buffer has data
            pthread_cond_wait(&buffer3Full, &mutex3);
        }
        
        strcat(outputBuffer, buffer3);

        // check for terminator, break while loop if found
        if (strcmp(buffer3, "DONE ") == 0) {
            break;
        }

        charCount += strlen(buffer3);

        while (charCount >= 80) {
            // perform work, printing 80 char lines at a time
            for (i = 0; i < 80; i++) {
                printf("%c", outputBuffer[outputIndex]);
                outputIndex++;
            }
            printf("\n");
            charCount -= 80;
        }

        // reset buffer 3
        memset(buffer3, '\0', sizeof(buffer3));
        pthread_cond_signal(&buffer3Empty);

        // unlock mutex
        pthread_mutex_unlock(&mutex3);
    }

    return NULL;
}