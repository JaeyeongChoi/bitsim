#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    int server_sockfd, client_sockfd;
    int state, client_len;
    int pid;

    FILE *fp;
    struct sockaddr_in clientaddr, serveraddr;

    char buf[255];
    char line[255];

    if (argc != 2)
    {
        printf("Usage : ./zipcode [port]\n");
        printf("예  : ./zipcode 4444\n");
        exit(0);
    }

    memset(line, '0', 255);
    state = 0;

    // 주소 파일을 읽어들인다.
    client_len = sizeof(clientaddr);
    if((fp = fopen("zipcode.txt", "a")) == NULL)
    {
        perror("file open error : ");
        exit(0);
    }

    // internet 기반의 소켓 생성 (INET)
    if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error : ");
        exit(0);
    }
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(atoi(argv[1]));

    state = bind(server_sockfd , (struct sockaddr *)&serveraddr,
            sizeof(serveraddr));
    if (state == -1)
    {
        perror("bind error : ");
        exit(0);
    }

    state = listen(server_sockfd, 5);
    if (state == -1)
    {
        perror("listen error : ");
        exit(0);
    }

    while(1)
    {
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&clientaddr,
                               &client_len);
        if (client_sockfd == -1)
        {
            perror("Accept error : ");
            exit(0);
        }
        while(1)
        {
            rewind(fp);
            memset(buf, '0', 255);
            if (read(client_sockfd, buf, 255) <= 0)
            {
                close(client_sockfd);
                break;
            }

            if (strncmp(buf, "quit",4) == 0)
            {
                write(client_sockfd, "bye bye", 8);
                close(client_sockfd);
                break;
            }

	    fprintf(fp, "%s\n", buf); //write(buf, 1, sizeof(buf), fp); 
            printf("send end\n");
	    write(client_sockfd, "end", 255);
        }
    }
    close(client_sockfd);
}
