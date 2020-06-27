#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#define loggerPath "/home/raz/Desktop/'CLOGGER'"  //enter here Client logger path
#define MAX 100
#define PORT 8888
#define SA struct sockaddr

void func(int sockfd)
{
    char buff[MAX];
    int n;
    for (;;) {
        bzero(buff, sizeof(buff));
        printf("Enter the string : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        printf("From Server : %s", buff);
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}

  char* print_tm()
{
	time_t mytime = time(NULL);
	char* time_str = ctime(&mytime);
	time_str[strlen(time_str) - 1] = '\0';
	return time_str;
}

  void CLOGGER(char* action)
{
	char* tm = print_tm();
	FILE* fp = fopen(loggerPath, "a+");
	fprintf(fp, "LOG: occurred at :[%s] Details: %s\n", tm, action);
	fclose(fp);
}



int main(int argc, char* argv[])
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        CLOGGER("Socket creation failed");
        exit(0);
    }

    else{
        printf("Socket successfully created..\n");
    	CLOGGER("Socket Created");
    }
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        CLOGGER("Connection with the server failed..");
        exit(0);
    }
    else{
        printf("connected to the server..\n");
        CLOGGER("Connected to the server");
    }

    for(int i = 0 ; i < 2 ; i ++){
    send(sockfd, argv[i+1], sizeof(argv[i+1]), 0);
    }


    int d = 0;
    recv(sockfd , &d, sizeof(d) , 0);
    printf("The distance between you and the site is: %d\n", d );



	int response = 0;
    if(d>20){
    	response = 1;
    	printf("You have gone too far !!\nCome Back To The Site\nOpen Chat\n");
	printf("Type 'exit' when you want to quit\n");
	send(sockfd, &response, sizeof(int), 0);
    	func(sockfd);
    	printf("Exit in 5 seconds\n");
    	sleep(5);
    	}
    else{
    	printf("You are in the right zone.. Keep going\nExit...");
    	sleep(5);
    }
    // function for chat
    //func(sockfd);

    // close the socket
    close(sockfd);
    return 0;
}
