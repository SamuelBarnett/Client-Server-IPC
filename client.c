/*======================================================================
|
|
|   Name: client.c
|
|   Written by: Samuel Barnett - December 2022
|
|   Purpose: Sends a file to a server to copy to and recieves a message telling it if its done.
|
|   Compile: gcc -o client client.c
|
|   usage:  ./client fileArgument
|
|   Description of parameters:
|     A file.
|
|
|   Subroutines/libraries required:
|
|      See include statements.
|
|------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#define SF "serv_fifo"
#define CF "client_fifo"
#define BUFFER_SIZE 1024

struct serverData
{
    int bytes;
    char BUF[BUFFER_SIZE];
};

typedef struct serverData serv;

int main(int argc, char *argv[])
{
    int fd = 0, done = 0, fileArg = 0, i, Clientfd = 0;
    int nread = 0;
    char fileBuffer[BUFFER_SIZE];
    serv Data;

    // Validation``````````````````````````````````````````````````````````````
    // makes sure the user has one argument.
    if (argc != 2)
    {
        printf("useage: ./sendFile 'fileArg'\n");
        exit(1);
    }
    else if (access(SF, F_OK) == -1)
    {
        printf("Server fifo not working, run the server code\n");
        exit(1);
    }
    //`````````````````````````````````````````````````````````````````````````
    // opens the client.server fifo for client to server transfers````````````
    if ((fd = open(SF, O_WRONLY | O_NONBLOCK)) < 0)
    {
        perror(SF);
        printf("Failed to open client.server fifo for client to server file transfers.\n");
        exit(1);
    }
    //`````````````````````````````````````````````````````````````````````````
    // opens the client.server fifo for client to server transfers````````````

    //`````````````````````````````````````````````````````````````````````````
    // client must write to the server fifo using filename from argv[]
    if ((fileArg = open(argv[1], O_RDONLY)) < 0)
    {
        perror("Error opening file\n");
        exit(1);
    }
    // client reads data from original file
    while ((nread = read(fileArg, &Data.BUF, BUFFER_SIZE)) > 0)
    {
        // client put nread in structure
        Data.bytes = nread;
        // write to structure
        int struct_bytes_written = write(fd, &Data, sizeof(Data));
        if (struct_bytes_written < 0)
        {
            printf("Error writing struct bytes from client\n");
            exit(1);
        }
        // Prints out all done message.
        if (Data.bytes < BUFFER_SIZE)
        {
            printf("\n\n\tALL DONE!\n\n");
            exit(0);
        }
    }
    close(fd);
    return 0;
};
