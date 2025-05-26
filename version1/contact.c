#include "contact.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record
{
    char name[50];
    char phone[50];
    char email[50];
} Details;

Details *contacts;
int count = 0;
int x = 0;
int choice;
FILE *pF = NULL;

/**
 * clearBuffer
 * ------------------
 * What: Clears the terminal screen and flushes stdin.
 * Args: none
 * Returns: void
 * Logic: Calls system("clear") then fflush(stdin) to drop any unread input.
 */
void clearBuffer()
{
    system("clear"); // clears the output screen
    fflush(stdin);   // clears the input buffers like '\n'
}

/**
 * clearInputBuffer
 * ------------------
 * What: Drains leftover characters from stdin until newline or EOF.
 * Args: none
 * Returns: void
 * Logic: Loops getchar() until '\n' or EOF.
 */
void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

/**
 * my_pause
 * ------------------
 * What: Pauses execution until the user presses a key.
 * Args: none
 * Returns: void
 * Logic: Prints a prompt then calls getchar().
 */
void my_pause(){
    printf("Press any key to continue...\n");
    getchar();
}

/**
 * loginPage
 * ------------------
 * What: Displays the main menu and dispatches to the chosen action.
 * Args: none
 * Returns: void
 * Logic: Prints menu options, reads global 'choice', then calls matching handler.
 */
void loginPage()
{
    clearBuffer();

    printf("\n\t\t\t\t------------------------------------------- \n");
    printf("\t\t\t\t\t     >>> Donna <<< \n");
    printf("\t\t\t\t------------------------------------------- \n\n");
    printf("\t\t\t\t\t[1] Add A New Contact. \n");
    printf("\t\t\t\t\t[2] Update A Contact. \n");
    printf("\t\t\t\t\t[3] Display All Contacts. \n");
    printf("\t\t\t\t\t[4] Search Contact. \n");
    printf("\t\t\t\t\t[5] Delete Contact. \n");
    printf("\t\t\t\t\t[6] Delete All Contacts. \n");
    printf("\t\t\t\t\t[7] Display Sorted Contacts. \n");
    printf("\t\t\t\t\t[0] EXIT. \n");
    printf("\t\t\t\t\t[9] SAVE. \n");
    printf("\t\t\t\t   ------------------------------------ \n\n");
    printf("\t\t\t\tEnter the number & Hit ENTER: ");
    scanf("%d", &choice);
    clearInputBuffer();

    switch (choice)
    {
    case 1:
        addcontact();
        break;
    case 2:
        editcontact();
        break;
    case 3:
        displaycontacts();
        break;
    case 4:
        searchcontact();
        break;
    case 5:
        deletecontact();
        break;
    case 6:
        Deleteall();
        break;
    case 7:
        displaySortedContacts();
        break;
    case 0:
        Exit();
        break;
    case 9:
        save();
        break;
    default:
        printf("---------------------------------------\n");
        printf("ERROR: Invalid input please try again. \n");
        printf("---------------------------------------\n");
        my_pause();
        loginPage();
    }
}

/**
 * infoscreen
 * ------------------
 * What: Shows the introduction banner for the app.
 * Args: none
 * Returns: void
 * Logic: Prints description text, pauses, then calls loginPage().
 */
void infoscreen()
{
    clearBuffer();
    printf("\n\t\t\t\t------------------------------------------ \n");
    printf("\t\t\t\t\t      >>> Donna <<< \n");
    printf("\t\t\t\t------------------------------------------ \n\n");
    printf("\t\tDonna is a software application that allows users to store, organize\n\t and manage large numbers of contact records efficiently. The system is designed\n\t    to have functions such as adding, updating, viewing and deleting contacts. \n\n");
    printf("\t\t\t\t------------------------------------ \n");
    printf("\t\t\t\t------------------------------------ \n\n");
    my_pause();
    loginPage();
}

/**
 * endScreen
 * ------------------
 * What: Displays the closing banner and exits the program.
 * Args: none
 * Returns: void (exits)
 * Logic: Prints credits then calls exit(0).
 */
void endScreen()
{
    clearBuffer();
    printf("\n\t\t\t\t------------------------------------------ \n");
    printf("\t\t\t\t\t      >>> Donna <<<     \n");
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
 * isvalidname
 * ------------------
 * What: Validates that a name contains only letters and spaces.
 * Args:
 *   char name[] – the input string
 * Returns:
 *   int – 1 if valid, 0 otherwise
 * Logic: Loops through 'name', rejects on first non-letter, non-space.
 */
int isvalidname(char name[])
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
 * What: Validates that phone number is exactly 10 digits.
 * Args:
 *   char number[] – input phone string
 * Returns:
 *   int – 2 if valid, 0 otherwise
 * Logic: Checks length == 10, then each char is digit.
 */
int isvalidnumber(char number[])
{
    int length = strlen(number);

    if (length != 10)
    {
        return 0; // Not 10 digits
    }

    for (int i = 0; i < length; i++)
    {
        if (number[i] < '0' || number[i] > '9')
        {
            return 0; // Non-digit found
        }
    }

    return 2; // All digits and length is 10
}

/**
 * isvalidemail
 * ------------------
 * What: Validates basic email format ending in ".com".
 * Args:
 *   char email[] – input email string
 * Returns:
 *   int – 2 if fully valid, 1 if partial, 0 if invalid
 * Logic: Scans for '@' and '.', counts punctuation, checks ".com" suffix.
 */
int isvalidemail(char email[])
{
    int x = 0;
    int spl = 0;
    for (int i = 0; email[i] != '\0'; i++)
    {
        if (email[i] == '@' && email[i + 1] != '\0' && email[i + 1] != '.' && i != 0 && email[i - 1] != '.')
        {
            x = 1;
        }
        if (email[i] == '@' || email[i] == '.')
        {
            spl++;
        }
        if (email[i] == '.' && email[i + 1] == 'c' && email[i + 2] == 'o' && email[i + 3] == 'm' && email[i + 4] == '\0')
        {
            x = 2;
        }
    }
    if (spl != 2)
    {
        x = 1;
    }

    return x;
}

/**
 * addcontact
 * ------------------
 * What: Adds one or more new contacts to the dynamic array.
 * Args: none
 * Returns: void
 * Logic: Reallocates if needed, loops per new contact, validates and appends.
 */
void addcontact()
{
    clearBuffer();

    if (count < x)
    {
        contacts = (Details *)realloc(contacts, 2 * count * sizeof(Details));
    }
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t\t\t     >>> Add Contacts <<< \n");
    printf("\t\t|---------------------------------------------------------------| \n");

    int no_of_contacts;

    printf("\n\t\t\t   (Enter 0 to go back to the main menu)\n");
    printf("\n\t\t Enter the number of contacts to add - ");
    scanf("%d", &no_of_contacts);
    getchar();

    if (no_of_contacts == 0)
    {
        loginPage();
    }

    for (int i = 0; i < no_of_contacts; i++)
    {
        clearBuffer();

        printf("\t|---------------------------------------------------------------| \n");
        printf("\t\t\t\t Contact %d\n", i + 1);

        char data[50];
    jump1:
        printf("\t\t\t Enter Name = ");
        fgets(data, sizeof(data), stdin);
        data[strcspn(data, "\n")] = 0;
        if (checkname(data) == 0)
        {
            if (isvalidname(data))
            {
                strncpy(contacts[count].name, data, sizeof(contacts[count].name) - 1);
                contacts[count].name[sizeof(contacts[count].name) - 1] = '\0';
            }
            else
            {
                printf("\n\t\tInvalid Name! Please enter a valid name.\n");
                goto jump1;
            }
        }
        else
        {
            printf("\n\t\tName already exists! Please enter a different name.\n");
            goto jump1;
        }

    jump5:
        printf("\t\t\t Enter Phone = ");
        fgets(data, sizeof(data), stdin);
        data[strcspn(data, "\n")] = 0;
        if (checkphone(data) == 0)
        {
            if (isvalidnumber(data) == 2)
            {
                strncpy(contacts[count].phone, data, sizeof(contacts[count].phone) - 1);
                contacts[count].phone[sizeof(contacts[count].phone) - 1] = '\0';
            }
            else
            {
                printf("\n\t\tInvalid Phone Number! Please enter a valid phone number.\n");
                goto jump5;
            }
        }
        else
        {
            printf("\n\t\tPhone number already exists! Please enter a different phone number.\n");
            goto jump5;
        }

    jump8:
        printf("\t\t\t Enter Email = ");
        fgets(data, sizeof(data), stdin);
        data[strcspn(data, "\n")] = 0;
        if (checkemail(data) == 0)
        {
            if (isvalidemail(data) == 2)
            {
                strncpy(contacts[count].email, data, sizeof(contacts[count].email) - 1);
                contacts[count].email[sizeof(contacts[count].email) - 1] = '\0';
                count++;
            }
            else
            {
                printf("\n\t\tInvalid Email! Please enter a valid email.\n");
                goto jump8;
            }
        }
        else
        {
            printf("\n\t\tEmail already exists! Please enter a different email.\n");
            goto jump8;
        }
    }

    clearBuffer();
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t| \tName\t| \tPhone Number\t  |  \t Email\t\t| \n");
    printf("\t\t|---------------------------------------------------------------| \n");
    int start = count - no_of_contacts;
    for (int i = 0; i < no_of_contacts; i++)
    {
        printf("\t\t| %s \t| %s \t| %s \t|\n",
               contacts[start + i].name,
               contacts[start + i].phone,
               contacts[start + i].email);
    }

    my_pause();
    loginPage();
}

/**
 * displaycontacts
 * ------------------
 * What: Displays all stored contacts in a tabular format.
 * Args: none
 * Returns: void
 * Logic: Iterates the contacts[] array up to 'count' and prints each entry.
 */
void displaycontacts()
{
    clearBuffer();
    printf("\t\t\t\t\t----------------------------- \n");
    printf("\t\t\t\t\t    >>> Contacts List <<< \n");
    printf("\t\t\t\t\t----------------------------- \n");
    printf("\t\t\t\t\t    Total Contacts = %d\n", count);
    printf("\t\t|-------------------------------------------------------------------------| \n");
    printf("\t\t| %-20s | %-15s | %-30s | \n", "Name", "Phone Number", "Email");
    printf("\t\t|-------------------------------------------------------------------------| \n\n");

    for (int i = 0; i < count; i++)
    {
        printf("\t\t| %-20s | %-15s | %-30s  \n",
               contacts[i].name,
               contacts[i].phone,
               contacts[i].email);
    }
    getchar();
    my_pause();
    loginPage();
}

/**
 * searchByName
 * ------------------
 * What: Finds and displays contacts whose name contains the query substring.
 * Args: none
 * Returns: void
 * Logic: Prompts for substring, loops through contacts[], uses strstr() for match.
 */
void searchByName()
{
    clearBuffer();
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t\t\t     >>> Search By Name <<< \n");
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\n\t\t\t   (Enter 0 to go back to the main menu)\n\n");
    char searchName[50];

    printf("\t\tEnter the name to search - ");
    fgets(searchName, sizeof(searchName), stdin);
    searchName[strcspn(searchName, "\n")] = 0;
    if (strcmp(searchName, "0") == 0)
    {
        loginPage();
    }

    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t| \tName\t| \tPhone Number\t  |  \t Email\t\t| \n");
    printf("\t\t|---------------------------------------------------------------| \n");

    for (int i = 0; i < count; i++)
    {
        if (strstr(contacts[i].name, searchName))
        {
            printf("\t\t| %-20s | %-15s | %-30s |\n",
                   contacts[i].name,
                   contacts[i].phone,
                   contacts[i].email);
        }
    }

    my_pause();
    searchcontact();
}

/**
 * searchByNumber
 * ------------------
 * What: Finds and displays contacts whose phone contains the query substring.
 * Args: none
 * Returns: void
 * Logic: Prompts for substring, loops through contacts[], uses strstr().
 */
void searchByNumber()
{
    clearBuffer();
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t\t\t     >>> Search By Phone Number <<< \n");
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\n\t\t\t   (Enter 0 to go back to the main menu)\n\n");
    char searchphone[50];

    printf("\t\tEnter the number to search - ");
    fgets(searchphone, sizeof(searchphone), stdin);
    searchphone[strcspn(searchphone, "\n")] = 0;
    if (strcmp(searchphone, "0") == 0)
    {
        loginPage();
    }

    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t| \tName\t| \tPhone Number\t  |  \t Email\t\t| \n");
    printf("\t\t|---------------------------------------------------------------| \n");

    for (int i = 0; i < count; i++)
    {
        if (strstr(contacts[i].phone, searchphone))
        {
            printf("\t\t| %-20s | %-15s | %-30s |\n",
                   contacts[i].name,
                   contacts[i].phone,
                   contacts[i].email);
        }
    }

    my_pause();
    searchcontact();
}

/**
 * searchByEmail
 * ------------------
 * What: Finds and displays contacts whose email contains the query substring.
 * Args: none
 * Returns: void
 * Logic: Prompts for substring, loops through contacts[], uses strstr().
 */
void searchByEmail()
{
    clearBuffer();
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t\t\t     >>> Search By Email <<< \n");
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\n\t\t\t   (Enter 0 to go back to the main menu)\n\n");
    char searchemail[50];

    printf("\t\tEnter the Email to search - ");
    fgets(searchemail, sizeof(searchemail), stdin);
    searchemail[strcspn(searchemail, "\n")] = 0;
    if (strcmp(searchemail, "0") == 0)
    {
        loginPage();
    }

    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t| \tName\t| \tPhone Number\t  |  \t Email\t\t| \n");
    printf("\t\t|---------------------------------------------------------------| \n");

    for (int i = 0; i < count; i++)
    {
        if (strstr(contacts[i].email, searchemail))
        {
            printf("\t\t| %-20s | %-15s | %-30s |\n",
                   contacts[i].name,
                   contacts[i].phone,
                   contacts[i].email);
        }
    }

    my_pause();
    searchcontact();
}

/**
 * searchcontact
 * ------------------
 * What: Menu to choose search by name, phone, or email.
 * Args: none
 * Returns: void
 * Logic: Reads sub-choice then calls the corresponding searchByX().
 */
void searchcontact()
{
    clearBuffer();
    int searchchoice;
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t\t\t     >>> Search Contacts <<< \n");
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\n\t\t\t   (Enter 0 to go back to the main menu)\n");
    printf("\n\t\t\t1. Search by Name");
    printf("\n\t\t\t2. Search by Phone Number");
    printf("\n\t\t\t3. Search by Email");
    printf("\n\n\t\tEnter the search parameter - ");
    scanf("%d", &searchchoice);
    clearInputBuffer();

    if (searchchoice == 0)
    {
        loginPage();
    }
    switch (searchchoice)
    {
    case 1:
        searchByName();
        break;
    case 2:
        searchByNumber();
        break;
    case 3:
        searchByEmail();
        break;
    default:
        printf("Invalid search parameter");
    }
    my_pause();
    loginPage();
}

/**
 * deletecontact
 * ------------------
 * What: Deletes the first contact that exactly matches a given name.
 * Args: none
 * Returns: void
 * Logic: Prompts for a name, validates it, shifts array elements down to overwrite.
 */
void deletecontact()
{
    clearBuffer();
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t\t\t     >>> Delete a Contact <<< \n");
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\n\t\t\t   (Enter 0 to go back to the main menu)\n");

    char searchname[50];
jump3:
    printf("\t\tEnter the Name to Delete - ");
    fgets(searchname, sizeof(searchname), stdin);
    searchname[strcspn(searchname, "\n")] = 0;
    if (strcmp(searchname, "0") == 0)
    {
        loginPage();
    }
    if (isvalidname(searchname))
    {
        for (int i = 0; i < count; i++)
        {
            if (strcmp(contacts[i].name, searchname) == 0)
            {
                printf("\n\t\t| %-20s | %-15s | %-30s |\n",
                       contacts[i].name,
                       contacts[i].phone,
                       contacts[i].email);
                for (int j = i; j < count - 1; j++)
                {
                    contacts[j] = contacts[j + 1];
                }
                count--;
                printf("\n\n\t\tContact Deleted Successfully!\n");
                my_pause();
                loginPage();
                return;
            }
        }
        printf("Contact not found!");
    }
    else
    {
        printf("\n\t\tInvalid Name! Please enter a valid name.\n");
        goto jump3;
    }
    my_pause();
    loginPage();
}

/**
 * Deleteall
 * ------------------
 * What: Deletes all contacts after confirmation.
 * Args: none
 * Returns: void
 * Logic: Prompts Y/N; if yes sets count=0, else does nothing.
 */
void Deleteall()
{
    clearBuffer();
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t\t\t   >>> DELETE ALL CONTACTS!! <<< \n");
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\n\n\t\tAre you sure you want to delete all contacts? (Y/N) - ");
    char choice = getchar();
    clearInputBuffer();
    if (choice == 'Y' || choice == 'y')
    {
        count = 0;
        printf("\n\n\t\t\t\tAll contacts deleted successfully!\n\t\t\t\t");
    }
    else
    {
        printf("\n\n\t\t\t\tOperation Cancelled!\n\t\t\t");
    }
    my_pause();
    loginPage();
}

/**
 * editcontact
 * ------------------
 * What: Finds and updates a contact field (name/phone/email).
 * Args: none
 * Returns: void
 * Logic: Menu to choose field, prompts for old value, validates, replaces with new.
 */
void editcontact()
{
    clearBuffer();
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t\t\t     >>> Edit a Contact <<< \n");
    printf("\t\t|---------------------------------------------------------------| \n");

    char searchname[50];
    char searchphone[50];
    char searchemail[50];
    char newname[50];
    char newphone[50];
    char newemail[50];
    int flag, choice;

jump_here:
    printf("\n\t\t\t   (Enter 0 to go back to the main menu)\n");
    printf("\n\n\t\twhat do you want to edit?\n");
    printf("\t\t1. Name\n");
    printf("\t\t2. Phone Number\n");
    printf("\t\t3. Email\n");
    printf("\n\t\tEnter your choice - ");
    scanf("%d", &choice);
    getchar();
    if (choice == 0)
    {
        loginPage();
    }

    flag = 0;
    switch (choice)
    {
    case 1:
    jump4:
        printf("\n\n\tEnter the name to edit - ");
        fgets(searchname, sizeof(searchname), stdin);
        searchname[strcspn(searchname, "\n")] = 0;
        if (strcmp(searchname, "0") == 0)
        {
            loginPage();
        }
        if (isvalidname(searchname))
        {
            for (int i = 0; i < count; i++)
            {
                if (strcmp(contacts[i].name, searchname) == 0)
                {
                    printf("\t\tFound the contact!\n");
                    flag = 1;
                }
            }
            if (!flag)
            {
                printf("\n\t\tContact not found!\n");
                goto jump_here;
            }
            printf("\n\n\tEnter the new name - ");
            fgets(newname, sizeof(newname), stdin);
            newname[strcspn(newname, "\n")] = 0;
            if (strcmp(newname, "0") == 0)
            {
                loginPage();
            }
            for (int i = 0; i < count; i++)
            {
                if (strcmp(contacts[i].name, searchname) == 0)
                {
                    strcpy(contacts[i].name, newname);
                    printf("\n\tName updated successfully!\n");
                    printf("\t\t| %-20s | %-15s | %-30s |\n",
                           contacts[i].name,
                           contacts[i].phone,
                           contacts[i].email);
                    my_pause();
                    loginPage();
                }
            }
        }
        else
        {
            printf("\n\t\tInvalid Name! Please enter a valid name.\n");
            goto jump4;
        }
        break;

    case 2:
    jump7:
        printf("\n\n\tEnter the phone number to edit - ");
        fgets(searchphone, sizeof(searchphone), stdin);
        searchphone[strcspn(searchphone, "\n")] = 0;
        if (strcmp(searchphone, "0") == 0)
        {
            loginPage();
        }
        if (isvalidnumber(searchphone) == 2)
        {
            for (int i = 0; i < count; i++)
            {
                if (strcmp(contacts[i].phone, searchphone) == 0)
                {
                    printf("\t\tFound the contact!\n");
                    flag = 1;
                }
            }
            if (!flag)
            {
                printf("\n\t\tContact not found!\n");
            }
            printf("\n\n\tEnter the new phone number - ");
            fgets(newphone, sizeof(newphone), stdin);
            newphone[strcspn(newphone, "\n")] = 0;
            if (strcmp(newphone, "0") == 0)
            {
                loginPage();
            }
            if (isvalidnumber(newphone) == 2)
            {
                for (int i = 0; i < count; i++)
                {
                    if (strcmp(contacts[i].phone, searchphone) == 0)
                    {
                        strcpy(contacts[i].phone, newphone);
                        printf("\n\tPhone number updated successfully!\n");
                        printf("\t\t| %-20s | %-15s | %-30s |\n",
                               contacts[i].name,
                               contacts[i].phone,
                               contacts[i].email);
                        my_pause();
                        loginPage();
                    }
                }
            }
        }
        else
        {
            printf("\n\t\tInvalid Phone Number! Please enter a valid phone number.\n");
            goto jump7;
        }
        break;

    case 3:
    jump10:
        printf("\n\n\tEnter the email to edit - ");
        fgets(searchemail, sizeof(searchemail), stdin);
        searchemail[strcspn(searchemail, "\n")] = 0;
        if (strcmp(searchemail, "0") == 0)
        {
            loginPage();
        }
        if (isvalidemail(searchemail) == 2)
        {
            for (int i = 0; i < count; i++)
            {
                if (strcmp(contacts[i].email, searchemail) == 0)
                {
                    printf("\t\tFound the contact!\n");
                    flag = 1;
                }
            }
            if (!flag)
            {
                printf("\n\t\tContact not found!\n");
                goto jump_here;
            }
        jump11:
            printf("\n\n\tEnter the new email - ");
            fgets(newemail, sizeof(newemail), stdin);
            newemail[strcspn(newemail, "\n")] = 0;
            if (strcmp(newemail, "0") == 0)
            {
                loginPage();
            }
            if (isvalidemail(newemail) == 2)
            {
                for (int i = 0; i < count; i++)
                {
                    if (strcmp(contacts[i].email, searchemail) == 0)
                    {
                        strcpy(contacts[i].email, newemail);
                        printf("\n\tEmail updated successfully!\n");
                        printf("\t\t| %-20s | %-15s | %-30s |\n",
                               contacts[i].name,
                               contacts[i].phone,
                               contacts[i].email);
                        my_pause();
                        loginPage();
                    }
                }
            }
            else
            {
                printf("\n\t\tInvalid Email! Please enter a valid email.\n");
                goto jump11;
            }
        }
        else
        {
            printf("\n\t\tInvalid Email! Please enter a valid email.\n");
            goto jump10;
        }
        break;

    default:
        printf("\n\n\tInvalid choice!\n");
        my_pause();
        loginPage();
    }
}

/**
 * sortByName
 * ------------------
 * What: Sorts contacts[] by name using bubble sort.
 * Args: none
 * Returns: void
 * Logic: Nested loops swapping adjacent out-of-order entries.
 */
void sortByName()
{
    Details temp;
    for (int i = 0; i < count; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            if (strcmp(contacts[j].name, contacts[j + 1].name) > 0)
            {
                temp = contacts[j];
                contacts[j] = contacts[j + 1];
                contacts[j + 1] = temp;
            }
        }
    }
    displaycontacts();
}

/**
 * sortByNumber
 * ------------------
 * What: Sorts contacts[] by phone number using bubble sort.
 * Args: none
 * Returns: void
 * Logic: Nested loops swapping adjacent out-of-order entries.
 */
void sortByNumber()
{
    Details temp;
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            if (strcmp(contacts[j].phone, contacts[j + 1].phone) > 0)
            {
                temp = contacts[j];
                contacts[j] = contacts[j + 1];
                contacts[j + 1] = temp;
            }
        }
    }
    displaycontacts();
}

/**
 * sortByEmail
 * ------------------
 * What: Sorts contacts[] by email using bubble sort.
 * Args: none
 * Returns: void
 * Logic: Nested loops swapping adjacent out-of-order entries.
 */
void sortByEmail()
{
    Details temp;
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            if (strcmp(contacts[j].email, contacts[j + 1].email) > 0)
            {
                temp = contacts[j];
                contacts[j] = contacts[j + 1];
                contacts[j + 1] = temp;
            }
        }
    }
    displaycontacts();
}

/**
 * displaySortedContacts
 * ------------------
 * What: Shows sort menu then calls the chosen sort function.
 * Args: none
 * Returns: void
 * Logic: Reads choice and dispatches to sortByName/Number/Email.
 */
void displaySortedContacts()
{
    clearBuffer();
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("\t\t\t\t     >>> Display Sorted Contacts <<< \n");
    printf("\t\t|---------------------------------------------------------------| \n");
    printf("How do you want to sort the contacts?\n");
    printf("1. Sort by Name\n");
    printf("2. Sort by Phone Number\n");
    printf("3. Sort by Email\n");
    printf("\n\tEnter your choice - ");
    scanf("%d", &choice);
    getchar();
    switch (choice)
    {
    case 1:
        sortByName();
        break;
    case 2:
        sortByNumber();
        break;
    case 3:
        sortByEmail();
        break;
    default:
        printf("Invalid choice!");
    }
    my_pause();
    loginPage();
}

/**
 * checkemail
 * ------------------
 * What: Checks if the given email already exists.
 * Args:
 *   char email[] – the email to check
 * Returns:
 *   int – 1 if found, 0 otherwise
 * Logic: Linear scan of contacts[] comparing strings.
 */
int checkemail(char email[])
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(contacts[i].email, email) == 0)
        {
            return 1;
        }
    }
    return 0;
}

/**
 * checkphone
 * ------------------
 * What: Checks if the given phone number already exists.
 * Args:
 *   char phone[] – the phone to check
 * Returns:
 *   int – 1 if found, 0 otherwise
 * Logic: Linear scan of contacts[] comparing strings.
 */
int checkphone(char phone[])
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(contacts[i].phone, phone) == 0)
        {
            return 1;
        }
    }
    return 0;
}

/**
 * checkname
 * ------------------
 * What: Checks if the given name already exists.
 * Args:
 *   char name[] – the name to check
 * Returns:
 *   int – 1 if found, 0 otherwise
 * Logic: Linear scan of contacts[] comparing strings.
 */
int checkname(char name[])
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(contacts[i].name, name) == 0)
        {
            return 1;
        }
    }
    return 0;
}

/**
 * initialize
 * ------------------
 * What: Loads contacts from "contacts.csv" into memory.
 * Args: none
 * Returns: void
 * Logic: Counts lines, mallocs 2*x entries, then fscanf() each record.
 */
void initialize()
{
    pF = fopen("contacts.csv", "r");
    int ch;
    while ((ch = fgetc(pF)) != EOF)
    {
        if (ch == '\n')
        {
            x++;
        }
    }
    rewind(pF);

    contacts = (Details *)malloc(2 * x * sizeof(Details));
    fclose(pF);

    pF = fopen("contacts.csv", "r");
    for (int i = 0; i < x; i++)
    {
        fscanf(pF, "%49[^,],%14[^,],%49[^\n]\n",
               contacts[i].name,
               contacts[i].phone,
               contacts[i].email);
        count++;
    }
    fclose(pF);

    infoscreen();
}

/**
 * save
 * ------------------
 * What: Writes all contacts back to "contacts.csv".
 * Args: none
 * Returns: void
 * Logic: Opens file for write, fprintf() each record, then prompts to exit/menu.
 */
void save()
{
    pF = fopen("contacts.csv", "w");
    if (pF == NULL)
    {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        if (fprintf(pF, "%s,%s,%s\n",
                    contacts[i].name,
                    contacts[i].phone,
                    contacts[i].email) < 0)
        {
            perror("Error writing to file");
            fclose(pF);
            return;
        }
    }
    fclose(pF);

    clearBuffer();
    printf("\n\n\t\t\t\t\tContacts saved successfully!\n");
    char ch;
    printf("\n\t\t\t\tEnter 0 to go back or y to EXIT - ");
    scanf(" %c", &ch);
    if (ch == '0')
    {
        loginPage();
    }
    else if (ch == 'Y' || ch == 'y')
    {
        endScreen();
        free(contacts);
    }
    else
    {
        loginPage();
    }
}

/**
 * Exit
 * ------------------
 * What: Prompts user to save before exiting.
 * Args: none
 * Returns: void
 * Logic: Asks Y/N; on yes calls save(), else frees memory and exits.
 */
void Exit()
{
    printf("\n\t\t\t\t(Enter 0 to go back to the main menu)\n");
    printf("\n\t\t\t\tDo you want to save the changes? (Y/N) - ");
    char ch;
    scanf(" %c", &ch);
    if (ch == '0')
    {
        loginPage();
    }
    else if (ch == 'Y' || ch == 'y')
    {
        save();
        printf("\n\n\t\t\t\t\t Contacts saved successfully!\n\t\t\t\t");
        my_pause();
        endScreen();
    }
    else
    {
        free(contacts);
        endScreen();
    }
}

/**
 * addgiven
 * ------------------
 * What: Writes a set of dummy contacts to "contacts.csv".
 * Args: none
 * Returns: void
 * Logic: Opens file, prints header, then loops over hard-coded array.
 */
void addgiven()
{
    pF = fopen("contacts.csv", "w");
    if (pF == NULL)
    {
        perror("Error opening file");
        return;
    }

    Details dummyContacts[] = {
        {"John Doe",    "1234567890", "john@example.com"},
        {"Alice Smith", "0987654321", "alice@example.com"},
        {"Bob Johnson", "1112223333", "bob@company.com"},
        {"Carol White", "4445556666", "carol@company.com"},
        {"David Brown", "7778889999", "david@example.com"},
        {"Eve Davis",   "6665554444", "eve@example.com"},
        {"Frank Miller","3334445555", "frank@example.com"},
        {"Grace Wilson","2223334444", "grace@example.com"},
        {"Hannah Clark","5556667777", "hannah@example.com"},
        {"Ian Lewis",   "8889990000", "ian@example.com"}
    };

    fprintf(pF, "--------------------------------\n");
    fprintf(pF, "Name\tPhone\tEmail\n");
    fprintf(pF, "--------------------------------\n");

    for (int i = 0; i < 10; i++)
    {
        fprintf(pF, "%s\t%s\t%s\n",
                dummyContacts[i].name,
                dummyContacts[i].phone,
                dummyContacts[i].email);
    }

    fclose(pF);
}
