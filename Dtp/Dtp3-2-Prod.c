#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "Dtp3.h"

void *numbersProducer(void *vargp)
{
    unsigned int number;
    char buffer[BUFFER_SIZE];
    int fd = *(int *)vargp;

    while (1)
    {
        number = rand() % 1000;
        printf("Generated number %u\n", number);
        // Prepare data to be written to file (integer to fixed-size string format)
        snprintf(buffer, BUFFER_SIZE, "%09d\n", number);
        // TODO - Write to file
        fprintf(stdout, "Writing number %u to file\n", number);
        if (write(fd, buffer, BUFFER_SIZE) == -1)
        {
            fprintf(stderr, "Error: %s: %s\n", PRODUCER_FILE_NAME, strerror(errno));
            return NULL;
        }
        sleep(1);
    }

    return NULL;
}

int main()
{
    int file_descriptor = -1;
    pthread_t tid_1;

    // TODO - Open file so that:
    // - only write-only operations are allowed
    // - file is created if not existing
    // - if file exists, append data (don't overwrite file)
    // - set permissions to 0644

    file_descriptor = open(PRODUCER_FILE_NAME, O_WRONLY | O_CREAT | O_APPEND, 0644);

    if (file_descriptor == -1)
    {
        fprintf(stderr, "Error: %s: %s\n", PRODUCER_FILE_NAME, strerror(errno));
        return -1;
    }
    
    fprintf(stdout, "File %s opened successfully\n", PRODUCER_FILE_NAME);

    // Modify the following statement if necessary (hint: it is!)

    pthread_create(&tid_1, NULL, numbersProducer, &file_descriptor);

    pthread_join(tid_1, NULL);

    exit(0);
}
