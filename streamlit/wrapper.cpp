#include <pybind11/pybind11.h>
#include <pybind11/stl.h> // For automatic conversion of STL containers like std::vector
#include <vector>
#include <string>
#include <stdexcept> // For throwing exceptions

// This extern "C" block is crucial if contact.h doesn't already have it
// for C++ compatibility. Our updated contact.h includes this.
extern "C" {
#include "contact.h"
}

namespace py = pybind11;

// Helper function to convert C's ContactData array to a Python list of dictionaries
py::list convert_c_array_to_py_list(ContactData* data_array, int num_items) {
    py::list py_list;
    if (data_array && num_items > 0) {
        for (int i = 0; i < num_items; ++i) {
            py::dict contact_dict;
            contact_dict["name"] = std::string(data_array[i].name);
            contact_dict["phone"] = std::string(data_array[i].phone);
            contact_dict["email"] = std::string(data_array[i].email);
            py_list.append(contact_dict);
        }
        // IMPORTANT: Free the memory allocated by the C functions
        free_contact_data_array(data_array);
    }
    return py_list;
}

PYBIND11_MODULE(contact_manager_c, m) {
    m.doc() = "Python bindings for the C contact management library";

    m.def("initialize", &initialize_library, "Initializes the contact list from contacts.csv");

    m.def("add_contact", 
          [](const char* name, const char* phone, const char* email) {
              int result = add_contact_py(name, phone, email);
              if (result == 1) return py::cast("Contact added successfully.");
              else if (result == -1) throw std::runtime_error("Invalid name format.");
              else if (result == -2) throw std::runtime_error("Invalid phone number format (must be 10 digits).");
              else if (result == -3) throw std::runtime_error("Invalid email format (must end with .com).");
              else if (result == -4) throw std::runtime_error("Phone number already exists.");
              else if (result == -5) throw std::runtime_error("Email already exists.");
              else if (result == -6) throw std::runtime_error("Memory allocation failed.");
              else throw std::runtime_error("Unknown error adding contact.");
          }, 
          "Adds a new contact",
          py::arg("name"), py::arg("phone"), py::arg("email"));

    m.def("get_contact_count", &get_contacts_count_py, "Gets the total number of contacts");

    m.def("get_all_contacts", []() {
        int num_contacts = 0;
        ContactData* contacts_c_array = get_all_contacts_py(&num_contacts);
        return convert_c_array_to_py_list(contacts_c_array, num_contacts);
    }, "Retrieves all contacts as a list of dictionaries");

    m.def("search_contacts", [](const char* query, int search_type) {
        int num_found = 0;
        ContactData* results_c_array = search_contacts_py(query, search_type, &num_found);
        return convert_c_array_to_py_list(results_c_array, num_found);
    }, "Searches contacts. search_type: 1 for name, 2 for phone, 3 for email.",
        py::arg("query"), py::arg("search_type"));

    m.def("delete_contact_by_email", 
        [](const char* email) {
            int result = delete_contact_by_email_py(email);
            if (result == 1) return true; // Deleted
            return false; // Not found or error
        }, 
        "Deletes a contact by email. Returns true if deleted, false otherwise.",
        py::arg("email"));

    m.def("edit_contact", 
        [](const char* old_email, const char* new_name, const char* new_phone, const char* new_email) {
            int result = edit_contact_py(old_email, new_name, new_phone, new_email);
            if (result == 1) return py::cast("Contact updated successfully.");
            else if (result == 0) throw std::runtime_error("Contact with original email not found.");
            else if (result == -1) throw std::runtime_error("Invalid new name format.");
            else if (result == -2) throw std::runtime_error("Invalid new phone number format.");
            else if (result == -3) throw std::runtime_error("Invalid new email format.");
            else if (result == -4) throw std::runtime_error("New phone number already exists for another contact.");
            else if (result == -5) throw std::runtime_error("New email already exists for another contact.");
            else throw std::runtime_error("Unknown error editing contact.");
        }, 
        "Edits an existing contact identified by old_email",
        py::arg("old_email"), py::arg("new_name"), py::arg("new_phone"), py::arg("new_email"));
    
    m.def("delete_all_contacts", &delete_all_contacts_py, "Deletes all contacts from memory. Does not save automatically.");
    m.def("save_contacts", &save_contacts_py, "Saves all contacts to contacts.csv");

    // Sorting
    m.def("sort_contacts_by_name", &sort_contacts_by_name_py, "Sorts contacts by name");
    m.def("sort_contacts_by_phone", &sort_contacts_by_phone_py, "Sorts contacts by phone number");
    m.def("sort_contacts_by_email", &sort_contacts_by_email_py, "Sorts contacts by email");

    // Validation and utility functions
    m.def("is_valid_name", [](const char* name) {
        return isvalidname(name) == 1; // [cite: 1]
    }, "Checks if the name is valid (letters and spaces only)", py::arg("name"));

    m.def("is_valid_number", [](const char* number) {
        return isvalidnumber(number) == 2; // Original C returns 2 if valid (10 digits) [cite: 1]
    }, "Checks if the phone number is valid (exactly 10 digits)", py::arg("number"));

    m.def("is_valid_email", [](const char* email) {
        return isvalidemail(email) == 2; // Original C returns 2 if valid (contains @, ., ends with .com) [cite: 1]
    }, "Checks if the email is valid (basic format with @, ., and ending '.com')", py::arg("email"));

    m.def("check_phone_exists", [](const char* phone) {
        return checkphone(phone) == 1; // [cite: 1]
    }, "Checks if a phone number already exists", py::arg("phone"));

    m.def("check_email_exists", [](const char* email) {
        return checkemail(email) == 1; // [cite: 1]
    }, "Checks if an email already exists", py::arg("email"));
}