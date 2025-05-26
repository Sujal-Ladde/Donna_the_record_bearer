# Donna - The Record Bearer

A simple command‑line address book application that lets users add, edit, search, display, sort, and delete contacts. This repository walks through two versions of the project, showcasing an array‑based implementation (v1) and a linked‑list implementation (v2) with progressively improved sorting algorithms and data structures.

---

## Table of Contents

* [Features](#features)
* [Version History](#version-history)

  * [Version 1: Array + Bubble Sort](#version-1-array--bubble-sort)
  * [Version 2: Linked List + Merge Sort](#version-2-linked-list--merge-sort)
* [Installation](#installation)
* [Usage](#usage)
* [File Structure](#file-structure)
* [Drawbacks & Next Steps](#drawbacks--next-steps)
* [License](#license)

---

## Features

Both versions support:

1. **Add Contacts**: Insert new entries (name, phone, email).
2. **Display Contacts**: List all stored contacts.
3. **Search Contacts**: Lookup by name, phone number, or email.
4. **Update Contacts**: Modify existing entries.
5. **Delete Contacts**: Remove single or all contacts.
6. **Persistence**: Load from and save to a CSV file (`contacts.csv`).

Version‑specific sorting:

* **v1**: Sort by name, phone, or email using **bubble sort**.
* **v2**: Sort by name, phone, or email using **merge sort** on a dynamic linked list.

---

## Version History

### Version 1: Array + Bubble Sort

* **Data Structure**: Fixed‑size array of `struct Record`.
* **Sorting**: In‑place bubble sort (`O(n²)` time complexity).

#### Features

* Simple in‑memory storage of contacts.
* Straightforward indexing by array position.
* Basic menu‑driven interface.

#### Drawbacks

* **Fixed Capacity**: Maximum contacts limited by array size constant; adding beyond limit fails or overwrites.
* **Inefficient Sorting**: Bubble sort scales poorly for large data sets (worst‑case `O(n²)`).
* **Rigid Memory**: No dynamic allocation—wastes space if under‑utilized, cannot grow when needed.

---

### Version 2: Linked List + Merge Sort

* **Data Structure**: Singly linked list (`Node` structs allocated with `malloc`).
* **Sorting**: Top‑down merge sort on the list (`O(n log n)` time complexity).

#### Enhancements Over v1

1. **Dynamic Sizing**: List grows or shrinks at runtime; no fixed limits.
2. **Efficient Sorting**: Merge sort dramatically reduces sort time on larger data.
3. **Duplicate Handling**: Supports multiple contacts with the same name; disambiguation in edit/delete flows.
4. **Flexible Search/Delete/Edit**: Search, edit, or delete by name, phone, or email with user‑driven disambiguation when duplicates exist.
5. **Cleaner Memory Management**: Contacts freed correctly when deleted or on exit.

#### Drawbacks & Edge Cases

* **Fragmentation**: Non-contiguous memory may introduce overhead in very large lists.
* **Recursion Depth**: Merge sort recursion may hit stack limits on extremely large lists (millions of nodes).

---

## Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/Sujal-Ladde/Donna_the_record_bearer.git
   cd address-book-donna
   ```
2. Build with make:

   ```bash
  make clean
  make all
   ```

---

## Usage

Run the binary:

```bash
./donna
```

Main menu options:

1. Add A New Contact
2. Update A Contact
3. Display All Contacts
4. Search Contact
5. Delete Contact
6. Delete All Contacts
7. Display Sorted Contacts
8. Save to CSV
9. Exit

Follow on‑screen prompts to perform operations.

---

## File Structure

```
/address-book-donna
├── v1                                       # Version 1: Array + Bubble Sort
│   ├── contact.c                            # Core logic: dynamic array, CSV load/save, bubble sort, input validation via goto
│   ├── contact.h                            # Shared declarations for v1 structs and functions
│   ├── main.c                               # Entry point calling initialize()
│   └── contacts.csv                         # Sample data file loaded at startup
├── v2                                       # Version 2: Linked List + Merge Sort
│   └── contact.c                            # Core logic: linked list, merge sort, advanced edit/delete flows
├── contact.h                                # Common header (Node struct) for v2
├── Makefile                                 # Build targets for v1 and v2
└── README.md                                # This documentation
```

---

## Drawbacks & Next Steps

* **Performance**: Further optimize search by adding indexing or a hash map.
* **Concurrency**: Add file locking for multi‑process safety.
* **UI**: Migrate to a GUI or web interface for better usability.
* **Testing**: Integrate automated unit tests and CI pipeline.

---

## Author

**Sujal Ladde** • [laddesujal273@gmail.com](mailto:laddesujal273@gmail.com)
