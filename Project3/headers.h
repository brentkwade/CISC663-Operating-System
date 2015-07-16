/**
 * R-shell 1.0.0 HEADERS
 * Description: prototypes and declarations
 * Author: Daniel Graziotin, 4801, daniel.graziotin@unibz.it
 * http://daniel.graziotin.net/projects/R-shell
 */

/**
 * LICENSE
 * This file is part of R-shell.
 *
 *   R-shell is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   R-shell is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with R-shell.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/
//#include "sysload.h" // to get load warning in the shell
#include <stdio.h>
#include <stdlib.h>                                                                            // just for malloc()
#include <unistd.h>                                                                           // unix standard lib (fork, dup2 etc.)
#include <string.h>                                                                            // string functions
#include <sys/types.h>                                                                     // extra data types
#include <signal.h>                                                                            // signals
#include <sys/wait.h>                                                                        // wait functions
#include <fcntl.h>                                                                               // file descriptor operations
#include <termios.h>                                                                         // terminal control data structures
#define TRUE 1
#define FALSE !TRUE

extern char **environ;						//variable in stdio.h to print the environment variables of shell

static const char* SHELL_VERSION = "1.0.0";

/* definition of user input data structures */
#define BUFFER_MAX_LENGTH 50    						// maximum length of the buffer (50 chars)
#define TOKEN_MAX_LENGTH 50                                                	// maximum number of tokens that a path variable can have
static char* currentDirectory;                                                           // buffer to contain the current directory
static char userInput = '\0';
static char buffer[BUFFER_MAX_LENGTH];                                    // buffer for user input line, a counter to count no. of tokens
static int bufferChars = 0;
static char* prefix;								//to store the prompt prefix

static char *commandArgv[5];                                                         // array of strings that compose the command given
static int commandArgc = 0;                                                            // count of the words of the command given
static int noclobber = 0;
static char* pipbuffer;							// used to copy full buffer variable to perform buffer check 
						// to keep track of count of tokens in path environment variable
//char* tokenarr[TOKEN_MAX_LENGTH];
//char* accessarr[TOKEN_MAX_LENGTH];		// to hold path tokens, and concatenated tokens (concat with parameter)
/*************************
*	for warnload
**************************/
int warnloadflag = 0; 						//flag 0 says no warn load thread is created.
double warnload = 0.2;  
pthread_t tid1;


/* definition of job status */
#define FOREGROUND 'F'
#define BACKGROUND 'B'
#define SUSPENDED 'S'
#define WAITING_INPUT 'W'

/* standard file descriptors */
#define STDIN 1
#define STDOUT 2

/* definition of search type parameters of a job */
#define BY_PROCESS_ID 1
#define BY_JOB_ID 2
#define BY_JOB_STATUS 3

static int numActiveJobs = 0; 							// counts the number of active processes

typedef struct job { 									// struct that defines a job
        int id;
        char *name;
        pid_t pid;
        pid_t pgid; // not yet useful
        int status;
        char *descriptor;
        struct job *next;
} t_job;

static t_job* jobsList = NULL; 							// linked list of active processes

/**********************************************************************
* History Struct and functions 
***********************************************************************/
struct Node 
{
	char* data;
	struct Node* prev;
	struct Node* next;
};
char input[50];
struct Node* head; // global variable - pointer to head node.

/* shell pid, pgid, default terminal, terminal modes */
static pid_t RSH_PID;
static pid_t RSH_PGID;
static int RSH_TERMINAL, RSH_IS_INTERACTIVE;
static struct termios RSH_TMODES;

/*********************************************************************
 * USER INPUT FUNCTIONS
 *********************************************************************/

/* gets a line of text from user */
void getTextLine();
/* populates commandArgv[] with every word that compose the command line */
void populateCommand();
/* frees the buffer and commandArgv[] */
void destroyCommand();

/*********************************************************************
 * FUNCTIONS REGARDING ACTIVE JOBS
 *********************************************************************/

/*inserts an active process in the linked list*/
t_job * insertJob(pid_t pid, pid_t pgid, char* name, char* descriptor,
                  int status);
/* deletes a no more active process from the linked list */
t_job* delJob(t_job* job);
/* returns a Job object */
t_job* getJob(int searchValue, int searchParameter);
/* prints the active processes launched by the shell */
void printJobs();

/*********************************************************************
 * PROMPT AND ERROR FUNCTIONS
 *********************************************************************/

/* displays a welcome screen */
void welcomeScreen();
/* displays a nice prompt for the shell */
void shellPrompt();
/* promt prefix command */
void promptCmd();

/*********************************************************************
 * SHELL FUNCTIONS
 *********************************************************************/

/* handles the command given: is it a built-in command or an external program? */
void handleUserCommand();
/* checks if a user command is a built-in command and executes it */
int checkBuiltInCommands();
/* executes a system command */
void executeCommand(char *command[], char *file, int newDescriptor,
                    int executionMode);
/* creates a child process */
void launchJob(char *command[], char *file, int newDescriptor,
               int executionMode);
/* puts a job in foreground, and continue it if it was suspended */
void putJobForeground(t_job* job, int continueJob);
/* puts a job in background, and continue it if it was suspended */
void putJobBackground(t_job* job, int continueJob);
/* wait for a specific job */
void waitJob(t_job* job);
/* kills a Job given its id */
void killJob(int jobId);
/* changes directory */
void changeDirectory();
/* initializes the shell */
void init();
/* to print current working directory */
void printpwd();
/* print pid of shell */
void printpid();
/* to print the environment of a shell */
void envprint();
/* to set environment variable */
void envset();
/* to simulate which command */
void printwhich();
/* to simulate where command */
void printwhere();
/*********************************************************************
 * SIGNAL HANDLERS
 *********************************************************************/

/* signal handler for SIGCHLD */
void signalHandler_child(int p);
