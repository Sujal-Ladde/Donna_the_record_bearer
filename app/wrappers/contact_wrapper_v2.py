# contact_wrapper_v2.py
import ctypes
import os
import platform

# Define the ContactRecord structure for ctypes, matching your C struct
# This MUST be identical to the one used in contact_wrapper_v1.py
class ContactRecord(ctypes.Structure):
    _fields_ = [("name", ctypes.c_char * 50),
                ("phone", ctypes.c_char * 50),
                ("email", ctypes.c_char * 50)]

# Determine library extension and attempt to load the C library
lib_filename_base = "contact_v2_lib" # Changed for Version 2
lib_ext = ""
if platform.system() == "Windows":
    lib_ext = ".dll"
elif platform.system() == "Linux":
    lib_ext = ".so" 
elif platform.system() == "Darwin": # macOS
    lib_ext = ".dylib"
else:
    raise OSError("Unsupported OS for loading shared C library")

lib_filename = f"{lib_filename_base}{lib_ext}"

# Path to the compiled_libs directory relative to this wrapper file's location
wrapper_dir = os.path.dirname(__file__) 
project_root = os.path.dirname(wrapper_dir) if wrapper_dir else os.getcwd() # Handle script in current dir
lib_path_to_try = os.path.join(project_root, "compiled_libs", lib_filename)

# Also check for common Linux 'lib' prefix if the first attempt fails
if not os.path.exists(lib_path_to_try) and platform.system() == "Linux":
    lib_filename_with_prefix = f"lib{lib_filename_base}{lib_ext}"
    lib_path_to_try_with_prefix = os.path.join(project_root, "compiled_libs", lib_filename_with_prefix)
    if os.path.exists(lib_path_to_try_with_prefix):
        lib_path_to_try = lib_path_to_try_with_prefix
    # If neither found, and script dir is empty (ran from same dir), try local as fallback
    elif not os.path.exists(lib_path_to_try) and not wrapper_dir:
        lib_path_to_try = lib_filename 

if not os.path.exists(lib_path_to_try) and not wrapper_dir: # Final fallback for current dir if others fail
     lib_path_to_try = lib_filename


try:
    c_lib = ctypes.CDLL(lib_path_to_try)
except OSError as e:
    print(f"CRITICAL ERROR: Could not load V2 C library.")
    print(f"Attempted path: '{lib_path_to_try}' (using base filename: '{lib_filename}')")
    print(f"Details: {e}")
    print("\nPlease ensure that:")
    print(f"1. You have successfully compiled 'contact_v2_lib.c' into a shared library named '{lib_filename}'.")
    print(f"2. The compiled library ('{lib_filename}') is located in the 'compiled_libs/' directory relative to your project root, or in your system's library path.")
    print("3. The compiled library's architecture (32-bit/64-bit) matches your Python interpreter's architecture.")
    raise

# --- Define argtypes and restype for C functions from contact_v2_lib.h ---
# These must match the function signatures in your contact_v2_lib.h, 
# including the lib_v2_ prefix.

# API int lib_v2_initialize(const char* data_file_path);
c_lib.lib_v2_initialize.argtypes = [ctypes.c_char_p]
c_lib.lib_v2_initialize.restype = ctypes.c_int

# API void lib_v2_cleanup();
c_lib.lib_v2_cleanup.argtypes = []
c_lib.lib_v2_cleanup.restype = None

# API char* lib_v2_add_contact(const char* name, const char* phone, const char* email);
c_lib.lib_v2_add_contact.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
c_lib.lib_v2_add_contact.restype = ctypes.POINTER(ctypes.c_char) 

# API char* lib_v2_edit_contact(const char* old_email_id, const char* new_name, const char* new_phone, const char* new_email);
c_lib.lib_v2_edit_contact.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
c_lib.lib_v2_edit_contact.restype = ctypes.POINTER(ctypes.c_char)

# API ContactRecord* lib_v2_get_all_contacts(int* out_count);
c_lib.lib_v2_get_all_contacts.argtypes = [ctypes.POINTER(ctypes.c_int)]
c_lib.lib_v2_get_all_contacts.restype = ctypes.POINTER(ContactRecord)

# API ContactRecord* lib_v2_search_contacts(const char* query, int search_type, int* out_count);
c_lib.lib_v2_search_contacts.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.POINTER(ctypes.c_int)]
c_lib.lib_v2_search_contacts.restype = ctypes.POINTER(ContactRecord)

# API int lib_v2_delete_contact_by_email(const char* email);
c_lib.lib_v2_delete_contact_by_email.argtypes = [ctypes.c_char_p]
c_lib.lib_v2_delete_contact_by_email.restype = ctypes.c_int

# API int lib_v2_delete_all_contacts();
c_lib.lib_v2_delete_all_contacts.argtypes = []
c_lib.lib_v2_delete_all_contacts.restype = ctypes.c_int

# API int lib_v2_sort_contacts(int sort_type);
c_lib.lib_v2_sort_contacts.argtypes = [ctypes.c_int]
c_lib.lib_v2_sort_contacts.restype = ctypes.c_int

# API int lib_v2_save_contacts(const char* data_file_path);
c_lib.lib_v2_save_contacts.argtypes = [ctypes.c_char_p]
c_lib.lib_v2_save_contacts.restype = ctypes.c_int

# API int lib_v2_is_valid_name(const char* name);
c_lib.lib_v2_is_valid_name.argtypes = [ctypes.c_char_p]
c_lib.lib_v2_is_valid_name.restype = ctypes.c_int

# API int lib_v2_is_valid_number(const char* number);
c_lib.lib_v2_is_valid_number.argtypes = [ctypes.c_char_p]
c_lib.lib_v2_is_valid_number.restype = ctypes.c_int

# API int lib_v2_is_valid_email(const char* email);
c_lib.lib_v2_is_valid_email.argtypes = [ctypes.c_char_p]
c_lib.lib_v2_is_valid_email.restype = ctypes.c_int

# API void lib_v2_free_string(char* str_ptr);
c_lib.lib_v2_free_string.argtypes = [ctypes.POINTER(ctypes.c_char)] 
c_lib.lib_v2_free_string.restype = None

# API void lib_v2_free_contact_records(ContactRecord* records, int count);
c_lib.lib_v2_free_contact_records.argtypes = [ctypes.POINTER(ContactRecord), ctypes.c_int]
c_lib.lib_v2_free_contact_records.restype = None


# --- Pythonic wrapper functions (identical names to contact_wrapper_v1.py) ---
def _c_char_p_to_py_string_and_free(c_char_p_result):
    if not c_char_p_result: return None
    py_string = ""
    try:
        raw_val = ctypes.cast(c_char_p_result, ctypes.c_char_p).value
        if raw_val: py_string = raw_val.decode('utf-8', 'replace')
    except Exception as e: py_string = f"Error decoding C string: {e}"
    finally: c_lib.lib_v2_free_string(c_char_p_result) # Calls lib_v2_
    return py_string

def initialize(data_file_path="../data/contacts.csv"): # Default CSV can be version specific
    c_path = data_file_path.encode('utf-8') if data_file_path else None
    return c_lib.lib_v2_initialize(c_path) == 0

def cleanup():
    c_lib.lib_v2_cleanup()

def add_contact(name, phone, email):
    result_ptr = c_lib.lib_v2_add_contact(name.encode('utf-8'), phone.encode('utf-8'), email.encode('utf-8'))
    return _c_char_p_to_py_string_and_free(result_ptr)

def edit_contact(old_email_id, new_name, new_phone, new_email):
    result_ptr = c_lib.lib_v2_edit_contact(
        old_email_id.encode('utf-8'), new_name.encode('utf-8'),
        new_phone.encode('utf-8'), new_email.encode('utf-8')
    )
    return _c_char_p_to_py_string_and_free(result_ptr)

def _c_records_to_py_list_and_free(c_records_ptr, count_val):
    if not c_records_ptr or count_val == 0:
        if c_records_ptr: c_lib.lib_v2_free_contact_records(c_records_ptr, count_val) # Calls lib_v2_
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
    finally: c_lib.lib_v2_free_contact_records(c_records_ptr, count_val) # Calls lib_v2_
    return py_list

def get_all_contacts():
    count = ctypes.c_int()
    c_records_ptr = c_lib.lib_v2_get_all_contacts(ctypes.byref(count))
    return _c_records_to_py_list_and_free(c_records_ptr, count.value)

def search_contacts(query, search_type): 
    count = ctypes.c_int()
    c_records_ptr = c_lib.lib_v2_search_contacts(query.encode('utf-8'), ctypes.c_int(search_type), ctypes.byref(count))
    return _c_records_to_py_list_and_free(c_records_ptr, count.value)

def delete_contact_by_email(email):
    return c_lib.lib_v2_delete_contact_by_email(email.encode('utf-8')) == 0

def delete_all_contacts():
    return c_lib.lib_v2_delete_all_contacts() == 0

def sort_contacts(sort_type): 
    return c_lib.lib_v2_sort_contacts(ctypes.c_int(sort_type)) == 0

def save_contacts(data_file_path="../data/contacts.csv"): # Default CSV can be version specific
    c_path = data_file_path.encode('utf-8') if data_file_path else None
    return c_lib.lib_v2_save_contacts(c_path) == 0

def is_valid_name(name):
    return c_lib.lib_v2_is_valid_name(name.encode('utf-8')) == 1

def is_valid_number(number):
    return c_lib.lib_v2_is_valid_number(number.encode('utf-8')) == 1

def is_valid_email(email):
    return c_lib.lib_v2_is_valid_email(email.encode('utf-8')) == 1

