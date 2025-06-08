// contact_v2_lib.h
#ifndef CONTACT_V2_LIB_H
#define CONTACT_V2_LIB_H

#include <stdio.h> // For FILE if used in struct, or for general use
#include <stdlib.h> // For malloc, free
#include <string.h> // For strcpy, strcmp, etc.
// Add any other standard headers your Node struct or API functions might need implicitly

#ifdef _WIN32
    #define API __declspec(dllexport)
#else
    #define API
#endif

// >>>>> THIS IS THE CRUCIAL PART <<<<<
// Define Node structure for the linked list (INTERNAL to V2 logic, but needed by its functions)
typedef struct Node {
    char name[50];
    char phone[50];
    char email[50];
    struct Node *next;
} Node;
// >>>>> END CRUCIAL PART <<<<<


// Structure to pass contact data between C and Python
// This MUST be identical to the one in contact_v1_lib.h
typedef struct {
    char name[50];
    char phone[50];
    char email[50];
} ContactRecord;

// API Function Declarations
API int lib_v2_initialize(const char* data_file_path);
API void lib_v2_cleanup();
// ... rest of the API function declarations ...
API char* lib_v2_add_contact(const char* name, const char* phone, const char* email);
API char* lib_v2_edit_contact(const char* old_email_id, const char* new_name, const char* new_phone, const char* new_email);
API ContactRecord* lib_v2_get_all_contacts(int* out_count);
API ContactRecord* lib_v2_search_contacts(const char* query, int search_type, int* out_count);
API int lib_v2_delete_contact_by_email(const char* email);
API int lib_v2_delete_all_contacts();
API int lib_v2_sort_contacts(int sort_type);
API int lib_v2_save_contacts(const char* data_file_path);
API int lib_v2_is_valid_name(const char* name);
API int lib_v2_is_valid_number(const char* number);
API int lib_v2_is_valid_email(const char* email);
API void lib_v2_free_string(char* str_ptr);
API void lib_v2_free_contact_records(ContactRecord* records, int count);

#endif // CONTACT_V2_LIB_H