#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h> // not valid in windows, need to test in linux


void fork_pattern_1(int n)
{
    pid_t pids[n];
    for (int i = 1; i <= n; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            srand(time(NULL) ^ (getpid() << 16));
            int sleep_time = rand() % 8 + 1;
            printf("Process %d (PID %d) beginning, will sleep for %d seconds\n", i, getpid(), sleep_time);
            sleep(sleep_time);
            printf("Process %d (PID %d) exiting\n", i, getpid());
            exit(0);
        }
        else
        {
            pids[i-1] = pid;
        }
    }
    for (int i = 0; i < n; i++)
    {
        waitpid(pids[i], NULL, 0);
    }
}


void fork_pattern_2(int n)
{
    for (int i = 1; i <= n; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            srand(time(NULL) ^ (getpid() << 16));
            int sleep_time = rand() % 8 + 1;
            printf("Process %d (PID %d) beginning, will sleep for %d seconds\n", i, getpid(), sleep_time);
            if (i < n)
            {
                printf("Process %d (PID %d) creating Process %d\n", i, getpid(), i + 1);
            }
            sleep(sleep_time);
            printf("Process %d (PID %d) exiting\n", i, getpid());
            exit(0);
        }
        else
        {
            waitpid(pid, NULL, 0);
        }
    }
}

void fork_pattern_3(int level, int max_level) // Error ridden, need to fix
{
    if (level > max_level) return;

    pid_t pid1 = fork();
    if (pid1 == 0)
    {
        srand(time(NULL) ^ (getpid() << 16) ^ (level << 8));
        int sleep_time = rand() % 8 + 1;
        printf("Process %d (PID %d, PPID %d) beginning, will sleep for %d seconds\n", 2 * level - 1, getpid(), getppid(), sleep_time);
        fork_pattern_3(level + 1, max_level);
        sleep(sleep_time);
        printf("Process %d (PID %d, PPID %d) exiting\n", 2 * level - 1, getpid(), getppid());
        exit(0);
    }
    else
    {
        printf("Process %d (PID %d) creating left child (PID %d)\n", 2 * level - 1, getpid(), pid1);
        pid_t pid2 = fork();
        if (pid2 == 0)
        {
            srand(time(NULL) ^ (getpid() << 16) ^ (level << 8));
            int sleep_time = rand() % 8 + 1;
            printf("Process %d (PID %d, PPID %d) beginning, will sleep for %d seconds\n", 2 * level, getpid(), getppid(), sleep_time);
            fork_pattern_3(level + 1, max_level);
            sleep(sleep_time);
            printf("Process %d (PID %d, PPID %d) exiting\n", 2 * level, getpid(), getppid());
            exit(0);
        }
        else
        {
            printf("Process %d (PID %d) creating right child (PID %d)\n", 2 * level, getpid(), pid2);
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        }
    }
}


int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <number of things> <pattern number>\n", argv[0]);
        exit(1);
    }

    int numberOfThings = atoi(argv[1]);
    int pattern = atoi(argv[2]);

    if (numberOfThings < 1 || numberOfThings > 256)
    {
        printf("Number of things must be between 1 and 256\n");
        exit(1);
    }

    srand(time(NULL));

    switch (pattern)
    {
        case 1:
            fork_pattern_1(numberOfThings);
            break;
        case 2:
            fork_pattern_2(numberOfThings);
            break;
        case 3:
            fork_pattern_3(1, numberOfThings / 2);
            break;
        default:
            printf("Invalid pattern number. Must be 1, 2, or 3.\n");
            exit(1);
    }

    return 0;
}
