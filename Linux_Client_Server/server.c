#include <stdio.h>  
#include <string.h>   //strlen  
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>   //close  
#include <arpa/inet.h>    //close  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros  
#include <time.h>
#include <math.h>
#define loggerPath "/home/user/Desktop/'SLOGGER'"  //enter here Client logger path


#define MAX 100
#define TRUE   1  
#define FALSE  0  
#define PORT 8888  


void func(int master_socket)
{
    char buff[MAX];
    int n;
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);
        // read the message from client and copy it in buffer
        read(master_socket, buff, sizeof(buff));
        // print buffer which contains the client contents
        printf("From client: %s", buff);
        printf("Enter the string: ");
        bzero(buff, MAX);
        n = 0;
        // copy server message in the buffer
        while ((buff[n++] = getchar()) != '\n')
            ;

        // and send that buffer to client
        write(master_socket, buff, sizeof(buff));

        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}

// Driver function


char* print_tm()
{
	time_t mytime = time(NULL);
	char* time_str = ctime(&mytime);
	time_str[strlen(time_str) - 1] = '\0';
	return time_str;
}

void SLOGGER(char* action)

{
	char* tm = print_tm();
	FILE* fp = fopen(loggerPath, "a+");
	fprintf(fp, "LOG: occurred at :[%s] Details: %s\n", tm, action);
	fclose(fp);
}



int main()   
{   
    int opt = TRUE;   
    int master_socket , addrlen , new_socket , client_socket[5] ,  
          max_clients = 5 , activity, i , valread , sd;   
    int max_sd;   
    struct sockaddr_in address;   
         
    char buffer[1025];  //data buffer of 1K  
         
    //set of socket descriptors  
    fd_set readfds;   
     
    //initialise all client_socket[] to 0 so not checked  
    for (i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = 0;   
    }   
         
    //create a master socket  
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   
    {
        SLOGGER("Socket Creation Failed\n");   
        perror("socket failed");   
        exit(EXIT_FAILURE);   
    }
    else{
        printf("Socket Create Success.. \n");
        SLOGGER("Socket Create Success..");
    }   
     
    //set master socket to allow multiple connections ,  
    //this is just a good habit, it will work without this  
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  
          sizeof(opt)) < 0 )   
    {   
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    }   
     
    //type of socket created  
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons( PORT );   
         
    //bind the socket to localhost port 8888  
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)   
    {   
        SLOGGER("Bind Failed...\n");
        perror("Bind failed");   
        exit(EXIT_FAILURE);   
    } 
    else {
		SLOGGER("Server binding success");
        printf("Server binding success!\n");
    }  


    printf("Listener on port %d \n", PORT);   
         
    //try to specify maximum of 3 pending connections for the master socket  
    if (listen(master_socket, 3) < 0)   
    {   
        SLOGGER("Listen failed..");
        perror("listen");   
        exit(EXIT_FAILURE);   
    }   
    else{
        SLOGGER("Listen success\n");
        printf("Listen sucess!\n");
    }
         
    //accept the incoming connection  
    addrlen = sizeof(address);   
    puts("Waiting for connections ...");   
         
    while(TRUE)   
    {   
        //clear the socket set  
        FD_ZERO(&readfds);   
     
        //add master socket to set  
        FD_SET(master_socket, &readfds);   
        max_sd = master_socket;   
             
        //add child sockets to set  
        for ( i = 0 ; i < max_clients ; i++)   
        {   
            //socket descriptor  
            sd = client_socket[i];   
                 
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);   
                 
            //highest file descriptor number, need it for the select function  
            if(sd > max_sd)   
                max_sd = sd;   
        }   
     
        //wait for an activity on one of the sockets , timeout is NULL ,  
        //so wait indefinitely  
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
       
        if ((activity < 0) && (errno!=EINTR))   
        {   
            printf("select error");   
        }   
             
        //If something happened on the master socket ,  
        //then its an incoming connection  
        if (FD_ISSET(master_socket, &readfds))   
        {   
            if ((new_socket = accept(master_socket,  
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   
             

            
            int temp[4];
            char *argv[4];
            for ( i =0; i<2; i++) {
                argv[i] = (char *)malloc(sizeof(char));
                recv(new_socket , argv[i], sizeof(argv[i]) , 0);
                temp[i] = atoi(argv[i]);
                }
            temp[2] = 23; temp[3] = 24; // (x,y) cordinations of the site x->temp[2] y->temp[3]

            //calc the distance betweeen the patrol to the site
            int d = sqrt(pow((temp[3]-temp[1]),2) + pow((temp[2] -temp[0]),2)) ;
            printf("Distance between the cordinations (the site and the security car): %d\n", d);


            if(send(new_socket , &d, sizeof(d), 0)){
                SLOGGER("Distance Sended");
                }
            else {
                printf("Error in sending the number");
                SLOGGER("Sending the number has been failed");
            }


            int response = 0;
            recv(new_socket , &response, sizeof(int) , 0);
            if(response){
                func(new_socket);
                printf("Try to connect again in 5 seconds\n");
                sleep(5);
            }
            else{
                printf("Try to connect again in 5 seconds\n");
                sleep(5);
            }
            
            //add new socket to array of sockets  
            for (i = 0; i < max_clients; i++)   
            {   
                //if position is empty  
                if( client_socket[i] == 0 )   
                {   
                    client_socket[i] = new_socket;   
                    printf("Adding to list of sockets as %d\n" , i);   
                         
                    break;   
                }   
            }   
        }   
             
        //else its some IO operation on some other socket 
        for (i = 0; i < max_clients; i++)   
        {   
            sd = client_socket[i];   
                 
            if (FD_ISSET( sd , &readfds))   
            {   
                //Check if it was for closing , and also read the  
                if ((valread = read( sd , buffer, 1024)) == 0)   
                {   
                    //Somebody disconnected , get his details and print  
                    getpeername(sd , (struct sockaddr*)&address ,(socklen_t*)&addrlen);   
                    printf("Host disconnected \n");   
                    SLOGGER("Host disconnected");    
                    //Close the socket and mark as 0 in list for reuse  
                    close( sd );   
                    client_socket[i] = 0;   
                }   
                     
                //Echo back the message that came in  
                else 
                {   
                    //set the string terminating NULL byte on the end  
                    //of the data read  
                    buffer[valread] = '\0';   
                    send(sd , buffer , strlen(buffer) , 0 );   
                }   
            }   
        } 

    }
        return 0;   

         
}   
