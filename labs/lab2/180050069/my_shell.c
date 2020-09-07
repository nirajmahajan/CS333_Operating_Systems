#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

static bool input_taken = false;
static int background_count = 0;
static int foreground_count = 0;
static int tokens_count = 0;
static pid_t background_processes[64];
static pid_t foreground_processes[64];

void sigintHandler(int sig_num) 
{ 
    /* Reset handler to catch SIGINT next time. 
       Refer http://en.cppreference.com/w/c/program/signal */
    signal(SIGINT, sigintHandler); 
    if (input_taken){
    	printf("\n"); 
    } else{
    	printf("\n$ "); 
    }
    fflush(stdout);
} 

/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

void reap(){
	for(int i = 0; i < 64; i++){
		if (background_processes[i] == -1){
			continue;
		}
		int status;
		pid_t cpid = waitpid(background_processes[i], &status, WNOHANG);
		if (cpid == 0 || cpid == -1){
			continue;
		} else {
			printf("[%d]+ Done\nShell: Background process finished with status %d\n",i, status);
			background_count -= 1;
			background_processes[i] = -1;
		}
	}
}

void reap_exit(){
	for(int i = 0; i < 64; i++){
		if (background_processes[i] == -1){
			continue;
		}
		int status;
		pid_t cpid = waitpid(background_processes[i], &status, WUNTRACED);
		if (cpid == 0 || cpid == -1){
			continue;
		} else {
			printf("[%d]+ Shell: Background process reaped with status %d\n",i, status);
			background_count -= 1;
			background_processes[i] = -1;
		}
	}
}

bool foreground_reap(){
	bool all_reaped = true;
	for(int i = 0; i < 64; i++){
		if (foreground_processes[i] == -1){
			continue;
		}
		int status;
		pid_t cpid = waitpid(foreground_processes[i], &status, WNOHANG);
		if (cpid == 0 || cpid == -1){
			all_reaped = false;
			continue;
		} else {
			// printf("[%d]+ Done\nShell: Foreground process finished with status %d\n",i, status);
			foreground_count -= 1;
			foreground_processes[i] = -1;
		}
	}
	return all_reaped;
}

bool execute_command(char **tokens, bool backgroundmode){
	if (backgroundmode && background_count == 64) {
		printf("Background process limit of 64 exceeded\n");
		return false;
	}
	if (tokens[0] == NULL){
		return false;
	}
	else if (strcmp(tokens[0], "exit") == 0) {
		if (tokens[1] == NULL){
			for(int i=0; i < 64; i++){
				if (background_processes[i] == -1){
					continue;
				}
				kill(background_processes[i], SIGINT);
				printf("[%d]+ Interrupt\nShell: Background process interrupted\n",i);
			}
			for(int i=0;i<tokens_count;i++){
				free(tokens[i]);
			}
			reap_exit();
			free(tokens);
			exit(0);
		}
		else{
			printf("Shell: Incorrect command\n");
		}
	}
	else if (strcmp(tokens[0], "cd") == 0)	 {
		if (tokens[1] == NULL){
			printf("Shell: Incorrect command\n");
		}
		else if (tokens[2] == NULL){
			int cd_rc = chdir(tokens[1]);
			if (cd_rc != 0){
				printf("Shell: Incorrect command\n");
			}
		}
		else{
			printf("Shell: Incorrect command\n");
		}
	}
	else{
		int rc = fork();
		if (rc < 0) {
			printf("Fork Failed\n");
			exit(1);
		}
		else if (rc == 0){
			// child
			signal(SIGINT, SIG_DFL);
			if (backgroundmode) {
				setpgid(0,0);
			}
			int evp = execvp(tokens[0], tokens);
			printf("Command not found\n");
			// sleep(1);
			reap();
			_exit(1);
		}
		else{
			if (backgroundmode){
				for(int i = 0; i < 64; i++){
					if (background_processes[i] == -1){
						printf("[%d] %d\n", i,rc);
						background_processes[i] = rc;
						background_count += 1;
						break;
					}
				}
			} else{
				int status;
				int rc_wait = waitpid(rc,&status,0);
				if (WIFSIGNALED(status)){
					return true;
				} else{
					return false;
				}
			}
		}
	}
}

void execute_command_parallel(char **tokens){
	if (foreground_count == 64) {
		printf("Foreground process limit of 64 exceeded\n");
		return;
	}
	if (tokens[0] == NULL){
		return;
	}
	else if (strcmp(tokens[0], "exit") == 0) {
		if (tokens[1] == NULL){
			for(int i=0; i < 64; i++){
				if (background_processes[i] == -1){
					continue;
				}
				kill(background_processes[i], SIGINT);
			}
			for(int i=0; i < 64; i++){
				if (foreground_processes[i] == -1){
					continue;
				}
				kill(foreground_processes[i], SIGINT);
			}
			for(int i=0;i < tokens_count;i++){
				free(tokens[i]);
			}
			reap_exit();
			foreground_reap();
			free(tokens);
			exit(0);
		}
		else{
			printf("Shell: Incorrect command\n");
		}
	}
	else if (strcmp(tokens[0], "cd") == 0)	 {
		if (tokens[1] == NULL){
			printf("Shell: Incorrect command\n");
		}
		else if (tokens[2] == NULL){
			int cd_rc = chdir(tokens[1]);
			if (cd_rc != 0){
				printf("Shell: Incorrect command\n");
			}
		}
		else{
			printf("Shell: Incorrect command\n");
		}
	}
	else{
		int rc = fork();
		if (rc < 0) {
			printf("Fork Failed\n");
			exit(1);
		}
		else if (rc == 0){
			// child
			signal(SIGINT, SIG_DFL);
			int evp = execvp(tokens[0], tokens);
			printf("Command not found\n");
			sleep(1);
			reap();
			_exit(1);
		}
		else{
			for(int i = 0; i < 64; i++){
				if (foreground_processes[i] == -1){
					printf("[%d] %d\n", i,rc);
					foreground_processes[i] = rc;
					foreground_count += 1;
					break;
				}
			}
		}
	}
}

int main(int argc, char* argv[]) {
	for(int i = 0; i < 64; i++){
		background_processes[i] = -1;
		foreground_processes[i] = -1;
	}
	char line[MAX_INPUT_SIZE];            
	char **tokens;              
	int i;

	FILE* fp;
	if(argc == 2) {
		fp = fopen(argv[1],"r");
		if(fp < 0) {
			printf("File doesn't exists.");
			return -1;
		}
	}

	signal(SIGINT, sigintHandler);
	while(1) {	
		input_taken = false;		
		// Reap the dead
		reap();

		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		if(argc == 2) { // batch mode
			if(fgets(line, sizeof(line), fp) == NULL) { // file reading finished
				break;	
			}
			line[strlen(line) - 1] = '\0';
		} else { // interactive mode
			printf("$ ");
			scanf("%[^\n]", line);
			getchar();
		}
		input_taken = true;
		// printf("Command entered: %s (remove this debug output later)\n", line);
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);
   
        //do whatever you want with the commands, here we just print them

		bool backgroundmode = false;
		bool andandmode = false;
		bool andandandmode = false;

		tokens_count = 0;
		int count = 1;
		for(i=0;tokens[i]!=NULL;i++){
			tokens_count += 1;
			if (strcmp(tokens[i],"&") == 0){
				backgroundmode = true;
				tokens[i] = NULL;
				count += 1;
			} else if (strcmp(tokens[i],"&&") == 0){
				andandmode = true;
				tokens[i] = NULL;
				count += 1;
			} else if (strcmp(tokens[i],"&&&") == 0){
				andandandmode = true;
				tokens[i] = NULL;
				count += 1;
			}
		}

		int encountered_count = 0;
		bool stopall = false;
		if (backgroundmode) {
			execute_command(tokens, true);
			encountered_count += 1;
			for(i=1;i < tokens_count;i++){
				if (encountered_count == count) {
					break;
				}
				if (tokens[i] == NULL) {
					execute_command(&tokens[i+1], true);
					encountered_count += 1;
				}
			}
		}
		else if (andandmode) {
			stopall = execute_command(tokens, false);
			reap();
			encountered_count += 1;
			if (stopall){
				continue;
			}
			for(i=1;i < tokens_count;i++){
				if (stopall){
					break;
				}
				if (encountered_count == count) {
					break;
				}
				if (tokens[i] == NULL) {
					stopall = execute_command(&tokens[i+1], false);
					reap();
					encountered_count += 1;
				}
			}
		} else if (andandandmode){
			execute_command_parallel(tokens);
			encountered_count += 1;
			for(i=1;i < tokens_count;i++){
				if (encountered_count == count) {
					break;
				}
				if (tokens[i] == NULL) {
					execute_command_parallel(&tokens[i+1]);
					encountered_count += 1;
				}
			}

			bool breaking_condition = false;
			while(!breaking_condition){
				breaking_condition = foreground_reap();
			}
		} else {
			execute_command(tokens, backgroundmode);
		}

		// Freeing the allocated memory	
		for(i=0;i < tokens_count;i++){
			free(tokens[i]);
		}
		free(tokens);

	}
	return 0;
}
