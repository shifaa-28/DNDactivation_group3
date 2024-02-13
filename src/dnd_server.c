#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
 
#define MAX_NUMBERS 10
#define MAX_GROUPS 5
#define PORT 8778
 
typedef struct {
    char numbers[MAX_NUMBERS][15];
    int num_numbers;
} DNDList;
 
typedef struct {
    char groups[MAX_GROUPS][20];
    int num_groups;
} DNDGroups;
 
typedef struct {
    char username[50];
    char password[50];
    char blocked_calls_filename[60];
} UserInfo;
 
// Debug log levels
#define FATAL 0
#define INFO 1
#define WARNING 2
#define DEBUG 3
 
// Function to print debug log messages
void debugLog(int level, const char *message) {
    switch (level) {
        case FATAL:
            fprintf(stderr, "[FATAL] %s\n", message);
            break;
        case INFO:
            printf("[INFO] %s\n", message);
            break;
        case WARNING:
            printf("[WARNING] %s\n", message);
            break;
        case DEBUG:
            printf("[DEBUG] %s\n", message);
            break;
        default:
            fprintf(stderr, "[UNKNOWN LEVEL] %s\n", message);
            break;
    }
}
 
// Function prototypes
bool isValidUser(char *number);
void handleClient(int clientSocket, UserInfo *userInfo, DNDList *blockedList, DNDGroups *blockedGroups);
 
int main() {
    UserInfo userInfo;
    DNDList blockedList = {.num_numbers = 0};
    DNDGroups blockedGroups = {.num_groups = 0};
 
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
 
    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
 
    // Set up server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);
 
    // Bind socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
 
    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
 
    debugLog(INFO, "Server listening on port 8778...");
 
    while (1) {
        // Accept incoming connection
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen)) == -1) {
            perror("Accept failed");
            continue;
        }
 
        debugLog(INFO, "Connection accepted.");
 
        // Handle client requests
        handleClient(clientSocket, &userInfo, &blockedList, &blockedGroups);
 
        close(clientSocket);
        debugLog(INFO, "Connection closed.");
    }
 
    close(serverSocket);
 
    return 0;
}
 
bool isValidUser(char *number) {
    FILE *mobileRecordsFile = fopen("MobileRecords.txt", "r");
    if (!mobileRecordsFile) {
        perror("Error opening MobileRecords.txt");
        exit(EXIT_FAILURE);
    }
 
    char line[50];
    char userNumber[15];
    while (fgets(line, sizeof(line), mobileRecordsFile)) {
        sscanf(line, " %[^,],", userNumber);
        if (strcmp(userNumber, number) == 0) {
            fclose(mobileRecordsFile);
            return true;
        }
    }
 
    fclose(mobileRecordsFile);
    return false;
}
 
void handleClient(int clientSocket, UserInfo *userInfo, DNDList *blockedList, DNDGroups *blockedGroups) {
    char buffer[1024];
    char number[15];
    int choice;
 
    // Send welcome message to the client
    snprintf(buffer, sizeof(buffer), "Welcome to the DND Service!\n");
    send(clientSocket, buffer, strlen(buffer), 0);
    debugLog(INFO, "Sent welcome message to the client.");
 
    // Receive client's mobile number
    recv(clientSocket, number, sizeof(number), 0);
 
    // Check if the user is valid based on the MobileRecords.txt
    if (!isValidUser(number)) {
        debugLog(FATAL, "Invalid user. Connection closed.");
        close(clientSocket);
        return;
    }
 
    // Continue with the DND service activation
    debugLog(INFO, "User is valid.");
 
    // Receive client's choice
    recv(clientSocket, &choice, sizeof(choice), 0);
 
    FILE *file;
    switch (choice) {
        case 1:
            // Activate DND
            debugLog(INFO, "Client activated DND service.");
            printf("Enter your choice for DND mode (1 for global, 2 for selective):\n");
            recv(clientSocket, &choice, sizeof(choice), 0);
            switch (choice) {
                case 1:
                    printf("Global DND activated for number: %s. Client will not receive any calls.\n", number);
                    // Store user's information in CSV
                    file = fopen("userServerInfo.csv", "a");
                    fprintf(file, "%s,G\n", number);
                    fclose(file);
                    break;
                case 2:
                    printf("Selective DND activated for number: %s. Calls from specified numbers/groups will be blocked.\n", number);
                    printf("Enter the number of numbers you want to block: ");
                    int num_to_block;
                    recv(clientSocket, &num_to_block, sizeof(num_to_block), 0);
                    // Store blocked numbers in array
                    char blockedNumbers[MAX_NUMBERS][15];
                    for (int i = 0; i < num_to_block; ++i) {
                        recv(clientSocket, buffer, sizeof(buffer), 0);
                        strcpy(blockedNumbers[i], buffer);
                    }
                    // Store all blocked numbers in CSV
                    file= fopen("userServerInfo.csv", "a");
                    fprintf(file, "%s,S", number);
                    for (int i = 0; i < num_to_block; ++i) {
                        fprintf(file, ",%s", blockedNumbers[i]);
                    }
                    fprintf(file, "\n");
                    fclose(file);
                    printf("Blocked %d numbers for number: %s.\n", num_to_block, number);
                    break;
                default:
                    debugLog(WARNING,"Invalid choice for DND mode.\n");
                    break;
            }
            break;
        default:
            debugLog(WARNING,"Invalid choice for activation.\n");
            break;
    }
}
