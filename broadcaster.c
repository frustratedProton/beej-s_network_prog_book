#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVERPORT 4950

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in their_addr;
    struct hostent *he;
    int nbytes;
    int broadcast = 1;

    if (argc != 3) {
        fprintf(stderr, "usage: broadcaster hostname message\n");
        exit(1);
    }

    if ((he = gethostbyname(argv[1])) == NULL) {
        perror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // ==this call is what allows broadcast packets to be sent -- beej==
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast,
                   sizeof(broadcast)) == -1) {
        perror("setsockopt (SO_BROADCAST)");
        exit(1);
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(SERVERPORT);
    // had to change to use h_addr_list[0]
    their_addr.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
    memset(their_addr.sin_zero, '\0', sizeof(their_addr.sin_zero));

    nbytes = sendto(sockfd, argv[2], strlen(argv[2]), 0,
                    (struct sockaddr *)&their_addr, sizeof their_addr);

    if (nbytes == -1) {
        perror("sendto");
        exit(1);
    }

    printf("sent %d bytes to %s\n", nbytes, inet_ntoa(their_addr.sin_addr));

    close(sockfd);

    return 0;
}
