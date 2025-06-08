#include "contact.h"

#define LINE_LEN 58

Node *head = NULL;
int count = 0;
static FILE *pF = NULL;


/**
 * clearBuffer
 * ------------------
 * What: Clears the terminal screen and flushes stdout.
 * Args: none
 * Returns: void
 * Logic: Calls system("clear") then fflush(stdout) to ensure output is clean.
 */
void clearBuffer()
{
    system("clear");
    fflush(stdout);
}

/**
 * clearInputBuffer
 * ------------------
 * What: Discards any leftover characters in stdin until newline/EOF.
 * Args: none
 * Returns: void
 * Logic: Loops getchar() until '\n' or EOF.
 */
void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/**
 * my_pause
 * ------------------
 * What: Prompts the user to press a key before continuing.
 * Args: none
 * Returns: void
 * Logic: Prints a message, then calls getchar() to wait.
 */
void my_pause()
{
    printf("Press any key to continue...");
    getchar();
}

/**
 * initialize
 * ------------------
 * What: Loads contacts from CSV into the linked list and shows intro.
 * Args: none
 * Returns: void
 * Logic: Opens "contacts.csv", reads each line, sscanf into a new Node,
 *        pushes onto head, increments count, then closes file and calls infoscreen().
 */
void initialize()
{
    pF = fopen("contacts.csv", "r");
    if (pF)
    {
        char line[200];
        while (fgets(line, sizeof(line), pF))
        {
            Node *n = malloc(sizeof *n);
            if (sscanf(line,
                       "%49[^,],%49[^,],%49[^\n]\n",
                       n->name, n->phone, n->email) == 3)
            {
                n->next = head;
                head = n;
                count++;
            }
            else
                free(n);
        }
        fclose(pF);
    }
    infoscreen();
}

/**
 * infoscreen
 * ------------------
 * What: Displays the application title/introduction screen.
 * Args: none
 * Returns: void
 * Logic: Calls clearBuffer(), prints banner & description, pauses, then calls loginPage().
 */
void infoscreen()
{
    clearBuffer();
    printf("\n\t\t\t\t------------------------------------------ \n");
    printf("\t\t\t\t\t      >>> Donna <<<      \n");
    printf("\t\t\t\t------------------------------------------ \n\n");
    printf("\tDonna is a software application that allows users to store, organize and manage large numbers of contact records efficiently.\n");
    printf("\tThe system supports adding, updating, viewing and deleting contacts.\n\n");
    printf("\t\t\t\t------------------------------------ \n");
    printf("\t\t\t\t------------------------------------ \n\n");
    my_pause();
    loginPage();
}

/**
 * endScreen
 * ------------------
 * What: Displays the closing screen and exits the program.
 * Args: none
 * Returns: void (exits via exit(0))
 * Logic: Prints developer info, then calls exit(0).
 */
void endScreen()
{
    clearBuffer();
    printf("\n\t\t\t\t------------------------------------------ \n");
    printf("\t\t\t\t\t      >>> Donna <<<      \n");
    printf("\t\t\t\t------------------------------------------ \n\n");
    printf("\t\t\t\tDeveloped By: Sujal ladde \n\n");
    printf("\t\t\t\tGitHub: Zozozaza\n");
    printf("\t\t\t\tLinkedIn: Sujal Ladde \n\n");
    printf("\t\t\t\t------------------------------------------ \n");
    printf("\t\t\t\t\t\tThank You. \n");
    printf("\t\t\t\t------------------------------------------ \n\n");
    exit(0);
}

/**
 * loginPage
 * ------------------
 * What: Shows the main menu and dispatches based on user choice.
 * Args: none
 * Returns: void
 * Logic: Clears screen, prints options, reads choice, calls corresponding function.
 */
void loginPage()
{
    clearBuffer();
    printf("\n\t\t\t\t------------------------------------------- \n");
    printf("\t\t\t\t\t      >>> Donna <<<      \n");
    printf("\t\t\t\t------------------------------------------- \n\n");
    printf("\t\t\t\t[1] Add A New Contact. \n");
    printf("\t\t\t\t[2] Update A Contact. \n");
    printf("\t\t\t\t[3] Display All Contacts. \n");
    printf("\t\t\t\t[4] Search Contact. \n");
    printf("\t\t\t\t[5] Delete Contact. \n");
    printf("\t\t\t\t[6] Delete All Contacts. \n");
    printf("\t\t\t\t[7] Display Sorted Contacts. \n");
    printf("\t\t\t\t[0] EXIT. \n");
    printf("\t\t\t\t[9] SAVE. \n");
    printf("\t\t\t\t------------------------------------ \n\n");
    printf("\t\t\t\tEnter the number & Hit ENTER: ");
    int choice;
    scanf("%d", &choice);
    clearInputBuffer();
    switch (choice)
    {
    case 1: addcontact();             break;
    case 2: editcontact();            break;
    case 3: displaycontacts();        break;
    case 4: searchcontact();          break;
    case 5: deletecontact();          break;
    case 6: Deleteall();              break;
    case 7: displaySortedContacts();  break;
    case 9: save();                   break;
    case 0: Exit();                   break;
    default:
        printf("---------------------------------------\n");
        printf("ERROR: Invalid input please try again. \n");
        printf("---------------------------------------\n");
        my_pause();
        loginPage();
    }
}

/**
 * addcontact
 * ------------------
 * What: Prompts the user to add one or more new contacts.
 * Args: none
 * Returns: void
 * Logic: Reads count n, loops n times reading and validating name/phone/email,
 *        prepends each new Node to head, increments count, then shows the last n added.
 */
void addcontact() {
    clearBuffer();
    int n; char buf[50];
    printf("\t\t\t\tAdd Contacts (enter 0 to cancel) \n");
    printf("\t\t\t\tEnter number of contacts to add: ");
    scanf("%d", &n); clearInputBuffer();
    if (n <= 0) { loginPage(); return; }

    for (int i = 0; i < n; i++) {
        Node *nw = malloc(sizeof *nw);
        do {
            printf("Name: ");
            fgets(buf, sizeof buf, stdin);
            buf[strcspn(buf, "\n")] = '\0';
        } while (!isvalidname(buf));
        strcpy(nw->name, buf);

        do {
            printf("Phone: ");
            fgets(buf, sizeof buf, stdin);
            buf[strcspn(buf, "\n")] = '\0';
        } while (!isvalidnumber(buf) || checkphone(buf));
        strcpy(nw->phone, buf);

        do {
            printf("Email: ");
            fgets(buf, sizeof buf, stdin);
            buf[strcspn(buf, "\n")] = '\0';
        } while (!isvalidemail(buf) || checkemail(buf));
        strcpy(nw->email, buf);

        nw->next = head;
        head = nw;
        count++;
    }

    clearBuffer();
    printf("\t\t|-------------------------------------------------------------------------| \n");
    printf("\t\t| %-20s | %-15s | %-30s | \n", "Name", "Phone Number", "Email");
    printf("\t\t|-------------------------------------------------------------------------| \n");
    Node *p = head;
    for (int i = 0; i < n && p; i++, p = p->next)
        printf("\t\t| %-20s | %-15s | %-30s  \n", p->name, p->phone, p->email);

    my_pause();
    loginPage();
}

/**
 * displaycontacts
 * ------------------
 * What: Displays all contacts in a table.
 * Args: none
 * Returns: void
 * Logic: Prints header with total count, then iterates the linked list printing each Node.
 */
void displaycontacts() {
    clearBuffer();
    printf("\t\t\t\t\t----------------------------- \n");
    printf("\t\t\t\t\t    >>> Contacts List <<< \n");
    printf("\t\t\t\t\t----------------------------- \n");
    printf("\t\t\t\t\t    Total Contacts = %d\n", count);
    printf("\t\t|-------------------------------------------------------------------------| \n");
    printf("\t\t| %-20s | %-15s | %-30s | \n", "Name", "Phone Number", "Email");
    printf("\t\t|-------------------------------------------------------------------------| \n\n");
    for (Node *p = head; p; p = p->next)
        printf("\t\t| %-20s | %-15s | %-30s  \n", p->name, p->phone, p->email);
    my_pause();
    loginPage();
}

/**
 * searchcontact
 * ------------------
 * What: Presents a submenu to search by name/phone/email.
 * Args: none
 * Returns: void
 * Logic: Reads choice, then calls searchByName/Number/Email accordingly.
 */
void searchcontact()
{
    clearBuffer();
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t\t\t     >>> Search Contacts <<< \n");
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\n\t\t\t   (Enter 0 to go back to the main menu)\n");
    printf("\n\t\t\t1. Search by Name");
    printf("\n\t\t\t2. Search by Phone Number");
    printf("\n\t\t\t3. Search by Email");
    printf("\n\n\t\tEnter the search parameter - ");
    int ch;
    scanf("%d", &ch);
    clearInputBuffer();
    if (!ch)                        { loginPage(); return; }
    if (ch == 1)                    searchByName();
    else if (ch == 2)               searchByNumber();
    else if (ch == 3)               searchByEmail();
    else {
        printf("Invalid\n");
        my_pause();
        searchcontact();
    }
}

/**
 * searchByName
 * ------------------
 * What: Finds and displays contacts whose name contains the query.
 * Args:
 *   char buf[]  – the substring to match
 * Returns: void
 * Logic: Reads a line into buf, iterates list, uses strstr() to filter and print.
 */
void searchByName()
{
    clearBuffer();
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t\t\t     >>> Search By Name <<< \n");
    printf("\t\t|---------------------------------------------------------------| \n\n");
    char buf[50];
    fgets(buf, sizeof buf, stdin);
    buf[strcspn(buf, "\n")] = '\0';
    if (!strcmp(buf, "0")) { loginPage(); return; }

    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t| \tName\t| \tPhone Number\t  |  \t Email\t\t| \n");
    printf("\t\t|---------------------------------------------------------------| \n");
    for (Node *p = head; p; p = p->next)
        if (strstr(p->name, buf))
            printf("\t\t| %-20s | %-15s | %-30s |\n", p->name, p->phone, p->email);
    my_pause();
    loginPage();
}

/**
 * searchByNumber
 * ------------------
 * What: Finds and displays contacts whose phone contains the query.
 * Args:
 *   char buf[]  – the substring to match
 * Returns: void
 * Logic: Reads buf, iterates list, strstr() on p->phone, prints matches.
 */
void searchByNumber()
{
    clearBuffer();
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t\t\t     >>> Search By Phone Number <<< \n");
    printf("\t\t|---------------------------------------------------------------| \n\n");
    char buf[50];
    fgets(buf, sizeof buf, stdin);
    buf[strcspn(buf, "\n")] = '\0';
    if (!strcmp(buf, "0")) { loginPage(); return; }

    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t| \tName\t| \tPhone Number\t  |  \t Email\t\t| \n");
    printf("\t\t|---------------------------------------------------------------| \n");
    for (Node *p = head; p; p = p->next)
        if (strstr(p->phone, buf))
            printf("\t\t| %-20s | %-15s | %-30s |\n", p->name, p->phone, p->email);
    my_pause();
    loginPage();
}

/**
 * searchByEmail
 * ------------------
 * What: Finds and displays contacts whose email contains the query.
 * Args:
 *   char buf[]  – the substring to match
 * Returns: void
 * Logic: Reads buf, iterates list, strstr() on p->email, prints matches.
 */
void searchByEmail()
{
    clearBuffer();
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t\t\t     >>> Search By Email <<< \n");
    printf("\t\t|---------------------------------------------------------------| \n\n");
    char buf[50];
    fgets(buf, sizeof buf, stdin);
    buf[strcspn(buf, "\n")] = '\0';
    if (!strcmp(buf, "0")) { loginPage(); return; }

    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t| \tName\t| \tPhone Number\t  |  \t Email\t\t| \n");
    printf("\t\t|---------------------------------------------------------------| \n");
    for (Node *p = head; p; p = p->next)
        if (strstr(p->email, buf))
            printf("\t\t| %-20s | %-15s | %-30s |\n", p->name, p->phone, p->email);
    my_pause();
    loginPage();
}

/**
 * deletecontact
 * ------------------
 * What: Deletes a specific contact, searching by name/phone/email with disambiguation.
 * Args: none
 * Returns: void
 * Logic: Presents search-by menu, collects all exact matches, if >1 lets user pick,
 *        then unlinks/frees the chosen node and decrements count.
 */
void deletecontact()
{
    clearBuffer();
    printf("\t\t|---------------------------------------------------------------|\n");
    printf("\t\t\t     >>> Delete a Contact <<<\n");
    printf("\t\t|---------------------------------------------------------------|\n\n");
    printf("\t\t   (Enter 0 to go back to the main menu)\n\n");
    printf("\t\t1. Delete by Name\n");
    printf("\t\t2. Delete by Phone Number\n");
    printf("\t\t3. Delete by Email\n\n");
    printf("\t\tChoose option: ");

    int opt;
    scanf("%d", &opt);
    clearInputBuffer();
    if (opt == 0) { loginPage(); return; }
    if (opt < 1 || opt > 3) {
        printf("Invalid choice.\n");
        my_pause();
        deletecontact();
        return;
    }

    char query[50];
    const char *field = (opt == 1 ? "Name" : opt == 2 ? "Phone Number" : "Email");
    printf("\nEnter %s (exact match): ", field);
    fgets(query, sizeof query, stdin);
    query[strcspn(query, "\n")] = '\0';
    if (!strcmp(query, "0")) { loginPage(); return; }

    Node *matches[100];
    int mcount = 0;
    for (Node *p = head; p; p = p->next) {
        if ((opt == 1 && !strcmp(p->name, query)) ||
            (opt == 2 && !strcmp(p->phone, query)) ||
            (opt == 3 && !strcmp(p->email, query)))
        {
            matches[mcount++] = p;
        }
    }

    if (mcount == 0) {
        printf("\nNo contacts found for %s \"%s\".\n", field, query);
        my_pause();
        loginPage();
        return;
    }

    int choice = 0;
    if (mcount > 1) {
        printf("\nFound %d matching contacts:\n\n", mcount);
        for (int i = 0; i < mcount; i++) {
            printf("  [%d] %s, %s, %s\n",
                   i + 1,
                   matches[i]->name,
                   matches[i]->phone,
                   matches[i]->email);
        }
        printf("\nEnter the number of the contact to delete: ");
        scanf("%d", &choice);
        clearInputBuffer();
        if (choice < 1 || choice > mcount) {
            printf("Invalid selection.\n");
            my_pause();
            loginPage();
            return;
        }
        choice--;
    }

    Node *target = matches[(mcount == 1) ? 0 : choice];
    Node *prev = NULL, *cur = head;
    while (cur && cur != target) {
        prev = cur;
        cur = cur->next;
    }
    if (prev) prev->next = cur->next;
    else       head       = cur->next;
    free(cur);
    count--;

    printf("\nContact deleted successfully!\n");
    my_pause();
    loginPage();
}

/**
 * Deleteall
 * ------------------
 * What: Deletes every contact in the list after confirmation.
 * Args: none
 * Returns: void
 * Logic: Prompts Y/N; if yes, iterates freeing all nodes, resets head and count.
 */
void Deleteall()
{
    clearBuffer();
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t\t\t   >>> DELETE ALL CONTACTS!! <<< \n");
    printf("\t\t|---------------------------------------------------------------| \n\n");
    printf("\t\tAre you sure you want to delete all contacts? (Y/N) - ");
    char c;
    scanf(" %c", &c);
    clearInputBuffer();
    if (c == 'Y' || c == 'y') {
        Node *cur = head;
        while (cur) {
            Node *tmp = cur;
            cur = cur->next;
            free(tmp);
        }
        head = NULL;
        count = 0;
        printf("\nAll contacts deleted successfully!\n");
    } else {
        printf("\nOperation Cancelled!\n");
    }
    my_pause();
    loginPage();
}

/**
 * editcontact
 * ------------------
 * What: Finds and edits a single contact by name/phone/email.
 * Args: none
 * Returns: void
 * Logic: Similar to deletecontact(), but after selecting the node,
 *        prompts which field to update, validates input, and applies change.
 */
void editcontact()
{
    clearBuffer();
    printf("\t\t|---------------------------------------------------------------|\n");
    printf("\t\t\t     >>> Edit a Contact <<<\n");
    printf("\t\t|---------------------------------------------------------------|\n\n");
    printf("\t\t   (Enter 0 to go back to the main menu)\n\n");
    printf("\t\t1. Search by Name\n");
    printf("\t\t2. Search by Phone Number\n");
    printf("\t\t3. Search by Email\n\n");
    printf("\t\tChoose option: ");

    int opt;
    scanf("%d", &opt);
    clearInputBuffer();
    if (opt == 0) { loginPage(); return; }
    if (opt < 1 || opt > 3) {
        printf("Invalid choice.\n");
        my_pause();
        editcontact();
        return;
    }

    char query[50];
    const char *field = (opt == 1 ? "Name" : opt == 2 ? "Phone Number" : "Email");
    printf("\nEnter %s (exact match): ", field);
    fgets(query, sizeof query, stdin);
    query[strcspn(query, "\n")] = '\0';
    if (!strcmp(query, "0")) { loginPage(); return; }

    Node *matches[100];
    int mcount = 0;
    for (Node *p = head; p; p = p->next) {
        if ((opt == 1 && !strcmp(p->name, query)) ||
            (opt == 2 && !strcmp(p->phone, query)) ||
            (opt == 3 && !strcmp(p->email, query)))
        {
            matches[mcount++] = p;
        }
    }

    if (mcount == 0) {
        printf("\nNo contacts found for %s \"%s\".\n", field, query);
        my_pause();
        loginPage();
        return;
    }

    int choice = 0;
    if (mcount > 1) {
        printf("\nFound %d matching contacts:\n\n", mcount);
        for (int i = 0; i < mcount; i++) {
            printf("  [%d] %s, %s, %s\n",
                   i + 1,
                   matches[i]->name,
                   matches[i]->phone,
                   matches[i]->email);
        }
        printf("\nEnter the number of the contact to edit: ");
        scanf("%d", &choice);
        clearInputBuffer();
        if (choice < 1 || choice > mcount) {
            printf("Invalid selection.\n");
            my_pause();
            loginPage();
            return;
        }
        choice--;
    }

    Node *target = matches[(mcount == 1) ? 0 : choice];

    printf("\nEditing contact: %s, %s, %s\n\n", 
           target->name, target->phone, target->email);
    printf("What would you like to update?\n");
    printf(" 1. Name\n 2. Phone Number\n 3. Email\n\n");
    printf("Enter choice: ");
    int field_opt;
    scanf("%d", &field_opt);
    clearInputBuffer();

    char nv[50];
    printf("Enter new value (or 0 to cancel): ");
    fgets(nv, sizeof nv, stdin);
    nv[strcspn(nv, "\n")] = '\0';
    if (!strcmp(nv, "0")) {
        printf("Edit cancelled.\n");
        my_pause();
        loginPage();
        return;
    }

    bool valid = false;
    switch (field_opt) {
        case 1:
            if (isvalidname(nv)) {
                strcpy(target->name, nv);
                valid = true;
            }
            break;
        case 2:
            if (isvalidnumber(nv) && !checkphone(nv)) {
                strcpy(target->phone, nv);
                valid = true;
            }
            break;
        case 3:
            if (isvalidemail(nv) && !checkemail(nv)) {
                strcpy(target->email, nv);
                valid = true;
            }
            break;
        default:
            printf("Invalid edit choice.\n");
    }

    if (valid) {
        printf("\nContact updated to:\n");
        printf("| %-20s | %-15s | %-30s |\n",
               target->name, target->phone, target->email);
    } else {
        printf("\nInvalid input or duplicate detected. No changes made.\n");
    }

    my_pause();
    loginPage();
}

/**
 * cmpName
 * ------------------
 * What: Comparison callback for merge-sort by name.
 * Args:
 *   Node *a, *b – two nodes to compare
 * Returns:
 *   int truth value (a ≤ b)
 * Logic: strcmp(a->name, b->name) ≤ 0.
 */
static int cmpName(Node *a, Node *b)  { return strcmp(a->name,  b->name)  <= 0; }

/**
 * cmpPhone
 * ------------------
 * What: Comparison callback for merge-sort by phone.
 * Args:
 *   Node *a, *b
 * Returns: int truth value (a ≤ b)
 * Logic: strcmp(a->phone, b->phone) ≤ 0.
 */
static int cmpPhone(Node *a, Node *b) { return strcmp(a->phone, b->phone) <= 0; }

/**
 * cmpEmail
 * ------------------
 * What: Comparison callback for merge-sort by email.
 * Args:
 *   Node *a, *b
 * Returns: int truth value (a ≤ b)
 * Logic: strcmp(a->email, b->email) ≤ 0.
 */
static int cmpEmail(Node *a, Node *b) { return strcmp(a->email, b->email) <= 0; }

/**
 * frontBackSplit
 * ------------------
 * What: Splits a linked list into two halves.
 * Args:
 *   Node *source – head of original list
 *   Node **frontRef, **backRef – out parameters for two sublists
 * Returns: void
 * Logic: Uses fast/slow pointer to find midpoint, breaks link.
 */
static void frontBackSplit(Node *source, Node **frontRef, Node **backRef) {
    Node *slow = source, *fast = source->next;
    while (fast) {
        fast = fast->next;
        if (fast) {
            slow = slow->next;
            fast = fast->next;
        }
    }
    *frontRef = source;
    *backRef  = slow->next;
    slow->next = NULL;
}

/**
 * sortedMerge
 * ------------------
 * What: Merges two sorted lists using cmp callback.
 * Args:
 *   Node *a, *b – heads of two sorted lists
 *   int (*cmp)(Node*,Node*) – comparison function
 * Returns:
 *   Node* – head of merged sorted list
 * Logic: Recursively picks the smaller head based on cmp.
 */
static Node *sortedMerge(Node *a, Node *b, int (*cmp)(Node*,Node*)) {
    if (!a) return b;
    if (!b) return a;
    if (cmp(a,b)) {
        a->next = sortedMerge(a->next, b, cmp);
        return a;
    } else {
        b->next = sortedMerge(a, b->next, cmp);
        return b;
    }
}

/**
 * mergeSort
 * ------------------
 * What: Recursively sorts a linked list via merge sort.
 * Args:
 *   Node *h – head of list to sort
 *   int (*cmp)(Node*,Node*) – comparison function
 * Returns:
 *   Node* – head of sorted list
 * Logic: Base case single/empty; split list; sort halves; merge.
 */
static Node *mergeSort(Node *h, int (*cmp)(Node*,Node*)) {
    if (!h || !h->next) return h;
    Node *a, *b;
    frontBackSplit(h, &a, &b);
    a = mergeSort(a, cmp);
    b = mergeSort(b, cmp);
    return sortedMerge(a, b, cmp);
}

/**
 * sortByName
 * ------------------
 * What: Sorts full list by name (ascending).
 * Args: none
 * Returns: void
 * Logic: Calls mergeSort with cmpName, then displaycontacts().
 */
void sortByName() {
    head = mergeSort(head, cmpName);
    displaycontacts();
}

/**
 * sortByNumber
 * ------------------
 * What: Sorts full list by phone number (ascending).
 * Args: none
 * Returns: void
 * Logic: Calls mergeSort with cmpPhone, then displaycontacts().
 */
void sortByNumber() {
    head = mergeSort(head, cmpPhone);
    displaycontacts();
}

/**
 * sortByEmail
 * ------------------
 * What: Sorts full list by email (ascending).
 * Args: none
 * Returns: void
 * Logic: Calls mergeSort with cmpEmail, then displaycontacts().
 */
void sortByEmail() {
    head = mergeSort(head, cmpEmail);
    displaycontacts();
}

/**
 * displaySortedContacts
 * ------------------
 * What: Presents a submenu to choose sort field and then sorts.
 * Args: none
 * Returns: void
 * Logic: Reads choice, calls corresponding sortByX().
 */
void displaySortedContacts()
{
    clearBuffer();
    printf("\t\t|---------------------------------------------------------------|\n");
    printf("\t\t\t     >>> Display sorted Contacts <<<\n");
    printf("\t\t|---------------------------------------------------------------|\n\n");
    printf("\t\t   (Enter 0 to go back to the main menu)\n\n");
    printf("\t\t1. Sort by Name\n");
    printf("\t\t2. Sort by Phone Number\n");
    printf("\t\t3. Sort by Email\n\n");
    printf("\t\tChoose option: ");
    int ch;
    scanf("%d", &ch);
    clearInputBuffer();
    if (ch == 1)      sortByName();
    else if (ch == 2) sortByNumber();
    else if (ch == 3) sortByEmail();
    else {
        printf("Invalid choice!\n");
        my_pause();
        displaySortedContacts();
    }
}

/**
 * save
 * ------------------
 * What: Saves all contacts back to "contacts.csv".
 * Args: none
 * Returns: void
 * Logic: Opens CSV for write, iterates list printing each record, closes file,
 *        then asks user whether to exit or return to menu.
 */
void save()
{
    pF = fopen("contacts.csv", "w");
    if (!pF)
    {
        perror("Error");
        loginPage();
        return;
    }
    for (Node *p = head; p; p = p->next)
        fprintf(pF, "%s,%s,%s\n", p->name, p->phone, p->email);
    fclose(pF);

    clearBuffer();
    printf("\nContacts saved successfully!\n");
    printf("Enter 0 to go back or y to EXIT - ");
    char c;
    scanf(" %c", &c);
    clearInputBuffer();
    if (c == 'y' || c == 'Y')
        endScreen();
    else
        loginPage();
}

/**
 * Exit
 * ------------------
 * What: Prompts to save before quitting, then exits.
 * Args: none
 * Returns: void
 * Logic: Asks Y/N; if yes calls save(), else calls endScreen().
 */
void Exit()
{
    clearBuffer();
    printf("Save changes? (Y/N): ");
    char c;
    scanf(" %c", &c);
    clearInputBuffer();
    if (c == 'Y' || c == 'y')
        save();
    else
        endScreen();
}


/**
 * isvalidname
 * ------------------
 * What: Validates that name contains only letters and spaces.
 * Args:
 *   const char name[] – input string
 * Returns:
 *   int – 1 if valid, 0 otherwise
 * Logic: Iterates each char; if non-letter/non-space found, returns 0.
 */
int isvalidname(const char name[])
{
    int x = 0;
    for (int i = 0; name[i] != '\0'; i++)
    {
        if ((name[i] >= 'a' && name[i] <= 'z') ||
            (name[i] >= 'A' && name[i] <= 'Z') ||
            (name[i] == ' '))
        {
            x = 1;
        }
        else
        {
            x = 0;
            break;
        }
    }
    return x;
}

/**
 * isvalidnumber
 * ------------------
 * What: Validates that phone is exactly 10 digits.
 * Args:
 *   const char number[] – input string
 * Returns:
 *   int – 2 if valid, 0 otherwise
 * Logic: Checks strlen == 10, then each char is digit.
 */
int isvalidnumber(const char number[])
{
    int length = strlen(number);
    if (length != 10)
        return 0;
    for (int i = 0; i < length; i++)
        if (number[i] < '0' || number[i] > '9')
            return 0;
    return 2;
}

/**
 * isvalidemail
 * ------------------
 * What: Validates basic email format ending in ".com".
 * Args:
 *   const char email[] – input string
 * Returns:
 *   int – >=1 for partial, 2 for full (has @ and ends in .com), 0 if invalid
 * Logic: Counts '@' and '.', ensures one of each, checks ".com" suffix.
 */
int isvalidemail(const char email[])
{
    int x = 0, spl = 0;
    for (int i = 0; email[i] != '\0'; i++)
    {
        if (email[i] == '@' &&
            email[i+1] != '\0' &&
            email[i+1] != '.' &&
            i != 0 &&
            email[i-1] != '.')
        {
            x = 1;
        }
        if (email[i] == '@' || email[i] == '.')
            spl++;
        if (email[i] == '.' &&
            email[i+1] == 'c' &&
            email[i+2] == 'o' &&
            email[i+3] == 'm' &&
            email[i+4] == '\0')
        {
            x = 2;
        }
    }
    if (spl != 2)
        x = 1;
    return x;
}

/**
 * checkname
 * ------------------
 * What: Checks if a name already exists in the list.
 * Args:
 *   const char *s – name to search
 * Returns:
 *   int – 1 if found, 0 otherwise
 * Logic: Iterates list and strcmp() to each node’s name.
 */
int checkname(const char *s)
{
    for (Node *p = head; p; p = p->next)
        if (!strcmp(p->name, s))
            return 1;
    return 0;
}

/**
 * checkphone
 * ------------------
 * What: Checks if a phone number already exists.
 * Args:
 *   const char *s – phone to search
 * Returns:
 *   int – 1 if found, 0 otherwise
 * Logic: Iterates list and strcmp() to each node’s phone.
 */
int checkphone(const char *s)
{
    for (Node *p = head; p; p = p->next)
        if (!strcmp(p->phone, s))
            return 1;
    return 0;
}

/**
 * checkemail
 * ------------------
 * What: Checks if an email already exists.
 * Args:
 *   const char *s – email to search
 * Returns:
 *   int – 1 if found, 0 otherwise
 * Logic: Iterates list and strcmp() to each node’s email.
 */
int checkemail(const char *s)
{
    for (Node *p = head; p; p = p->next)
        if (!strcmp(p->email, s))
            return 1;
    return 0;
}
