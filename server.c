#include "headers.h"
#include "prototypes.h"

char buffer[MAX];
int serverSocket, clientSocket; 
struct sockaddr_in server, client; 

struct onlineUser
{
	char authToken[MAX], username[MAX], channelName[MAX];
	bool online;
	unsigned int seenMessageCount;
}onlineUsers[MAXUSERS];

int main() 
{
	createDataDirectories();
	printf("\tTime\t\t\tProcess\n\n");
	createSocket();
	getRequest();
	shutdown(serverSocket, SHUT_RDWR); 
}

//Function for create & verify socket
void createSocket()
{
	// Create and verify socket
	serverSocket = socket(AF_INET, SOCK_STREAM, 0); 
	if (serverSocket == -1) {
		printTime();
		printf("Socket creation failed. Exiting now ...\n"); 
		exit(0); 
	} 
	else
	{
		printTime();
		printf("Socket successfully created\n"); 
	}
	// Assign IP and port
	bzero(&server, sizeof(server)); 
	server.sin_family = AF_INET; 
	server.sin_addr.s_addr = htonl(INADDR_ANY); 
	server.sin_port = htons(PORT); 
	// Bind newly created socket to given IP and verify
	if ((bind(serverSocket, (SA*)&server, sizeof(server))) != 0) {
		printTime();
		printf("Socket binding failed. Exiting now ...\n"); 
		exit(0); 
	} 
	else
	{
		printTime();
		printf("Socket successfully bound\n"); 
	}
	// Now server is ready to listen and verify 
	if ((listen(serverSocket, 5)) != 0) {
		printTime();
		printf("Listen failed. Exiting now ...\n"); 
		exit(0); 
	} 
	else
	{
		printTime();
		printf("Server listening\n"); 
	}
}

// Function for accept client request to connect to socket
void acceptClient()
{
	// Accept the data packet from client and verify
	socklen_t len = sizeof(client);
	clientSocket = accept(serverSocket, (SA*)&client, &len); 
	if (clientSocket < 0) { 
		printTime();
		printf("Server accceptance failed. Exiting now ...\n"); 
		exit(0); 
	} 
	else
	{
		printTime();
		printf("Server acccepted the client\n"); 
	}
}

// Function for get request from client 
void getRequest() 
{ 
	char command[MAX];
	while (true)
	{
		acceptClient();
		memset(buffer, 0, sizeof(buffer)); 
		// Read the message from client and copy it to buffer 
		recv(clientSocket, buffer, sizeof(buffer), 0);
		printTime();
		printf("Request : %s", buffer);
		// Process client request
		sscanf(buffer, "%s", command);
		if (!strcmp(command, "register"))
			registerUser();
		else if (!strcmp(command, "login"))
			loginUser();
		else if (!strcmp(command, "logout"))
			logoutUser();
		else if (!strcmp(command, "create"))
			creatChannel();
		else if (!strcmp(command, "join"))
			joinChannel();
		else if (!strcmp(command, "leave"))
			leaveChannel();
		else if (!strcmp(command, "refresh"))
			refresh();
		else if (!strcmp(command, "send"))
			sendMessage();
		else if (!strcmp(command, "channel"))
			channelMembers();
		else if (!strcmp(command, "search"))
		{
			sscanf(buffer + 7, "%s", command);
			if (!strcmp(command, "members"))
				searchMembers();
		}
		// Send the buffer to client 
		printTime();
		printf("Respond : %s\n", buffer);
		send(clientSocket, buffer, sizeof(buffer), 0);
		//close(clientSocket);
	} 
} 

// Function to signup new user
void registerUser()
{
	char username[MAX], password[MAX], fileName[MAX], fileAddress[MAX];
	JSONc *respond = JSONc_createObject(),
	*type, *content, *data = JSONc_createObject(), *usernameJSON, *passwordJSON;
	int usernameError = 0;
	sscanf(buffer+9, "%s%s", username, password);
	username[strlen(username)-1] = 0; //Removing ',' from the username end
	memset(fileName, 0, sizeof(fileName));
	name2JSONFileName(username, fileName);
	memset(fileAddress, 0, sizeof(fileAddress));
	username2FileAddress(fileName, fileAddress);
	if (isUserExist(fileName))
	{
		type = JSONc_createString("Error");
		content = JSONc_createString("This username is already taken.");
		usernameError = 1;
	}
	else
	{
		type = JSONc_createString("Successful");
		content = JSONc_createString("");
	}
	JSONc_addItem2Object(respond, "type", type);
	JSONc_addItem2Object(respond, "content", content);
	memset(buffer, 0, sizeof(buffer));
	strcat(buffer, JSONc_printUnformatted(respond));
	if (usernameError) return;
	usernameJSON = JSONc_createString(username);
	passwordJSON = JSONc_createString(password);
	JSONc_addItem2Object(data, "username", usernameJSON);
	JSONc_addItem2Object(data, "password", passwordJSON);
	FILE * userJSON = fopen(fileAddress, "w");
	fprintf(userJSON, "%s\n", JSONc_printUnformatted(data));
	fclose(userJSON);
}

// Function that print current date and time to console
void printTime()
{
	time_t currentTime = time(NULL);
	char currentTimeFormatted[MAX];
	strcpy(currentTimeFormatted, ctime(&currentTime));
	currentTimeFormatted[strlen(currentTimeFormatted)-1] = 0;
	printf("%s | ", currentTimeFormatted);
}

// Function for login
void loginUser()
{
	char username[MAX], password[MAX], fileName[MAX], fileAddress[MAX],
	savedJSONString[MAX];
	JSONc *respond = JSONc_createObject(),
	*type, *content, *data = JSONc_createObject(), *usernameJSON, *passwordJSON,
	*savedJSON;
	int error = 0;
	sscanf(buffer+5, "%s%s", username, password);
	username[strlen(username)-1] = 0; //Removing ',' from the username end
	memset(fileName, 0, sizeof(fileName));
	name2JSONFileName(username, fileName);
	for (int i = 0; i < MAXUSERS; i++)
		if (!strcmp(username, onlineUsers[i].username) && onlineUsers[i].online)
		{
			error = true;
			type = JSONc_createString("Error");
			content = JSONc_createString("This user is already loged in.");
			break;
		}
	if(!isUserExist(fileName) && !error)
	{
		type = JSONc_createString("Error");
		content = JSONc_createString("User not exist.");
		error = 1;
	}
	else if (!error)
	{
		memset(fileAddress, 0, sizeof(fileAddress));
		username2FileAddress(fileName, fileAddress);
		FILE *userJSON = fopen(fileAddress, "r");
		memset(savedJSONString, 0, sizeof(savedJSONString));
		fscanf(userJSON, "%[^\n]", savedJSONString);
		savedJSON = JSONc_parse(savedJSONString);
		if(strcmp(password, JSONc_getObjectItem(savedJSON, "password") -> stringVal))
		{
			type = JSONc_createString("Error");
			content = JSONc_createString("Wrong password.");
			error = 1;
		}
		JSONc_delete(savedJSON);
		fclose(userJSON);
	}
	if (!error)
	{
		int x;
		for (int i = 0; i < MAXUSERS; i++)
		{
			if (!onlineUsers[i].online){
				x = i;
				break;
			}
		}
		char authToken[21];
		memset(authToken, 0, sizeof(authToken));
		makeToken(username, authToken);
		type = JSONc_createString("AuthToken");
		content = JSONc_createString(authToken);
		onlineUsers[x].online = true;
		strcpy(onlineUsers[x].authToken, authToken);
		strcpy(onlineUsers[x].username, username);
		strcpy(onlineUsers[x].channelName, "");
	}
	JSONc_addItem2Object(respond, "type", type);
	JSONc_addItem2Object(respond, "content", content);
	memset(buffer, 0, sizeof(buffer));
	strcat(buffer, JSONc_printUnformatted(respond));
}

// Function that make random 20 character authentication token
void makeToken(char *username, char *authToken)
{
	unsigned int seed = 0;
	for (int i = 0; i < strlen(username); i++)
		seed += *(username + i);
	for (int i = 0; i < 20; i++)
	{
		*(authToken + i) = seed%26 + 65;
		seed += seed % 913 * 3 / 5;
	}
}

// Function to logout a user
void logoutUser()
{
	JSONc *respond = JSONc_createObject(), *type, *content;
	char authToken[MAX];
	int error = 1;
	sscanf(buffer+7, "%s", authToken);
	for (int i = 0; i < MAXUSERS; i++)
	{
		if (!strcmp(authToken, onlineUsers[i].authToken))
		{
			onlineUsers[i].online = false;
			type = JSONc_createString("Successful");
			content = JSONc_createString("");
			error = 0;
			break;
		}
	}
	if (error)
	{
		type = JSONc_createString("Error");
		content = JSONc_createString("This user isn't loged in right now.");
	}
	JSONc_addItem2Object(respond, "type", type);
	JSONc_addItem2Object(respond, "content", content);
	memset(buffer, 0, sizeof(buffer));
	strcat(buffer, JSONc_printUnformatted(respond));
}

// Function to create new channel
void creatChannel()
{
	char channelName[MAX], authToken[MAX], fileName[MAX], fileAddress[MAX],
	fileContent[MAX], text[MAX];
	JSONc *respond = JSONc_createObject(), *type, *content,
	*data = JSONc_createObject(), *messages, *name,
	*message = JSONc_createObject(), *sender, *textJSON;
	bool error = false;
	int x;
	sscanf(buffer, "%*s%*s%s%s", channelName, authToken);
	channelName[strlen(channelName)-1] = 0;
	memset(fileName, 0, sizeof(fileName));
	name2JSONFileName(channelName, fileName);
	x = indexOfThisToken(authToken);
	if (x == -1)
	{
		error = true;
		type = JSONc_createString("Error");
		content = JSONc_createString("Authentication token is not available.");
	}
	if (isChannelExist(fileName))
	{
		error = true;
		type = JSONc_createString("Error");
		content = JSONc_createString("Channel name is not available.");
	}
	if (!error)
	{
		strcpy(onlineUsers[x].channelName, channelName);
		sender = JSONc_createString("server");
		memset(text, 0, sizeof(text));
		strcat(text, onlineUsers[x].username);
		strcat(text, " created ");
		strcat(text, channelName);
		textJSON = JSONc_createString(text);
		JSONc_addItem2Object(message, "sender", sender);
		JSONc_addItem2Object(message, "content", textJSON);
		messages = JSONc_createArray();
		JSONc_addItem2Array(messages, message);
		name = JSONc_createString(channelName);
		JSONc_addItem2Object(data, "messages", messages);
		JSONc_addItem2Object(data, "name", name);
		memset(fileContent, 0, sizeof(fileContent));
		strcat(fileContent, JSONc_printUnformatted(data));
		memset(fileAddress, 0, sizeof(fileAddress));
		channelName2FileAddress(fileName, fileAddress);
		FILE *channelJSON = fopen(fileAddress, "w");
		fprintf(channelJSON, "%s\n", fileContent);
		fclose(channelJSON);
		type = JSONc_createString("Successful");
		content = JSONc_createString("");
	}
	JSONc_addItem2Object(respond, "type", type);
	JSONc_addItem2Object(respond, "content", content);
	memset(buffer, 0, sizeof(buffer));
	strcat(buffer, JSONc_printUnformatted(respond));
}

// Function for add user to a channel
void joinChannel()
{
	char channelName[MAX], authToken[MAX], fileName[MAX], fileAddress[MAX],
	fileContent[MAX], text[MAX];
	JSONc *respond = JSONc_createObject(), *type, *content,
	*data, *messages, *name,
	*message = JSONc_createObject(), *sender, *textJSON;
	bool error = false;
	int x;
	sscanf(buffer, "%*s%*s%s%s", channelName, authToken);
	channelName[strlen(channelName)-1] = 0;
	memset(fileName, 0, sizeof(fileName));
	name2JSONFileName(channelName, fileName);
	x = indexOfThisToken(authToken);
	if (x == -1)
	{
		error = true;
		type = JSONc_createString("Error");
		content = JSONc_createString("Authentication token is not valid.");
	}
	else if (!isChannelExist(fileName))
	{
		error = true;
		type = JSONc_createString("Error");
		content = JSONc_createString("Channel name is not exist.");
	}
	if (!error)
	{
		//Send a joined massage in channel
		sender = JSONc_createString("server");
		memset(text, 0, sizeof(text));
		strcat(text, onlineUsers[x].username);
		strcat(text, " joined channel");
		memset(fileAddress, 0, sizeof(fileAddress));
		channelName2FileAddress(fileName, fileAddress);
		FILE *channelJSON = fopen(fileAddress, "r+");
		fscanf(channelJSON, "%[^\nEOF]", fileContent);
		data = JSONc_parse(fileContent);
		name = JSONc_getObjectItem(data, "name");
		messages = JSONc_getObjectItem(data, "messages");
		sender = JSONc_createString("server");
		textJSON = JSONc_createString(text);
		JSONc_addItem2Object(message, "sender", sender);
		JSONc_addItem2Object(message, "content", textJSON);
		JSONc_addItem2Array(messages, message);
		channelJSON = freopen(fileAddress, "w", channelJSON);
		fprintf(channelJSON, "%s\n", JSONc_printUnformatted(data));
		fclose(channelJSON);
		// End of joined message
		memset(onlineUsers[x].channelName, 0, sizeof(onlineUsers[x].channelName));
		strcat(onlineUsers[x].channelName, channelName);
		type = JSONc_createString("Successful");
		content = JSONc_createString("");
	}
	JSONc_addItem2Object(respond, "type", type);
	JSONc_addItem2Object(respond, "content", content);
	memset(buffer, 0, sizeof(buffer));
	strcat(buffer, JSONc_printUnformatted(respond));
}

// Function for send a message to channel
void sendMessage()
{
	char messageText[MAX], authToken[MAX], savedString[MAX],
	fileName[MAX], fileAddress[MAX];
	JSONc *respond = JSONc_createObject(), *type, *content,
	*data, *messages, *name,
	*message = JSONc_createObject(), *sender, *messageContent;
	bool error = false;
	int x, i = 0, j = 0;
	while ((messageText[i] = buffer[5 + i]) != ',') i++;
	messageText[i] = 0;
    i+=2;
	while ((authToken[j] = buffer[5 + i]) != '\n') i++, j++;
	authToken[j] = 0;
	printf("%s - %s\n", authToken, messageText);
	x = indexOfThisToken(authToken);
	if (x == -1)
	{
		error = true;
		type = JSONc_createString("Error");
		content = JSONc_createString("Authentication token is not valid.");
	}
	else if (!strcmp(onlineUsers[x].channelName, ""))
	{
		error = true;
		type = JSONc_createString("Error");
		content = JSONc_createString("You are not in a channel.");
	}
	if (!error)
	{
		memset(fileName, 0, sizeof(fileName));
		name2JSONFileName(onlineUsers[x].channelName, fileName);
		memset(fileAddress, 0, sizeof(fileAddress));
		channelName2FileAddress(fileName, fileAddress);
		FILE *channleJSON = fopen(fileAddress, "r+");
		fscanf(channleJSON, "%[^\n]", savedString);
		data = JSONc_parse(savedString);
		messages = JSONc_getObjectItem(data, "messages");
		name = JSONc_getObjectItem(data, "name");
		sender = JSONc_createString(onlineUsers[x].username);
		messageContent = JSONc_createString(messageText);
		JSONc_addItem2Object(message, "sender", sender);
		JSONc_addItem2Object(message, "content", messageContent);
		JSONc_addItem2Array(messages, message);
		rewind(channleJSON);
		fprintf(channleJSON, "%s\n", JSONc_printUnformatted(data));
		fclose(channleJSON);
		type = JSONc_createString("Successful");
		content = JSONc_createString("");
	}
	JSONc_addItem2Object(respond, "type", type);
	JSONc_addItem2Object(respond, "content", content);
	memset(buffer, 0, sizeof(buffer));
	strcat(buffer, JSONc_printUnformatted(respond));
}

// Function that put user unread messages in buffer
void refresh()
{	
	char channelName[MAX], authToken[MAX], fileName[MAX], fileAddress[MAX],
	fileContent[MAX];
	JSONc *respond = JSONc_createObject(), *type, *content,
	*data, *messages, *message;
	bool error = false;
	int x;
	sscanf(buffer, "%*s%s", authToken);
	x = indexOfThisToken(authToken);
	if (x != -1)
		strcpy(channelName, onlineUsers[x].channelName);
	memset(fileName, 0, sizeof(fileName));
	name2JSONFileName(channelName, fileName);
	if (x == -1)
	{
		error = true;
		type = JSONc_createString("Error");
		content = JSONc_createString("Authentication token is not valid.");
	}
	else if (!isChannelExist(fileName))
	{
		error = true;
		type = JSONc_createString("Error");
		content = JSONc_createString("Channel name is not exist.");
	}
	if (!error)
	{
		type = JSONc_createString("List");
		memset(fileAddress, 0, sizeof(fileAddress));
		channelName2FileAddress(fileName, fileAddress);
		FILE *channelJSON = fopen(fileAddress, "r");
		fscanf(channelJSON, "%[^\n]", fileContent);
		data = JSONc_parse(fileContent);
		messages = JSONc_getObjectItem(data, "messages");
		int messagesCount = JSONc_getArraySize(messages);
		content = JSONc_createArray();
		for (int i = onlineUsers[x].seenMessageCount; i < messagesCount; i++)
		{
			message = JSONc_getArrayItem(messages, i);
			JSONc *sender = JSONc_getObjectItem(message, "sender"),
			*messageContent = JSONc_getObjectItem(message, "content"),
			*senderUnseen = JSONc_createString(sender -> stringVal),
			*messageContentUnseen = JSONc_createString(messageContent -> stringVal),
			*messageUnseen = JSONc_createObject();
			JSONc_addItem2Object(messageUnseen, "sender", senderUnseen);
			JSONc_addItem2Object(messageUnseen, "content", messageContentUnseen);
			JSONc_addItem2Array(content, messageUnseen);
			onlineUsers[x].seenMessageCount++;
		}
		fclose(channelJSON);
	}
	JSONc_addItem2Object(respond, "type", type);
	JSONc_addItem2Object(respond, "content", content);
	memset(buffer, 0, sizeof(buffer));
	strcat(buffer, JSONc_printUnformatted(respond));	
}

// Function that put channel members array in buffer
void channelMembers()
{
	char authToken[MAX];
	JSONc *respond = JSONc_createObject(), *type, *content, *member;
	int x;
	bool error = false;
	sscanf(buffer, "%*s%*s%s", authToken);
	x = indexOfThisToken(authToken);
	if (x == -1)
	{
		type = JSONc_createString("Error");
		content = JSONc_createString("Authentication token is not valid.");
		error = true;
	}
	if (!error)
	{
		type = JSONc_createString("List");
		content = JSONc_createArray();
		for (int i = 0; i < MAXUSERS; i++)
		{
			if (onlineUsers[i].online && !strcmp(onlineUsers[x].channelName, onlineUsers[i].channelName))
			{
				member = JSONc_createString(onlineUsers[i].username);
				JSONc_addItem2Array(content, member);
			}
		}
	}
	JSONc_addItem2Object(respond, "type", type);
	JSONc_addItem2Object(respond, "content", content);
	memset(buffer, 0, sizeof(buffer));
	strcat(buffer, JSONc_printUnformatted(respond));
}

// Function that remove user from channel
void leaveChannel()
{
	char channelName[MAX], authToken[MAX], fileName[MAX], fileAddress[MAX],
	fileContent[MAX], text[MAX];
	JSONc *respond = JSONc_createObject(), *type, *content,
	*data, *messages, *name,
	*message = JSONc_createObject(), *sender, *textJSON;
	bool error = false;
	int x;
	sscanf(buffer, "%*s%s", authToken);
	x = indexOfThisToken(authToken);
	if (x != -1)
		strcpy(channelName, onlineUsers[x].channelName);
	memset(fileName, 0, sizeof(fileName));
	name2JSONFileName(channelName, fileName);
	if (x == -1)
	{
		error = true;
		type = JSONc_createString("Error");
		content = JSONc_createString("Authentication token is not valid.");
	}
	else if (!isChannelExist(fileName))
	{
		error = true;
		type = JSONc_createString("Error");
		content = JSONc_createString("Channel name is not exist.");
	}
	if (!error)
	{
		//Send a leaved massage in channel
		sender = JSONc_createString("server");
		memset(text, 0, sizeof(text));
		strcat(text, onlineUsers[x].username);
		strcat(text, " leaved channel");
		memset(fileAddress, 0, sizeof(fileAddress));
		channelName2FileAddress(fileName, fileAddress);
		FILE *channelJSON = fopen(fileAddress, "r+");
		fscanf(channelJSON, "%[^\n]", fileContent);
		data = JSONc_parse(fileContent);
		name = JSONc_getObjectItem(data, "name");
		messages = JSONc_getObjectItem(data, "messages");
		sender = JSONc_createString("server");
		textJSON = JSONc_createString(text);
		JSONc_addItem2Object(message, "sender", sender);
		JSONc_addItem2Object(message, "content", textJSON);
		JSONc_addItem2Array(messages, message);
		channelJSON = freopen(fileAddress, "w", channelJSON);
		fprintf(channelJSON, "%s\n", JSONc_printUnformatted(data));
		fclose(channelJSON);
		// End of leaved message
		memset(onlineUsers[x].channelName, 0, sizeof(onlineUsers[x].channelName));
		strcat(onlineUsers[x].channelName, "");
		type = JSONc_createString("Successful");
		content = JSONc_createString("");
	}
	JSONc_addItem2Object(respond, "type", type);
	JSONc_addItem2Object(respond, "content", content);
	memset(buffer, 0, sizeof(buffer));
	strcat(buffer, JSONc_printUnformatted(respond));	
}

// Function that find index of an online user by online user authentication token
int indexOfThisToken(char *token)
{
	for(int i = 0; i < MAXUSERS; i++)
		if (onlineUsers[i].online && !strcmp(token, onlineUsers[i].authToken))
			return i;
	return -1;
}

// Function for search among usernames in channel
void searchMembers()
{
	char keyUsername[MAX], authToken[MAX];
	int x;
	JSONc *respond = JSONc_createObject(), *type, *content; 
	bool error = false, found = false;
	sscanf(buffer, "%*s%*s%s%s", keyUsername, authToken);
	keyUsername[strlen(keyUsername) - 1] = 0;
	x = indexOfThisToken(authToken);
	if (x == -1)
	{
		type = JSONc_createString("Error");
		content = JSONc_createString("Authentication token is not valid.");
		error = true;
	}
	else
	{
		type = JSONc_createString("Successful");
		for (int i = 0; i < MAXUSERS; i++)
			if (onlineUsers[i].online && !strcmp(onlineUsers[x].channelName, onlineUsers[i].channelName))
			{
				if (i != x)
				{
					found = true;
					break;
				}	
			}
		if (found)
			content = JSONc_createString("True");
		else
			content = JSONc_createString("False");
	}
	JSONc_addItem2Object(respond, "type", type);
	JSONc_addItem2Object(respond, "content", content);
	memset(buffer, 0, sizeof(buffer));
	strcat(buffer, JSONc_printUnformatted(respond));
}
