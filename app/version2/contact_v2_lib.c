// contact_v2_lib.c
#include "contact_v2_lib.h" // Your new API header
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>   
#include <stdbool.h> 

// Internal static global variables for V2 (linked list)
static Node *s_head_v2 = NULL;
static int s_count_v2 = 0;
// IMPORTANT: Ensure this path is correct relative to where your C library will be run from,
// or preferably, always pass the full path from Python.
// For consistency with app.py, data files will be in a 'data' subdirectory of the project root.
// The Python wrapper should construct the full path to "data/contacts.csv".
// This default is less critical if Python always provides the path.
static const char* DEFAULT_CSV_FILE_PATH_V2 = "contacts_v2_default.csv"; 

// Helper to allocate string for Python to consume
static char* allocate_and_copy_string_v2(const char* original) {
    if (!original) {
        char* empty_str = (char*)malloc(1);
        if (empty_str) empty_str[0] = '\0';
        return empty_str;
    }
    char* new_str = (char*)malloc(strlen(original) + 1);
    if (new_str) {
        strcpy(new_str, original);
    }
    return new_str; 
}

API void lib_v2_free_string(char* str_ptr) {
    if (str_ptr) {
        free(str_ptr);
    }
}

API void lib_v2_free_contact_records(ContactRecord* records, int count) {
    if (records) {
        free(records);
    }
}

// --- Validation Functions ---
API int lib_v2_is_valid_name(const char name[]) {
    if (name == NULL || name[0] == '\0') return 0;
    for (int i = 0; name[i] != '\0'; i++) {
        if (!((name[i] >= 'a' && name[i] <= 'z') ||
              (name[i] >= 'A' && name[i] <= 'Z') ||
              (name[i] == ' '))) {
            return 0;
        }
    }
    return 1;
}

API int lib_v2_is_valid_number(const char number[]) {
    if (number == NULL) return 0;
    int length = strlen(number);
    if (length != 10) return 0;
    for (int i = 0; i < length; i++) {
        if (number[i] < '0' || number[i] > '9') return 0;
    }
    return 1;
}

API int lib_v2_is_valid_email(const char email[]) {
    if (email == NULL) return 0;
    const char *at_symbol = strchr(email, '@');
    if (!at_symbol || at_symbol == email) return 0;
    const char *dot_symbol = strrchr(at_symbol, '.');
    if (!dot_symbol || dot_symbol == at_symbol + 1 || dot_symbol[1] == '\0') return 0;
    if ((dot_symbol - (at_symbol + 1)) < 1) return 0; 
    if (strlen(dot_symbol + 1) < 2) return 0;
    return 1;
}

// Internal helper functions to check for duplicates
static int internal_check_email_exists_v2(const char email[]) {
    for (Node *p = s_head_v2; p; p = p->next) {
        if (strcmp(p->email, email) == 0) return 1;
    }
    return 0;
}

// --- Core API Functions ---
API int lib_v2_initialize(const char* data_file_path) {
    lib_v2_cleanup(); 
    const char* file_to_open = data_file_path; // Python wrapper MUST provide a valid path
    if (!file_to_open) {
        // Fallback if Python sends NULL, though wrapper should prevent this.
        // This might indicate a logic error in the Python wrapper if it occurs.
        // For now, let's assume an empty list if no path.
        return 0; 
    }

    FILE* pF = fopen(file_to_open, "r");
    if (!pF) { 
        return 0; // File doesn't exist, treat as empty list, return success.
    }

    char line[256]; // Increased buffer slightly for safety
    char name_buf[50], phone_buf[50], email_buf[50];

    // Skip header if present (simple one-line skip) - adjust if your CSV has no header
    if (fgets(line, sizeof(line), pF) == NULL) { // Check if file is empty or just header
        fclose(pF);
        return 0; // Empty file or only header
    }
    // A more robust header check would be to see if it matches "Name,Phone,Email"

    while (fgets(line, sizeof(line), pF)) { // Start reading data lines
        Node *n = (Node*)malloc(sizeof(Node));
        if (!n) { 
            fclose(pF); lib_v2_cleanup(); return -2; 
        }
        
        // Initialize node fields to empty strings for safety before sscanf
        n->name[0] = '\0'; n->phone[0] = '\0'; n->email[0] = '\0'; n->next = NULL;

        if (sscanf(line, "%49[^,],%49[^,],%49[^\n]", n->name, n->phone, n->email) == 3) {
            n->name[49] = '\0'; n->phone[49] = '\0'; n->email[49] = '\0';
            n->next = s_head_v2;
            s_head_v2 = n;
            s_count_v2++;
        } else {
            free(n); 
        }
    }
    fclose(pF);
    return 0; 
}

API void lib_v2_cleanup() {
    Node *current = s_head_v2; Node *next_node;
    while (current != NULL) { next_node = current->next; free(current); current = next_node; }
    s_head_v2 = NULL; s_count_v2 = 0;
}

API char* lib_v2_add_contact(const char* name, const char* phone, const char* email) {
    if (!lib_v2_is_valid_name(name)) return allocate_and_copy_string_v2("Error: Invalid name format.");
    if (!lib_v2_is_valid_number(phone)) return allocate_and_copy_string_v2("Error: Invalid phone number.");
    if (!lib_v2_is_valid_email(email)) return allocate_and_copy_string_v2("Error: Invalid email format.");
    if (internal_check_email_exists_v2(email)) return allocate_and_copy_string_v2("Error: Email already exists.");

    Node *newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) return allocate_and_copy_string_v2("Error: Memory allocation failed.");
    strncpy(newNode->name, name, 49); newNode->name[49] = '\0';
    strncpy(newNode->phone, phone, 49); newNode->phone[49] = '\0';
    strncpy(newNode->email, email, 49); newNode->email[49] = '\0';
    newNode->next = s_head_v2; s_head_v2 = newNode; s_count_v2++;
    return allocate_and_copy_string_v2("Contact added successfully (LinkedList).");
}

API ContactRecord* lib_v2_get_all_contacts(int* out_count) {
    if (!out_count) return NULL;
    *out_count = 0;
    if (s_count_v2 == 0 || !s_head_v2) return NULL;
    ContactRecord* records_array = (ContactRecord*)malloc(s_count_v2 * sizeof(ContactRecord));
    if (!records_array) return NULL;
    Node *current = s_head_v2; int i = 0;
    for (i = 0; i < s_count_v2 && current; i++, current = current->next) {
        strncpy(records_array[i].name, current->name, 49); records_array[i].name[49] = '\0';
        strncpy(records_array[i].phone, current->phone, 49); records_array[i].phone[49] = '\0';
        strncpy(records_array[i].email, current->email, 49); records_array[i].email[49] = '\0';
    }
    // If loop terminated because !current but i < s_count_v2, then list is corrupted or count is wrong.
    if (i != s_count_v2) { 
        // This indicates an inconsistency. For safety, return what was successfully copied.
        // Or, you could free records_array and return NULL to signal a more severe internal error.
        // For now, we'll return what we got and update the count.
        s_count_v2 = i; // Correct the count based on actual nodes traversed
    }
    *out_count = s_count_v2;
    if(s_count_v2 == 0 && i == 0 && records_array != NULL) { // If list became empty due to correction
        free(records_array);
        return NULL;
    }
    return records_array;
}

API ContactRecord* lib_v2_search_contacts(const char* query, int search_type, int* out_count) {
    // ... (Implementation remains largely the same as the robust one provided before) ...
    // (Ensure it handles s_head_v2 being NULL correctly)
    if (!out_count || !query) { if(out_count) *out_count = 0; return NULL; }
    *out_count = 0;
    if (s_count_v2 == 0 || !s_head_v2) return NULL;
    int match_count = 0; Node *p_count = s_head_v2;
    while(p_count){
        int found = 0;
        if (search_type == 1 && strstr(p_count->name, query)) found = 1;
        else if (search_type == 2 && strstr(p_count->phone, query)) found = 1;
        else if (search_type == 3 && strstr(p_count->email, query)) found = 1;
        if (found) match_count++;
        p_count = p_count->next;
    }
    if (match_count == 0) return NULL;
    ContactRecord* matches = (ContactRecord*)malloc(match_count * sizeof(ContactRecord));
    if (!matches) return NULL;
    int current_match_idx = 0; Node *p_data = s_head_v2;
    while(p_data && current_match_idx < match_count){
        int found = 0;
        if (search_type == 1 && strstr(p_data->name, query)) found = 1;
        else if (search_type == 2 && strstr(p_data->phone, query)) found = 1;
        else if (search_type == 3 && strstr(p_data->email, query)) found = 1;
        if (found) {
            strncpy(matches[current_match_idx].name, p_data->name, 49); matches[current_match_idx].name[49] = '\0';
            strncpy(matches[current_match_idx].phone, p_data->phone, 49); matches[current_match_idx].phone[49] = '\0';
            strncpy(matches[current_match_idx].email, p_data->email, 49); matches[current_match_idx].email[49] = '\0';
            current_match_idx++;
        }
        p_data = p_data->next;
    }
    *out_count = current_match_idx;
    return matches;
}

API char* lib_v2_edit_contact(const char* old_email_id, const char* new_name, const char* new_phone, const char* new_email) {
    // ... (Implementation remains largely the same as the robust one provided before) ...
    if (!lib_v2_is_valid_name(new_name)) return allocate_and_copy_string_v2("Error: Invalid new name.");
    if (!lib_v2_is_valid_number(new_phone)) return allocate_and_copy_string_v2("Error: Invalid new phone.");
    if (!lib_v2_is_valid_email(new_email)) return allocate_and_copy_string_v2("Error: Invalid new email.");
    Node *target = NULL; Node *p_find = s_head_v2;
    while(p_find){ if (strcmp(p_find->email, old_email_id) == 0) { target = p_find; break; } p_find = p_find->next; }
    if (!target) return allocate_and_copy_string_v2("Error: Contact to edit not found.");
    if (strcmp(old_email_id, new_email) != 0) { // Email is being changed
        Node *p_check = s_head_v2;
        while(p_check){ if(p_check != target && strcmp(p_check->email, new_email) == 0) return allocate_and_copy_string_v2("Error: New email already exists."); p_check = p_check->next; }
    }
    strncpy(target->name, new_name, 49); target->name[49] = '\0';
    strncpy(target->phone, new_phone, 49); target->phone[49] = '\0';
    strncpy(target->email, new_email, 49); target->email[49] = '\0';
    return allocate_and_copy_string_v2("Contact updated successfully (LinkedList).");
}

API int lib_v2_delete_contact_by_email(const char* email) {
    // ... (Implementation remains largely the same as the robust one provided before) ...
    Node *current = s_head_v2, *prev = NULL;
    if (current != NULL && strcmp(current->email, email) == 0) { s_head_v2 = current->next; free(current); s_count_v2--; return 0; }
    while (current != NULL && strcmp(current->email, email) != 0) { prev = current; current = current->next; }
    if (current == NULL) return -1; // Not found
    prev->next = current->next; free(current); s_count_v2--; return 0;
}

API int lib_v2_delete_all_contacts() { lib_v2_cleanup(); return 0; }

// contact_v2_lib.c
// ... (keep all includes, API definitions, static globals s_head_v2, s_count_v2,
//      allocate_and_copy_string_v2, lib_v2_free_string, lib_v2_free_contact_records,
//      validation functions, internal_check_... functions,
//      lib_v2_initialize, lib_v2_cleanup, lib_v2_add_contact, lib_v2_get_all_contacts,
//      lib_v2_search_contacts, lib_v2_edit_contact, lib_v2_delete_contact_by_email,
//      lib_v2_delete_all_contacts - ALL OF THESE REMAIN THE SAME AS BEFORE) ...

// --- Merge Sort Implementation (with iterative merge) ---

// Comparison functions (ensure they don't have early returns for NULL, as sortedMerge handles NULL lists)
static int cmpName_v2(Node *a, Node *b)  { return strcmp(a->name,  b->name)  <= 0; }
static int cmpPhone_v2(Node *a, Node *b) { return strcmp(a->phone, b->phone) <= 0; }
static int cmpEmail_v2(Node *a, Node *b) { return strcmp(a->email, b->email) <= 0; }

// frontBackSplit_v2 remains the same as before
static void frontBackSplit_v2(Node *source, Node **frontRef, Node **backRef) {
    if (source == NULL || source->next == NULL) {
        *frontRef = source; 
        *backRef = NULL; 
        return;
    }
    Node *slow = source; 
    Node *fast = source->next; 
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }
    *frontRef = source;      
    *backRef  = slow->next;  
    slow->next = NULL;       
}

// NEW Iterative Sorted Merge function
static Node *sortedMerge_v2_iterative(Node *a, Node *b, int (*cmp)(Node*,Node*)) {
    if (!a) return b;
    if (!b) return a;

    Node *result_head = NULL;
    Node *current_tail = NULL;

    // Pick the initial head
    if (cmp(a, b)) {
        result_head = a;
        a = a->next;
    } else {
        result_head = b;
        b = b->next;
    }
    current_tail = result_head;

    // Iterate through the rest of the lists
    while (a && b) {
        if (cmp(a, b)) {
            current_tail->next = a;
            current_tail = a;
            a = a->next;
        } else {
            current_tail->next = b;
            current_tail = b;
            b = b->next;
        }
    }

    // Append remaining nodes of a or b
    if (a) {
        current_tail->next = a;
    } else if (b) {
        current_tail->next = b;
    }
    // The last node's next will be NULL if both a and b are exhausted,
    // or it will point to the rest of the appended list.

    return result_head;
}


// mergeSort_v2 now calls the iterative merge
static Node *mergeSort_v2(Node *h, int (*cmp)(Node*,Node*)) {
    if (!h || !h->next) return h;
    Node *a = NULL, *b = NULL; 
    frontBackSplit_v2(h, &a, &b);
    
    a = mergeSort_v2(a, cmp);
    b = mergeSort_v2(b, cmp);
    
    // Call the iterative merge function
    return sortedMerge_v2_iterative(a, b, cmp); 
}

// lib_v2_sort_contacts remains the same, it will use the updated mergeSort_v2
API int lib_v2_sort_contacts(int sort_type) {
    if (s_count_v2 < 2 || !s_head_v2) return 0; 

    int (*compare_func)(Node*, Node*) = NULL;
    if (sort_type == 1) compare_func = cmpName_v2;
    else if (sort_type == 2) compare_func = cmpPhone_v2;
    else if (sort_type == 3) compare_func = cmpEmail_v2;
    else return -1; 
    
    s_head_v2 = mergeSort_v2(s_head_v2, compare_func);
    
    // IMPORTANT: After sorting, the number of nodes SHOULD be the same.
    // If nodes are lost, s_count_v2 would be an overestimate.
    // A robust solution would be to re-count the nodes after sort if loss is suspected,
    // or ensure the sort is provably correct.
    // For now, we assume sort preserves count if correct.
    // If after this change, count is still wrong, then mergeSort_v2 or frontBackSplit_v2
    // are still losing nodes.
    
    // To verify count after sort (for debugging, can be removed later):
    /*
    int actual_count_after_sort = 0;
    Node* temp_counter = s_head_v2;
    while(temp_counter) {
        actual_count_after_sort++;
        temp_counter = temp_counter->next;
    }
    if (s_count_v2 != actual_count_after_sort) {
        // This indicates nodes were lost during sort!
        // You could log this or handle it. For now, update the count.
        // printf("Warning: Node count mismatch after sort. Original: %d, After sort: %d\n", s_count_v2, actual_count_after_sort);
        s_count_v2 = actual_count_after_sort;
    }
    */
    return 0; 
}

// ... (lib_v2_save_contacts and other API functions remain the same as before) ...
API int lib_v2_save_contacts(const char* data_file_path) {
    const char* file_to_save = data_file_path;
    if (!file_to_save) return -3; // No path provided

    FILE* pF = fopen(file_to_save, "w");
    if (!pF) return -1;

    // Optional: Write header
    // fprintf(pF, "Name,Phone,Email\n");

    for (Node *p = s_head_v2; p; p = p->next) {
        if (fprintf(pF, "%s,%s,%s\n", p->name, p->phone, p->email) < 0) {
            fclose(pF); return -2;
        }
    }
    fclose(pF); return 0;
}