// contact_v1_lib.h
#ifndef CONTACT_V1_LIB_H
#define CONTACT_V1_LIB_H

#ifdef _WIN32
    #define API __declspec(dllexport) // For Windows DLL
#else
    #define API // For Linux/macOS SO
#endif

// Structure to pass contact data between C and Python
typedef struct {
    char name[50];
    char phone[50];
    char email[50];
} ContactRecord;

// Initialization and Cleanup
API int lib_v1_initialize(const char* data_file_path); // Returns 0 on success, -1 on error
API void lib_v1_cleanup();                            // Frees allocated memory

// Core Operations (returning char* for status messages. Caller must free with lib_v1_free_string)
API char* lib_v1_add_contact(const char* name, const char* phone, const char* email);
API char* lib_v1_edit_contact(const char* old_email_id, const char* new_name, const char* new_phone, const char* new_email);

// Data Retrieval (caller must free records with lib_v1_free_contact_records)
API ContactRecord* lib_v1_get_all_contacts(int* out_count);
API ContactRecord* lib_v1_search_contacts(const char* query, int search_type, int* out_count); // search_type: 1=name, 2=phone, 3=email

// Deletion (returning int for status: 0 for success, specific error codes or -1 for failure)
API int lib_v1_delete_contact_by_email(const char* email);
API int lib_v1_delete_all_contacts();

// Sorting (returning int for status)
API int lib_v1_sort_contacts(int sort_type); // sort_type: 1=name, 2=phone, 3=email

// Persistence (returning int for status)
API int lib_v1_save_contacts(const char* data_file_path);

// Validation functions (returning 1 for true/valid, 0 for false/invalid)
API int lib_v1_is_valid_name(const char* name);
API int lib_v1_is_valid_number(const char* number);
API int lib_v1_is_valid_email(const char* email); // Simpler: 1 valid, 0 invalid

// Utility for Python to free memory allocated by C for strings
API void lib_v1_free_string(char* str_ptr);
API void lib_v1_free_contact_records(ContactRecord* records, int count); // If records themselves are copied, only outer array might need free.
                                                                    // If array of pointers, each record might need free.
                                                                    // For simplicity, we'll assume get_all_contacts returns a single block.

#endif // CONTACT_V1_LIB_H