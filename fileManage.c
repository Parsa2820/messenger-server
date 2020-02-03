// Function that create Data/Channels and Data/Users folders
void createDataDirectories()
{
	mkdir("./Data", 0777);
	mkdir("./Data/Users", 0777);
	mkdir("./Data/Channels", 0777);
}

// Function that check is user json file exist or not
int isUserExist(char *fileName)
{
    struct dirent *de;
    DIR *dr = opendir("./Data/Users");
    while ((de = readdir(dr)) != NULL)
        if (!strcmp(de -> d_name, fileName))
        {
            closedir(dr);
            return 1;
        }
    closedir(dr);
    return 0;
}

// Function that check is channel json file exist or not
int isChannelExist(char *fileName)
{
    struct dirent *de;
    DIR *dr = opendir("./Data/Channels");
    while ((de = readdir(dr)) != NULL)
        if (!strcmp(de -> d_name, fileName))
        {
            closedir(dr);
            return 1;
        }
    closedir(dr);
    return 0;    
}

// Function that add .json postfix to a given name
void name2JSONFileName(const char *name, char *fileName)
{
	strcat(fileName, name);
	strcat(fileName, ".json");
}

// Function that add ./Data/Users/ before file name
void username2FileAddress(const char *fileName, char *fileAddress)
{
	strcat(fileAddress, "./Data/Users/");
	strcat(fileAddress, fileName);
}

// Function that add ./Data/Channels/ before file name
void channelName2FileAddress(const char *fileName, char *fileAddress)
{
    strcat(fileAddress, "./Data/Channels/");
    strcat(fileAddress, fileName);
}
