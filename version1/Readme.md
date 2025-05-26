# Address Book (Donna) – Version 1

A simple command-line address book application using a fixed-size array and bubble sort for ordering.

---

## Table of Contents

* [Features](#features)
* [Implementation Details](#implementation-details)
* [Setup & Installation](#setup--installation)
* [Usage](#usage)
* [File Structure](#file-structure)
* [Limitations](#limitations)
* [Next Steps](#next-steps)
* [License](#license)

---

## Features

* **Add Contacts**: Insert new entries (name, phone, email).
* **Display Contacts**: List all stored contacts.
* **Search Contacts**: Lookup by name, phone number, or email.
* **Update Contacts**: Modify existing entries.
* **Delete Contacts**: Remove single or all contacts.
* **Sorting**: Order contacts by name, phone, or email using bubble sort.
* **Persistence**: Load from and save to a CSV file (`contacts.csv`).
* **Simple Menu**: Text-based interface with input validation.

---

## Implementation Details

* **Data Structure**: `struct Record records[MAX_CONTACTS]` – fixed-size array in memory.
* **Sorting Algorithm**: Bubble sort (`O(n²)`), swapping adjacent records until sorted.
* **Validation**:

  * **Names**: must consist only of letters (`A–Z`, `a–z`) and spaces.
  * **Phone Numbers**: exactly **10** characters, all digits (`0–9`).
  * **Emails**:

    1. Must contain exactly one `@` symbol.
    2. Must include a single `.` (dot) before the domain suffix.
    3. Domain must end with `.com`.
    4. Prohibits consecutive or trailing punctuation (no `.@` or `@@`).
* **File I/O**:

  * On startup, reads existing contacts from `contacts.csv`.
  * On saving or exiting, writes all contacts back to `contacts.csv`.

---

## Setup & Installation

1. Clone the repository and navigate to version 1:

   ```bash
   git clone https://github.com/Sujal-Ladde/Donna_the_record_bearer/tree/main/version1
   cd address-book-donna/v1
   ```
2. Build with the provided Makefile:

   ```bash
   make
   ```

---

## Usage

Run the application:

```bash
./donna
```

Choose from the main menu:

1. Add A New Contact
2. Update A Contact
3. Display All Contacts
4. Search Contact
5. Delete Contact
6. Delete All Contacts
7. Display Sorted Contacts
8. Save
9. Exit

Follow the prompts for each operation. Enter `0` at any prompt to cancel and return to the menu.

---

## File Structure

```
/v1
├── .vscode        # VSCode workspace settings (optional)
├── main.c         # Entry point: initializes data, shows menu, dispatches actions
├── contact.c      # Core logic: array operations, validation, CSV I/O, bubble sort
├── contact.h      # Struct definitions (Record) and function prototypes
├── contacts.csv   # Sample data file used for loading and saving contacts
└── Makefile       # Build script (run `make` to compile)
```

---

## Limitations

* **Fixed Capacity**: Limited by the `MAX_CONTACTS` constant; cannot exceed array bounds.
* **Inefficient Sorting**: Bubble sort has poor performance (`O(n²)`) on large datasets.
* **Memory Usage**: Array allocates full capacity regardless of actual number of contacts.
* **Basic Validation**: Email and phone checks are minimal; may not catch all invalid formats.

---

## Next Steps

* Transition to a dynamic structure (linked list) to remove capacity limits.
* Implement a faster sorting algorithm (merge sort or quicksort) to improve scalability.
* Enhance input validation and error handling for robustness.
* Add unit tests and integrate a CI pipeline for automated testing.

---

## Author

**Sujal Ladde** • [laddesujal273@gmail.com](mailto:laddesujal273@gmail.com)

Happy hiding! 🚀
