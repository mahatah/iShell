%{						
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "iShell_build.c"
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>


%}


%%

[-+a-zA-Z.0-9/_<>|~]+ {						/* looks for all char and ints (also, lex will not allow comments above this line, above are the libraries needed to make the magic happen and the lex implementation*/
					
char *command_arg;
	

	/*this if statement checks if it is the first run of the parser and if main_command is not clear. If so then it clears the struct instance. */
	
	if (user_command[storedi].main_command[0] != '\0' && lex_loop == 0){	
	user_command[storedi].main_command[0] = '\0';
	}

	if (lex_loop == 0){					/*this allows me to store the user command into the struct user_command on first run through*/
	strcpy(user_command[storedi].main_command, yytext);
	strcpy(user_command[storedi].arguement, "\0");
	lex_loop = 1;
	} else if (lex_loop == 1) {
	strcat(user_command[storedi].arguement, yytext);	/*This adds yytext to the arguement part of the struct*/
	lex_loop = 2;	
	} else {
	strcat(user_command[storedi].arguement, " ");
	strcat(user_command[storedi].arguement, yytext);
	}







}				

[ \t]+										/* looks for all white spaces and eliminates them */

[\n] {										/* THIS IS WHAT HAPPENS IF A NEW LINE IS FOUND */
	int iholder = 0;
	int check_size;
	char *ptr_check_size = (char*)malloc(5);


	 ptr_HIST_SIZE = getenv("HISTSIZE");					/*this gets the enviornment variable*/
	 HIST_SIZE = (int)atoi(ptr_HIST_SIZE);

	if (lex_loop == 0){							/*if user has just pressed enter then the first loop is /n, return error 4 to loop user*/
	return 4;
	}						
	
	find_num_argue();							/*This sets the number of arguements in the loop in num_arg in the struct*/


	if (strcmp(user_command[storedi].main_command, "cd")==0){		/*checks CD command*/	
	change_dir();
	}
	else if (strcmp(user_command[storedi].main_command, "exit")==0){
	return 3;
	}
	else if (strcmp(user_command[storedi].main_command, "env")==0){
	environ_disp(yytext);
	}
	else if (strcmp(user_command[storedi].main_command, "history")==0){ 		/*this crazy mess will print history HIST_SIZE in order however it is set*/
	history();
	}

	else if (strcmp(user_command[storedi].main_command, "unsetenv")==0){		/*this unsets an environment variable of the users choice*/
	unset_env();
	}

	else if (strcmp(user_command[storedi].main_command, "setenv")==0){
	if (user_command[storedi].num_arg < 2)	{				/*if no arguement is provided the send arguement error*/
	no_arguement_error();
	}
	else if (check_two_argue() == 0) {					/*check to make sure there are two arguements*/
	set_environ();
	}
	 

	}else {
	check_paths();								/*if no other command is found check the paths directories to see if it exists there. If it does then execute that code.*/
	}
	
	 ptr_check_size = getenv("HISTSIZE");					/*this finds the enviornment variable HISTSIZE and makes the global variable HIST_SIZE equal to it*/
	 check_size = (int)atoi(ptr_check_size);

	if (HIST_SIZE != check_size){						/*this if statement copies the structs above the HISTSIZE limits and sends then respectivly below the limit*/

    	for (iholder=0; iholder <= 100; iholder++) {     			 /*adds 1 to iholder for the following*/
   	 user_command[iholder].main_command[0] = '\0';      			 /*sets all main_commands to null*/
    	user_command[iholder].arguement[0] = '\0';        			/*sets all arguements to null*/
    	user_command[iholder].num_arg = 0;    
	}	
	HIST_SIZE = check_size;
	times_ran = 0;								/*resets times ran to 0 because times ran is only useful when it describes the times ran at the HIST_SIZE limit*/
	}

	if (storedi >= HIST_SIZE - 1){						/*Checks to see if the number of structs is greater than or equal to hist_size and if it is, starts overwriting the structs*/
	storedi = -1;
	}
return 0;
}





%%


int main (int argc, char *argv[]) {
    int iholder = 0;
    int ret = 0;


    setenv_minix("HISTSIZE", "10", 1);				/*this sets the history size to the default 10*/
         ptr_HIST_SIZE = getenv("HISTSIZE");			/*this gets the enviornment variable and sets the global variable HIST_SIZE to 10*/
	 HIST_SIZE = (int)atoi(ptr_HIST_SIZE);
	
     
    for (iholder=0; iholder <= 100; iholder++) {     			 /*adds 1 to iholder for the following*/
    user_command[iholder].main_command[0] = '\0';      			 /*sets all main_commands to null*/
    user_command[iholder].arguement[0] = '\0';        			/*sets all arguements to null*/
    user_command[iholder].num_arg = 0;    
	}
   	
	
	while(1){
	int return_catch = 0;	
	printf("iShell>");	
	return_catch=yylex();						/*lex is the parser used in order to tokenize the user's commands*/
	
	while (return_catch == 4){					/*checks to see if the user just pressed enter without entry, if so, loop him*/
	printf("iShell>");
	return_catch=yylex();
	}

	times_ran++;							/*keeps track of the number of times we have looped*/
	storedi++;
	if (return_catch == 3){
	printf("Bye Bye\n");
	return 0;	
	}
	lex_loop = 0;							/*resets the global variable lex_loop to 0 for the next input*/
	
}	
return 0;


}
int yywrap() {
	return 0;
}

/******************************************************************************************
*Program: iShell
* Author: Matthew Price
* February 19, 2009
*******************************************************************************************/
