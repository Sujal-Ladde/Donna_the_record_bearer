# Donna – Version 2

An enhanced command-line address book application using a dynamic singly linked list and efficient merge sort for ordering. Version 2 builds on the array-based v1 by introducing scalable data structures, faster sorting, and flexible edit/delete workflows.

---

## Table of Contents

* [Features](#features)
* [Implementation Highlights](#implementation-highlights)
* [Setup & Installation](#setup--installation)
* [Usage](#usage)
* [File Structure](#file-structure)
* [Performance & Limitations](#performance--limitations)
* [Future Enhancements](#future-enhancements)
* [License](#license)

---

## Features

1. **Dynamic Storage**: Contacts stored in a `malloc`-managed singly linked list—no fixed capacity.
2. **Fast Sorting**: Merge sort on the linked list (`O(n log n)`), selectable by name, phone, or email.
3. **Flexible Search/Update/Delete**:

   * Lookup by name, phone number, or email.
   * Handles duplicates by listing all matches for disambiguation.
   * Edit or delete a specific contact from multiple matches.
4. **Full CRUD**: Create, read, update, delete single or all contacts.
5. **CSV Persistence**: Load from and save to `contacts.csv` on disk.
6. **Validation**: Ensures proper name (letters/spaces), 10-digit phone, and well-formed `.com` emails.
7. **User Interface**: Clear menu-driven prompts with cancel (`0`) options at any step.

---

## Implementation Highlights

* **Node Structure**: Each contact is a `Node` containing `name`, `phone`, `email`, and `next` pointer.
* **Merge Sort**:

  * **Split**: Fast/slow pointer splits the list into halves.
  * **Recursive Sort**: Sort each half.
  * **Merge**: In-place merge by pointer manipulation.
* **Duplicate Handling**:

  * `deletecontact()` and `editcontact()` gather all nodes matching the search key.
  * If multiple, prints indexed list (with all fields) and prompts user to choose the target.
* **Memory Management**:

  * Allocates new nodes on add.
  * Frees nodes on delete and on program exit.

---

## Setup & Installation

1. Clone the repository and go to v2:

   ```bash
   git clone https://github.com/yourusername/address-book-donna.git
   cd address-book-donna/v2
   ```
2. Build using the provided Makefile:

   ```bash
   make
   ```

   This produces the `donna` executable.

---

## Usage

Run the program:

```bash
./donna
```

Main menu options:

```
1) Add A New Contact
2) Update A Contact
3) Display All Contacts
4) Search Contact
5) Delete Contact
6) Delete All Contacts
7) Display Sorted Contacts
9) Save to CSV
0) Exit
```

* Enter the number for the desired action.
* Enter `0` at any prompt to cancel and return to the main menu.

---

## File Structure

```
/v2
├── .vscode        # Editor settings (optional)
├── contact.h      # Node definition and function prototypes
├── contact.c      # Core logic: linked list operations, merge sort, search/edit/delete flows
├── main.c         # Entry point: initialization, menu loop
├── contacts.csv   # Data file for persistent storage
├── Makefile       # Build script (`make donna`)
└── donna          # Compiled executable (after make)
```

---

## Performance & Limitations

* **Time Complexity**:

  * Add/Search/Delete/Edit: O(n) in the worst case (traverse list).
  * Sort: O(n log n) due to merge sort.
* **Memory Overhead**: Each contact allocates a separate `Node`—fragmentation may occur under heavy churn.
* **Recursion Depth**: Merge sort recursion could hit call-stack limits on extremely large lists (e.g., millions of nodes).

---

## Future Enhancements

* **Indexing**: Add hash-based indexing for O(1) lookups by phone or email.
* **Persistence Upgrades**: Support JSON or binary formats and incremental saves.
* **Concurrency**: Implement file locks or thread-safe operations.
* **Testing & CI**: Integrate unit tests and continuous integration workflows.
* **UI Improvements**: Create a cross-platform GUI or web front-end.

---

## Author

**Sujal Ladde** • [laddesujal273@gmail.com](mailto:laddesujal273@gmail.com)
