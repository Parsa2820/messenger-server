# Messenger Server Version 1.0
## Table of contents
* [Prerequisites](#Prerequisites)
* [Functions in server.c](#Functions-in-server.c)
    * [createSocket](#createSocket)
    * [acceptClient](#acceptClient)
    * [getRequest](#getRequest)
    * [registerUser](#registerUser)
    * [printTime](#printTime)
    * [loginUser](#loginUser)
    * [makeToken](#makeToken)
    * [logoutUser](#logoutUser)
    * [creatChannel](#creatChannel)
    * [joinChannel](#joinChannel)
    * [sendMessage](#sendMessage)
    * [refresh](#refresh)
    * [channelMembers](#channelMembers)
    * [leaveChannel](#leaveChannel)
    * [indexOfThisToken](#indexOfThisToken)
    * [searchMembers](#searchMembers)
* [Functions in fileManage.c](#Functions-in-fileManage.c)
    * [createDataDirectories](#createDataDirectories)
    * [isUserExist](#isUserExist)
    * [isChannelExist](#isChannelExist)
    * [name2JSONFileName](#name2JSONFileName)
    * [username2FileAddress](#username2FileAddress)
    * [channelName2FileAddress](#channelName2FileAddress)
* [Default port and IP address](#Default-port-and-IP-address)

## Prerequisites
First of all you need standard C libraries listed below
1. stdio and stdlib
2. string
3. sys/types
4. sys/stat
5. dirent
6. stdbool
7. sys/socket
8. netinet/ip
9. time

you also need [JOSNc](https://github.com/Parsa2820/JSONc) (See the link for docs)

compiled with gcc 9.2.1 on linux (Windows needs different libraries)

## Functions in server.c

### createSocket
```c
void createSocket();
```
This function designed for create and verify socket.

### acceptClient
```c
void acceptClient();
```
This Function designed to accept client trying to connect to server.

### getRequest
```c
void getRequest();
```
This function designed to get request from client and read first word from buffer to decide call wich function and performe wich task.

### registerUser
```c
void registerUser();
```
This function check if username is available or not. If it was it make new file under "./Data/Users" for new user and if it wasn't it fills buffer with Error and error message.

### printTime
```c
void printTime();
```
This function print current time ands date in a human readable format. It used for print server log.

### loginUser
```c
void loginUser();
```
This function check requested username and password and if the login was correct fill buffer with authentication token generated from username.

### makeToken
```c
void makeToken(char *username, char *authToken);
```
This function get username `(char *username)` and an empty string `(char *authToken)` and make an authentication token from username using sum of username ascii codes and puts it into empty string as result.

### logoutUser
```c
void logoutUser();
```
This function change online user state in onlineUsers array to offline.

### creatChannel
```c
void creatChannel();
```
This function create new channel. First it validate request and if it wasn't valid it fills buffer with Error and error message. Else this make a new file for channel in ./Data/Channels and add a message `username created channel` into it.


### joinChannel
```c
void joinChannel();
```
This function add an user to given channel. First it validate request and if it wasn't valid it puts Error and error message in the buffer. Else it add user to channel and update channel data file with `username joined channel` message.

### sendMessage
```c
void sendMessage();
```
This function first validate request and if it wasn't valid it puts Error and error message in the buffer. If the request was valid it read channel data from file and add new message into it and then update the channel data file.

### refresh
```c
void refresh();
```
This function puts unseen messages of an online user into buffer. First this function validate request and if the request wasn't valid fill buffer with Error and error message; If not it puts unseen messages of given user (founds by authentication token) in array and put it into buffer.

### channelMembers
```c
void channelMembers();
```
This function puts channel members array in the buffer. First this function validate request and if the request wasn't valid fill buffer with Error and error message; If not it search onlineUsers for matching channel name and if it mached, add it to members array.

### leaveChannel
```c
void leaveChannel();
```
This function designed for leave online user current channel. First this function validate request and if the request wasn't valid fill buffer with Error and error message; If not it reads channel info from "./Data/Channels" and put a "`username` leaved channel" message in messages array and then update channel data.

### indexOfThisToken
```c
int indexOfThisToken(char *authToken);
```
This function take an authentication token `(char *authToken)` as input. Then search in onlineUsers array to find The given authentication token. Finaly it returns index of online user in output and if doesn't find given authentication token returns -1 as result.

### searchMembers
```c
void searchMembers();
```
This function search for a username in user related to authentication token channel after validating authentication token. If it finds username it fill respond JSON content with "True" else "False".

## Functions in fileManage.c

### createDataDirectories
```c
void createDataDirectories();
```
This function make 3 directories for storing messenger datas. First "./Data" and then "Channels" and "Users" in "./Data" directory. If one of them already exist, function skip that folder. And doesn't have any retyrn value.

### isUserExist
```c
int isUserExist(char *fileName);
```
This function take file name `(char *fileName)` as input and list "./Data/Users" directory files. If given file name existed in list, it returns 1, else it returns 0.

### isChannelExist
```c
int isChannelExist(char *fileName);
```
This function take file name `(char *fileName)` as input and list "./Data/Channels" directory files. If given file name existed in list, it returns 1, else it returns 0.

### name2JSONFileName
```c
void name2JSONFileName(const char *name, char *fileName);
```
This function take a name `(const char *name)` and file name `(char *fileName)` as input. file name use to store function result value. This function just add ".json" postfix to given name. And doesn't have any retyrn value.

### username2FileAddress
```c
void username2FileAddress(const char *fileName, char *fileAddress);
```
This function take a file name `(const char *fileName)` and file address `(char *fileAddress`) as input. Actualy file address use to store function result value. This function just add "./Data/Users/" before file name and put it into file address. And doesn't have any retyrn value.

### channelName2FileAddress
```c
void channelName2FileAddress(const char *fileName, char *fileAddress);
```
This function take a file name `(const char *fileName)` and file address `(char *fileAddress`) as input. Actualy file address use to store function result value. This function just add "./Data/Channels/" before file name and put It into file address. And doesn't have any retyrn value.

## Default port and IP address
For this server default port is : 
- Port : 12345

It is obvious that you can change it `(server.c Ln 15)`