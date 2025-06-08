#ifndef CONTACT_H
#define CONTACT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
// Node for singly linked list of contacts
typedef struct Node {
    char name[50];
    char phone[50];
    char email[50];
    struct Node *next;
} Node;

extern Node *head;
extern int count;

// Core functionality
void initialize();
void loginPage();
void infoscreen();
void endScreen();
void save();
void Exit();

// CRUD operations
void addcontact();
void displaycontacts();
void searchcontact();
void searchByName();
void searchByNumber();
void searchByEmail();
void deletecontact();
void Deleteall();
void editcontact();

// Utility
void clearBuffer();
void clearInputBuffer();
void my_pause();
int isvalidname(const char *name);
int isvalidnumber(const char *number);
int isvalidemail(const char *email);
int checkname(const char *name);
int checkphone(const char *phone);
int checkemail(const char *email);

// Sorting
void displaySortedContacts();
void sortByName();
void sortByNumber();
void sortByEmail();

#endif // CONTACT_H
