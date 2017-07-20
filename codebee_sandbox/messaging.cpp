#include <iostream>
#include <chrono>
#include <mutex>

#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#ifdef __linux__
    #include <sys/prctl.h>
#endif
#include <signal.h>

#include "messaging.h"

/* pipe[] is a list of file descriptors
   pipe[0] is for read and
   pipe[1] is for write */
#define READ_FD  0
#define WRITE_FD 1

using namespace std;

// Mutex to only allow one function to output at a time
mutex cerrMutex;

void Messaging::sendString(int id, string sendString) {
    // Append newline at the end of message
    sendString += '\n';

    UnixConnection connection = connections.at(id);
    // Assumes each character is 1 byte
    ssize_t charsWritten = write(connection.write, sendString.c_str(), sendString.length());
    if (charsWritten < sendString.length()) {
        cerr << "Complete string not written to pipe" << endl;
        throw 1;
    }
}

string Messaging::getString(int id) {
    // Currently using a constant 5 second timeout for request
    string curString;
    int timeRemaining = timeoutMilliseconds;
    auto timePoint = chrono::high_resolution_clock::now();

    UnixConnection connection = connections.at(id);
    char buffer;

    // Create set of buffers to check if bytes exist
    fd_set set;
    FD_ZERO(&set); // Clear the set
    FD_SET(connection.read, &set); // Add read file descriptor to set

    while(true) {
        timeRemaining = timeoutMilliseconds - chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - timePoint).count();

        if (timeRemaining < 0) {
            cerr << "Request timeout during getString for " << id << endl;
            throw 1;
        }

        // Check if there are bytes in the pipe
        // Timeout struct with seconds and milliseconds
        struct timeval timeout;
        timeout.tv_sec = timeRemaining / 1000.0;
        timeout.tv_usec = (timeRemaining % 1000) * 1000;
        int numNonEmptyPipes = select(connection.read+1, &set, NULL, NULL, &timeout);

        if(numNonEmptyPipes > 0) {
            // Assumes each character is 1 byte
            read(connection.read, &buffer, 1);

            if (buffer == '\n') break;
            else curString += buffer;
        } else {
            string errorMessage = "Bot # " + to_string(id) + " timeout or error";
            lock_guard<mutex> guard(cerrMutex);
            cerr << errorMessage << endl;
            throw 1;
        }
    }

    // Remove carraige return from string if it exists
    if(curString.back() == '\r') curString.pop_back();

    return curString;
}

void Messaging::startAndConnectBot(string botName) {
    // Pipes for parent to write and read
    int writePipe[2];
    int readPipe[2];

    if(pipe(writePipe)) {
        cerr << "Error creating write pipe" << endl;
        throw 1;
    }

    if(pipe(readPipe)) {
        cerr << "Error creating read pipe" << endl;
        throw 1;
    }

    pid_t parent_pid_before_fork = getpid();

    // Fork a child process
    pid_t pid = fork();
    if(pid == 0) {
        // This is the child process

        // Set child process group id as the itself instead of the parent
        // This is to prevent parent process from dying in case child process is killed
        setpgid(getpid(), getpid());


#ifdef __linux__
        // Install a parent death signal
        int status = prctl(PR_SET_PDEATHSIG, SIGTERM);
        if (status == -1) {
            cout << "Error installing parent death signal" << endl;
            throw 1;
        }

        // Test in case original parent process exited before prctl call.
        if (getppid() != parent_pid_before_fork)
            exit(1);
#endif

        // Set child processes stdin to point to parents cout read pipe
        dup2(writePipe[READ_FD], STDIN_FILENO);

        // Set child processes stdout to point to parents cin write pipe
        dup2(readPipe[WRITE_FD], STDOUT_FILENO);
        dup2(readPipe[WRITE_FD], STDERR_FILENO);

        // Execute bot and exit
        execl("/bin/sh", "sh", "-c", botName.c_str(), (char*) NULL);

        exit(1);
    } else if (pid < 0) {
        cerr << "Fork failed" << endl;
        throw 1;
    }

    UnixConnection connection;
    connection.read = readPipe[READ_FD];
    connection.write = writePipe[WRITE_FD];

    connections.push_back(connection);
    processes.push_back(pid);
}

int Messaging::handleInitialMessaging(int id, string initialMap) {
    string response;
    string playerId = to_string(id);

    try {
        sendString(id, playerId);
        sendString(id, initialMap);

        cout << "Init message send to player " << playerId << endl;

        response = getString(id);

        cout << "Init message received from player " << playerId << " " << response << endl;

        return 0;
    } catch(...) {
        cerr << "Bot # " << playerId << " errored during initial messaging" << endl;
    }

    return -1;
}

int Messaging::handleTurnMessaging(int id, string currInsects, string *actions) {
    string response;

    try {
        if(isProcessDead(id)) {
            actions->clear();
            *actions = "[]";
            return 0;
        }

        sendString(id, currInsects);

        cout << "Current map sent to bot" << endl;

        response = getString(id);

        cout << "Turn moves retrieved from bot" << endl;

        *actions = response;

        return 0;
    } catch(...) {
        cerr << "Bot # " << to_string(id) << " errored during turn messaging" << endl;

        actions->clear();
        *actions = "[]";
    }

    return -1;
}

void Messaging::killPlayer(int id) {
    if(isProcessDead(id)) return;

    // TODO: get leftover contents of pipe for bot

    kill(processes[id], SIGKILL);

    processes[id] = -1;
    connections[id].read = -1;
    connections[id].write = -1;
}

bool Messaging::isProcessDead(int id) {
    return processes[id] == -1;
}
