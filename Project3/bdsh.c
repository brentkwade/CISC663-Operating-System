#include "headers.h"                                                                            // declarations and prototypes
#include "utils.h"										 // utility functions	
#include "getusers.h"    									// watchuser functions      
#include "getstat.h"                                                                        
#include "piperedir.h"
int token_count;
/**
 * generic execute void, tries to execute built-in programs first, and everything else then.
 */
void handleUserCommand()
{
        if (checkBuiltInCommands() == 0) {
                launchJob(commandArgv, "STANDARD", 0, FOREGROUND);
        }
}

/**
 * built-in commands: exit, in, out, bg, fg, jobs, kill
 * returns 	1 if a built-in command is executed,
 * 			0 otherwise
 */
int checkBuiltInCommands()
{
	
	if ( (strchr(pipbuffer, '|') != NULL) || (strchr(pipbuffer, '&') != NULL) || (strchr(pipbuffer, '>') != NULL) || (strchr(pipbuffer, '<') != NULL) )
	{	
			
		pipredir();
		return 1;
	}
	else
	{
	if (strcmp("noclobber",commandArgv[0]) == 0){
		updateNoClobber();		
		return 1;	
	}
	if (strcmp("watchmail",commandArgv[0]) == 0){
		if (commandArgv[1] == NULL)
		{
			printf("R-Shell: Missing Parameters for watchmail\n");
		}
		if(commandArgv[1] != NULL && commandArgv[2] != NULL)
		{
			if (strcmp("off", commandArgv[2]) == 0)
			{
				deleteMailBox(commandArgv[1]);
				//return 1;//
			}
			else 
			{	
				strcpy(filename, commandArgv[1]);		
				createThread(filename);
			}
		}
		if (commandArgv[1] != NULL && commandArgv[2] == NULL)
		{
			filename = malloc(sizeof(commandArgv[1]));
			strcpy(filename, commandArgv[1]);					
			createThread(filename);
		}
		return 1;
	}
	if (strcmp("watchuser", commandArgv[0]) == 0) {
		if (commandArgv[1] == NULL)
		{
			printf("R-Shell: Missing Parameters for watchuser\n");	
		}
		if (commandArgv[1] != NULL && commandArgv[2] != NULL)
		{
			if (strcmp("off", commandArgv[2]) == 0)
			{
				Delete(commandArgv[1]);
			}
			else 
			{
				
				Insert(commandArgv[1]);
			}
		}
		if (commandArgv[1] != NULL && commandArgv[2] == NULL)
		{
			Insert(commandArgv[1]);
		}
		return 1;
	}
	if (strcmp("warnload", commandArgv[0]) == 0){
		if (commandArgv[1] == NULL)
		{
			printf("R-Shell: Missing Parameters for warnload\n");	
		}
		else
		{
			if (warnloadflag == 0)
				warnLoadThreadCreate(atof(commandArgv[1]));
			else
				updateThread(atof(commandArgv[1]));		
		}
		return 1;
	}
	if (strcmp("history", commandArgv[0]) == 0) {
		ReversePrint();
		return 1;
	}
	if (strcmp("alias", commandArgv[0]) == 0) {
		return 0;	
	}	
	if(strcmp("which", commandArgv[0]) == 0) {
		if (commandArgv[1] == NULL) {
			printf("R-Shell: Missing Parameters\n");
		}
		else {		
			printwhich();
		}
		return 1;
	}
	if(strcmp("where", commandArgv[0]) == 0) {
		if (commandArgv[1] == NULL) {
			printf("R-Shell: Missing Parameters\n");
		}
		else {		
			printwhere();
		}
		return 1;
	}
	if(strcmp("setenv", commandArgv[0]) == 0) {
		envset();
		return 1;
	}
	if(strcmp("printenv", commandArgv[0]) == 0) {
		envprint();
		return 1;			
	}
	if (strcmp("pid", commandArgv[0]) == 0) {
		printpid();
		return 1;	
	}
	if (strcmp("pwd", commandArgv[0]) == 0) {
		printpwd();
		return 1;
	}
	if (strcmp("prompt", commandArgv[0]) == 0) {
		promptCmd();
		return 1;
	}
        if (strcmp("exit", commandArgv[0]) == 0) {
                exit(EXIT_SUCCESS);
        }
        if (strcmp("cd", commandArgv[0]) == 0) {
                changeDirectory();
                return 1;
        }
        if (strcmp("kill", commandArgv[0]) == 0) {
                if (commandArgv[1] == NULL)
                        return 0;
                killJob(atoi(commandArgv[1]));
                return 1;
        }
	if (strcmp("ls", commandArgv[0]) == 0) {
		return 0;
	}
	if (strcmp("cd", commandArgv[0]) == 0) {
		return 0;	
	}
	}//end of else
        return 0;
}
/*
*	WHERE COMMAND
*/
void initwhich()
{	
	//token_count = 0;
	
}
void tcount()
{
	char *token = NULL;
	const char *path = getenv("PATH");
	/* PATH is something like "/usr/bin:/bin:/usr/sbin:/sbin" */
	
	char *copy = (char *)malloc(strlen(path) + 1);
	token_count = 0;	
	strcpy(copy, path);
	token = strtok(copy, ":");
	//puts(token);		
	token_count++;	
	while (token = strtok(NULL, ":")) {		
		//puts(token);		
		token_count++;		
	}
	//free(copy);
	copy = NULL;	
}
void printwhere()
{
	token_count = 0;
	char *token = NULL, *tokencat, *file = NULL;
	const char *path = getenv("PATH");
	tcount();
	char *tokenarr[token_count], *accessarr[token_count];	
	int j=0, a=0;
	file = malloc(sizeof(commandArgv[1]) + 3); 
	strcat(file,"/");	
	strcat(file,commandArgv[1]);
	puts(file);
	char *copy = (char *)malloc(strlen(path) + 1);
	
	strcpy(copy, path);
	token = strtok(copy, ":");
	
	while (j < token_count) {
		tokenarr[j] = malloc(sizeof(token));		
		strcpy(tokenarr[j], token);
		token = strtok(NULL, ":");
		j++;		
	}

	j = 0;
	while (j < token_count) {
		tokencat = malloc( sizeof(char*) * (strlen(tokenarr[j]) + strlen(file)) );
				
		strcpy(tokencat, tokenarr[j]);
		strcat(tokencat, file);
		puts(tokencat);
		accessarr[j] = malloc(sizeof(tokencat));
		strcpy(accessarr[j], tokencat);
		if( access( tokencat, F_OK ) == 0 )
		{
			accessarr[a] = malloc(sizeof(tokencat));
			strcpy(accessarr[a], tokencat);	
			puts(accessarr[a]);			
			a++;						
		}
		j++;
	}
	
	//printf("gcc found @: %s\t match count: %d\n", accessarr[0],a);
	//free(copy);
	//copy = NULL;

	
	printf("a = %d\n",a);
	if ( commandArgv[1] != NULL)
	{
		if (strcmp("which", commandArgv[0]) == 0) {
			printf("found %s @ %s\n", commandArgv[1], accessarr[0]);
		}
		else {
			int j = 0;
			printf("All occurence of %s @ \n",commandArgv[1]);
			while ( j < a) {
				printf("\t%s\n", accessarr[j]);
				j++;					
			}			
		}
	}
	else{
		printf("R-shell : Missing Parameter\n");
	}
	//free(copy);
	//copy = NULL;
}

/*
*	WHICH COMMAND
*/
void printwhich()
{
	printwhere();
}

/*
*	setenv command to export variable to environment
*/
void envset()
{
	int status=1;
	if(commandArgv[1] == NULL) {
		envprint();
	}
	else if(commandArgv[1] != NULL && commandArgv[2] == NULL) {
		status = setenv(commandArgv[1],NULL, 1);
		if (status == 0) printf("success!!!\n");	
	}
	else {
		status = setenv(commandArgv[1],commandArgv[2], 1);
		if (status == 0) printf("success!!!\n");
	}
	
}

/*
*	PRINTENV command to execute
*/
void envprint()
{
	int i = 0;
	while(environ[i]) {
  		printf("%s\n", environ[i++]);
	}
}


/*
*	PID of shell
*/
void printpid()
{
	printf("R-shell : %d\n", (int)RSH_PID);
}

/*
*
*/
void printpwd()
{
	fprintf(stdout,"%s\n",getcwd(currentDirectory, 1024));
}

/*
*
*	to get user input from the prompt again if no parameter to the prompt command is provided.
*
*/
void promptCmd()
{
	char* bufferPointer;
	char onechar, buff[BUFFER_MAX_LENGTH];
	int charcount,argsc=0;
	char* argsv[5];	
	prefix = NULL;
	onechar = '\0';
	charcount = 0;
	if(commandArgv[1] == NULL)
	{
		prefix = NULL;
		printf("Missing prompt parameter. Prompt reset.\n");
		
 		/*
		while ((onechar != '\n') && (charcount < BUFFER_MAX_LENGTH)) {
                	buff[charcount++] = onechar;
                	onechar = getchar();
        	}
        	buff[charcount] = 0x00;
		printf("%s: ",buff);
		bufferPointer = strtok(buff, " ");
		
        	while (bufferPointer != NULL) {                                               // while the pointer is not NULL
                	argsv[argsc] = bufferPointer;                   // populate commandArgv array
                	bufferPointer = strtok(NULL, " ");
                	argsc++;
        	}
		if (argsc > 1) {
			printf("Too many parameters");
		}		
		else {
			prefix = malloc( sizeof( argsv[0] ));
			strcpy(prefix, argsv[0]);
		}*/
	}
	else
	{
		prefix = malloc( sizeof( commandArgv[1] ));
		strcpy(prefix, commandArgv[1]);
	}
}




/**
 * executes a program redirecting STDIN or STDOUT if newDescriptor != STANDARD
 */
void executeCommand(char *command[], char *file, int newDescriptor,
                    int executionMode)
{
        int commandDescriptor;
        /**
         *  Set the STDIN/STDOUT channels of the new process.
         */
        if (newDescriptor == STDIN) {
                commandDescriptor = open(file, O_RDONLY, 0600);                                        // open file for read only (it's STDIN)
                dup2(commandDescriptor, STDIN_FILENO);
                close(commandDescriptor);
        }
        if (newDescriptor == STDOUT) {
                commandDescriptor = open(file, O_CREAT | O_TRUNC | O_WRONLY, 0600); // open (create) the file truncating it at 0, for write only
                dup2(commandDescriptor, STDOUT_FILENO);
                close(commandDescriptor);
        }
        if (execvp(*command, command) == -1)
                perror("R-shell(execvp)");
}

/**
 * forks a process and launches a program as child
 */
void launchJob(char *command[], char *file, int newDescriptor,
               int executionMode)
{
        pid_t pid;
        pid = fork();
        switch (pid) {
        case -1:
                perror("R-shell(fork)");
                exit(EXIT_FAILURE);
                break;
        case 0:
                /**
                 *  we set the handling for job control signals back to the default.
                 */
                signal(SIGINT, SIG_DFL);
                signal(SIGQUIT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);
                signal(SIGCHLD, &signalHandler_child);
                signal(SIGTTIN, SIG_DFL);
                usleep(20000);                                                             // fixes a synchronization bug. Needed for short commands like ls
                setpgrp();                                                                                     // make the child as new process group leader
                if (executionMode == FOREGROUND)
                        tcsetpgrp(RSH_TERMINAL, getpid());                                           // if we want the process to be in foreground
                if (executionMode == BACKGROUND)
                        printf("[%d] %d\n", ++numActiveJobs, (int) getpid());              // inform the user about the new job in bg

                executeCommand(command, file, newDescriptor, executionMode);

                exit(EXIT_SUCCESS);
                break;
        default:
                setpgid(pid, pid);                                                                        // we also make the child a new process group leader from here
                // to avoid race conditions
                jobsList = insertJob(pid, pid, *(command), file, (int) executionMode); // insert the job in the list

                t_job* job = getJob(pid, BY_PROCESS_ID);                             // and get it as job object

                if (executionMode == FOREGROUND) {
                        putJobForeground(job, FALSE);                                              // put the job in foreground (if desired)
                }
                if (executionMode == BACKGROUND)
                        putJobBackground(job, FALSE);                                             // put the job in background (if desired)
                break;
        }
}

/**
 * puts a job in foreground. If continueJob = TRUE, sends the process group
 * a SIGCONT signal to wake it up. After the job is waited successfully, it
 * restores the control of the terminal to the shell
 */
void putJobForeground(t_job* job, int continueJob)
{
        job->status = FOREGROUND;                                                   // set its status in the list as FOREGROUND
        tcsetpgrp(RSH_TERMINAL, job->pgid);                                 // give it the control of the terminal
        if (continueJob) {                                                                        // continue the job (if desired)
                if (kill(-job->pgid, SIGCONT) < 0)                                           // by sending it a SIGCONT signal
                        perror("kill (SIGCONT)");
        }

        waitJob(job);                                                                                 // wait for the job
        tcsetpgrp(RSH_TERMINAL, RSH_PGID);                              // give the shell control of the terminal
}

/**
 * puts a job in background, and sends the job a continue signal, if continueJob = TRUE
 * puts the shell in foreground
 */
void putJobBackground(t_job* job, int continueJob)
{
        if (job == NULL)
                return;

        if (continueJob && job->status != WAITING_INPUT)
                job->status = WAITING_INPUT;		// fixes another synchronization bug: if the child process launches
		// a SIGCHLD and is set to WAITING_INPUT before this point has been
        // reached, then it would be set to BACKGROUND again

        if (continueJob)                        // if desired, continue the job
                if (kill(-job->pgid, SIGCONT) < 0)
                        perror("kill (SIGCONT)");

        tcsetpgrp(RSH_TERMINAL, RSH_PGID);                             // paranoia: give the shell control of terminal
}

/**
 * waits for a job, blocking unless it has been suspended.
 * Deletes the job after it has been executed
 */
void waitJob(t_job* job)
{
        int terminationStatus;
        while (waitpid(job->pid, &terminationStatus, WNOHANG) == 0) {      // while there are child to be waited
                if (job->status == SUSPENDED)                              // exit if the job has been set to be stopped
                        return;
        }
        jobsList = delJob(job);                                            // delete the job
}

/**
 * kills a Job given its number
 */
void killJob(int jobId)
{
        //t_job *job = getJob(jobId, BY_JOB_ID);                                   // get the job from the list
        if (kill(jobId, SIGKILL) < 0)
		perror("couldn't kill");                                                               // send the job a SIGKILL signal
}

/**
 * changes current directory
 */
void changeDirectory()
{
        if (commandArgv[1] == NULL) {
                chdir(getenv("HOME"));                                                        // simulate a "cd" command
        } else {
                if (chdir(commandArgv[1]) == -1) {                                  // go to the new directory
                        printf(" %s: no such directory\n", commandArgv[1]);
                }
        }
}

/**
 * initializes variables and enables job control
 * NOTE: function substantially stolen by the very useful glibc manual:
 * http://www.gnu.org/software/libc/manual/html_node/Implementing-a-Shell.html
 */
void init()
{
        RSH_PID = getpid();                                                             // retrieve the pid of the shell
        RSH_TERMINAL = STDIN_FILENO;                                       // terminal = STDIN
        RSH_IS_INTERACTIVE = isatty(RSH_TERMINAL);            // the shell is interactive if STDIN is the terminal

        if (RSH_IS_INTERACTIVE) {                                                 // is the shell interactive?
                while (tcgetpgrp(RSH_TERMINAL) != (RSH_PGID = getpgrp()))
                        kill(RSH_PID, SIGTTIN);                                                    // make sure we are in the foreground

                /**
                 * ignore all the job control stop signals and install custom signal handlers
                 */
                signal(SIGQUIT, SIG_IGN);
                signal(SIGTTOU, SIG_IGN);
                signal(SIGTTIN, SIG_IGN);
                signal(SIGTSTP, SIG_IGN);
                signal(SIGINT, SIG_IGN);
                signal(SIGCHLD, &signalHandler_child);

                setpgid(RSH_PID, RSH_PID);                                         // we make the shell process as new process group leader
                RSH_PGID = getpgrp();
                if (RSH_PID != RSH_PGID) {
                        printf("Error, the shell is not process group leader");
                        exit(EXIT_FAILURE);
                }
                if (tcsetpgrp(RSH_TERMINAL, RSH_PGID) == -1)      // if Rsh cannot grab control of the terminal
                        tcgetattr(RSH_TERMINAL, &RSH_TMODES);             // we save default terminal attributes for shell.

                currentDirectory = (char*) calloc(1024, sizeof(char));
        } else {
                printf("Could not make R-shell interactive. Exiting..\n");
                exit(EXIT_FAILURE);
        }
}

int main(int argc, char **argv, char **envp)
{
        init();
        welcomeScreen();
        shellPrompt();                                                                         // prints the prompt
        while (TRUE) {
		//shellPrompt();		
                userInput = getchar();
                switch (userInput) {
                case '\n':                                                                               // if the user hits the enter key
                        shellPrompt();                                                                     // nothing happens
                        break;
                default:
                        getTextLine();                                                                     // store user input in buffer
                        handleUserCommand();                                                   // execute user command
                        shellPrompt();                                                                    // print the prompt
                        break;
                }
        }
        printf("\n");
        return 0;
}

/*********************************************************************
 * SIGNAL HANDLERS
 *********************************************************************/

/**
 * signal handler for SIGCHLD
 */
void signalHandler_child(int p)
{
        pid_t pid;
        int terminationStatus;
        pid = waitpid(WAIT_ANY, &terminationStatus, WUNTRACED | WNOHANG); // intercept the process that sends the signal
        if (pid > 0) {                                                                          // if there are information about it
                t_job* job = getJob(pid, BY_PROCESS_ID);                      // get the job from the list
                if (job == NULL)
                        return;
                if (WIFEXITED(terminationStatus)) {                                                    // case the process exits normally
                        if (job->status == BACKGROUND) {                             // child in background terminates normally
                                printf("\n[%d]+  Done\t   %s\n", job->id, job->name); // inform the user
                                jobsList = delJob(job);                                                    // delete it from the list
                        }
                } else if (WIFSIGNALED(terminationStatus)) {                                  // the job dies because of a signal
                        printf("\n[%d]+  KILLED\t   %s\n", job->id, job->name); // inform the user
                        jobsList = delJob(job);                                                     // delete the job from the list
                } else if (WIFSTOPPED(terminationStatus)) {                                  // a job receives a SIGSTP signal
                        if (job->status == BACKGROUND) {                           // the job is in bg
                                tcsetpgrp(RSH_TERMINAL, RSH_PGID);
                                changeJobStatus(pid, WAITING_INPUT);                     // change its status to "waiting for input"
                                printf("\n[%d]+   suspended [wants input]\t   %s\n",
                                       numActiveJobs, job->name);                                  // inform the user
                        } else {                                                                   // otherwise, the job is going to be suspended
                                tcsetpgrp(RSH_TERMINAL, job->pgid);
                                changeJobStatus(pid, SUSPENDED);                         // we modify the status
                                printf("\n[%d]+   stopped\t   %s\n", numActiveJobs, job->name); // and inform the user
                        }
                        return;
                } else {
                        if (job->status == BACKGROUND) {                          // otherwise, delete the job from the list
                                jobsList = delJob(job);
                        }
                }
                tcsetpgrp(RSH_TERMINAL, RSH_PGID);
        }
}
