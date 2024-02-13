# DND Client Server Implementation

## Overview
This project implements a Do Not Disturb (DND) service where subscribers can block or stop unwanted calls. The service allows subscribers to activate the DND service either at the global level or at the selective level. When activated globally, the subscriber will not receive any calls. When activated selectively, the subscriber can specify certain numbers or groups from which calls will be blocked. Upon activation or deactivation of the DND service, the subscriber will receive confirmation messages.

## Features
- Activation of DND service
- Deactivation of DND service
- Global DND activation (blocks all calls)
- Selective DND activation (blocks calls from specified numbers or groups)
- Confirmation messages upon activation or deactivation

## Components
The project consists of two main components:
1. **Server**: Handles incoming client requests, manages user information, and activates the DND service based on client requests.
2. **Client**: Allows subscribers to connect to the server, enter their preferences for DND activation, and receive confirmation messages.

## Usage
1. **Server**: Compile and run the `server.c` file. The server will listen for incoming connections on the specified port.
2. **Client**: Compile and run the `client.c` file. Connect to the server using the specified IP address and port. Follow the prompts to activate or deactivate the DND service.

## Installation
1. Clone the repository:
    ```bash
    git clone https://github.com/your_username/dnd-client-server.git
    ```
2. Compile the server and client files:
    ```bash
    gcc server.c -o server
    gcc client.c -o client
    ```
3. Run the server:
    ```bash
    ./server
    ```
4. Run the client:
    ```bash
    ./client
    ```

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contributing
Contributions are welcome! Please feel free to open an issue or submit a pull request.

## Contact
For questions or feedback, please contact [Your Name](mailto:your_email@example.com).
