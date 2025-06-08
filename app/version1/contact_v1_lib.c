// contact_v1_lib.c
#include "contact_v1_lib.h" // Use the API header we defined
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the struct if not already in the header (it should be)
// typedef struct {
//     char name[50];
//     char phone[50];
//     char email[50];
// } ContactRecord;

// Internal static global variables for V1
static ContactRecord *s_contacts_v1 = NULL;
static int s_count_v1 = 0;
static int s_capacity_v1 = 0; 
static const char* DEFAULT_CSV_FILE_PATH_V1 = "../data/contacts.csv"; // Default if NULL passed

// Helper to allocate string for Python to consume
// Python wrapper will be responsible for calling lib_v1_free_string
static char* allocate_and_copy_string(const char* original) {
    if (!original) { // Handle NULL input safely
        char* empty_str = (char*)malloc(1);
        if (empty_str) empty_str[0] = '\0';
        return empty_str; // Return empty string to avoid NULL pointer issues in Python
    }
    char* new_str = (char*)malloc(strlen(original) + 1);
    if (new_str) {
        strcpy(new_str, original);
    }
    return new_str; // Can be NULL if malloc fails
}

API void lib_v1_free_string(char* str_ptr) {
    if (str_ptr) {
        free(str_ptr);
    }
}

API void lib_v1_free_contact_records(ContactRecord* records, int count) {
    // This function assumes 'records' is a single block of memory allocated for 'count' ContactRecords.
    // If 'records' was an array of pointers, each pointer would need to be freed.
    if (records) {
        free(records);
    }
}

// --- Validation Functions (Adapted from your contact1.c) ---
API int lib_v1_is_valid_name(const char name[]) {
    if (name == NULL || name[0] == '\0') return 0; // Empty name is invalid
    for (int i = 0; name[i] != '\0'; i++) {
        if (!((name[i] >= 'a' && name[i] <= 'z') ||
              (name[i] >= 'A' && name[i] <= 'Z') ||
              (name[i] == ' '))) {
            return 0; // Invalid character
        }
    }
    return 1; // Valid
}

API int lib_v1_is_valid_number(const char number[]) {
    if (number == NULL) return 0;
    int length = strlen(number);
    if (length != 10) {
        return 0; // Not 10 digits
    }
    for (int i = 0; i < length; i++) {
        if (number[i] < '0' || number[i] > '9') {
            return 0; // Non-digit found
        }
    }
    return 1; // Valid (was 2 in your original, API expects 1 for true)
}

API int lib_v1_is_valid_email(const char email[]) {
    if (email == NULL) return 0;
    const char *at_symbol = strchr(email, '@');
    if (at_symbol == NULL || at_symbol == email) return 0; // No '@' or starts with '@'

    const char *dot_symbol = strrchr(at_symbol, '.'); // Last dot after '@'
    // Dot must exist, not be immediately after '@', and have something after it.
    if (dot_symbol == NULL || dot_symbol == at_symbol + 1 || dot_symbol[1] == '\0') return 0;
    
    // Example: Ensure domain part has at least one char before dot, and TLD has at least two chars
    // e.g. a@b.co is valid, a@.com is not, a@b.c is not
    if ((dot_symbol - (at_symbol + 1)) < 1) return 0; // No character between @ and last .
    if (strlen(dot_symbol + 1) < 2) return 0;         // TLD too short

    // Further checks can be added (e.g., no consecutive dots, alphanumeric before/after @ and .)
    return 1; // Simplified: Passes basic structural checks
}

// Internal helper functions to check for duplicates
static int internal_check_email_exists(const char email[]) {
    for (int i = 0; i < s_count_v1; i++) {
        if (strcmp(s_contacts_v1[i].email, email) == 0) {
            return 1; // Found
        }
    }
    return 0; // Not found
}

// --- Core API Functions ---

API int lib_v1_initialize(const char* data_file_path) {
    lib_v1_cleanup(); // Clear any existing data

    const char* file_to_open = data_file_path ? data_file_path : DEFAULT_CSV_FILE_PATH_V1;
    FILE* pF = fopen(file_to_open, "r");
    if (!pF) { // File doesn't exist or can't be opened
        s_capacity_v1 = 10; // Default initial capacity
        s_contacts_v1 = (ContactRecord*)malloc(s_capacity_v1 * sizeof(ContactRecord));
        if (!s_contacts_v1) return -1; // Malloc failure
        s_count_v1 = 0;
        return 0; // Success (initialized empty)
    }

    int lines = 0;
    char buffer[256]; // For reading lines to count them
    while (fgets(buffer, sizeof(buffer), pF) != NULL) {
        lines++;
    }
    rewind(pF);

    s_capacity_v1 = lines > 0 ? (lines + 10) : 10; // Allocate for current lines + some buffer
    s_contacts_v1 = (ContactRecord*)malloc(s_capacity_v1 * sizeof(ContactRecord));
    if (!s_contacts_v1) {
        fclose(pF);
        return -2; // Malloc failure
    }

    s_count_v1 = 0; // Reset count before loading
    // Skip header if present - assuming simple CSV without explicit header handling here
    // If your CSV has a header, you might want to fgets once before the loop.
    // For simplicity, this fscanf will try to parse from the first line.
    // Adjust format string if your CSV is different (e.g. spaces around commas)
    while (s_count_v1 < lines && s_count_v1 < s_capacity_v1 &&
           fscanf(pF, "%49[^,],%49[^,],%49[^\n]\n",
                  s_contacts_v1[s_count_v1].name,
                  s_contacts_v1[s_count_v1].phone,
                  s_contacts_v1[s_count_v1].email) == 3) {
        s_contacts_v1[s_count_v1].name[49] = '\0'; // Ensure null termination
        s_contacts_v1[s_count_v1].phone[49] = '\0';
        s_contacts_v1[s_count_v1].email[49] = '\0';
        s_count_v1++;
    }
    fclose(pF);
    return 0; // Success
}

API void lib_v1_cleanup() {
    if (s_contacts_v1) {
        free(s_contacts_v1);
        s_contacts_v1 = NULL;
    }
    s_count_v1 = 0;
    s_capacity_v1 = 0;
}

API char* lib_v1_add_contact(const char* name, const char* phone, const char* email) {
    if (!lib_v1_is_valid_name(name)) return allocate_and_copy_string("Error: Invalid name format.");
    if (!lib_v1_is_valid_number(phone)) return allocate_and_copy_string("Error: Invalid phone number (must be 10 digits).");
    if (!lib_v1_is_valid_email(email)) return allocate_and_copy_string("Error: Invalid email format.");
    
    if (internal_check_email_exists(email)) return allocate_and_copy_string("Error: Email already exists.");
    // Add other uniqueness checks if needed (e.g., for phone or name)

    if (s_count_v1 >= s_capacity_v1) {
        int new_capacity = s_capacity_v1 > 0 ? s_capacity_v1 * 2 : 10;
        ContactRecord* temp = (ContactRecord*)realloc(s_contacts_v1, new_capacity * sizeof(ContactRecord));
        if (!temp) return allocate_and_copy_string("Error: Memory reallocation failed.");
        s_contacts_v1 = temp;
        s_capacity_v1 = new_capacity;
    }

    strncpy(s_contacts_v1[s_count_v1].name, name, 49); s_contacts_v1[s_count_v1].name[49] = '\0';
    strncpy(s_contacts_v1[s_count_v1].phone, phone, 49); s_contacts_v1[s_count_v1].phone[49] = '\0';
    strncpy(s_contacts_v1[s_count_v1].email, email, 49); s_contacts_v1[s_count_v1].email[49] = '\0';
    s_count_v1++;

    return allocate_and_copy_string("Contact added successfully.");
}

API ContactRecord* lib_v1_get_all_contacts(int* out_count) {
    if (!out_count) return NULL;
    *out_count = 0;
    if (s_count_v1 == 0 || s_contacts_v1 == NULL) return NULL;

    ContactRecord* records_copy = (ContactRecord*)malloc(s_count_v1 * sizeof(ContactRecord));
    if (!records_copy) return NULL; // Malloc failure

    memcpy(records_copy, s_contacts_v1, s_count_v1 * sizeof(ContactRecord));
    
    *out_count = s_count_v1;
    return records_copy;
}

API ContactRecord* lib_v1_search_contacts(const char* query, int search_type, int* out_count) {
    if (!out_count || !query) {
        if(out_count) *out_count = 0;
        return NULL;
    }
    *out_count = 0;
    if (s_count_v1 == 0) return NULL;

    ContactRecord* matches = (ContactRecord*)malloc(s_count_v1 * sizeof(ContactRecord)); // Max possible matches
    if (!matches) return NULL;

    int current_match_count = 0;
    for (int i = 0; i < s_count_v1; i++) {
        int found = 0;
        switch (search_type) {
            case 1: // Name
                if (strstr(s_contacts_v1[i].name, query)) found = 1;
                break;
            case 2: // Phone
                if (strstr(s_contacts_v1[i].phone, query)) found = 1;
                break;
            case 3: // Email
                if (strstr(s_contacts_v1[i].email, query)) found = 1;
                break;
        }
        if (found) {
            memcpy(&matches[current_match_count++], &s_contacts_v1[i], sizeof(ContactRecord));
        }
    }

    if (current_match_count == 0) {
        free(matches);
        return NULL;
    }

    // Optional: Realloc to actual size if significantly smaller than s_count_v1
    ContactRecord* final_matches = (ContactRecord*)realloc(matches, current_match_count * sizeof(ContactRecord));
     if (!final_matches && current_match_count > 0) { // realloc failed but matches had content
        *out_count = current_match_count; // return original matches buffer
        return matches; 
    }


    *out_count = current_match_count;
    return final_matches ? final_matches : matches; // return realloced or original if realloc failed
}


API char* lib_v1_edit_contact(const char* old_email_id, const char* new_name, const char* new_phone, const char* new_email) {
    if (!lib_v1_is_valid_name(new_name)) return allocate_and_copy_string("Error: Invalid new name format.");
    if (!lib_v1_is_valid_number(new_phone)) return allocate_and_copy_string("Error: Invalid new phone number.");
    if (!lib_v1_is_valid_email(new_email)) return allocate_and_copy_string("Error: Invalid new email format.");

    int found_idx = -1;
    for (int i = 0; i < s_count_v1; i++) {
        if (strcmp(s_contacts_v1[i].email, old_email_id) == 0) {
            found_idx = i;
            break;
        }
    }

    if (found_idx == -1) return allocate_and_copy_string("Error: Contact to edit not found (by old email).");

    // Check if new email already exists (if it's different from the old one and belongs to another contact)
    if (strcmp(old_email_id, new_email) != 0 && internal_check_email_exists(new_email)) {
        return allocate_and_copy_string("Error: New email already exists for another contact.");
    }
    // Add similar checks for new_name and new_phone if they need to be unique and changed

    strncpy(s_contacts_v1[found_idx].name, new_name, 49); s_contacts_v1[found_idx].name[49] = '\0';
    strncpy(s_contacts_v1[found_idx].phone, new_phone, 49); s_contacts_v1[found_idx].phone[49] = '\0';
    strncpy(s_contacts_v1[found_idx].email, new_email, 49); s_contacts_v1[found_idx].email[49] = '\0';
    
    return allocate_and_copy_string("Contact updated successfully.");
}

API int lib_v1_delete_contact_by_email(const char* email) {
    int found_idx = -1;
    for (int i = 0; i < s_count_v1; i++) {
        if (strcmp(s_contacts_v1[i].email, email) == 0) {
            found_idx = i;
            break;
        }
    }

    if (found_idx == -1) return -1; // Not found

    // Shift elements
    for (int i = found_idx; i < s_count_v1 - 1; i++) {
        s_contacts_v1[i] = s_contacts_v1[i + 1];
    }
    s_count_v1--;
    // Optional: Clear the last (now unused) element if desired, though not strictly necessary
    // memset(&s_contacts_v1[s_count_v1], 0, sizeof(ContactRecord));
    return 0; // Success
}

API int lib_v1_delete_all_contacts() {
    s_count_v1 = 0;
    // The allocated memory for s_contacts_v1 is kept for potential re-use or freed by lib_v1_cleanup.
    // If you want to free immediately and realloc on next add:
    // if(s_contacts_v1) free(s_contacts_v1); s_contacts_v1 = NULL; s_capacity_v1 = 0;
    return 0; // Success
}

// Bubble Sort implementations
static void internal_bubble_sort(int sort_type) {
    ContactRecord temp;
    for (int i = 0; i < s_count_v1 - 1; i++) {
        for (int j = 0; j < s_count_v1 - i - 1; j++) {
            int comparison = 0;
            if (sort_type == 1)      // Name
                comparison = strcmp(s_contacts_v1[j].name, s_contacts_v1[j + 1].name);
            else if (sort_type == 2) // Phone
                comparison = strcmp(s_contacts_v1[j].phone, s_contacts_v1[j + 1].phone);
            else if (sort_type == 3) // Email
                comparison = strcmp(s_contacts_v1[j].email, s_contacts_v1[j + 1].email);
            
            if (comparison > 0) {
                temp = s_contacts_v1[j];
                s_contacts_v1[j] = s_contacts_v1[j + 1];
                s_contacts_v1[j + 1] = temp;
            }
        }
    }
}

API int lib_v1_sort_contacts(int sort_type) {
    if (sort_type < 1 || sort_type > 3) return -1; // Invalid sort type
    if (s_count_v1 < 2) return 0; // No need to sort
    internal_bubble_sort(sort_type);
    return 0; // Success
}

API int lib_v1_save_contacts(const char* data_file_path) {
    const char* file_to_save = data_file_path ? data_file_path : DEFAULT_CSV_FILE_PATH_V1;
    FILE* pF = fopen(file_to_save, "w");
    if (!pF) return -1; // Error opening file

    // Optional: write a header if your CSV format expects one
    // fprintf(pF, "Name,Phone,Email\n"); 

    for (int i = 0; i < s_count_v1; i++) {
        if (fprintf(pF, "%s,%s,%s\n",
                    s_contacts_v1[i].name,
                    s_contacts_v1[i].phone,
                    s_contacts_v1[i].email) < 0) {
            fclose(pF);
            return -2; // Error writing to file
        }
    }
    fclose(pF);
    return 0; // Success
}