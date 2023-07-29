#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <netdb.h> 
#include <unistd.h> // close

// #define DATA_BUFFER "Mona Lisa was painted by Leonardo da Vinci"
// #define PORT 7000

#define MAX_RETRY 10
#define MULTIPLIER 2
#define BASE_INTERVAL_MS 500
#define MAX_WAIT_INTERVAL_MS 8000
#define DATA_BUFFER 5000

int main (int argc, char* argv[]) {

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server IP> <port> <message>\n", argv[0]);
        return 1;
    }

    const char* server_ip = argv[1];
    int port = atoi(argv[2]);
    const char* message = argv[3];

    struct sockaddr_in server_addr;
    int sockfd, ret_val;
    socklen_t addrlen;
    
    /* Open a UDP socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
    if (sockfd == -1) {
        fprintf(stderr, "[Error] socket failed [%s]\n", strerror(errno));
        return -1;
    }
    printf("[Info] Created a socket with fd: %d\n", sockfd);
    
    /* Initialize the server address */
    server_addr.sin_family = AF_INET;         
    server_addr.sin_port = htons(port);     
    ret_val = inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
    if (ret_val == -1) {
        fprintf(stderr, "[Error] inet_pton error\n");
        close(sockfd);
        return 1;
    }
    // struct hostent *host; /* need netdb.h for this  */
    // host = gethostbyname("127.0.0.1");
    // server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    

    /* Send some data */
    int retry = 0, wait_interval = 0;
    fd_set read_fds;
    struct timeval timeout;
    FD_ZERO(&read_fds);
    while (retry < MAX_RETRY){

        /* Send message to server */
        ret_val = sendto(sockfd, message, strlen(message)+1, 0, 
            (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
        printf("ret_val: %d\n", ret_val);
        if (ret_val != -1) {
            printf("[Info] Successfully sent data (len %d bytes): %s\n", ret_val, message);
        } else {
            printf("[Error] sendto() failed [%s]\n", strerror(errno));
        }

        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        wait_interval = BASE_INTERVAL_MS * pow(MULTIPLIER, retry);
        wait_interval = (wait_interval > MAX_WAIT_INTERVAL_MS) ? MAX_WAIT_INTERVAL_MS : wait_interval;
        printf("[Debug] wait_interval: %d\n", wait_interval);
        if (wait_interval == MAX_WAIT_INTERVAL_MS) return 1;
        timeout.tv_sec = wait_interval/1000;
        timeout.tv_usec = wait_interval%1000 * 1000;  // microsecond -> millisecond
        int num_ready = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);

        if (num_ready == -1) {
            perror("[Error] in select");
            close(sockfd);
            exit(1);
        } else if (num_ready == 0) {
            // 超時處理...
            printf("[Info] Timeout occurred, retry times: %d\n", retry);
        } else {
            if (FD_ISSET(sockfd, &read_fds)) {
                /* Receive response from server */
                char buffer[DATA_BUFFER];
                memset(buffer, 0, sizeof(buffer));
                addrlen = sizeof(server_addr);
                ret_val = recvfrom(sockfd, buffer, DATA_BUFFER, 0,
                        (struct sockaddr *)&server_addr, &addrlen);
                if (ret_val != -1) {
                    printf("[Info] Received data (len %d bytes): %s\n", ret_val, buffer);
                    break;
                } else {
                    printf("[Error] recvfrom() failed [%s]\n", strerror(errno));
                }
            }
        }

        retry += 1;
    }
    
    /* Last step: close the socket */
    close(sockfd);
    return 0;
}