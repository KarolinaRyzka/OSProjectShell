//import some c libraries
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

//1. Command Line Prompt
void display_cl_prompt() {
	printf("the best shell ever ~$");
}

//2. Parser
char** parser(char *input) { //takes raw string from user and returns a pointer to an array of strings
	int bufsize = 64; //buffer size for tokens
	int curr = 0; //track current index of tokens
	char **tokens = malloc(bufsize * sizeof(char*)); //allocate mem for array of char*
	char *token; //temp

	if (!tokens) {
		fprintf(stderr, "the best shell ever: allocation error\n");
		exit(EXIT_FAILURE); //if tokens is null then allocation failed so print error message

	token = strtok(input, "\t\r\n"); //split strings in tokens based on spaces, tabs, returns, and newlines. Then returns pointer to first token in input
	
	while (token != NULL) {
		tokens[curr] = token;
		curr++; //place curr token into tokens array at curr
		if (curr >= bufsize) { //if index reaches the bufsize then allocate more mem
			bufsize += 64;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) { //if realloc fails handle error
				fprintf(stderr, "the best shell ever: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL< " \t\r\n"); //get next token
	}
	tokens[curr] = NULL; //stop loop
	return tokens;
}


//3. Executor

int executor(char *input) {
	char **args = parser(input);
	pid_t pid, wpid;
	
	pid = fork();
	if (pid == 0) {
		 
	} else {


	}
	return 1;
}

//4. Main




int main(){
	return 0;
}
