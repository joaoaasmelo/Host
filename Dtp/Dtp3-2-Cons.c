/*#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "Dtp3.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *oddNumbersConsumer(void *vargp)
{
    int prod_file_descriptor = *(int *)vargp;
    int file_descriptor_odd;
    unsigned int number;
    char buffer[BUFFER_SIZE];
    int read_size;

    // TODO - Open the odd numbers file so that:
    // - only write-only operations are allowed
    // - file is created if not existing
    // - if file exists, overwrite existing data
    // - set permissions to 0644

    file_descriptor_odd = open(ODD_NUMBERS_FILE_NAME, O_WRONLY | O_CREAT, 0644);

    if (file_descriptor_odd == -1)
    {
        fprintf(stderr, "Error: %s: %s\n", ODD_NUMBERS_FILE_NAME, strerror(errno));
        return NULL;
    }

    while (1)
    {
        // TODO - Read one number from the producers file. Recall
        // that numbers are in text format with 9 characters.
        pthread_mutex_lock(&mutex);
        read_size = read(prod_file_descriptor, buffer, BUFFER_SIZE);
        if (read_size > 0)
        {   // Convert text format (string) to integer
            number = atoi(buffer); // Convert text format (string) to integer
            if (number % 2 != 0)
            {
                printf("Storing odd number %u\n", number);
                snprintf(buffer, BUFFER_SIZE, "%09d\n", number);// Prepare data to be written to file (integer to fixed-size string format)
                if(write(file_descriptor_odd, buffer, BUFFER_SIZE)<0){
                    fprintf(stderr, "Error: %s: %s\n", ODD_NUMBERS_FILE_NAME, strerror(errno));
                    pthread_mutex_unlock(&mutex);
                    return NULL;
                }

                // TODO - Store formated odd number into the odd numbers file
            }
        }
        pthread_mutex_unlock(&mutex);
        usleep(250000);
    }

    return NULL;
}

void *evenNumbersConsumer(void *vargp)
{
    int file_descriptor = *(int *)vargp;
    int file_descriptor_even;
    unsigned int number;
    char buffer[BUFFER_SIZE];
    int read_size;

    // TODO - Open the even numbers file so that:
    // - only write-only operations are allowed
    // - file is created if not existing
    // - if file exists, overwrite existing data
    // - set permissions to 0644

    file_descriptor_even = open(EVEN_NUMBERS_FILE_NAME, O_WRONLY | O_CREAT, 0644);
    if (file_descriptor_even == -1)
    {
        fprintf(stderr, "Error: %s: %s\n", EVEN_NUMBERS_FILE_NAME, strerror(errno));
        return NULL;
    }

    while (1)
    {
        // TODO - Read one number from the producers file. Recall
        // that numbers are in text format with 9 characters.
        pthread_mutex_lock(&mutex);
        read_size = read(file_descriptor, buffer, BUFFER_SIZE);
        if (read_size > 0)
        {   // Convert text format (string) to integer
            number = atoi(buffer);
            if (number % 2 == 0)
            {
                printf("Storing even number %u\n", number);
                snprintf(buffer, BUFFER_SIZE, "%09d\n", number);
                if(write(file_descriptor_even, buffer, BUFFER_SIZE)<0){
                    fprintf(stderr, "Error: %s: %s\n", EVEN_NUMBERS_FILE_NAME, strerror(errno));
                    pthread_mutex_unlock(&mutex);
                    return NULL;
                }
                
                // TODO - Store formated even number into the even numbers file
            }
        }
        pthread_mutex_unlock(&mutex);
        usleep(250000);
    }

    return NULL;
}

int main()
{
    pthread_t tid_1, tid_2;

    // TODO - Open the producer file so that:
    // - only read operations are allowed
    // - file is created if not existing
    // - set permissions to 0644
    // Hint: remember that both threads will read from the same
    // file and how file descriptors track the progress within
    // the file....

    int file_descriptor = open(PRODUCER_FILE_NAME, O_RDONLY | O_CREAT, 0644);
    if (file_descriptor == -1)
    {
        fprintf(stderr, "Error: %s: %s\n", PRODUCER_FILE_NAME, strerror(errno));
        return -1;
    }

    pthread_mutex_init(&mutex, NULL);

    pthread_create(&tid_1, NULL, oddNumbersConsumer, &file_descriptor);
    pthread_create(&tid_2, NULL, evenNumbersConsumer, &file_descriptor);

    pthread_join(tid_1, NULL);
    pthread_join(tid_2, NULL);

    pthread_mutex_destroy(&mutex);

    exit(0);
}*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "Dtp3.h"

void *oddNumbersConsumer(void *vargp)
{
    int prod_file_descriptor = *(int *)vargp;
    int file_descriptor_odd;
    unsigned int number;
    char buffer[BUFFER_SIZE];
    int read_size;

    // TODO - Open the odd numbers file so that:
    // - only write-only operations are allowed
    // - file is created if not existing
    // - if file exists, overwrite existing data
    // - set permissions to 0644

    file_descriptor_odd = open(ODD_NUMBERS_FILE_NAME, O_WRONLY | O_CREAT, 0644);

    if (file_descriptor_odd == -1)
    {
        fprintf(stderr, "Error: %s: %s\n", ODD_NUMBERS_FILE_NAME, strerror(errno));
        return NULL;
    }

    while (1)
    {
        // TODO - Read one number from the producers file. Recall
        // that numbers are in text format with 9 characters.
        fcntl(prod_file_descriptor, F_SETLKW, LOCK_EX);
        read_size = read(prod_file_descriptor, buffer, BUFFER_SIZE);
        fcntl(prod_file_descriptor, F_SETLKW, LOCK_UN);
        if (read_size > 0)
        {   // Convert text format (string) to integer
            number = atoi(buffer); // Convert text format (string) to integer
            if (number % 2 != 0)
            {
                printf("Storing odd number %u\n", number);
                snprintf(buffer, BUFFER_SIZE, "%09d\n", number);// Prepare data to be written to file (integer to fixed-size string format)
                if(write(file_descriptor_odd, buffer, BUFFER_SIZE)<0){
                    fprintf(stderr, "Error: %s: %s\n", ODD_NUMBERS_FILE_NAME, strerror(errno));
                    return NULL;
                }

                // TODO - Store formated odd number into the odd numbers file
            }
        }
        
        usleep(250000);
    }

    return NULL;
}

void *evenNumbersConsumer(void *vargp)
{
    int prod_file_descriptor = *(int *)vargp;
    int file_descriptor_even;
    unsigned int number;
    char buffer[BUFFER_SIZE];
    int read_size;

    // TODO - Open the even numbers file so that:
    // - only write-only operations are allowed
    // - file is created if not existing
    // - if file exists, overwrite existing data
    // - set permissions to 0644

    

    file_descriptor_even = open(EVEN_NUMBERS_FILE_NAME, O_WRONLY | O_CREAT, 0644);
    if (file_descriptor_even == -1)
    {
        fprintf(stderr, "Error: %s: %s\n", EVEN_NUMBERS_FILE_NAME, strerror(errno));
        return NULL;
    }

    while (1)
    {
        // TODO - Read one number from the producers file. Recall
        // that numbers are in text format with 9 characters.
        fcntl(prod_file_descriptor, F_SETLKW, LOCK_EX);
        read_size = read(prod_file_descriptor, buffer, BUFFER_SIZE);
        fcntl(prod_file_descriptor, F_SETLKW, LOCK_UN);
        if (read_size > 0)
        {   // Convert text format (string) to integer
            number = atoi(buffer);
            if (number % 2 == 0)
            {
                printf("Storing even number %u\n", number);
                snprintf(buffer, BUFFER_SIZE, "%09d\n", number);
                if(write(file_descriptor_even, buffer, BUFFER_SIZE)<0){
                    fprintf(stderr, "Error: %s: %s\n", EVEN_NUMBERS_FILE_NAME, strerror(errno));
                    
                    return NULL;
                }
                
                // TODO - Store formated even number into the even numbers file
            }
        }
        
        usleep(250000);
    }

    return NULL;
}

int main()
{
    pid_t pid_1, pid_2;
    // TODO - Open the producer file so that:
    // - only read operations are allowed
    // - file is created if not existing
    // - set permissions to 0644
    // Hint: remember that both threads will read from the same
    // file and how file descriptors track the progress within
    // the file....

    int file_descriptor = open(PRODUCER_FILE_NAME, O_RDONLY | O_CREAT, 0644);
    if (file_descriptor == -1)
    {
        fprintf(stderr, "Error: %s: %s\n", PRODUCER_FILE_NAME, strerror(errno));
        return -1;
    }

    

    pid_1 = fork();
    if(pid_1 == 0){
        oddNumbersConsumer(&file_descriptor);
    }
    else{
        pid_2 = fork();
        if(pid_2 == 0){
            evenNumbersConsumer(&file_descriptor);
        }
    }

    wait(NULL);
    wait(NULL);

    exit(0);
}