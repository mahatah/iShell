/******************************************************************************************
* Program: iShell
* Author: Matthew Price
* February 19, 2009
* 
* iShell is a Shell program that can be used in the Minix platform as an alternate shell.
* The limitations and further information about the program can be found in the ReadMe.txt
* and UserManual.txt files.
*
*******************************************************************************************/



#include <stdio.h>								/*all libraries encountered to make Minix happy. These differ between Linux and Minix and, for functionality, redundent ones were used to make sure both platforms would be operational*/
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "iShell.h"

int history(void){								/*function to display history*/

	int hist_holder = 0;
	int wrap_holder = 0;
	int offset = 0;	
	ptr_HIST_SIZE = (char*)malloc(5);

	ptr_HIST_SIZE = getenv("HISTSIZE");					/*this finds the enviornment variable HISTSIZE and makes the global variable HIST_SIZE equal to it*/
	HIST_SIZE = (int)atoi(ptr_HIST_SIZE);	
		
	
	if (times_ran > HIST_SIZE - 1){
	hist_holder = times_ran;						/*allows hist holder to offset at the wrapped amount*/
	while (hist_holder >= HIST_SIZE){					/*stores the offset in int offset*/
	hist_holder = (hist_holder - HIST_SIZE);
	}
	hist_holder++;
	offset = hist_holder;
	}

													/*prints out the structs contents until it finds a null struct*/
	while (user_command[hist_holder].main_command[0] != '\0' && hist_holder < HIST_SIZE){		/*also, this starts at the hist_holder amount which is offset*/
	printf("%s", user_command[hist_holder].main_command);						/*to start where ever needed to print history in order*/
	printf(" %s \n", user_command[hist_holder].arguement);
	hist_holder++;
	}
										/*this prints from 0 to the offset amount, completing the whole history process*/
	if (times_ran > HIST_SIZE - 1){
	for (wrap_holder = 0; wrap_holder <= offset -1; wrap_holder++){
	printf("%s", user_command[wrap_holder].main_command);
	printf(" %s \n", user_command[wrap_holder].arguement);
	}
	}
}										

int environ_disp(char *yytext){							/*displays the environment variable*/
	int i = 0;


	while (environ[i] != NULL) {
	if (strcmp(environ[i], " ")!=0){					/*will display all of the enviorn elements expect those that only contain a " " as they have been deleted*/
	printf("%s\n",environ[i]);
	}
	i++;
	}
	return 0;
}

int set_environ(void){								/*sets an environment variable. Only to be called after a check_two_argue has been done to make sure there are at least two arguements*/
	int ret = 0;	
	int i = 0;
	int i2 = 0;
	int size_checker = 0;	
	char *argue1 = malloc(strlen(user_command[storedi].arguement));
	char *argue2 = malloc(strlen(user_command[storedi].arguement));
	

	for (i = 0; user_command[storedi].arguement[i] != ' '; i++){		/*argue1 takes the command until it hits a space, where it stops*/
	argue1[i] = user_command[storedi].arguement[i];
	}
	for (i = i + 1; user_command[storedi].arguement[i] != '\0'; i++){	/*skips the space and continues until it hits a NULL*/
	argue2[i2] = user_command[storedi].arguement[i];
	i2++;
	}
	argue2[i2] = '\0';
	
	size_checker = (int)atoi(argue2);					/*necessary to compare the input value to the maximum of 100*/
	
	if (strcmp(argue1, "HISTSIZE")==0 && size_checker > 100){		/*checks to make sure the user selected a value lower than or equal to 100 and will set the HISTSIZE to 100 otherwise*/
	printf("History size cannot exceed 100 but will be set at the maximum of 100.\n");
	strcpy(argue2, "100");
	}

	ret = setenv_minix(argue1, argue2, 1);					/*sends the variables to setenv_minix function to have the variable set in the environment*/
	free(argue1);
	free(argue2);	
	return 0;
	}


int change_dir(void){									/*Function to change directory*/
	int i = 0;									/*need to takes first arguement, run chdir() and then ignore the rest, currently tries to change directory to the entire arguement string*/
	long size;
	char *buf;
	char *ptr;

	

	i = chdir(user_command[storedi].arguement);					/*runs chdir and returns a value if successful or not*/
	size = pathconf(".", _PC_PATH_MAX);

	if (i == -1){									/*a value of -1 means that the chdir() was not successful*/
	printf("(\"%s\") is an invalid directory\n", user_command[storedi].arguement);
	}

	if ((buf = (char *)malloc((size_t)size)) != NULL){				/*otherwise allocate memory, get current directory stored in a pointer, and tell the user what directory they are in */
	ptr = getcwd(buf, (size_t)size);
	printf("%s is the current working directory\n", ptr);
	}
	lex_loop = 0;									/*sets lex_loop back to 0 so that the next command_main can be input*/

	return 0;
}

int unset_env(void){									/*unsets an environment variable if it exists in the environment*/
	int i = 0;
	int ret = 0;	
	char *argue1 = (char *)malloc(strlen(user_command[storedi].arguement)+3);
	
	if (user_command[storedi].arguement[0] == '\0'){				/*if there is not a sufficent number of arguements, return with the no_arguement_error*/
	no_arguement_error();
	return 0;
	}
	
	strcpy(argue1, user_command[storedi].arguement); 				/*copies the arguement into the argue1 pointer*/

	if (strcmp(argue1, "HISTSIZE")==0){						/*checks to make sure that you are not trying to unset HISTSIZE, which is really really bad...and tells the user we're not going to let them do that then returns without doing anything*/
	printf("You cannout unset HISTSIZE! iShell needs that!\n");
	}else{	
	int ret = unputenv_minix(argue1);						/*sends the arguements to unputenv_minix so that the variable can be unset*/
	free(argue1);									/*frees the now unneeded pointer*/
	return 0;
	}
	return 0;
}



int check_paths(void){								/*checks the paths in the environ variable for for anything that it can run*/
	int i = 0;
	int j = 0;
	int k = 0;
	int m = -1;
	int middle_pipe = 0;
	int execute_return = 0;
	int status, status1;
	int fid;
	int filed[10][2];
	int piped = 0;	
	pid_t pID, pID2, pID3;
	int argue_count = 0;							
	char *argue_arry[15];								/*makes an array that can hold 15 commands*/
	char **argue_arry_temp = malloc(strlen(user_command[storedi].arguement));	/*pointer needed for token size, allocated memory to it*/											
	char *search_space = (" ");
	char *full_argue = malloc(strlen(user_command[storedi].arguement));		/*same as above*/
	char *full_main_com = malloc(strlen(user_command[storedi].main_command));	/*same as above*/
	char *word = (char *)malloc(2);	
	
	for (j = 0; j <= 14; j++){							/*sets the entire a argue arry to '\0'*/
	argue_arry[j] = (char *)malloc(2);	
	argue_arry[j] = '\0';
	}
	
	strcpy(full_argue, user_command[storedi].arguement);				/*now, this is where it gets interesting. Because strtoks destroy strings, I need to copy from the struct to a temporary string*/
	strcpy(full_main_com, user_command[storedi].main_command);

	argue_arry[0] = malloc(strlen(user_command[storedi].main_command));
	strcpy(argue_arry[0], user_command[storedi].main_command);
	
	argue_arry[1] = malloc(strlen(user_command[storedi].arguement));			/*memory is allocated for the first command to the array built above*/
	argue_arry[1] = '\0';									/*sets the first array to '\0'. the value i is set to 0, but I wanted to put [0]'s here because it is important that they start at 0.*/
	argue_arry[1] = (char *)strtok_r(full_argue, search_space, argue_arry_temp);		/*tokens off the first command and stores it inside the first array*/
	argue_count++;
	if (argue_count < user_command[storedi].num_arg){
	for (i = 2; argue_count < user_command[storedi].num_arg; i++){			/*does this while the number of arguments is greater than the count of arguements*/
	argue_arry[i] = malloc(strlen(user_command[storedi].arguement));		/*same as above, except now with [i]*/
	argue_arry[i] = '\0';
	argue_arry[i] = (char *)strtok_r(NULL, search_space, argue_arry_temp);			/*searches for the next token and stores it in the next array slot*/
	argue_count++;									/*incriments argue_count so that we know how many arguements we have tokened*/
	}
	}	
	
										/*this starts the pipe checking function*/
	
	for (j = 0; argue_arry[j+1] != '\0'; j++){}				/*this incriments j to the last position ofthe last arguement*/
	for (j = j; j >= 0; j--){	
	if(strcmp(argue_arry[j], "|")==0 && argue_arry[j+1] !='\0'){		/*works back from the end until it finds a pipe*/	
	
	m++;
	pipe(filed[m]);	
	pID = fork();
	
	if (pID != 0){
	for (k = j; argue_arry[k] != '\0'; k++){				/*argue_arry[k] now holds the last word of the arguement. There should be information in argue_arry[k+1] and so on. These should all be nullified.*/
	argue_arry[k] = '\0';
	}

	piped = 1;								/*indicates whether there is a pipe or not. If there is we make this 1. otherwise it will stay at 0. Later, near execvp, we see if it is a 1 or 0.*/
										/*used for incrimenting pipes and tells the child process if it is a middle pipe or not*/
}else{										/*this is a child of the child process and should read from the other side*/		

	if (m >= 1){								/*this code was implemented in hopes of being able to have more than one pipe in the command line...unfortunatly this was never successful but the underlying code is solid and the remainder has been deleted*/
	middle_pipe = 1;
	}	
	
	j++;
	strcpy(full_main_com, argue_arry[j]);					/*copies the first arguement after a pipe and makes it the main command*/
	strcpy(argue_arry[0], full_main_com);
	j++;
	k = 1;
										/*incriment holder for replacing the information after a pipe in a command line to the lowerest part of the arry. (you must start at 1 because argue_arry[0] must the the same as full_main_com for execvp to work*/
	for(k = 1; argue_arry[j] != '\0'; j++){
	if (strcmp(argue_arry[j], "|")==0){					/*if it encounters the end or another pipe, it stops displacing the arguements*/
	break;
	}
	strcpy(argue_arry[k], argue_arry[j]);					/*copies the arguements after the pipe to the begining of the arguements so that everything is in the proper order for this pipe*/
	k++;
	}
	
	for (k = k; argue_arry[k] != '\0'; k++){				/*argue_arry[k] now holds the last word of the arguement. There should be information in argue_arry[k+1] and so on. These should all be nullified.*/
	argue_arry[k] = '\0';
	}	
	
	close(filed[m][1]);							/*closes one end of the pipe so that the information can be displayed*/
	close(STDIN_FILENO);
	dup(filed[m][0]);							/*functionality for more than one pipe exists with this layout but has not been implimented in this program.*/
	close(filed[m][0]);			
	
	for (j = 0; argue_arry[j] != '\0'; j++){
	if (strcmp(argue_arry[j], ">")==0 && argue_arry[j+1] != '\0'){		/* checks for Output redirect ">" and allows the user to OUTPUT to a file, the right side of the pipe*/
	word = (char*)malloc(strlen(argue_arry[j+1]));	
	word = argue_arry[j+1];
	for (j = j; argue_arry[j] != '\0'; j++){				/*this moves all the arguements down the arry by one*/
	argue_arry[j] = argue_arry[j+2];					/*execvp doesn't want to know the output file name or the ">" charector, we have to get rid of those but maintain the integrity of our arry, we do this by storing two array fields down*/
	}
	fid = open(word, O_WRONLY | O_CREAT);					/*opens the document the user has selected*/
	close(1);
	dup(fid);
	j=0;
	}
	}


	execute_return = execvp(full_main_com, argue_arry);			/*the right side of the pipe executes and uses the pipe to listen for information from the left*/
	if (execute_return == -1){
	printf("iShell: %s: command not found\n", full_main_com);		/*however, if the command can not be excuted, the child must leave the program*/
	exit(0);
	}	
	}
	}
	}
	
	



	pID2 = fork();									/*this forks the process so that we stil have iShell after it executes*/
	if (pID2 != 0){
	waitpid(pID2, &status, 0);	
	}else{
	for (j = 0; argue_arry[j] != '\0'; j++){			
	
	if (strcmp(argue_arry[j], "<")==0 && argue_arry[j+1] != '\0'){		/*checks for Input redirect "<" */
	word = (char*)malloc(strlen(argue_arry[j+1]));	
	word = argue_arry[j+1];
	for (j = j; argue_arry[j] != '\0'; j++){				/*this moves all the arguements down the arry by one*/
	argue_arry[j] = argue_arry[j+2];					/*execvp doesn't want to know the output file name or the ">" charector, we have to get rid of those but maintain the integrity of our arry, we do this by storing two array fields down*/
	}
	fid = open(word, O_RDONLY);						/*opens the document the user has selected*/
	close(0);
	dup(fid);
	j=0;
	}      

	if (strcmp(argue_arry[j], ">")==0 && argue_arry[j+1] != '\0' && piped == 0){		/* checks for Output redirect ">", (Since this operated on the main command, we cannot have it output somewhere if there is a pipe. Otherwise you would redirect away from a pipe then pipe and...well its just crazy and wrong and who knows what would happen*/
	word = (char*)malloc(strlen(argue_arry[j+1]));	
	word = argue_arry[j+1];
	for (j = j; argue_arry[j] != '\0'; j++){				/*this moves all the arguements down the arry by one*/
	argue_arry[j] = argue_arry[j+2];					/*execvp doesn't want to know the output file name or the ">" charector, we have to get rid of those but maintain the integrity of our arry, we do this by storing two array fields down*/
	}
	fid = open(word, O_WRONLY | O_CREAT);					/*opens the document the user has selected*/
	close(1);
	dup(fid);
	j=0;
	}
	}
	
	if (piped == 1){							/*if there is a pipe on this function, we need to close the pipe up or we will never finish with this child*/
	close(filed[m][0]);
        close(STDOUT_FILENO);
        dup(filed[m][1]);
        close(filed[m][1]);
	}

	execute_return = execvp(full_main_com, argue_arry);			/*only the most awesome function ever. Will automatically check all paths set in PATH enviornment variable and try to execute the user's command.*/		
	if (execute_return == -1){
	printf("iShell: %s: command not found\n", full_main_com);
	exit(0);
	}	
	}
      if (piped == 1){								/*closes the pipe for the parent process incase it was opened or an error occured in the child process of the right side of the first pipe*/
      close(filed[m][0]);
      close(filed[m][1]);     
      waitpid(pID, &status, 0);
	}

return 0;
}


int no_arguement_error(void){									/*error function that we send functions to if there are not enough arguements to complete the process*/
	printf("cannot process \"%s\"\n", user_command[storedi].main_command);
	printf("invalid number of arguements\n", user_command[storedi].main_command);
	return 0;
}

int check_two_argue(void){										/*checks to make sure there are at least two arguements input, if not, send to no_arguement_error*/
	int i = 0;

	for (i = 0; user_command[storedi].arguement[i] != '\0'; i++){
	if (user_command[storedi].arguement[i] == ' ' && user_command[storedi].arguement[i+1] != '\0'){
	return 0;										/*if a space is found and the next char is not a NULL then there are at least two arguments, otherwise, there are not. */
	}
	}
	no_arguement_error();									/*sends to the no_arguement_error so that the user can be notified that there was not a sufficent number of arguements*/
	return 1;
}

int find_num_argue(void){										/*figures out how many number of arguements there are in an instance of the struct*/
	int i = 0;

	for (i = 1; user_command[storedi].arguement[i] != '\0'; i++){
	if ((user_command[storedi].arguement[i-1] != ' ' && user_command[storedi].arguement[i] == ' ' && user_command[storedi].arguement[i+1] != ' ' )|| user_command[storedi].arguement[i+1] == '\0'){
	user_command[storedi].num_arg++;							/*stored the different number of arguements into the num_arg member of the struct*/
	}
	}
	return 0;
}

int setenv_minix(const char *env_name, const char *env_value, int m){				/*sets the environmental variable after it receives the information from setenv function*/
	char* temp;
	int ret;

	if(!(temp=malloc(strlen(env_name)+strlen(env_value)+3))){				/*allocates the needed memory for temp*/
	printf("NO MORE MEMORY!");	
	exit(-1);
	}
	
	strcpy(temp, env_name);									/*temp is copied and catcatinized into the variable name=value*/
	strcat(temp, "=");
	strcat(temp, env_value);

	ret = putenv(temp);									/*the value is then used in putenv which will change the value for an existing environment variable or create a new one*/
	
	return 0;
}

int unputenv_minix(const char *env_name){							/*this will unset an environment variable*/

	char* value_checker;
	char *temp;
	int ret;
	int i = 0;

	if(!(temp=malloc(strlen(env_name)+1))){							/*allocates the needed memory for temp*/
	printf("NO MORE MEMORY!");	
	exit(-1);
	}
		

	if (getenv(user_command[storedi].arguement) == '\0'){					/*if the environment doesn't exist, tell the user and then return 0*/
	printf("Environment variable \"%s\" does not exist!\n", env_name);
	free(temp);	
	return 0;
	}




	temp = malloc(strlen(getenv(env_name))+strlen(env_name)+3);				/*otherwise allocate more memory to temp which is the size of the "variable=value"*/
	strcpy(temp, env_name);
	strcat(temp, "=");
	strcat(temp, getenv(env_name));								/*then copy the value and catcatinate it to the env_name (which is the variable the user wants deleted) and store it in temp*/
	


	for (i = 0; environ[i] != '\0'; i++){							/*then search for any environment[x] that is equal to the user's "variable=value" and erase it when it is found*/
	if (strcmp(temp, environ[i]) == 0){
	strcpy(environ[i], " ");
	}
	}



	free(temp);										/*frees the allocated memory*/
	return 0;
}
