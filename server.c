#include <stdio.h>
#include <string.h> // strerror
#include <netinet/in.h> 
#include <errno.h>
#include <unistd.h> // close

#define DATA_BUFFER 5000
#define PORT 7000

int main () {    
    struct sockaddr_in server_addr, client_addr;
    int fd, ret_val;
    socklen_t addrlen = sizeof(client_addr); 

    /* Create UDP socket */
    fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
    if (fd == -1){
        fprintf(stderr, "[Error] socket failed [%s]\n", strerror(errno));
        return -1;
    }
    printf("[Info] Created a socket with fd: %d\n", fd);

    /* Initialize the socket address structure */
    server_addr.sin_family = AF_INET;         
    server_addr.sin_port = htons(PORT);     
    server_addr.sin_addr.s_addr = INADDR_ANY; 

    /* Bind the socket */
    ret_val = bind(fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
    if (ret_val != 0) {
        fprintf(stderr, "[Error] bind failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }

    while (1){
        char buffer[DATA_BUFFER];
        /* Receiving data */
        ret_val = recvfrom(fd, buffer, DATA_BUFFER, 0,
                    (struct sockaddr *)&client_addr, &addrlen);
        if (ret_val != -1) {
            printf("[Info] Received data (len %d bytes): %s\n", ret_val, buffer);
        } else {
            printf("[Error] recvfrom() failed [%s]\n", strerror(errno));
        }

        ret_val = sendto(fd, buffer, DATA_BUFFER, 0, 
                    (struct sockaddr*)&client_addr, sizeof(struct sockaddr_in));
        if (ret_val ==-1) {
            perror("[Info] Error in sending message");
            // continue;
        }else{
            printf("[Info] Sent data (len %d bytes): %s\n", ret_val, buffer);
        }
        break;
    }

}