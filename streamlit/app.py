import streamlit as st
import contact_manager_c # This is your compiled C module
import pandas as pd
import time # For timing operations
from faker import Faker # For generating dummy data
import csv # For writing dummy CSV
import random # For dummy phone numbers
import os # For process ID (used with psutil)
# psutil will be imported conditionally later to allow app to run if not installed initially

# 1. SET PAGE CONFIG MUST BE THE VERY FIRST STREAMLIT COMMAND
st.set_page_config(page_title="Donna Contact Manager & Perf Tester", layout="wide")

# --- CSS for Red "Delete All Contacts from Memory" Button ---
st.markdown("""
<style>
/* Container for the red button to help with layout and targeting */
.red-button-container { display: flex; width: 100%; }
.red-button-container > div[data-testid="stButton"] { width: 100%; }
.red-button-container div[data-testid="stButton"] > button {
    box-sizing: border-box !important;
    background-color: #D32F2F !important; /* Red */
    color: white !important;
    border: 1px solid #D32F2F !important;
    padding: 0.25rem 0.75rem !important; 
    border-radius: 0.25rem !important;
    line-height: 1.6 !important; font-weight: 400 !important; text-align: center !important;
    vertical-align: middle !important; cursor: pointer !important; user-select: none !important;
    transition: color .15s ease-in-out, background-color .15s ease-in-out, border-color .15s ease-in-out, box-shadow .15s ease-in-out !important;
}
.red-button-container div[data-testid="stButton"] > button:hover { background-color: #C62828 !important; color: white !important; border-color: #C62828 !important; }
.red-button-container div[data-testid="stButton"] > button:active { background-color: #B71C1C !important; color: white !important; border-color: #B71C1C !important; }
.red-button-container div[data-testid="stButton"] > button:focus { box-shadow: 0 0 0 0.2rem rgba(211, 47, 47, 0.5) !important; outline: none !important; }
.button-wrapper { display: flex; width: 100%; } /* For final action buttons alignment */
.button-wrapper > div[data-testid="stButton"] { width: 100%; }
</style>
""", unsafe_allow_html=True)

# --- Helper Functions ---
def display_contacts(contacts_list, context=""):
    if contacts_list:
        if context: st.caption(f"{context}: {len(contacts_list)} contact(s) found.")
        header = ["Name", "Phone", "Email"]
        df = pd.DataFrame([[c["name"], c["phone"], c["email"]] for c in contacts_list], columns=header)
        st.dataframe(df, use_container_width=True, hide_index=True)
    else: st.info(f"No contacts to display for {context if context else 'the current view'}.")

def show_error(e_message):
    st.toast(f"Error: {str(e_message)}", icon="❌")

def show_success(message):
    st.toast(message, icon="✅")

# --- Title and Caption ---
st.title("📖 Donna Contact Manager & Performance Monitor")
# The performance metrics will now be in the sidebar

# --- Initialization and Session State ---
if 'initialized' not in st.session_state:
    try:
        op_name = "Initialize (Load from CSV)"
        start_time = time.perf_counter()
        contact_manager_c.initialize()
        end_time = time.perf_counter()
        st.session_state.last_operation_details = {"name": op_name, "time": end_time - start_time}
        st.session_state.initialized = True
        st.session_state.contacts_dirty = True
    except Exception as e: 
        show_error(f"Initialization failed: {e}")
        st.session_state.last_operation_details = {"name": "Initialize (Load from CSV)", "time": "Error"}
        st.stop()

default_states = {
    "last_operation_details": st.session_state.get("last_operation_details", {"name": "App Start", "time": 0.0}), # Initialize if needed
    "show_delete_all_confirmation": False, "expander_search_results": None,
    "expander_last_search_query": "", "all_contacts": [],
    "contacts_dirty": st.session_state.get("contacts_dirty", True),
    "selected_email_for_manage_expander": None, 
    "show_edit_form_in_manage_expander": False,
    "show_confirm_delete_individual_dialog": False,
    "individual_contact_to_delete_email": None,
    "individual_contact_to_delete_name": None,
}
for key, default_value in default_states.items():
    if key not in st.session_state: st.session_state[key] = default_value

if st.session_state.contacts_dirty or (not st.session_state.all_contacts and st.session_state.initialized):
    op_name = "App Refresh (get_all_contacts)"
    start_time = time.perf_counter()
    st.session_state.all_contacts = contact_manager_c.get_all_contacts()
    end_time = time.perf_counter()
    if st.session_state.contacts_dirty : # Only update time if it was a truly dirty read (full reload)
         st.session_state.last_operation_details = {"name": op_name, "time": end_time - start_time}
    st.session_state.contacts_dirty = False

# --- Sidebar ---
with st.sidebar:
    st.header("🛠️ Utilities & Metrics")
    with st.expander("Generate Dummy Contacts", expanded=False):
        st.subheader("Create Dummy CSV")
        num_to_generate = st.number_input("Number of contacts:", min_value=100, max_value=1000000, value=10000, step=1000, key="num_dummy")
        dummy_csv_filename = st.text_input("Output Filename:", value="dummy_contacts.csv", key="dummy_fname")

        if st.button("Generate Dummy CSV File", key="gen_dummy_csv"):
            fake = Faker()
            op_name = f"Generate {num_to_generate:,} Dummy Contacts" # Formatted number
            start_time_gen = time.perf_counter() # Use a different start time variable
            try:
                with st.spinner(f"Generating {num_to_generate:,} contacts to '{dummy_csv_filename}'... this may take a moment."):
                    with open(dummy_csv_filename, 'w', newline='', encoding='utf-8') as csvfile:
                        fieldnames = ["name", "phone", "email"]
                        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
                        writer.writeheader()
                        for i in range(num_to_generate):
                            name = fake.name()
                            phone = "".join([str(random.randint(0, 9)) for _ in range(10)])
                            try:
                                first_name, last_name = name.split(" ", 1)
                                email_name_part = f"{first_name.lower().replace('.', '')}.{last_name.lower().replace(' ', '')}"
                            except ValueError: 
                                email_name_part = name.lower().replace(' ', '').replace('.', '')
                            email = f"{email_name_part}{random.randint(1,200)}@{fake.free_email_domain()}"
                            writer.writerow({"name": name, "phone": phone, "email": email})
                end_time_gen = time.perf_counter() # Use a different end time variable
                st.session_state.last_operation_details = {"name": op_name, "time": end_time_gen - start_time_gen}
                show_success(f"Generated {num_to_generate:,} contacts into {dummy_csv_filename}")
                st.info(f"To use this data, rename '{dummy_csv_filename}' to 'contacts.csv' and restart the application.")
            except Exception as e:
                st.session_state.last_operation_details = {"name": op_name, "time": "Error"}
                show_error(f"Failed to generate dummy CSV: {e}")

    st.markdown("---")
    st.subheader("📊 Performance Metrics")

    # Display Total Contacts
    if 'all_contacts' in st.session_state:
        st.metric(label="Contacts Loaded", value=f"{len(st.session_state.all_contacts):,}")

    # Display Last Operation Time
    if "last_operation_details" in st.session_state:
        details = st.session_state.last_operation_details
        op_name_display = details['name']
        
        if isinstance(details['time'], float):
            time_val_ms = details['time'] * 1000
            if details['time'] == 0: # Handle exactly zero time if it occurs
                 op_time_str = "0.000 ms (effectively instantaneous)"
            elif time_val_ms < 0.01: # If less than 0.01 ms (10 µs), show in µs with precision
                time_val_us = details['time'] * 1000000
                op_time_str = f"{time_val_us:.3f} µs"
            elif time_val_ms < 1: # If less than 1 ms, show in µs or more decimal places for ms
                time_val_us = details['time'] * 1000000 # Using µs for very small ms values
                op_time_str = f"{time_val_us:.0f} µs" # Or f"{time_val_ms:.4f} ms"
            elif time_val_ms < 1000: # If less than 1 second, show in ms
                op_time_str = f"{time_val_ms:.3f} ms"
            else: # Otherwise show in seconds
                op_time_str = f"{details['time']:.3f} s"
        else:
            op_time_str = str(details['time']) # e.g., "Error"

        st.markdown(f"**Last Operation:**")
        st.caption(f"`{op_name_display}`")
        st.markdown(f"**Time Taken:**")
        st.caption(f"`{op_time_str}`")

    # Display Current Memory Usage
    try:
        import psutil # Import here to make it an optional dependency
        process = psutil.Process(os.getpid())
        memory_info = process.memory_info()
        rss_mb = memory_info.rss / (1024 * 1024)
        st.metric(label="App Memory (RSS)", value=f"{rss_mb:.2f} MB")
    except ImportError:
        st.warning("`psutil` not installed. Memory usage unavailable. (Run: `pip install psutil`)")
    except Exception as e:
        show_error(f"Could not get memory usage: {e}")


# --- Main Content Area ---
st.header("Manage Your Contacts") # This header is for the main page actions

# Add New Contact Expander
# ... (Code for Add, Search, Manage Individual, View Full List expanders remains the same as your last correct version) ...
# ... Make sure to integrate the timing logic (start_time, end_time, update st.session_state.last_operation_details)
# ... around each call to contact_manager_c functions as shown in the examples above.

# --- Example of timing for Add Contact (Repeat this pattern for other C-module calls) ---
with st.expander("➕ Add New Contact", expanded=False):
    with st.form("add_contact_form", clear_on_submit=True):
        add_name = st.text_input("Name", key="add_name_f")
        add_phone = st.text_input("Phone (10 digits)", key="add_phone_f", max_chars=10)
        add_email = st.text_input("Email", key="add_email_f")
        add_submitted = st.form_submit_button("Add Contact")
        if add_submitted:
            if not all([add_name, add_phone, add_email]): st.warning("All fields are required.")
            elif not contact_manager_c.is_valid_name(add_name): st.warning("Invalid name format.")
            elif not contact_manager_c.is_valid_number(add_phone): st.warning("Invalid phone number.")
            elif not contact_manager_c.is_valid_email(add_email): st.warning("Invalid email format.")
            else:
                op_name = "Add Contact"
                try: 
                    start_time = time.perf_counter()
                    response = contact_manager_c.add_contact(add_name, add_phone, add_email)
                    end_time = time.perf_counter()
                    st.session_state.last_operation_details = {"name": op_name, "time": end_time - start_time}
                    show_success(response) 
                    st.session_state.contacts_dirty = True
                except Exception as e: 
                    st.session_state.last_operation_details = {"name": op_name, "time": "Error"}
                    show_error(str(e))

# Search Contacts Expander
search_expander_open = st.session_state.expander_search_results is not None
with st.expander("🔍 Search Contacts", expanded=search_expander_open):
    search_query_input = st.text_input("Search Query",value=st.session_state.expander_last_search_query, key="search_query_exp")
    search_type_option_input = st.selectbox("Search By", ("Name", "Phone", "Email"), key="search_type_exp")
    if st.button("Search", key="search_btn_exp"): 
        if search_query_input:
            op_name = f"Search by {search_type_option_input} for '{search_query_input}'"
            try:
                start_time = time.perf_counter()
                results = contact_manager_c.search_contacts(search_query_input, {"Name": 1, "Phone": 2, "Email": 3}[search_type_option_input])
                end_time = time.perf_counter()
                st.session_state.last_operation_details = {"name": op_name, "time": end_time - start_time}
                st.session_state.expander_search_results = results if results is not None else []
                st.session_state.expander_last_search_query = search_query_input
            except Exception as e:
                st.session_state.last_operation_details = {"name": op_name, "time": "Error"}
                show_error(str(e))
            st.rerun() 
        else: st.warning("Please enter a search query.")
    if st.session_state.expander_search_results is not None:
        st.markdown("---"); display_contacts(st.session_state.expander_search_results, f"Search results")


        if st.button("Clear Search Results", key="clear_search_btn_exp"):
            st.session_state.expander_search_results = None; st.session_state.expander_last_search_query = ""
            st.session_state.last_operation_details = {"name": "Clear Search", "time": 0.0} # Negligible time
            st.rerun()

# Manage Individual Contacts Expander
manage_expander_open = st.session_state.selected_email_for_manage_expander is not None or \
                       st.session_state.show_edit_form_in_manage_expander or \
                       st.session_state.show_confirm_delete_individual_dialog
with st.expander("⚙️ Manage Individual Contacts", expanded=manage_expander_open):
    # ... (Previous logic for selectbox and buttons)
    if not st.session_state.all_contacts:
        st.info("No contacts available to manage. Add some contacts first.")
        if st.button("Refresh Contact List", key="refresh_manage_exp_btn"): # Added btn to key
            st.session_state.contacts_dirty = True; st.rerun()
    else:
        contact_options_manage = ["-- Select a contact --"] + [f"{c['name']} ({c['email']})" for c in st.session_state.all_contacts]
        selected_display_name_for_manage = None
        if st.session_state.selected_email_for_manage_expander:
            try:
                contact_obj = next(c for c in st.session_state.all_contacts if c["email"] == st.session_state.selected_email_for_manage_expander)
                selected_display_name_for_manage = f"{contact_obj['name']} ({contact_obj['email']})"
            except StopIteration: st.session_state.selected_email_for_manage_expander = None 
        current_selection_index = 0
        if selected_display_name_for_manage and selected_display_name_for_manage in contact_options_manage:
            current_selection_index = contact_options_manage.index(selected_display_name_for_manage)
        
        selected_contact_display = st.selectbox(
            "Select a contact to manage:", contact_options_manage, index=current_selection_index,
            key="sb_manage_individual_contact_expander",
            disabled=st.session_state.show_confirm_delete_individual_dialog 
        )

        if not st.session_state.show_confirm_delete_individual_dialog and selected_contact_display != "-- Select a contact --":
            newly_selected_email = selected_contact_display.split('(')[-1].split(')')[0]
            if st.session_state.selected_email_for_manage_expander != newly_selected_email:
                st.session_state.selected_email_for_manage_expander = newly_selected_email
                st.session_state.show_edit_form_in_manage_expander = False 
                st.rerun()

            if st.session_state.selected_email_for_manage_expander:
                contact_name = selected_contact_display.split(' (')[0]
                email_to_act_on = st.session_state.selected_email_for_manage_expander
                manage_cols = st.columns(2)
                with manage_cols[0]: 
                    if st.button(f"✏️ Edit {contact_name}", key=f"edit_manage_exp_{email_to_act_on}", use_container_width=True, disabled=st.session_state.show_confirm_delete_individual_dialog):
                        st.session_state.show_edit_form_in_manage_expander = True; st.rerun()
                with manage_cols[1]: 
                    if st.button(f"🗑️ Delete {contact_name}", key=f"delete_trigger_manage_exp_{email_to_act_on}", type="secondary", use_container_width=True, disabled=st.session_state.show_confirm_delete_individual_dialog):
                        st.session_state.individual_contact_to_delete_email = email_to_act_on
                        st.session_state.individual_contact_to_delete_name = contact_name
                        st.session_state.show_confirm_delete_individual_dialog = True; st.rerun() 
        elif not st.session_state.show_confirm_delete_individual_dialog and selected_contact_display == "-- Select a contact --" and st.session_state.selected_email_for_manage_expander is not None:
             st.session_state.selected_email_for_manage_expander = None; st.session_state.show_edit_form_in_manage_expander = False; st.rerun()

    if st.session_state.get('show_confirm_delete_individual_dialog') and st.session_state.get('individual_contact_to_delete_email'):
        st.markdown("---")
        st.warning(f"**Confirm Deletion**\n\nAre you sure you want to delete: **{st.session_state.individual_contact_to_delete_name}** ({st.session_state.individual_contact_to_delete_email})?")
        confirm_cols = st.columns(2)
        with confirm_cols[0]:
            if st.button("✔️ Yes, Delete This Contact", key="confirm_delete_individual_yes", use_container_width=True, type="primary"):
                email_to_delete = st.session_state.individual_contact_to_delete_email; name_deleted = st.session_state.individual_contact_to_delete_name
                op_name = "Delete Individual Contact"
                try:
                    start_time = time.perf_counter()
                    deleted = contact_manager_c.delete_contact_by_email(email_to_delete)
                    end_time = time.perf_counter()
                    st.session_state.last_operation_details = {"name": op_name, "time": end_time - start_time}
                    if deleted:
                        show_success(f"Contact '{name_deleted}' deleted."); st.session_state.contacts_dirty = True
                        st.session_state.selected_email_for_manage_expander = None; st.session_state.show_edit_form_in_manage_expander = False
                    else: show_error(f"Failed to delete '{name_deleted}'.")
                except Exception as e: 
                    st.session_state.last_operation_details = {"name": op_name, "time": "Error"}
                    show_error(str(e))
                finally:
                    st.session_state.show_confirm_delete_individual_dialog = False; st.session_state.individual_contact_to_delete_email = None; st.session_state.individual_contact_to_delete_name = None
        with confirm_cols[1]:
            if st.button("❌ No, Cancel", key="confirm_delete_individual_no", use_container_width=True, type="secondary"):
                st.session_state.show_confirm_delete_individual_dialog = False; st.session_state.individual_contact_to_delete_email = None; st.session_state.individual_contact_to_delete_name = None
    
    if st.session_state.show_edit_form_in_manage_expander and st.session_state.selected_email_for_manage_expander and not st.session_state.show_confirm_delete_individual_dialog:
        email_being_edited = st.session_state.selected_email_for_manage_expander
        contact_to_edit_details = next((c for c in st.session_state.all_contacts if c["email"] == email_being_edited), None)
        if contact_to_edit_details:
            st.markdown("---")
            with st.form(key=f"manage_exp_edit_form_{email_being_edited}"):
                st.subheader(f"Editing: {contact_to_edit_details['name']}")
                edit_name = st.text_input("New Name", value=contact_to_edit_details['name'], key=f"mexp_edit_name_{email_being_edited}")
                edit_phone = st.text_input("New Phone", value=contact_to_edit_details['phone'], key=f"mexp_edit_phone_{email_being_edited}", max_chars=10)
                edit_email_new = st.text_input("New Email", value=contact_to_edit_details['email'], key=f"mexp_edit_email_{email_being_edited}")
                st.caption(f"Original Email ID: {email_being_edited}")
                form_cols = st.columns(2)
                with form_cols[0]:
                    if st.form_submit_button("💾 Save Changes", type="primary", use_container_width=True):
                        if not all([edit_name, edit_phone, edit_email_new]): st.warning("All fields required.")
                        elif not contact_manager_c.is_valid_name(edit_name): st.warning("Invalid name.")
                        elif not contact_manager_c.is_valid_number(edit_phone): st.warning("Invalid phone.")
                        elif not contact_manager_c.is_valid_email(edit_email_new): st.warning("Invalid email.")
                        else:
                            op_name = "Edit Contact"
                            try:
                                start_time = time.perf_counter()
                                response = contact_manager_c.edit_contact(email_being_edited, edit_name, edit_phone, edit_email_new)
                                end_time = time.perf_counter()
                                st.session_state.last_operation_details = {"name": op_name, "time": end_time - start_time}
                                show_success(response) 
                                st.session_state.contacts_dirty = True; st.session_state.show_edit_form_in_manage_expander = False
                                st.session_state.selected_email_for_manage_expander = edit_email_new 
                            except Exception as e: 
                                st.session_state.last_operation_details = {"name": op_name, "time": "Error"}
                                show_error(str(e))
                with form_cols[1]:
                    if st.form_submit_button("❌ Cancel Edit", use_container_width=True, type="secondary"):
                        st.session_state.show_edit_form_in_manage_expander = False
        else: st.warning("Contact to edit not found. Please re-select."); st.session_state.show_edit_form_in_manage_expander = False

# View Full Contact List Expander
with st.expander("📖 View Full Contact List", expanded=False):
    # ... (This part remains largely the same, but ensure sort functions are timed) ...
    st.header("Full Contact List")
    sort_cols = st.columns(3)
    sort_options = {"Name": contact_manager_c.sort_contacts_by_name, 
                    "Phone": contact_manager_c.sort_contacts_by_phone, 
                    "Email": contact_manager_c.sort_contacts_by_email}
    for i, (sort_label, sort_func_ptr) in enumerate(sort_options.items()): # Renamed sort_func to sort_func_ptr
        with sort_cols[i]:
            if st.button(f"Sort by {sort_label}", use_container_width=True, key=f"sort_{sort_label.lower()}_table_btn"): # Added btn to key
                op_name = f"Sort by {sort_label}"
                try:
                    start_time = time.perf_counter()
                    sort_func_ptr() # Call the function pointer
                    end_time = time.perf_counter()
                    st.session_state.last_operation_details = {"name": op_name, "time": end_time - start_time}
                    st.session_state.contacts_dirty = True; st.rerun()
                except Exception as e:
                    st.session_state.last_operation_details = {"name": op_name, "time": "Error"}
                    show_error(str(e))

    if st.session_state.contacts_dirty or (not st.session_state.all_contacts and st.session_state.initialized):
        op_name = "View List (get_all_contacts)" # More specific name
        start_time = time.perf_counter()
        st.session_state.all_contacts = contact_manager_c.get_all_contacts()
        end_time = time.perf_counter()
        # Only update if it's a significant load, not for every minor refresh unless specifically desired
        if st.session_state.contacts_dirty:
            st.session_state.last_operation_details = {"name": op_name, "time": end_time - start_time}
        st.session_state.contacts_dirty = False
    display_contacts(st.session_state.all_contacts, "Currently Stored Contacts")


# Final Actions (Save All / Delete All)
st.markdown("---"); st.subheader("Final Data Operations") 
# ... (Final buttons and delete all confirmation dialog - same as before with timing logic) ...
final_col1, final_col2 = st.columns(2)
with final_col1:
    st.markdown('<div class="button-wrapper">', unsafe_allow_html=True) 
    if st.button("💾 Save All to CSV", use_container_width=True, type="secondary", key="save_all_final_btn"): 
        op_name = "Save All to CSV"
        try: 
            start_time = time.perf_counter()
            contact_manager_c.save_contacts()
            end_time = time.perf_counter()
            st.session_state.last_operation_details = {"name": op_name, "time": end_time - start_time}
            show_success("Contacts saved to contacts.csv!")
        except Exception as e: 
            st.session_state.last_operation_details = {"name": op_name, "time": "Error"}
            show_error(str(e)) 
    st.markdown('</div>', unsafe_allow_html=True)
with final_col2: 
    st.markdown('<div class="button-wrapper red-button-container">', unsafe_allow_html=True) 
    if st.button("🗑️ Delete All Contacts from Memory", use_container_width=True, key="delete_all_final_btn", type="secondary"):
        st.session_state.show_delete_all_confirmation = True; st.rerun()
    st.markdown('</div>', unsafe_allow_html=True)

if st.session_state.show_delete_all_confirmation: 
    st.markdown("<div style='border: 2px solid #FF4B4B; padding: 15px; border-radius: 5px; margin-top:10px; background-color: #fff0f0;'>", unsafe_allow_html=True)
    st.subheader("🚨 Confirm: Delete ALL Contacts from Memory 🚨")
    st.warning("IRREVERSIBLE action. Unsaved data will be lost."); st.caption("To clear the CSV file, save after this operation.")
    confirm_cb = st.checkbox("Yes, I understand and want to delete ALL contacts from memory.", key="confirm_del_all_cb_final")
    del_confirm_col1, del_confirm_col2 = st.columns(2)
    with del_confirm_col1: 
        if st.button("Proceed and Delete All", disabled=not confirm_cb, type="primary", use_container_width=True, key="proceed_del_all_btn_final"): 
            if confirm_cb:
                op_name = "Delete All Contacts"
                try: 
                    start_time = time.perf_counter()
                    contact_manager_c.delete_all_contacts()
                    end_time = time.perf_counter()
                    st.session_state.last_operation_details = {"name": op_name, "time": end_time - start_time}
                    show_success("All contacts deleted from memory.") 
                    st.session_state.contacts_dirty = True; st.session_state.show_delete_all_confirmation = False; st.session_state.selected_email_for_manage_expander = None; 
                except Exception as e: 
                    st.session_state.last_operation_details = {"name": op_name, "time": "Error"}
                    show_error(str(e)) 
    with del_confirm_col2:
        if st.button("Cancel Deletion", use_container_width=True, type="secondary", key="cancel_del_all_btn_final"):
            st.session_state.show_delete_all_confirmation = False
    st.markdown("</div>", unsafe_allow_html=True)