/********************************************************************************************
This is a template for assignment on writing a custom Shell. 

Students may change the return types and arguments of the functions given in this template,
but do not change the names of these functions.

Though use of any extra functions is not recommended, students may use new functions if they need to, 
but that should not make code unnecessorily complex to read.

Students should keep names of declared variable (and any new functions) self explanatory,
and add proper comments for every logical step.

Students need to be careful while forking a new process (no unnecessory process creations) 
or while inserting the signal handler code (which should be added at the correct places).

Finally, keep your filename as myshell.c, do not change this name (not even myshell.cpp, 
as you dp not need to use any features for this assignment that are supported by C++ but not by C).
*********************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>			// exit()
#include <unistd.h>			// fork(), getpid(), exec()
#include <sys/wait.h>		// wait()
#include <signal.h>			// signal()
#include <fcntl.h>			// close(), open()
#define MAXCOM 32 	// max number of letters to be supported 
#define MAXLIST 100 	// max number of commands to be supported 

void parseInput(char* rawCommand, char** parsedCommand,char*delim)
{
	// This function will parse the input string into multiple commands or a single command with arguments depending on the delimiter (&&, ##, >, or spaces).
	
    int i; 
  
    for (i = 0; i < MAXLIST; i++) { 

        parsedCommand[i] = strsep(&rawCommand,delim); 
		//printf("\ncmd[%d]=%s",i,parsedCommand[i]);
        if (parsedCommand[i] == NULL)
		{ 
            break; 
		}
        if (strlen(parsedCommand[i]) == 0) 				//for handling more than 1 spaces
            i--; 
    } 
	if(parsedCommand[0]==NULL)							//for invalid comannds like ^A^Z
	{
		printf("\nShell: Incorrect command");
	}
}

void executeCommand(char** parsedCommand)
{
	if(parsedCommand[0]==NULL)
	{
		printf("\nShell: Incorrect command");
		exit(0);
	}
	else if(strcmp(parsedCommand[0],"cd")==0)
	{
		char newPWD[256];
		char*argument1=parsedCommand[1];
		int status;
		// Check whether argument1 is empty or not
		// If argument1 is empty then change directory to the directory given by the "HOME" environment variable
		// Else, change directory to the given directory (argument1) and print the appropriate error message if the given directory does not exist
		if(argument1==NULL || strlen(argument1)==0)
		{
			chdir(getenv("HOME"));
		}
		else{
			status = chdir(argument1);
			if (status != 0)
			printf("cd: %s: No such directory\n", argument1);
		}
	}
	else{
		// This function will fork a new process to execute a command
		int rc=fork();

		if(rc<0)
		{
			exit(0);      		//fork failed, exit
		}
		else if(rc==0)			//child (new) process
		{
			// Restore the default behavior for SIGINT and SIGTSTP signal
			signal(SIGINT, SIG_DFL);			
			signal(SIGTSTP,SIG_DFL);
			// -------- EXEC system call ---------
			
			if(execvp(parsedCommand[0], parsedCommand)<0)
			{
				printf("\nShell: Incorrect command");
				exit(0);
			}
			
			//exit(0);
			// -----------------------------------
		}
		else {                // parent process (rc holds child PID)
			int rc_wait = wait(NULL);
			return;
		}
	}
}

void executeParallelCommands()
{
	// This function will run multiple commands in parallel
}

void executeSequentialCommands()
{	
	// This function will run multiple commands in parallel
}

void executeCommandRedirection(char**parsedCommands)
{

	// This function will run a single command with output redirected to an output file specificed by user
	int rc = fork();
	
	if (rc < 0){			// fork failed; exit
		exit(0);
	}
	else if (rc == 0) {		// child (new) process

		// ------- Redirecting STDOUT --------
		
		close(STDOUT_FILENO);
		open(parsedCommands[1], O_CREAT | O_WRONLY | O_APPEND);

		// -----------------------------------
		
		execvp(parsedCommands[0],parsedCommands);

	} 
	else {              // parent process (rc holds child PID)
		int rc_wait = wait(NULL);
	}
}

int main()
{
	// Initial declarations
	char*command;
	char currWD[256];
	size_t commandsize=MAXCOM;
	int inputSize;
	char*parsedCommands[MAXLIST];

	// Ignore SIGINT and SIGTSTP signal
	signal(SIGINT, SIG_IGN);	
	signal(SIGTSTP,SIG_IGN);

	while(1)	// This loop will keep your shell running until user exits.
	{
		// Print the prompt in format - currentWorkingDirectory$
		getcwd(currWD,sizeof(currWD));
		printf("\n%s$",currWD);
		// accept input with 'getline()'
		command = (char *)malloc(commandsize * sizeof(char));			//32 bytes of storage are assigned to memory location buffer via the malloc() function
		if( command == NULL)											//to handle the condition when memory isn’t available
		{
			perror("Unable to allocate buffer");
			exit(1);
		}

		inputSize=getline(&command,&commandsize,stdin);
		command[inputSize-1]='\0';							//as getline takes '\n' also in the end of the string
		
		// Parse input with 'strsep()' for different symbols (&&, ##, >) and for spaces.
		parseInput(command,parsedCommands," "); 

		if(parsedCommands[0]==NULL)continue;	//for invalid inputs
		if(strcmp(parsedCommands[0],"exit")==0)	// When user uses exit command.
		{
			printf("Exiting shell...");
			exit(0);
			break;
		}
		
		//if(/*condition*///)
		//	executeParallelCommands();		// This function is invoked when user wants to run multiple commands in parallel (commands separated by &&)
		//else if(/*condition*/)
		//	executeSequentialCommands();	// This function is invoked when user wants to run multiple commands sequentially (commands separated by ##)
		//else 
		//if(/*condition*/)
		//	executeCommandRedirection(parsedCommands);	// This function is invoked when user wants redirect output of a single command to and output file specificed by user
		//else
			executeCommand(parsedCommands);		// This function is invoked when user wants to run a single commands
				
	}
	
	return 0;
}
