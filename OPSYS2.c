#include <stdio.h>
#include<sys/socket.h> //create an endpoint for communication
#include<arpa/inet.h>
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>      
//while program is running, this port is using by the program 
#define PORT_NUMBER 60019
//this variable uses for date calculation function
#define BUF_LEN 64

//Although a single client is requested in the assignment, we used threads in our design.
void *connection_handler(void *socket_desc);

//the equals function we wrote ourselves
int equals(char* s1, char* s2);

//Since the server uses date_time for each command when creating the response for the client, we wrote it in a function
struct tm* get_current_date();

//lower_case function we wrote ourselves
char* to_lower_case(char* s);

//The function to be used for the get_time command from the client
char* get_time();

//The function to be used for the get_date command from the client
char* get_date();

//The function to be used for the get_time_date command from 
char* get_time_date();

//The function to be used for the get_time_zone command from 
char* get_time_zone();

//The function to be used for the get_day_of_week command from 
char* get_day_of_week();

int main() {

    int socket_at_server_side, socket_at_client_side, c, *address_of_client_socket;
    struct sockaddr_in server, client;

    /*Although there is only one client in the system,
    we wanted to use threads so that the design would be more organized this way.*/
    pthread_t handler_of_client;//thread for client

    //Creating socket for server
    socket_at_server_side = socket(AF_INET, SOCK_STREAM, 0);

    //error state for creating socket operation
    if (socket_at_server_side == -1) {
        puts("Could not create socket of server.");
        return 1;
    }
    
    puts("Socket created");

    //Assigning values for socket address
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT_NUMBER);

    
    if(bind(socket_at_server_side, (struct sockaddr_in * )&server, sizeof(server)) < 0){
        puts("Server could not bind socket.");
        return 1;
    }

    puts("Socket is binded");


    /*
    here the server is fully provisioned and is constantly listening to the specified port
    */
    listen(socket_at_server_side, 1);

    puts("Now Server is Listening...");

    c = sizeof(struct sockaddr_in);

    /*
    If there is a request to connect from the client,
    the server accepts this connection and takes the incoming client to a thread
    and continues to listen to the commands from the client in that thread.
    */
    socket_at_client_side = accept(socket_at_server_side, (struct sockaddr*)&client, (socklen_t*)&c);

    if(socket_at_client_side < 0){
        puts("Client could not connect th server.");
        return 1;
    }

    puts("Connection accepted");

    /*
    In order to transfer the client in socket address threaded, a pointer is copied here
    */
    address_of_client_socket = malloc(1);
    *address_of_client_socket = socket_at_client_side;

    //Creating a thread to meet the client
    pthread_create(&handler_of_client, NULL, connection_handler, (void*)address_of_client_socket);

    //The main thread needs to wait for the client threading to finish.
    pthread_join(handler_of_client, NULL);    

    //socket closing on server side
    close(socket_at_server_side);

    //socket closing on client side
    close(socket_at_client_side);
    
    return 0;
}

void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *((int*)socket_desc);
    int max_command_length = 64;
    int exit_flag = 0;//flag to use for while loop

    char command_input_string[25] = "\nEnter a command: ";//a variable for "enter a command" 
    char* command_name= (char*)malloc(sizeof(char) * max_command_length);

    //The address where the command from the client will be written
    char* first_address_of_command = command_name;
    

    //static command names to use in controls to make equality checks more controllable
    char* get_time_command = (char*)malloc(sizeof(char)* max_command_length);
    char* get_date_command = (char*)malloc(sizeof(char)* max_command_length);
    char* get_time_and_date_command = (char*)malloc(sizeof(char)* max_command_length);
    char* get_time_zone_command = (char*)malloc(sizeof(char)* max_command_length);
    char* get_day_of_week_command = (char*)malloc(sizeof(char)* max_command_length);
    char* close_server_command = (char*)malloc(sizeof(char)* max_command_length);
    

    //Names of commands are written inside static commands
    get_time_command = "get_time";
    get_date_command = "get_date";
    get_time_and_date_command = "get_time_date";
    get_time_zone_command = "get_time_zone";
    get_day_of_week_command = "get_day_of_week";
    close_server_command = "close_server";

 
    while (!exit_flag) {
        /*Every time a command is received from the client,
        we have implemented such a solution in order to be able to bring the address where
        we wrote the command to the beginning and to delete the field where the old command was written.*/
        command_name = first_address_of_command;
        command_name = (char*)malloc(sizeof(char) * 32);
    
        //"enter a command" prints to console
        write(sock, command_input_string, strlen(command_input_string));
        //Receiving command from client
        recv(sock, (char*)command_name, 2000, 0);

        if(equals(command_name, get_time_command)){
            //If the incoming command is get_time,
            // then the appropriate function is called and the message is created and sent to the client.
            char* time = get_time();
            if(send(sock, time, strlen(time), 0) < 0){
                puts("Time could not be created.");
                
            }
        }
        else if(equals(command_name, get_date_command)){
            //If the incoming command is get_date,
            // then the appropriate function is called and the message is created and sent to the client.
            char* date = get_date();
            if(send(sock, date, strlen(date), 0) < 0){
                puts("Date could not be created.");
                
            }
        }
        else if(equals(command_name, get_time_and_date_command)){
            //If the incoming command is get_time_date,
            // then the appropriate function is called and the message is created and sent to the client.
            char* time_date = get_time_date();
            if(send(sock, time_date, strlen(time_date), 0) < 0){
                puts("Time and Date could not be created together.");
                
            }
        }
        else if(equals(command_name, get_time_zone_command)){
            //If the incoming command is get_time_zone,
            // then the appropriate function is called and the message is created and sent to the client.
            char* time_zone = get_time_zone();
            if(send(sock, time_zone, strlen(time_zone), 0) < 0){
                puts("Time Zone could not be created.");
                
            }
        }
        else if(equals(command_name, get_day_of_week_command)){
            //If the incoming command is get_day_of_week,
            // then the appropriate function is called and the message is created and sent to the client.
            char* day_of_week = get_day_of_week();
            if(send(sock, day_of_week, strlen(day_of_week), 0) < 0){
                puts("Day of week could not be created together.");
                
            }
        }
        else if(equals(command_name, close_server_command)){
            //If the incoming command is the close_server command, then exit_flag = 1,
            //which is the condition of the while loop in the thread created for the client, is done and the loop is exited.
            send(sock, "GOOD BYE\n", strlen("GOOD BYE\n"), 0);
            exit_flag = 1;
        }
        else{
            //If the incoming command is not a command defined on the server side,
            // a incorrect request message is sent to the client.
            if(send(sock, "INCORRECT_REQUEST", strlen("INCORRECT_REQUEST"), 0) < 0){
                puts("Send failed");
                
            }
        }
    }

    free(socket_desc); //Free the socket pointer

    return 0;
}

int equals(char* s1, char* s2){
    //this is the equals function we wrote
    int length_of_s1 = strlen(s1) - 2;
    int length_of_s2 = strlen(s2);
    int i;//loopVariable
    int controlFlag = 1;
    //firstly, incoming command converts to lower case
    char* lower_format = to_lower_case(s1);

    //if length of strings is not equals, this function returns 0(false)
    if(length_of_s1 != length_of_s2){
        return 0;
    }
    else {
        //if there is an unequal letter the function returns false
        for(i = 0;i < length_of_s1; i++){
            if(*(lower_format + i) != *(s2 + i)){
                controlFlag = 0;
                break;
            }
        }
        return controlFlag;
    }
}

char* to_lower_case(char* s){
    //this function is to lower case function that we wrote
    int i;//loopVariable
    int max_string_length = 64;//The variable we use to use the malloc function
    //freeing up memory
    char* return_string = (char*)malloc(sizeof(char) * max_string_length);
    int length_of_s = strlen(s) - 2;

    for(i = 0;i < length_of_s; i++){
        /*if the checked letter is an uppercase letter 
        then we add 32 to the ascii value of that letter to get the lowercase equivalent of that letter*/
        if((int)*(s + i) <= 90 && (int)*(s + i) >= 65){
            *(return_string + i) = (char)((int)(*(s + i)) + 32);
        }
        else{
            //if the checked letter is already lowercase, it is appended to the return string without modification
            *(return_string + i) = ((char)(*(s + i)));
        }
    }
    return return_string;
}

struct tm* get_current_date() {

    //Here, firstly, the time from 1970 to the present is taken in seconds.
    time_t now = time(NULL);
    //then time is converted from seconds to localtime
    struct tm *cur_time = localtime(&now);

    return cur_time;
}

char* get_time(){

    //A response is created for the incoming command by giving appropriate parameters to strtime
    char* formatted_time_address = (char*)malloc(sizeof(char)* BUF_LEN);

    strftime(formatted_time_address, BUF_LEN, "%T", get_current_date());

    return formatted_time_address;

}

char* get_date(){

    //A response is created for the incoming command by giving appropriate parameters to strtime
    char* formatted_date_address = (char*)malloc(sizeof(char)* BUF_LEN);

    strftime(formatted_date_address, BUF_LEN, "%d.%m.%Y", get_current_date());

    return formatted_date_address;

}

char* get_time_date(){

    //this function uses get_date and get_time function.
    char* time_date = (char*)malloc(sizeof(char) * BUF_LEN);

    time_date = strcat(get_time(), ", \0");

    time_date = strcat(time_date, get_date());

    return time_date;
}

char* get_time_zone(){
    
    //A response is created for the incoming command by giving appropriate parameters to strtime
    char* formatted_time_zone = (char*)malloc(sizeof(char)* BUF_LEN);

    strftime(formatted_time_zone, BUF_LEN, "%z", get_current_date());

    return formatted_time_zone;
}

char* get_day_of_week(){

    //A response is created for the incoming command by giving appropriate parameters to strtime
    char* formatted_day_of_the_week = (char*)malloc(sizeof(char)* BUF_LEN);

    strftime(formatted_day_of_the_week, BUF_LEN, "%A", get_current_date());

    return formatted_day_of_the_week;
}