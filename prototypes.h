//Function for create & verify socket
void createSocket();
// Function for accept client request to connect to socket
void acceptClient();
// Function for get request from client 
void getRequest();
// Function to signup new user
void registerUser();
// Function that print current date and time to console
void printTime();
// Function for login
void loginUser();
// Function that make random 20 character authentication token
void makeToken(char *, char *);
// Function to logout a user
void logoutUser();
// Function to create new channel
void creatChannel();
// Function for add user to a channel
void joinChannel();
// Function for send a message to channel
void sendMessage();
// Function that put user unread messages in buffer
void refresh();
// Function that put channel members array in buffer
void channelMembers();
// Function that remove user from channel
void leaveChannel();
// Function that find index of an online user by online user authentication token
int indexOfThisToken(char *);
// Function for search among usernames in channel
void searchMembers();