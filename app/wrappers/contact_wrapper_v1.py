# contact_wrapper_v1.py
import ctypes
import os
import platform

# Define the ContactRecord structure for ctypes, matching your C struct
class ContactRecord(ctypes.Structure):
    _fields_ = [("name", ctypes.c_char * 50),
                ("phone", ctypes.c_char * 50),
                ("email", ctypes.c_char * 50)]

# contact_wrapper_v1.py
# ... (imports and ContactRecord class definition remain the same) ...

# Determine library extension based on OS
lib_filename_base = "contact_v1_lib"
lib_ext = ""
if platform.system() == "Windows":
    lib_ext = ".dll"
elif platform.system() == "Linux":
    lib_ext = ".so" # Assumes .so, might be libcontact_v1_lib.so
elif platform.system() == "Darwin": # macOS
    lib_ext = ".dylib"
else:
    raise OSError("Unsupported OS for loading shared C library")

lib_filename = f"{lib_filename_base}{lib_ext}"

# Path to the compiled_libs directory relative to this wrapper file's location
# This assumes 'wrappers' and 'compiled_libs' are siblings under 'app'
wrapper_dir = os.path.dirname(__file__) # Directory of contact_wrapper_v1.py
project_root = os.path.dirname(wrapper_dir) # Go up one level to 'app' directory
lib_path_to_try = os.path.join(project_root, "compiled_libs", lib_filename)

# Also check for common Linux 'lib' prefix if the first attempt fails
if not os.path.exists(lib_path_to_try) and platform.system() == "Linux":
    lib_filename_with_prefix = f"lib{lib_filename_base}{lib_ext}"
    lib_path_to_try_with_prefix = os.path.join(project_root, "compiled_libs", lib_filename_with_prefix)
    if os.path.exists(lib_path_to_try_with_prefix):
        lib_path_to_try = lib_path_to_try_with_prefix # Use this path if found
    else: # If neither found, try direct name in compiled_libs (original attempt)
         pass # lib_path_to_try remains the original attempt

if not os.path.exists(lib_path_to_try): # If still not found, try locally as a fallback
    local_lib_path = os.path.join(wrapper_dir, lib_filename)
    if os.path.exists(local_lib_path):
        lib_path_to_try = local_lib_path


try:
    c_lib = ctypes.CDLL(lib_path_to_try)
except OSError as e:
    print(f"CRITICAL ERROR: Could not load V1 C library.")
    print(f"Attempted path: '{lib_path_to_try}' (using base filename: '{lib_filename}')")
    print(f"Details: {e}")
    print("\nPlease ensure that:")
    print(f"1. You have successfully compiled 'contact_v1_lib.c' into a shared library.")
    print(f"2. The compiled library is located at the path above or in your system's library path.")
    print("3. The library's architecture (32-bit/64-bit) matches your Python interpreter's.")
    raise

# ... (rest of contact_wrapper_v1.py as provided previously) ...
# # --- Define argtypes and restype for C functions from contact_v1_lib.h ---

# API int lib_v1_initialize(const char* data_file_path);
c_lib.lib_v1_initialize.argtypes = [ctypes.c_char_p]
c_lib.lib_v1_initialize.restype = ctypes.c_int

# API void lib_v1_cleanup();
c_lib.lib_v1_cleanup.argtypes = []
c_lib.lib_v1_cleanup.restype = None

# API char* lib_v1_add_contact(const char* name, const char* phone, const char* email);
c_lib.lib_v1_add_contact.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
c_lib.lib_v1_add_contact.restype = ctypes.POINTER(ctypes.c_char) # Using POINTER for easier handling if NULL

# API char* lib_v1_edit_contact(const char* old_email_id, const char* new_name, const char* new_phone, const char* new_email);
c_lib.lib_v1_edit_contact.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
c_lib.lib_v1_edit_contact.restype = ctypes.POINTER(ctypes.c_char)

# API ContactRecord* lib_v1_get_all_contacts(int* out_count);
c_lib.lib_v1_get_all_contacts.argtypes = [ctypes.POINTER(ctypes.c_int)]
c_lib.lib_v1_get_all_contacts.restype = ctypes.POINTER(ContactRecord)

# API ContactRecord* lib_v1_search_contacts(const char* query, int search_type, int* out_count);
c_lib.lib_v1_search_contacts.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.POINTER(ctypes.c_int)]
c_lib.lib_v1_search_contacts.restype = ctypes.POINTER(ContactRecord)

# API int lib_v1_delete_contact_by_email(const char* email);
c_lib.lib_v1_delete_contact_by_email.argtypes = [ctypes.c_char_p]
c_lib.lib_v1_delete_contact_by_email.restype = ctypes.c_int

# API int lib_v1_delete_all_contacts();
c_lib.lib_v1_delete_all_contacts.argtypes = []
c_lib.lib_v1_delete_all_contacts.restype = ctypes.c_int

# API int lib_v1_sort_contacts(int sort_type);
c_lib.lib_v1_sort_contacts.argtypes = [ctypes.c_int]
c_lib.lib_v1_sort_contacts.restype = ctypes.c_int

# API int lib_v1_save_contacts(const char* data_file_path);
c_lib.lib_v1_save_contacts.argtypes = [ctypes.c_char_p]
c_lib.lib_v1_save_contacts.restype = ctypes.c_int

# API int lib_v1_is_valid_name(const char* name);
c_lib.lib_v1_is_valid_name.argtypes = [ctypes.c_char_p]
c_lib.lib_v1_is_valid_name.restype = ctypes.c_int

# API int lib_v1_is_valid_number(const char* number);
c_lib.lib_v1_is_valid_number.argtypes = [ctypes.c_char_p]
c_lib.lib_v1_is_valid_number.restype = ctypes.c_int

# API int lib_v1_is_valid_email(const char* email);
c_lib.lib_v1_is_valid_email.argtypes = [ctypes.c_char_p]
c_lib.lib_v1_is_valid_email.restype = ctypes.c_int

# API void lib_v1_free_string(char* str_ptr);
c_lib.lib_v1_free_string.argtypes = [ctypes.POINTER(ctypes.c_char)] # Match POINTER(ctypes.c_char)
c_lib.lib_v1_free_string.restype = None

# API void lib_v1_free_contact_records(ContactRecord* records, int count);
c_lib.lib_v1_free_contact_records.argtypes = [ctypes.POINTER(ContactRecord), ctypes.c_int]
c_lib.lib_v1_free_contact_records.restype = None


# --- Pythonic wrapper functions ---
def _c_char_p_to_py_string(c_char_p_result):
    """Converts C string pointer to Python string and calls C to free it."""
    if not c_char_p_result:
        return None
    try:
        # Convert the POINTER(c_char) to a c_char_p to get the string value
        py_string = ctypes.cast(c_char_p_result, ctypes.c_char_p).value
        if py_string is not None:
            py_string = py_string.decode('utf-8', 'replace')
        else: # Should not happen if C returns valid pointer or NULL
            py_string = "Error: C function returned invalid string pointer."
    except Exception as e:
        py_string = f"Error decoding C string: {e}"
    finally:
        # Always try to free, C function should handle NULL if it allocated NULL (it shouldn't)
        c_lib.lib_v1_free_string(c_char_p_result)
    return py_string

def initialize(data_file_path="../data/contacts.csv"):
    # Pass NULL to C if data_file_path is None or empty, C lib should handle default
    c_path = data_file_path.encode('utf-8') if data_file_path else None
    return c_lib.lib_v1_initialize(c_path) == 0 # True for success

def cleanup():
    c_lib.lib_v1_cleanup()

def add_contact(name, phone, email):
    c_name = name.encode('utf-8')
    c_phone = phone.encode('utf-8')
    c_email = email.encode('utf-8')
    result_ptr = c_lib.lib_v1_add_contact(c_name, c_phone, c_email)
    return _c_char_p_to_py_string(result_ptr)

def edit_contact(old_email_id, new_name, new_phone, new_email):
    result_ptr = c_lib.lib_v1_edit_contact(
        old_email_id.encode('utf-8'),
        new_name.encode('utf-8'),
        new_phone.encode('utf-8'),
        new_email.encode('utf-8')
    )
    return _c_char_p_to_py_string(result_ptr)

def _c_records_to_py_list(c_records_ptr, count_val):
    if not c_records_ptr or count_val == 0:
        if c_records_ptr: # If ptr exists but count is 0, still needs free if C allocated.
            c_lib.lib_v1_free_contact_records(c_records_ptr, count_val)
        return []
    
    py_list = []
    try:
        for i in range(count_val):
            record_c = c_records_ptr[i]
            py_list.append({
                "name": record_c.name.decode('utf-8', 'replace'),
                "phone": record_c.phone.decode('utf-8', 'replace'),
                "email": record_c.email.decode('utf-8', 'replace')
            })
    finally:
        c_lib.lib_v1_free_contact_records(c_records_ptr, count_val) # Free the C array
    return py_list

def get_all_contacts():
    count = ctypes.c_int()
    c_records_ptr = c_lib.lib_v1_get_all_contacts(ctypes.byref(count))
    return _c_records_to_py_list(c_records_ptr, count.value)

def search_contacts(query, search_type): # search_type: 1=name, 2=phone, 3=email
    count = ctypes.c_int()
    c_records_ptr = c_lib.lib_v1_search_contacts(query.encode('utf-8'), ctypes.c_int(search_type), ctypes.byref(count))
    return _c_records_to_py_list(c_records_ptr, count.value)

def delete_contact_by_email(email):
    return c_lib.lib_v1_delete_contact_by_email(email.encode('utf-8')) == 0

def delete_all_contacts():
    return c_lib.lib_v1_delete_all_contacts() == 0

def sort_contacts(sort_type): # sort_type: 1=name, 2=phone, 3=email
    return c_lib.lib_v1_sort_contacts(ctypes.c_int(sort_type)) == 0

def save_contacts(data_file_path="../data/contacts.csv"):
    c_path = data_file_path.encode('utf-8') if data_file_path else None
    return c_lib.lib_v1_save_contacts(c_path) == 0

def is_valid_name(name):
    return c_lib.lib_v1_is_valid_name(name.encode('utf-8')) == 1

def is_valid_number(number):
    return c_lib.lib_v1_is_valid_number(number.encode('utf-8')) == 1

def is_valid_email(email):
    return c_lib.lib_v1_is_valid_email(email.encode('utf-8')) == 1
