//import some c libraries
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

// job table that stores PID and command
typedef struct job{
	pid_t pid;
	char command[1024];
} job_;

#define MAX_JOBS 64
job_t jobs[MAX_JOBS];
int job_count = 0;


//1. Command Line Prompt
void display_cl_prompt() {
	printf("time to shellabrate ~$ ");
}

//2. Parser
char** parser(char *input) { //takes raw string from user and returns a pointer to an array of strings
	int bufsize = 64; //buffer size for tokens
	int curr = 0; //track current index of tokens
	char **tokens = malloc(bufsize * sizeof(char*)); //allocate mem for array of char*
	char *token; //temp

	if (!tokens) {
		fprintf(stderr, "shell nah: allocation error\n");
		exit(EXIT_FAILURE); //if tokens is null then allocation failed so print error message
	}
	token = strtok(input, "\t\r\n"); //split strings in tokens based on spaces, tabs, returns, and newlines. Then returns pointer to first token in input
	
	while (token != NULL) {
		tokens[curr] = token;
		curr++; //place curr token into tokens array at curr
		if (curr >= bufsize) { //if index reaches the bufsize then allocate more mem
			bufsize += 64;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) { //if realloc fails handle error
				fprintf(stderr, "shell nah: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, " \t\r\n"); //get next token
	}
	tokens[curr] = NULL; //stop loop
	return tokens;
}

//3. Executor

int executor(char *input) {
	char **args = parser(input);
	pid_t pid; 
	pid_t wpid;
	int status;
	int background = 0;
	
	if (args[0] == NULL){ //if empty
		return 1;
	}

	if (strncmp(args[0], "exit", 4) == 0){
		printf("leaving the shellabration ... goodbye!");
		return 0; // exit
	}

	for (int i = 0; args[i] != NULL; i++){ //& for background processing
		if (strcmp(args[i], "&") == 0){
			background = 1;
			args[i] = NULL;
			break;
		}
	}
	
	//handle jobs command here

	if (strncmp(args[0], "jobs", 4) == 0){
		for(int i = 0; i < job_count; i++){
			printf("[%d] PID: %d Command: %s\n", i + 1, jobs[i].pid, jobs[i].command);
		}
		return 1;
	}

	//handle kill command here




	if (strncmp(args[0], "pwd", 3) == 0){ //print working directory
		char cwd[1024];
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			printf("%s\n", cwd);
		} else {
			perror("shell nah error");
		}
	return 1;
	} else if (strncmp(args[0], "ls", 2) == 0) { //list directory
		pid = fork(); // execvp for ls
		if (pid == 0) {
			//child process
			if (execvp("ls", args) == -1) {
				perror("shell nah error");
			}
			exit(EXIT_FAILURE);
		} else if (pid < 0){
			perror("shell nah error");
		} else {
			//parent
			do {
				wpid = waitpid(pid, &status, WUNTRACED);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		}
		return 1;
	}

	for(int i = 0; args[i] != NULL; i++){
		if(strncmp(args[i], 0) == "&"){
			background = 1;
			args[i] = NULL;
			break;   
		}
	}

	// load processes
	pid = fork();
	if (pid == 0) {
		//child
		if (execvp(args[0], args) == -1){
			perror("shell nah error");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		perror("shell nah error");
	} else {
		//parent
		if(background){
			//add background process to job table here
			if(job_count < MAX_JOBS){
				jobs[job_count].pid = pid;
				strncpy(jobs[job_count].command, input, sizeof(jobs[job_count].command)-1);
				jobs[job_count].command[sizeof(jobs[job_count].command) - 1] = '\0';
				job_count++;
				printf("[Background Process] PID: %d Command: %s\n", pid, input);
			} else {
				printf("Job table is full. Cannot track more background jobs.\n");
			}
		} else {
			do {
				wpid = waitpid(pid, &status, WUNTRACED);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		} 	
	}
	return 1;
}

//4. Main
int main(){
	char input[1024]; //current input limit
	int status = 1;

	while (status) {
		display_cl_prompt();
		if (fgets(input, sizeof(input), stdin) == NULL){
			break; //exit on EOF
		}
		input[strcspn(input, "\n")] = '\0'; 
		status = executor(input);
	}
	return 0;

}
