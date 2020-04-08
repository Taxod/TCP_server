// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include<string>
#include <iostream>
#define PORT 8888

using namespace std;

int main(int argc, char const *argv[]) 
{ 
    int sock = 0;
    int valread; //return recv length(sizeof)
    //使用 IP4 格式結構 struct sockaddr_in (in 表示 internet) 設定 internet 位址資訊
    struct sockaddr_in serv_addr;
     
    //SOCK_STREAM use TCP
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    serv_addr.sin_family = AF_INET; //use IPv4
    //Host to Network Short，將 short (2byte) 資料順序從 host 轉換至 network
    serv_addr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   //連線到socket server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 

    int portnum_lowerbound = 1024;
    int portnum_higherbound = 65535;

    string _str;
    char* sendmsg;
    char* buffer;
    buffer = new char [1000]();
    valread = recv( sock , buffer,1000,0); 
    printf("%s",buffer ); 
    delete buffer;
    while(getline(cin,_str)){
        _str = _str + '\n';
        sendmsg = new char [1000]();
        buffer = new char [1000]();
        int loc;
        loc = _str.find("#");
        if (loc != string::npos)
        {
            string substring, portnumber;
            substring = _str.substr(0,loc);
            portnumber = _str.substr(loc+1,_str.length());   
            // ---------------------------------------------------------------------register
            if (substring == "REGISTER")
            {
                //"100 OK"
                //"210 FAIL"
                strcpy (sendmsg, _str.c_str());
                send(sock , sendmsg , strlen(sendmsg) , 0 ); 
                valread = recv( sock , buffer, 1000,0); 
                printf("%s",buffer ); 
                /* send and get msg */
            }else{ // --------------------------------------------------------------login
                // if (stoi(portnumber) <= portnum_lowerbound || stoi(portnumber) >= portnum_higherbound)
                // {
                //     cout << "wrong input\n";
                //     continue;
                // }

                //List
                //"220 AUTH_FAIL"
                
                strcpy (sendmsg, _str.c_str());
                send(sock , sendmsg , strlen(sendmsg) , 0 );
                
                
                valread = recv( sock , buffer, 1000,0); /*Account Balance*/
                printf("%s",buffer ); 
                
                if (strstr(buffer,"220 AUTH_FAIL"))
                {
                    continue;
                }else
                {
                    delete buffer;/*number of account online*/
                    buffer = new char[1000]();
                    valread = recv( sock , buffer, 1000,0); 
                    printf("%s",buffer );
                    char* k = strchr(buffer,':'); 
                    int n = atoi(&(k[2]));


                    delete buffer;
                    buffer = new char[1000]();
                    valread = recv( sock , buffer, 1000,0); 
                    printf("%s",buffer ); 

                
                    for (size_t i = 0; i < n-1; i++)
                    {
                        delete buffer;
                        buffer = new char[1000]();
                        valread = recv( sock , buffer, 1000,0); 
                        printf("%s",buffer ); 
                    }
                    
                }
            }
        }else if (_str == "List\n")
        {
            //----------------------------------------------------------------------------List
            strcpy (sendmsg, _str.c_str());
            send(sock , sendmsg , strlen(sendmsg) , 0 );
            
            valread = recv( sock , buffer, 1000,0); /*Account Balance*/
            printf("%s",buffer );   
            
            delete buffer;
            buffer = new char[1000]();
            valread = recv( sock , buffer, 1000,0); 
            printf("%s",buffer );         
        }else if (_str == "Exit\n")
        {//--------------------------------------------------------------Exit
            strcpy (sendmsg, _str.c_str());
            send(sock , sendmsg , strlen(sendmsg) , 0 );
            valread = recv( sock , buffer, 1000,0); 
            printf("%s",buffer );
            // close(sock);
        }else{
            // ----------------------------------------------------------wrong input
            cout << "wrong input";
            continue;
        }
        delete sendmsg;
        delete buffer;
    }
     
    return 0; 
    }