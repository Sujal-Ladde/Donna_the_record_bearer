#ifndef CONTACT_H
#define CONTACT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> //
#include <stdbool.h> //

#ifdef __cplusplus
extern "C" {
#endif

// Node for singly linked list of contacts
typedef struct Node { //
    char name[50]; //
    char phone[50]; //
    char email[50]; //
    struct Node *next; //
} Node;

extern Node *head; // extern so it can be accessed by contact.c
extern int count; // extern so it can be accessed by contact.c

// Structure for returning contact data to Python
typedef struct ContactData {
    char name[50];
    char phone[50];
    char email[50];
} ContactData;

// --- Library-friendly C functions to be wrapped by Pybind11 ---

/**
 * @brief Initializes the contact list from "contacts.csv".
 * Call this once when the application/library starts.
 */
void initialize_library();

/**
 * @brief Adds a new contact.
 * @param name Name of the contact.
 * @param phone Phone number of the contact.
 * @param email Email address of the contact.
 * @return 1 on success.
 * -1 if name is invalid.
 * -2 if phone number is invalid.
 * -3 if email is invalid.
 * -4 if phone number already exists.
 * -5 if email already exists.
 * -6 if memory allocation failed.
 */
int add_contact_py(const char* name, const char* phone, const char* email);

/**
 * @brief Gets the current number of contacts.
 * @return Total number of contacts.
 */
int get_contacts_count_py();

/**
 * @brief Retrieves all contacts.
 * The caller is responsible for freeing the returned array using free_contact_data_array.
 * @param num_contacts Pointer to an integer where the number of contacts will be stored.
 * @return Pointer to an array of ContactData structs, or NULL if no contacts or error.
 */
ContactData* get_all_contacts_py(int* num_contacts);

/**
 * @brief Frees the memory allocated for an array of ContactData.
 * @param data_array Pointer to the array of ContactData to be freed.
 */
void free_contact_data_array(ContactData* data_array);

/**
 * @brief Searches contacts based on a query and type.
 * The caller is responsible for freeing the returned array using free_contact_data_array.
 * @param query The search string.
 * @param search_type 1 for name, 2 for phone, 3 for email.
 * @param num_found Pointer to an integer where the number of found contacts will be stored.
 * @return Pointer to an array of ContactData structs matching the query, or NULL.
 */
ContactData* search_contacts_py(const char* query, int search_type, int* num_found);

/**
 * @brief Deletes a contact identified by its email address.
 * @param email The email of the contact to delete.
 * @return 1 if deleted, 0 if not found.
 */
int delete_contact_by_email_py(const char* email);

/**
 * @brief Edits an existing contact, identified by their original email.
 * @param old_email The current email of the contact to edit.
 * @param new_name The new name for the contact.
 * @param new_phone The new phone number for the contact.
 * @param new_email The new email for the contact.
 * @return 1 on success.
 * 0 if contact with old_email not found.
 * -1 if new_name is invalid.
 * -2 if new_phone is invalid.
 * -3 if new_email is invalid.
 * -4 if new_phone already exists for another contact.
 * -5 if new_email already exists for another contact.
 */
int edit_contact_py(const char* old_email, const char* new_name, const char* new_phone, const char* new_email);

/**
 * @brief Deletes all contacts from the list.
 */
void delete_all_contacts_py();

/**
 * @brief Saves the current contact list to "contacts.csv".
 */
void save_contacts_py();

// Sorting functions
void sort_contacts_by_name_py();
void sort_contacts_by_phone_py();
void sort_contacts_by_email_py();

// Validation and checking functions (can be called from Python)
int isvalidname(const char *name); //
int isvalidnumber(const char *number); //
int isvalidemail(const char *email); //
int checkname(const char *name);   // Optional: if needed to check if name exists
int checkphone(const char *phone); //
int checkemail(const char *email); //

#ifdef __cplusplus
}
#endif

#endif // CONTACT_H