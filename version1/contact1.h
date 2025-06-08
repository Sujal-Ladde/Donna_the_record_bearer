
#ifndef CONTACT_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include<windows.h>  // Sleep() => for windows OS 
#include <unistd.h>

void initialize();
void displaySortedContacts();
void sortByEmail();
void sortByNumber();
void sortByName();
void infoscreen();
void loginPage();
void searchByName();
void searchByEmail();
void searchByNumber();
void searchcontact();
void deletecontact();
void displaycontacts();
void editcontact();
void addcontact();
void clearBuffer();
void clearinputbuffer();
void Deleteall();
void endScreen();
int checkemail(char email[]);
int checkphone(char phone[]);
int checkname(char name[]);
void addgiven();
void Exit();
void save();


#endif
