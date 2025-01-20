# Communication via Memory-Mapped Files

This project demonstrates an application that enables communication between multiple instances using memory-mapped files. Each instance can send messages provided by the user to other instances through a shared memory-mapped file. All instances are notified of the existence of a new message, which they retrieve and display in real time.

## Features

-> Message Broadcasting: Allows user-inputted messages to be transmitted from one instance to all others;
-> Memory-Mapped File Mechanism: Utilizes memory-mapped files for efficient inter-process communication (IPC);
-> Real-Time Updates: All instances receive and display messages instantly after they are sent;
-> Multi-Instance Support: Scalable to accommodate multiple running instances simultaneously.

## Technologies Used

Programming Language: [Specify language, e.g., Python, C++];
Memory-Mapped Files: For IPC and shared memory utilization;
Concurrency: Thread or process-based notification system.

## How It Works

Each application instance creates or connects to a shared memory-mapped file;
When a user enters a message in one instance, it is written to the shared file;
A notification mechanism alerts all other instances of the new message;
The instances read the message from the memory-mapped file and display it to the user.

### Setup and Usage

1. Clone the repository:
   ```bash
   git clone https://github.com/andricolae/sender-receiver-memory-map-C.git

2. Navigate to the project directory:
   ```bash
   cd proiect

3. Run multiple instances
   ```bash
   cd proiect\x64\Debug

4. Open in terminal and launch as many instances as you want
   ```bash
   .\proiect.exe

Follow the instructions and type a message in one instance and observe it being broadcasted to others in real-time.
