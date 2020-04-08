#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include<string>
#include<iostream>
using namespace std;
//g++ s.cpp -lpthread -o s
//the thread function
#define PORT 8888
void *connection_handler(void *);
string namelist[10];
string IP[10];
string Port_[10];
string iptmp;
int nameflag = 0; 
bool online[10] = {0};

int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
    
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    // puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error sendmsg
        perror("bind failed. Error");
        return 1;
    }
    // puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
     
    
	pthread_t thread_id;
	
    while(true)
    {
        //Accept and incoming connection
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        // puts("Connection accepted");
        char* ipp = inet_ntoa(client.sin_addr);  
        iptmp = ipp;
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        // puts("Handler assigned");
    }
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *sendmsg;
    char *client_message;
    sendmsg = new char [2000]();
    client_message = new char[2000]();
    string strmsg;
    string _str;
    //Send some messages to the client
    strmsg = "connnect acccept!\n";
    strcpy(sendmsg,strmsg.c_str());
    send(sock , sendmsg , strlen(sendmsg),0);
    memset(sendmsg, 0, 2000);
    string substring;
    string name;
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        
        for (size_t i = 0; i < read_size ; i++)
        {
            _str += client_message[i];
        }
        
        int loc;
        loc = _str.find("#");
        
        //===============================================================================
        if (loc != string::npos)
        {
            // string substring;
            substring = _str.substr(0,loc);
            name = _str.substr(loc+1,_str.length());
            // ---------------------------------------------------------------------register
            if (substring == "REGISTER")
            {
                
                bool diff = true;
                for (size_t i = 0; i < nameflag; i++)
                {
                    if (name == namelist[i])
                    {
                        strmsg = "210 FAIL\n";
                        strcpy(sendmsg,strmsg.c_str());
                        send(sock,sendmsg,strlen(sendmsg),0);
                        diff = false;
                    }
                    
                }
                if (diff)
                { 
                    name.erase(name.find("\n"),2);
                    namelist[nameflag] = name;
                    // cout << namelist[nameflag] << "*\n";
                    nameflag ++;
                    strmsg = "100 OK\n";
                    strcpy(sendmsg,strmsg.c_str());
                    send(sock , sendmsg , strlen(sendmsg) , 0 ); 
                }
                
            }else{ // --------------------------------------------------------------login
                bool diff = true;
                for (size_t i = 0; i < nameflag; i++)
                {
                    if (substring == namelist[i])
                    {
                        cout << "login in success!\n";
                        Port_[i] = name;
                        IP[i] = iptmp; 
                        online[i] = true;
                        diff = false;            
                        strmsg += "account balance 10000\nnumber of account online: ";
                        int onsite = 0;
                        for (size_t j = 0; j < 10; j++)
                        {
                            if (online[j])
                            {
                                onsite++;
                            }
                            
                        }
                        strmsg += to_string(onsite);
                        strmsg += "\n";
                        for (size_t j = 0; j < nameflag; j++)
                        {
                            if (online[j])
                            {
                                strmsg += namelist[j];
                                strmsg += "#";
                                strmsg += IP[j];
                                strmsg += "#";
                                strmsg += Port_[j];
                            }
                        }
                        strcpy (sendmsg, strmsg.c_str());
                        send(sock , sendmsg , strlen(sendmsg) , 0 );
                    }
                    
                }
                if (diff)
                {                        
                    strmsg = "220 AUTH_FAIL\n";
                    strcpy(sendmsg,strmsg.c_str());
                    send(sock,sendmsg,strlen(sendmsg),0);
                }
            }
        }else if (_str == "List\n")
        {
            //----------------------------------------------------------------------------List
            
            strmsg += "account balance 10000\nnumber of account online: ";
            int onsite = 0;
            for (size_t j = 0; j < 10; j++)
            {
                if (online[j])
                {
                    onsite++;
                }
                
            }
            strmsg += to_string(onsite);
            strmsg += "\n";
            for (size_t j = 0; j < nameflag; j++)
            {
                if (online[j])
                {
                    strmsg += namelist[j];
                    strmsg += "#";
                    strmsg += IP[j];
                    strmsg += "#";
                    strmsg += Port_[j];
                }
            }
            strcpy (sendmsg, strmsg.c_str());
            send(sock , sendmsg , strlen(sendmsg) , 0 );
        }else if (_str == "Exit\n")
        {//--------------------------------------------------------------Exit
            for (size_t i = 0; i < nameflag; i++)
                {
                    if (substring == namelist[i]){
                        online[i] = false;
                    }
                }
            strmsg = "Bye\n";
            strcpy(sendmsg,strmsg.c_str());
            send(sock , sendmsg , strlen(sendmsg) , 0 );
            // close(sock);
        }else{
            // ----------------------------------------------------------wrong input
            strmsg = "Wrong input\n";
            strcpy(sendmsg,strmsg.c_str());
            send(sock , sendmsg , strlen(sendmsg) , 0 );
        }
    
        //===============================================================================
		//clear the sendmsg buffer
        // cout << "clear\n";
		memset(client_message, 0, 2000);
		memset(sendmsg, 0, 2000);
        _str = "";
        strmsg = "";
    }    
    delete client_message;
    delete sendmsg;
    return 0;
} 