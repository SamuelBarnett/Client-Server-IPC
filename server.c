/*======================================================================
|
|
|   Name: server.c
|
|   Written by: Samuel Barnett - December 2022
|
|   Purpose: To create a means for the client to copy a file to the new destination and send a message back.
|
|   Compile: gcc -o serv server.c
|
|   usage:  ./serv &
|
|   Description of parameters:
|     Allows process to run in the background
|
|
|   Subroutines/libraries required:
|
|      See include statements.
|
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

// send message function
int sendmsg(int Clientfd);

int main()
{
    serv Data;
    int ofid;
    int fd = 0, Clientfd = 0, done = 0, count = 0;
    // Makes fifo for server to transfer files.
    if (mkfifo(SF, 0666) == -1)
        if (errno != EEXIST)
        {
            perror("receiver: mkfifo sf");
            exit(1);
        }
    // creates server->client Fifo for server to send messages to `````````````
    if (mkfifo(CF, 0666) == -1)
        if (errno != EEXIST)
        {
            perror("receiver: mkfifo cf");
            exit(1);
        }
    //`````````````````````````````````````````````````````````````````````````
    if (access(CF, F_OK) == -1 || access(SF, F_OK) == -1)
    {
        printf("Client or Server fifo not working\n");
        exit(1);
    }
    // opens the server fifo to read and output to file``````````````
    if ((fd = open(SF, O_RDWR)) < 0)
    {
        perror(SF);
        exit(1);
    }
    // opens local file
    ofid = open("lab6_sourcefile_local_clone.pdf", O_RDWR | O_CREAT | O_APPEND, 0777);

    //```````````````````````````````````````````````````````````````
    printf("SERVER: right before while\n");
    // waits for input from client
    while (!done)
    {
        // Reading structure
        printf("SERVER: right before structRead\n");

        int structbytes = read(fd, &Data, sizeof(Data));
        if (structbytes < 0)
        {
            printf("Failed to read struct on server side\n");
            exit(1);
        }
        // reading bytes
        printf("SERVER: right before read\n");
        printf("in server while loop, bytes are %d\n", Data.bytes);
        // writing bytes to new file
        if (write(ofid, Data.BUF, Data.bytes) == -1)
        {
            printf("error writing file in WriteFile function\n");
            exit(1);
        }
        // exits if the bytes are less than buffer size
        if (Data.bytes < BUFFER_SIZE)
        {
            printf("End of file reached.\n");
            exit(0);
        }
    }
    printf("at end of server code\n");

    // sends message to CF fifo
    sendmsg(Clientfd);
    // Closing
    close(Clientfd);
    close(fd);
    return 0;
};
/*======================================================================
|
|
|   Function: sendmsg
|
|   Written by: Samuel Barnett - December 2022
|
|   Purpose: Sends message to client that the process has finished transfering files.
|
|
|   Description of parameters:
|    int Clientfd: pointer to opening client fifo.
|
|   Subroutines/libraries required:
|      none.
|
|
|------------------------------------------------------------------*/
int sendmsg(int Clientfd)
{
    char msgbuf[BUFFER_SIZE];
    // Opens client
    if ((Clientfd = open(CF, O_WRONLY | O_NONBLOCK)) < 0)
    {
        perror(CF);
        exit(1);
    }

    strncpy(msgbuf, "ALL DONE", sizeof(msgbuf));

    if (write(Clientfd, msgbuf, BUFFER_SIZE) == -1)
    {
        printf("failed to send message\n");
        exit(1);
    }
}
