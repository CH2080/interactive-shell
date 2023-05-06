#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int childPids[5];

void PrintArray(char **array, int arrayLen)
{
    for (int i = 0; i < arrayLen; i++)
    {
        printf("index: %d = %s\n", i, array[i]);
    }
}

void CheckForExit(char *string)
{
    if (strcmp(string, "exit") == 0)
    {
        printf("exiting...\n");
        exit(0);
    }
}

void ChangeDirectory(char cdToThis[48])
{
    char cwd[512]; // curent working directory
    char nwd[512]; // new working directory
    char slashCdToThis[49] = "/";

    strcat(slashCdToThis, cdToThis);

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd() error");
    }

    strcpy(nwd, cwd);
    strcat(nwd, slashCdToThis);

    if (chdir(nwd) != 0)
    {
        printf("Error: cd unsuccessful\n");
    }
}

void ShowPid()
{
    for (int i = 0; i < 5; i++)
    {
        if (childPids[i] == 0)
        {
            printf("N/A\n");
        }
        else
        {
            printf("%d\n", childPids[i]);
        }
    }
}

void AddToChildPids(int pid)
{
    for (int i = 4; i > 0; i--)
    {
        childPids[i] = childPids[i - 1];
    }
    childPids[0] = pid;
}

int main()
{

    while (1)
    {
        char userInput[80];
        char buffer[4096];
        char *tokens[10];
        int numOfTokens = 0;
        int pid;
        char cwd[512];

        // getting userInput
        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            perror("getcwd() error");
        }
        printf("\033[1;32m%s/$ \033[0m", cwd);
        if (!fgets(buffer, 4096, stdin))
        {
            return 1;
        }
        if (sscanf(buffer, "%[^\n]", userInput) != 1)
        {
            fprintf(stderr, "Error: Command could not be executedd \n");
            continue;
        }

        // parsing into tokens
        int i = 0;
        tokens[i] = strtok(userInput, " ");
        while (tokens[i] != NULL)
        {
            i++;
            tokens[i] = strtok(NULL, " ");
        }
        numOfTokens = i + 1;
        i = 0;

        // checking input
        if (strcmp(tokens[0], "exit") == 0)
        {
            printf("exiting...\n");
            exit(0);
        }
        else if (strcmp(tokens[0], "cd") == 0)
        {
            if (tokens[1] != NULL)
            {
                ChangeDirectory(tokens[1]);
            }
            else
            {
                printf("Error: Command could not be executed \n");
            }
            continue;
        }
        else if (strcmp(tokens[0], "showpid") == 0)
        {
            if (tokens[1] == NULL)
            {
                ShowPid();
            }
            else
            {
                printf("Error: Command could not be executed \n");
            }
            continue;
        }

        pid = fork();
        if (pid == 0)
        { // child
            if ((execvp(tokens[i], tokens)) == -1)
            {
                printf("Error: Command could not be executed \n");
            }
            return 1; // killing the child, error ocurred in execvp.
        }
        else
        { // parent
            AddToChildPids(pid);
            wait(NULL); // wait for child to terminate.
        }
    }

    return 0;
}