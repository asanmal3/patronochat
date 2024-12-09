#include "server.h"
#include <sys/stat.h>

void init_uchat_daemon(void) 
{
    pid_t pid = 0;  // Declare a variable to store the process ID
    pid_t sid = 0;  // Declare a variable to store the session ID

    pid = fork();  // Fork the process to create a child process
    if (pid < 0)  // If fork fails, exit the program
    {
        exit(1);
    }
    if (pid > 0)  // In the parent process
    {
        printf("<Uchat server started>\n");  // Print server start message
        printf("Child process ID: %d \n", pid);  // Print the child process ID
        exit(0);  // Exit the parent process, allowing the child to run as daemon
    }
    
    umask(0);  // Set the file mode creation mask to 0, ensuring the child can create files with the desired permissions
    sid = setsid();  // Create a new session and become the session leader
    if (sid < 0)  // If creating a new session fails, exit the program
    {
        exit(1);
    }

    // Close the standard file descriptors (stdin, stdout, stderr) to detach from the terminal
    close(STDIN_FILENO);  // Close the standard input
    close(STDOUT_FILENO);  // Close the standard output
    close(STDERR_FILENO);  // Close the standard error
}
