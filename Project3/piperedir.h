/*
*
*	pipe and redirection header
*
*/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <wait.h>

void execute(char **, int, char **);
void handle_signal(int);
int parse(char *, char **, char **, int *);
void chop(char *);
//int noclobber = 0;
int *status;
#define INPUT_STRING_SIZE 80

#define NORMAL 				00
#define OUTPUT_REDIRECTION 	11
#define INPUT_REDIRECTION 	22
#define PIPELINE 			33
#define BACKGROUND1			44
#define OUTPUT_APP	55
#define OUTPUT_REDIRECTION_WITH_ERROR 66
#define OUTPUT_REDIRECTION_WITH_APPEND_ERROR 77
#define PIPELINE_WITH_ERROR 88

typedef void (*sighandler_t)(int);
int flag = 0;
int pipredir()
{
	int i, mode = NORMAL, cmdArgc;
	size_t len = INPUT_STRING_SIZE;
	char *cpt, *inputString, *cmdArgv[INPUT_STRING_SIZE], *supplement = NULL;
	inputString = (char*)malloc(sizeof(char)*INPUT_STRING_SIZE);	
	char curDir[100];
	mode = NORMAL;
	strcpy(inputString, pipbuffer);	
	strcat(inputString, "\n");		
	cmdArgc = parse(inputString, cmdArgv, &supplement, &mode);
	execute(cmdArgv, mode, &supplement);
	return 0;
}

int parse(char *inputString, char *cmdArgv[], char **supplementPtr, int *modePtr)
{
	int cmdArgc = 0, terminate = 0;
	char *srcPtr = inputString;
	
	while(*srcPtr != 0x00 && terminate == 0)
	{		
		*cmdArgv = srcPtr;
		cmdArgc++;
		while(*srcPtr != ' ' && *srcPtr != '\t' && *srcPtr != '\0' && *srcPtr != '\n' && terminate == 0)
		{
			switch(*srcPtr)
			{
				case '&':
                    			*cmdArgv = '\0';
					          		            *modePtr = BACKGROUND1;
                		        flag = 1;
                   
                    	while(*srcPtr == ' ' || *srcPtr == '\t')
				srcPtr++;
			*supplementPtr = srcPtr;
			chop(*supplementPtr);
			terminate = 1;
			break;	
				case '>': 
					*cmdArgv = '\0';
					srcPtr++;
					
                    			if(*srcPtr == '&')
                    			{
                        			*modePtr = OUTPUT_REDIRECTION_WITH_ERROR;
						srcPtr++;
                    			}
					else if(*srcPtr == '>')
					{
                        			*srcPtr++;
                        			if(*srcPtr == '&')
                        			{
                            				*modePtr = OUTPUT_REDIRECTION_WITH_APPEND_ERROR;
                            				srcPtr++;
                        			}
						else
                       				{
                            				*modePtr = OUTPUT_APP;
                            				srcPtr++;
                        			}
					}
                    			else
                        			*modePtr = OUTPUT_REDIRECTION;
                    
                    			while(*srcPtr == ' ' || *srcPtr == '\t')
						srcPtr++;
					*supplementPtr = srcPtr;
					chop(*supplementPtr);
					terminate = 1;
					break;

				case '<':
					*modePtr = INPUT_REDIRECTION;
					*cmdArgv = '\0';
					srcPtr++;
					while(*srcPtr == ' ' || *srcPtr == '\t')
						srcPtr++;
					*supplementPtr = srcPtr;
					chop(*supplementPtr);
					terminate = 1;
					break;
                		case '|':
					*cmdArgv = '\0';
					srcPtr++;
                    			if(*srcPtr == '&')
                    			{
                        			*modePtr = PIPELINE_WITH_ERROR;
                        			srcPtr++;
                    			}
                    			else
                        			*modePtr = PIPELINE;
					while(*srcPtr == ' ' || *srcPtr == '\t')
						srcPtr++;
					*supplementPtr = srcPtr;
					terminate = 1;
					break;
				
			}
			srcPtr++;
		}
		while((*srcPtr == ' ' || *srcPtr == '\t' || *srcPtr == '\n') && terminate == 0)
		{
			*srcPtr = '\0';
			srcPtr++;
		}
		cmdArgv++;
	}	
	*cmdArgv = '\0';
	return cmdArgc;
}

void chop(char *srcPtr)
{
	while(*srcPtr != ' ' && *srcPtr != '\t' && *srcPtr != '\n')
	{
		srcPtr++;
	}
	*srcPtr = '\0';
}

void execute(char **cmdArgv, int mode, char **supplementPtr)
{
	pid_t pid, pid2;
	FILE *fp;
	int mode2 = NORMAL, cmdArgc, status1, status2;
	char *cmdArgv2[INPUT_STRING_SIZE], *supplement2 = NULL;
	int myPipe[2];
	if(mode == PIPELINE)
	{
		if(pipe(myPipe))					//create pipe
		{
			fprintf(stderr, "Pipe failed!");
			exit(-1);
		}
		parse(*supplementPtr, cmdArgv2, &supplement2, &mode2);
	}
	pid = fork();
	if( pid < 0)
	{
		printf("Error occured");
		exit(-1);
	}
	else if(pid == 0)
	{
		switch(mode)
		{
			case OUTPUT_REDIRECTION:
				if (noclobber == 1)
				{printf("Cannot overwrite file\n"); break;}
				else{fp = fopen(*supplementPtr, "w+");
				dup2(fileno(fp), 1);
				break;}
            		case OUTPUT_REDIRECTION_WITH_ERROR:
		                if (noclobber == 1)
				{printf("Cannot overwrite file\n"); break;}
				else{fp = fopen(*supplementPtr, "w+");
		                dup2(fileno(fp), 1);
		                dup2(2, fileno(fp));
		                break;}
		        case OUTPUT_REDIRECTION_WITH_APPEND_ERROR:
                		if (noclobber == 1)
				{printf("Cannot append to file\n"); break;}
				else{
				fp = fopen(*supplementPtr, "a");
		                dup2(fileno(fp), 1);
		                dup2(2, fileno(fp));
		                break;}
			case OUTPUT_APP:
				if (noclobber == 1)
				{printf("Cannot append to file\n"); break;}
				else{
				fp = fopen(*supplementPtr, "a");
				dup2(fileno(fp), 1);
				break;}
			case INPUT_REDIRECTION:
				fp = fopen(*supplementPtr, "r");
				dup2(fileno(fp), 0);
				break;
			case PIPELINE:
				close(myPipe[0]);		//close input of pipe
				dup2(myPipe[1], 1);
				close(myPipe[1]);
				break;
		        case PIPELINE_WITH_ERROR:
		                close(myPipe[0]);
		                dup2(myPipe[1], 1);
		                dup2(2, 1);
		                close(myPipe[1]);
		                break;
		}
		execvp(*cmdArgv, cmdArgv);
	}
	else
	{	//printf("HI\n");
		if(mode == BACKGROUND1)
		{
			pid = fork();
		
           		pid = wait(status);
			if (pid > 0)
			{
				
				pid = waitpid(0, status, 0);
				printf("waitpid reaped child pid %d\n", pid);
				
			}
		
			if (pid == 0)
                	{
				//printf("TEST %d\n",pid);
				execvp(cmdArgv[0], cmdArgv);
                		perror(cmdArgv[0]);
                		//exit(0);
			}            
                	if (pid < 0)
			{
				//printf("TEST %d\n",pid);
				//perror("fork");
   			        exit(-1);
			}              
            			
		}	
	else if(mode == PIPELINE)
	{
			waitpid(pid, &status1, 0);		//wait for process 1 to finish
			pid2 = fork();
			if(pid2 < 0)
			{
				printf("error in forking");
				exit(-1);
			}
			else if(pid2 == 0)
			{
				close(myPipe[1]);		//close output to pipe
				dup2(myPipe[0], fileno(stdin));
				close(myPipe[0]);
				execvp(*cmdArgv2, cmdArgv2);
			}
			else
			{
				close(myPipe[0]);
				close(myPipe[1]);
			}
		}
		else
			waitpid(pid, &status1, 0);
	}
}

//xasdasd
