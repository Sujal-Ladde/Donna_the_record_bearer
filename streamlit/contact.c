#include "contact.h" // [cite: 1]
#include <stdio.h>    // Included via contact.h
#include <stdlib.h>   // Included via contact.h
#include <string.h>   // Included via contact.h
#include <ctype.h>    // Included via contact.h
#include <stdbool.h>  // Included via contact.h

// Global variables
Node *head = NULL; // [cite: 1]
int count = 0; // [cite: 1]
static FILE *pF = NULL; // [cite: 1]

// --- Implementation of library-friendly C functions ---

void initialize_library() {
    // Free existing list if any (e.g., if called multiple times, though typically once)
    while (head) {
        Node *temp = head;
        head = head->next;
        free(temp);
    }
    head = NULL;
    count = 0;

    pF = fopen("contacts.csv", "r"); // [cite: 1]
    if (pF) {
        char line[200];
        while (fgets(line, sizeof(line), pF)) { // [cite: 1]
            Node *n = malloc(sizeof(Node)); // [cite: 1]
            if (!n) { /* Handle malloc failure if necessary */ continue; }
            if (sscanf(line, "%49[^,],%49[^,],%49[^\n]\n", n->name, n->phone, n->email) == 3) { // [cite: 1]
                n->next = head; // [cite: 1]
                head = n; // [cite: 1]
                count++; // [cite: 1]
            } else {
                free(n); // [cite: 1]
            }
        }
        fclose(pF); // [cite: 1]
        pF = NULL;
    }
}

int add_contact_py(const char* name_str, const char* phone_str, const char* email_str) {
    if (!isvalidname(name_str)) return -1; // [cite: 1]
    if (isvalidnumber(phone_str) != 2) return -2; // Original returns 2 for valid 10-digit number [cite: 1]
    if (isvalidemail(email_str) != 2) return -3; // Original returns 2 for valid .com email [cite: 1]

    // Check for duplicates before adding
    if (checkphone(phone_str)) return -4; // [cite: 1]
    if (checkemail(email_str)) return -5; // [cite: 1]

    Node *nw = malloc(sizeof(Node));
    if (!nw) return -6; // Malloc failed

    strncpy(nw->name, name_str, sizeof(nw->name) - 1);
    nw->name[sizeof(nw->name) - 1] = '\0';
    strncpy(nw->phone, phone_str, sizeof(nw->phone) - 1);
    nw->phone[sizeof(nw->phone) - 1] = '\0';
    strncpy(nw->email, email_str, sizeof(nw->email) - 1);
    nw->email[sizeof(nw->email) - 1] = '\0';

    nw->next = head; // [cite: 1]
    head = nw; // [cite: 1]
    count++; // [cite: 1]
    return 1; // Success
}

int get_contacts_count_py() {
    return count; // [cite: 1]
}

ContactData* get_all_contacts_py(int* num_contacts) {
    *num_contacts = count; // [cite: 1]
    if (count == 0) return NULL;

    ContactData* contacts_array = malloc(count * sizeof(ContactData));
    if (!contacts_array) {
        *num_contacts = 0;
        return NULL; // Malloc failed
    }

    Node *p = head; // [cite: 1]
    for (int i = 0; i < count; i++) {
        if (!p) { // Should ideally not happen if count is accurate
            *num_contacts = i; // Actual number copied
            // Might indicate an issue, but return what we have
            return contacts_array;
        }
        strncpy(contacts_array[i].name, p->name, sizeof(contacts_array[i].name) -1);
        contacts_array[i].name[sizeof(contacts_array[i].name) -1] = '\0';
        strncpy(contacts_array[i].phone, p->phone, sizeof(contacts_array[i].phone) -1);
         contacts_array[i].phone[sizeof(contacts_array[i].phone) -1] = '\0';
        strncpy(contacts_array[i].email, p->email, sizeof(contacts_array[i].email) -1);
        contacts_array[i].email[sizeof(contacts_array[i].email) -1] = '\0';
        p = p->next; // [cite: 1]
    }
    return contacts_array;
}

void free_contact_data_array(ContactData* data_array) {
    if (data_array) {
        free(data_array);
    }
}

ContactData* search_contacts_py(const char* query, int search_type, int* num_found) {
    *num_found = 0;
    if (!head || !query || query[0] == '\0') return NULL;

    int matches_capacity = 10; // Initial capacity
    ContactData* found_array = malloc(matches_capacity * sizeof(ContactData));
    if(!found_array) return NULL;

    int current_match_idx = 0;
    for (Node *p = head; p; p = p->next) { // [cite: 1]
        bool is_match = false;
        if (search_type == 1 && strstr(p->name, query)) is_match = true; // [cite: 1]
        else if (search_type == 2 && strstr(p->phone, query)) is_match = true; // [cite: 1]
        else if (search_type == 3 && strstr(p->email, query)) is_match = true; // [cite: 1]

        if (is_match) {
            if (current_match_idx >= matches_capacity) {
                matches_capacity *= 2;
                ContactData* temp = realloc(found_array, matches_capacity * sizeof(ContactData));
                if (!temp) {
                    free(found_array);
                    *num_found = 0;
                    return NULL; // Realloc failed
                }
                found_array = temp;
            }
            strncpy(found_array[current_match_idx].name, p->name, sizeof(found_array[current_match_idx].name)-1);
            found_array[current_match_idx].name[sizeof(found_array[current_match_idx].name)-1] = '\0';
            strncpy(found_array[current_match_idx].phone, p->phone, sizeof(found_array[current_match_idx].phone)-1);
            found_array[current_match_idx].phone[sizeof(found_array[current_match_idx].phone)-1] = '\0';
            strncpy(found_array[current_match_idx].email, p->email, sizeof(found_array[current_match_idx].email)-1);
            found_array[current_match_idx].email[sizeof(found_array[current_match_idx].email)-1] = '\0';
            current_match_idx++;
        }
    }

    *num_found = current_match_idx;
    if (current_match_idx == 0) {
        free(found_array);
        return NULL;
    }
    // Optional: Shrink to fit
    ContactData* final_array = realloc(found_array, current_match_idx * sizeof(ContactData));
    if(!final_array && current_match_idx > 0) { // realloc could fail but original is still valid
        return found_array; // return unshrinked if realloc fails
    }
    return final_array;
}

int delete_contact_by_email_py(const char* email_str) {
    Node *cur = head, *prev = NULL; // [cite: 1]
    while (cur) {
        if (strcmp(cur->email, email_str) == 0) { // [cite: 1]
            if (prev) {
                prev->next = cur->next; // [cite: 1]
            } else {
                head = cur->next; // [cite: 1]
            }
            free(cur); // [cite: 1]
            count--; // [cite: 1]
            return 1; // Deleted
        }
        prev = cur; // [cite: 1]
        cur = cur->next; // [cite: 1]
    }
    return 0; // Not found
}

int edit_contact_py(const char* old_email_str, const char* new_name_str, const char* new_phone_str, const char* new_email_str) {
    Node *target = NULL;
    for (Node *p = head; p; p = p->next) { // [cite: 1]
        if (strcmp(p->email, old_email_str) == 0) { // [cite: 1]
            target = p;
            break;
        }
    }

    if (!target) return 0; // Original contact not found

    // Validate new data
    if (!isvalidname(new_name_str)) return -1; // [cite: 1]
    if (isvalidnumber(new_phone_str) != 2) return -2; // [cite: 1]
    if (isvalidemail(new_email_str) != 2) return -3; // [cite: 1]

    // Check for duplicates only if the phone/email is actually changing
    // And if the new phone/email belongs to another contact
    if (strcmp(target->phone, new_phone_str) != 0) {
        Node *temp_node = head;
        while(temp_node){
            if(temp_node != target && strcmp(temp_node->phone, new_phone_str) == 0) return -4; // New phone exists for another contact
            temp_node = temp_node->next;
        }
    }
    if (strcmp(target->email, new_email_str) != 0) {
         Node *temp_node = head;
        while(temp_node){
            if(temp_node != target && strcmp(temp_node->email, new_email_str) == 0) return -5; // New email exists for another contact
            temp_node = temp_node->next;
        }
    }

    strcpy(target->name, new_name_str); // [cite: 1]
    strcpy(target->phone, new_phone_str); // [cite: 1]
    strcpy(target->email, new_email_str); // [cite: 1]
    return 1; // Success
}

void delete_all_contacts_py() {
    Node *cur = head; // [cite: 1]
    while (cur) {
        Node *tmp = cur; // [cite: 1]
        cur = cur->next; // [cite: 1]
        free(tmp); // [cite: 1]
    }
    head = NULL; // [cite: 1]
    count = 0; // [cite: 1]
}

void save_contacts_py() {
    pF = fopen("contacts.csv", "w"); // [cite: 1]
    if (!pF) {
        // In a library, direct perror might not be best.
        // For now, this is simple. A more robust solution might involve error codes.
        perror("Error opening file for saving in save_contacts_py");
        return;
    }
    // To save in the original order (or current sorted order), we need to traverse
    // from head to tail. If the list was reversed during load, this saves it reversed.
    // To save in a specific order (e.g. original from file, or always sorted by name),
    // we might need to load into a temporary array, sort it if needed, then write.
    // For now, it saves in the current linked list order.
    // If contacts were added by prepending, the save order will be reverse of addition.
    // To save in the same order as they appear in `get_all_contacts_py` (which reflects the internal list order):
    
    // If saving in reverse order of the linked list (as it's a stack)
    // Node **nodes = malloc(count * sizeof(Node*));
    // Node *current = head;
    // int i = 0;
    // while(current != NULL && i < count){
    //    nodes[i++] = current;
    //    current = current->next;
    // }
    // for(int j = count -1; j>=0; j--){
    //    fprintf(pF, "%s,%s,%s\n", nodes[j]->name, nodes[j]->phone, nodes[j]->email);
    // }
    // free(nodes);

    // Simple save in current list order (head first)
    for (Node *p = head; p; p = p->next) { // [cite: 1]
        fprintf(pF, "%s,%s,%s\n", p->name, p->phone, p->email); // [cite: 1]
    }
    fclose(pF); // [cite: 1]
    pF = NULL;
}

// --- Sorting related C functions (Merge Sort - from original code) ---
// These static helper functions are used by the sort_contacts_by_..._py functions.
// They don't need to be in contact.h if they are only used internally in this file.

static int cmpName(Node *a, Node *b)  { return strcmp(a->name,  b->name)  <= 0; } // [cite: 1]
static int cmpPhone(Node *a, Node *b) { return strcmp(a->phone, b->phone) <= 0; } // [cite: 1]
static int cmpEmail(Node *a, Node *b) { return strcmp(a->email, b->email) <= 0; } // [cite: 1]

static void frontBackSplit(Node *source, Node **frontRef, Node **backRef) { // [cite: 1]
    if (source == NULL || source->next == NULL) {
        *frontRef = source;
        *backRef = NULL;
        return;
    }
    Node *slow = source; // [cite: 1]
    Node *fast = source->next; // [cite: 1]
    while (fast != NULL) { // [cite: 1]
        fast = fast->next; // [cite: 1]
        if (fast != NULL) {
            slow = slow->next; // [cite: 1]
            fast = fast->next; // [cite: 1]
        }
    }
    *frontRef = source; // [cite: 1]
    *backRef  = slow->next; // [cite: 1]
    slow->next = NULL; // [cite: 1]
}

static Node *sortedMerge(Node *a, Node *b, int (*cmp)(Node*,Node*)) { // [cite: 1]
    if (!a) return b; // [cite: 1]
    if (!b) return a; // [cite: 1]
    Node *result = NULL;
    if (cmp(a,b)) { // [cite: 1]
        result = a;
        result->next = sortedMerge(a->next, b, cmp); // [cite: 1]
    } else {
        result = b;
        result->next = sortedMerge(a, b->next, cmp); // [cite: 1]
    }
    return result;
}

static Node *mergeSort(Node *h, int (*cmp)(Node*,Node*)) { // [cite: 1]
    if (!h || !h->next) return h; // [cite: 1]
    Node *a, *b;
    frontBackSplit(h, &a, &b); // [cite: 1]
    a = mergeSort(a, cmp); // [cite: 1]
    b = mergeSort(b, cmp); // [cite: 1]
    return sortedMerge(a, b, cmp); // [cite: 1]
}

void sort_contacts_by_name_py() {
    head = mergeSort(head, cmpName); // [cite: 1]
}
void sort_contacts_by_phone_py() {
    head = mergeSort(head, cmpPhone); // [cite: 1]
}
void sort_contacts_by_email_py() {
    head = mergeSort(head, cmpEmail); // [cite: 1]
}

// --- Validation and checking functions (from original code) ---
int isvalidname(const char name[]) { // [cite: 1]
    if (name == NULL || name[0] == '\0') return 0; // Cannot be empty
    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha(name[i]) && name[i] != ' ') { // [cite: 1]
            return 0; // Contains non-alpha/non-space
        }
    }
    return 1; // Valid (original was 0 or 1, let's stick to 1 for true)
}

int isvalidnumber(const char number[]) { // [cite: 1]
    int length = strlen(number); // [cite: 1]
    if (length != 10) return 0; // [cite: 1]
    for (int i = 0; i < length; i++) {
        if (!isdigit(number[i])) return 0; // [cite: 1]
    }
    return 2; // Valid (original returns 2 for valid) [cite: 1]
}

int isvalidemail(const char email[]) { // [cite: 1]
    // Basic validation: must contain '@', '.', and ".com" at the end. No spaces.
    // Original logic was a bit convoluted. This is a simplified common check.
    int len = strlen(email);
    if (len < 6) return 0; // Min: a@b.cm

    char *at_sym = strchr(email, '@');
    char *dot_sym = strrchr(email, '.'); // Last dot

    if (!at_sym || !dot_sym || at_sym > dot_sym) return 0; // '@' must exist, '.' must exist after '@'
    if (at_sym == email || at_sym == email + len -1 ) return 0; // no char before @ or after
    if (dot_sym == email || dot_sym == email + len -1 ) return 0; // no char before . or after

    // Check for ".com" ending more robustly
    if (strcmp(dot_sym, ".com") != 0) return 1; // Has '@' and '.', but not ending in ".com", original returns 1 for partial [cite: 1]

    for(int i=0; email[i] != '\0'; i++){
        if(email[i] == ' ') return 0; // No spaces allowed
    }
    // Original returned 2 for full valid (.com ending)
    return 2; // [cite: 1]
}

int checkname(const char *s) { // [cite: 1]
    for (Node *p = head; p; p = p->next)
        if (strcmp(p->name, s) == 0) return 1; // [cite: 1]
    return 0;
}

int checkphone(const char *s) { // [cite: 1]
    for (Node *p = head; p; p = p->next)
        if (strcmp(p->phone, s) == 0) return 1; // [cite: 1]
    return 0;
}

int checkemail(const char *s) { // [cite: 1]
    for (Node *p = head; p; p = p->next)
        if (strcmp(p->email, s) == 0) return 1; // [cite: 1]
    return 0;
}

// Note: Original main.c is not needed as the entry point will be via Python.
// Original UI functions (infoscreen, loginPage, etc.) are removed as they are not library-friendly.