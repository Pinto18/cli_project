//File for handling user login related functions
//Nicholas DiPinto
//CS415 Assignment 1
#ifndef _USER_H
#define _USER_H

/*Pre Processor Directive *****************************************/
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "minishell.h"
/******************************************************************/

/***** Global Constants *******************************************/
const char *usersFile = "users.txt";
const char *securityQ1 = "What is the name of your first pet?";
const char *securityQ2 = "What elementary school did you attend?";
const char *securityQ3 = "What city were you born in?";
/******************************************************************/

/****** Global Variables ******************************************/
char curUser[BUFSIZ];
/******************************************************************/

/***** Function Prototypes ****************************************/
int login();
char* createFileName(char *user, char*fileExt); 
int createUserFile();
int changePassword(char *userName, int cmdCall);  //if called from command line to change password, this function will behave differently
int setCurUser(char *user);
int securityChallenge();
/******************************************************************/

int login()  //logging a user into the program
{
   char userName[BUFSIZ], *password; 
   char buf[BUFSIZ];   //file input buffer
   char *userFile, *passFile;  //for values inside the file
   char *fileName;
   FILE *inFile;

   /////////////////////////////Receive User Input////////////////////////
   printf(">>Username: ");
   scanf("%s", userName);
   password = getpass(">>Password: ");
   //////////////////////////////////////////////////////////////////////

   ////////////////////////////Create File Name ////////////////////////
   fileName = createFileName(userName, ".txt");
   /////////////////////////////////////////////////////////////////////

   ////////////////////////Accessing File//////////////////////////////
   inFile = fopen(fileName, "r");
   if(inFile)  //check that file can be opened
   {
      while(!feof(inFile))  //loop until you have reached the end of the file
      {
         buf[0] = '\0';
         fscanf(inFile, "%s", buf);  //read one line and store it in buf array
         if(strlen(buf) == 0)  //start again at the top of the loop if it is reading an empty line
            continue;
         userFile = buf;
         passFile = strchr(buf,':'); //points to delimiter in buffer
         passFile[0] = '\0';  //change the delimiter to a null character
         passFile++;  //move to the next character
         if((strcmp(userFile, userName) == 0) && (strcmp(crypt(password, passFile), passFile) == 0)) //if the values in the file match with the inputted values
         {   
            setCurUser(userName);
            return 1;  //succesful login       
         }
         else
            return 0;   //unsuccesful login
      }
   }
   fclose(inFile);
   return 0;
}

int createUserFile()  //creating a new user
{
   char salt[2]; //salt for crypt function
   const char *saltChars = "abcdefghiejklmnopqrstuvwxyz"/*Range of character supported*/
   "ABCDEFGHIJKLMNOPQRSTUVWXYZ" /*as a value for salt in crypt()*/
   "0123456789";

   char userName[BUFSIZ], password1[BUFSIZ], password2[BUFSIZ], *buf;
   char answer1[BUFSIZ], answer2[BUFSIZ], answer3[BUFSIZ];
   char *fileName;
   FILE *newFile;

   ///////////////Build the Salt /////////////////////
   srand(time(NULL));
   salt[0] = saltChars[rand() % 62];  //62 is the range of the salt values
   salt[1] = saltChars[rand() % 62];
   
   /////////////User Input///////////////////////////
   printf(">>Username: ");
   scanf("%s", userName);
   fileName = createFileName(userName, ".txt");
   newFile = fopen(fileName, "a+");

   do
   {
      buf = getpass(">>Password: ");
      sprintf(password1, "%s", buf); //coping password to a stable pointer
      buf = getpass(">>Enter Again: ");
      sprintf(password2, "%s", buf);
      if(strcmp(password1, password2) != 0)  //compare the two passwords for validation
         printf("\nPasswords do not match. Try again.");
   }
   while(strcmp(password1, password2) != 0);

   buf = crypt(password1, salt);
   ///////////////////////Setting Answers to Security Questions /////////////////
   printf(">>Answer these following questions: \n");
   printf(">>%s\n",securityQ1);
   printf(">>Answer: ");
   scanf("%s", answer1);
   printf(">>%s\n", securityQ2);
   printf(">>Answer: ");
   scanf("%s", answer2);
   printf(">>%s\n", securityQ3);
   printf(">>Answer: ");
   scanf("%s", answer3);
   /////////////////////////////////////////////////////////////////////////////
   if(newFile)  //the file was able to open
   {
      fprintf(newFile, "%s:%s\n", userName,buf);
      buf = crypt(answer1, salt);
      fprintf(newFile, "%s\n", buf);
      buf = crypt(answer2, salt);
      fprintf(newFile, "%s\n", buf);
      buf = crypt(answer3, salt);
      fprintf(newFile, "%s\n", buf);
   }
   else   //program was not able to open file
      printf("ERROR: Could not open file.\n");
   fclose(newFile);
   return 0;
}

char* createFileName(char *user, char* fileExt)  //formatting a username into a file name
{
   size_t len = strlen(user) + strlen(fileExt);
   char *ret = (char*)malloc(len * sizeof(char) + 1);
   return strcat(user, fileExt);
}

int setCurUser(char *user)
{
   int index = 0;
   
   while(user[index] != '\0')
   {
      curUser[index] = user[index];   //copy in the logged in user name to a global variable
      index++;
   }
   curUser[index] = '\0';   //place a null terminator at the end of the char array
   return 0;
}

int changePassword(char *userName, int cmdCall)  //changing a user's password
{
   char salt[2];
   const char *saltChars = "abcdefghijklmnopqrstuvwxyz" /*Range of characters supported*/
   "ABCDEFGHIJKLMNOPQRSTUVWXYZ" /*as a value for salt in crypt*/
   "0123456789";

   char password1[BUFSIZ], *curPassword, newPassword1[BUFSIZ], newPassword2[BUFSIZ], *buf, *encryptPass;
   char *userBuf;
   char *fileName;
   FILE *userFile;

   /////////////////BUILD SALT////////////////////////
   srand(time(NULL));
   salt[0] = saltChars[rand() % 62];
   salt[1] = saltChars[rand() % 62];

   userFile = fopen(userName, "r+");
   if(cmdCall)  //user is changing password because they entered the command to do so
   {
      do
      {
         buf = getpass(">>Enter current password: ");
         sprintf(password1, "%s", buf);
         while(!feof(userFile))  //obtain the user's current password
         {
            buf[0] = '\0';
            fscanf(userFile, "%s", buf);  //read in a single line
            if(strlen(buf) == 0)
               continue;
            userBuf = buf;
            curPassword = strchr(buf, ':');
            curPassword[0] = '\0';
            curPassword++;
         }
         if(strcmp(crypt(password1, curPassword), curPassword) != 0)
            printf("ERROR: Incorrect Password. Try Again.\n");
      }
      while(strcmp(crypt(password1,curPassword), curPassword) != 0);  //stop when the user inputs the correct password
   }
   do
   {
      buf = getpass(">>Enter a new password: ");
      sprintf(newPassword1, "%s", buf);
      buf = getpass(">>Enter again: ");
      sprintf(newPassword2, "%s", buf);
      if(strcmp(newPassword1, newPassword2) != 0)  //confirm that user knows their new password
         printf("ERROR: Passwords do not match.Try Again.\n");
   }
   while(strcmp(newPassword1, newPassword2) != 0);
   encryptPass = crypt(newPassword1, salt);  //encrypt the new password
   fseek(userFile, strlen(userName) + 1, SEEK_SET);  //find where the old password is located
   fputs(encryptPass, userFile);  //enter the new password into the user file
   fclose(userFile);
   printf(">>Password Changed.\n");
   return 0;
}
 int securityChallenge()
{
   char salt[2];
   const char *saltChars = "abcdefghijklmnopqrstuvwxyz"
   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
   "0123456789";
   char userName[BUFSIZ], buf[BUFSIZ];
   char answer1[BUFSIZ], answer2[BUFSIZ], answer3[BUFSIZ];  //answers entered by user
   char fileA1[BUFSIZ], fileA2[BUFSIZ], fileA3[BUFSIZ]; //answers to security questions inside the user file
   char *fileName;
   FILE *userFile;

   printf(">>Let's try some questions:\n");
   printf(">>Enter username: ");
   scanf("%s", userName);
   fileName = createFileName(userName, ".txt");
   userFile = fopen(fileName, "r");
   setCurUser(userName);
   
   if(userFile)
   {
      while(!feof(userFile))
      {
         ////////First Security Question////////////////////
         buf[0] = '\0';
         fileA1[0] = '\0';
         fileA2[0] = '\0';
         fileA3[0] = '\0';
         printf(">>%s\n", securityQ1);
         printf(">>Answer: ");
         scanf("%s", answer1);
         fgets(buf, BUFSIZ, userFile);  //not the line containing the answer
         fgets(fileA1, BUFSIZ, userFile);  //this is the line containing the answer
         fileA1[strlen(fileA1)-1] = '\0';
         if(strcmp(crypt(answer1, fileA1), fileA1) == 0) //user got the first question correct
         {
            fclose(userFile);
            return 1;
         }
         else
         {
            printf(">>ERROR: Incorrect Answer.\n>>Here's another question:\n>>%s\n", securityQ2);
            printf(">>Answer: ");
            scanf("%s", answer2);
            /////////////Second Securtity Question ///////////////////////////
            fgets(fileA2, BUFSIZ, userFile);
            fileA2[strlen(fileA2) - 1] = '\0';
            if(strcmp(crypt(answer2, fileA2), fileA2) == 0)  //user got the second question correct
            {
               fclose(userFile);   
               return 1;
            }
            else
            {
               printf(">>ERROR: Incorrect Answer.\n>>Here's your last question:\n>>%s\n", securityQ3);
               printf(">>Answer: ");
               scanf("%s", answer3);
               ///////////// Third Security Question ////////////////////////
               fgets(fileA3, BUFSIZ, userFile);
               fileA3[strlen(fileA3) - 1] = '\0';
               if(strcmp(crypt(answer3, fileA3), fileA3) == 0)
               {
                  fclose(userFile);
                  return 1;
               }
               else
               {
                  fclose(userFile);
                  return 0;
               }
            }
         }
      }
   }
   else   //file does not exist
   {
      fclose(userFile);
      printf("ERROR: Account Does Not Exist!\n");
      return 0;
   }
   return 0;
}
#endif
