#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int some_global_var = 0;

int main()
{
    int some_local_var = 0;
    int ret1, ret2;

    ret1 = fork();

    if (ret1 == 0) {
        // First child process running here because fork returned zero
        printf("Child(%d) says: Hello!\n", getpid());

        printf("Child(%d) says: Global (%p) incremented to %d!\n", getpid(), &some_global_var, ++some_global_var );
        printf("Child(%d) says: Local (%p) incremented to %d!\n", getpid(), &some_local_var, ++some_local_var );

        printf("Child(%d) says: Done!\n", getpid());

        exit(0); // Quits the process immediately from any point in the program 
    }
    else {
        // Parent process running here because fork returned value non-zero.
        printf("Parent says: First child's id: %d!\n", ret1);

        ret2 = fork();

        if (ret2 == 0) {
            // Second child process running here because fork returned zero
            printf("Child(%d) says: Hello!\n", getpid());

            printf("Child(%d) says: Global (%p) incremented to %d!\n", getpid(), &some_global_var, ++some_global_var );
            printf("Child(%d) says: Local (%p) incremented to %d!\n", getpid(), &some_local_var, ++some_local_var );

            printf("Child(%d) says: Done!\n", getpid());

            exit(0); // Quits the process immediately from any point in the program 
        }
        else {
            // Parent process running here because fork returned value non-zero.
            printf("Parent says: Second child's id: %d!\n", ret2);

            // Wait until both children exit. NULL means I don't care about the child's exit code.
            int c1 = wait(NULL);
            printf("Parent says: First child %d is done!\n", c1);

            int c2 = wait(NULL);
            printf("Parent says: Second child %d is done and so am I!\n", c2);

            printf("Parent says: Global (%p) is %d!\n", &some_global_var, some_global_var);
            printf("Parent says: Local (%p) is %d!\n", &some_local_var, some_local_var);
        }
    }

    return 0;
}
