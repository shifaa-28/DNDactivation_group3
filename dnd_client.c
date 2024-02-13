 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h> // Include for isdigit()
#define PORT 8925
// Function prototypes
void displayMenu();
void activateDND(int clientSocket);
bool isValidPhoneNumber(const char *number);
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
int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    // Create socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    // Set up server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP address of the server
    serverAddr.sin_port = htons(PORT);
    // Connect to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    char number[15];
    printf("Enter your number: ");
    scanf("%14s", number); // Limit input to 14 characters to prevent buffer overflow
    // Validate the input number
    if (!isValidPhoneNumber(number)) {
        debugLog(FATAL, "Invalid number.");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }
    // Send client's number to the server
    send(clientSocket, number, sizeof(number), 0);
    debugLog(INFO, "Sent client's number to the server.");
    printf("Welcome to DND Service!\n");
    displayMenu();
    int choice;
    scanf("%d", &choice);
    // Send client's choice to the server
    send(clientSocket, &choice, sizeof(choice), 0);
    debugLog(INFO, "Sent client's choice to the server.");
    switch (choice) {
        case 1:
            // Activate DND
            activateDND(clientSocket);
            break;
        case 2:
            // Deactivate DND
            printf("Deactivation message sent.\n");
            break;
        default:
            printf("Invalid choice.\n");
            break;
    }
    close(clientSocket);
    return 0;
}
void displayMenu() {
    printf("1. Activation\n");
    printf("2. Deactivation\n");
    printf("Enter your choice: ");
}
void activateDND(int clientSocket) {
    int choice;
    printf("Enter your choice for activation (1 for global, 2 for selective):\n");
    scanf("%d", &choice);
    // Send client's choice for activation to the server
    send(clientSocket, &choice, sizeof(choice), 0);
    debugLog(INFO, "Sent client's choice for activation to the server.");
    switch (choice) {
        case 1:
            printf("Global DND activated.\n");
            break;
        case 2:
            printf("Selective DND activated.\n");
            // Enter the number of numbers to block
            int num_to_block;
            printf("Enter the number of numbers you want to block: ");
            scanf("%d", &num_to_block);
            // Send the number of numbers to block to the server
            send(clientSocket, &num_to_block, sizeof(num_to_block), 0);
            debugLog(INFO, "Sent the number of numbers to block to the server.");
            // Enter the numbers to block
            char buffer[15];
            for (int i = 0; i < num_to_block; ++i) {
                printf("Enter number to block: ");
                scanf("%14s", buffer); // Limit input to 14 characters to prevent buffer overflow
                // Validate the input number
                if (!isValidPhoneNumber(buffer)) {
                    debugLog(FATAL, "Invalid number.");
                    close(clientSocket);
                    exit(EXIT_FAILURE);
                }
                // Send each number to block to the server
                send(clientSocket, buffer, sizeof(buffer), 0);
                debugLog(INFO, "Sent a number to block to the server.");
            }
            printf("Blocked numbers sent to server.\n");
            break;
        default:
            printf("Invalid choice for activation.\n");
            break;
    }
}
bool isValidPhoneNumber(const char *number) {
    int len = strlen(number);
    if (len != 10)
        return false;
    for (int i = 0; i < len; i++) {
        if (!isdigit(number[i]))
            return false;
    }
    return true;
}