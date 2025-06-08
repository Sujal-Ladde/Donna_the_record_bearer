# streamlit_app/app.py
import streamlit as st
import pandas as pd
import time
import os
import sys

# Get the absolute path of the directory containing app.py (streamlit_app)
streamlit_app_dir = os.path.abspath(os.path.dirname(__file__))
# Get the absolute path of the project root (app) by going one level up
project_root = os.path.abspath(os.path.join(streamlit_app_dir, os.pardir))

# Add the project root to sys.path if it's not already there
# Inserting at the beginning (index 0) gives it higher precedence
if project_root not in sys.path:
    sys.path.insert(0, project_root)
# --- Backend wrappers import -------------------------------------
try:
    import wrappers.contact_wrapper_v1 as cm_v1
    initialization_error_v1 = None
except Exception as e:
    cm_v1 = None
    initialization_error_v1 = e

try:
    import wrappers.contact_wrapper_v2 as cm_v2
    initialization_error_v2 = None
except Exception as e:
    cm_v2 = None
    initialization_error_v2 = e
# -----------------------------------------------------------------

# 1. SET PAGE CONFIG MUST BE THE VERY FIRST STREAMLIT COMMAND
st.set_page_config(page_title="Contact Manager Perf. Tester", layout="wide", initial_sidebar_state="expanded")

# --- Custom CSS to adjust spacing and metric font-size ---
# --- Custom CSS to adjust spacing and metric font-size ---
st.markdown("""
<style>
  /* target every inline code span in the main container */
  .block-container code {
    font-size: 2rem !important;   /* try 1.5rem, 2rem, etc. to taste */
    line-height: 2rem !important;
  }
</style>
""", unsafe_allow_html=True)



# --- CSS for Red "Delete All Contacts from Memory" Button ---
st.markdown("""
<style>
.red-button-container div[data-testid="stButton"] > button {
    box-sizing: border-box !important; background-color: #D32F2F !important; color: white !important;
    border: 1px solid #D32F2F !important; padding: 0.25rem 0.75rem !important; 
    border-radius: 0.25rem !important; line-height: 1.6 !important; font-weight: 400 !important;
    text-align: center !important; vertical-align: middle !important; cursor: pointer !important;
    user-select: none !important;
    transition: color .15s ease-in-out, background-color .15s ease-in-out, border-color .15s ease-in-out, box-shadow .15s ease-in-out !important;
}
.red-button-container div[data-testid="stButton"] > button:hover {
    background-color: #C62828 !important; color: white !important; border-color: #C62828 !important;
}
.red-button-container div[data-testid="stButton"] > button:active {
    background-color: #B71C1C !important; color: white !important; border-color: #B71C1C !important;
}
.red-button-container div[data-testid="stButton"] > button:focus {
    background-color: rgba(211, 47, 47, 0.5) !important; outline: none !important;
}
.button-wrapper { display: flex; width: 100%; }
.button-wrapper > div[data-testid="stButton"] { width: 100%; }
</style>
""", unsafe_allow_html=True)

# --- Helper Functions ---
def display_contacts(contacts_list, context=""):
    if contacts_list:
        if context:
            st.caption(f"{context}: {len(contacts_list)} contact(s) found.")
        header = ["Name", "Phone", "Email"]
        data_for_df = []
        if isinstance(contacts_list[0], dict):
            data_for_df = [[c.get("name","N/A"), c.get("phone","N/A"), c.get("email","N/A")] for c in contacts_list]
        else:
            data_for_df = contacts_list
        
        if data_for_df:
            df = pd.DataFrame(data_for_df, columns=header)
            st.dataframe(df, use_container_width=True, hide_index=True)
        elif contacts_list:
            st.write("Contacts data is present but not in expected list-of-dicts format:", contacts_list)
    else:
        st.info(f"No contacts to display for {context if context else 'the current view'}.")

def show_error(e_message):
    st.toast(f"Error: {str(e_message)}", icon="❌")

def show_success(message):
    st.toast(message, icon="✅")


# --- Title and Caption ---
st.title("📖 Contact Manager - Performance Tester")


# --- Session State Initialization ---
default_app_states = {
    "last_operation_details": {"name": "App Loaded", "time": "N/A"},
    "active_backend_module": None,
    "active_backend_name": None,
    "backend_initialized": False,
    "contacts_dirty_app": True,
    "all_contacts_app": [],
    "show_delete_all_confirmation": False,
    "expander_search_results": None,
    "expander_last_search_query": "",
    "selected_email_for_manage_expander": None,
    "show_edit_form_in_manage_expander": False,
    "show_confirm_delete_individual_dialog": False,
    "individual_contact_to_delete_email": None,
    "individual_contact_to_delete_name": None,
}

for key, default_value in default_app_states.items():
    if key not in st.session_state:
        st.session_state[key] = default_value


# --- Sidebar ---
with st.sidebar:
    st.header("⚙️ Configuration & Metrics")

    backend_map = {}
    if "cm_v1" in globals() and cm_v1:
        backend_map["Version 1 (Array + BubbleSort)"] = cm_v1
    if "cm_v2" in globals() and cm_v2:
        backend_map["Version 2 (LinkedList + MergeSort)"] = cm_v2

    if not backend_map:
        st.error("CRITICAL: No backend C modules (V1 or V2) could be imported. Application cannot run.")
        if initialization_error_v1:
            st.caption(f"V1 Import Error Hint: {initialization_error_v1}")
        if initialization_error_v2:
            st.caption(f"V2 Import Error Hint: {initialization_error_v2}")
        st.stop()

    backend_keys = list(backend_map.keys())
    current_backend_name_in_state = st.session_state.get("active_backend_name")
    default_radio_index = 0
    if current_backend_name_in_state and current_backend_name_in_state in backend_keys:
        default_radio_index = backend_keys.index(current_backend_name_in_state)

    st.subheader("Select Backend Implementation")
    chosen_version_name = st.radio(
        "",
        backend_keys,
        key="backend_choice_selector_radio",
        index=default_radio_index
    )

    if (st.session_state.active_backend_name != chosen_version_name
        or not st.session_state.backend_initialized):
        st.session_state.active_backend_module = backend_map[chosen_version_name]
        st.session_state.active_backend_name = chosen_version_name
        active_module_for_init = st.session_state.active_backend_module

        # Initialize the selected backend
        op_name = f"Initialize ({st.session_state.active_backend_name})"
        try:
            start_time = time.perf_counter()
            csv_path_for_init = os.path.join(project_root, "data", "contacts.csv")

            if not os.path.exists(csv_path_for_init):
                os.makedirs(os.path.join(project_root, "data"), exist_ok=True)
                with open(csv_path_for_init, 'w') as f:
                    pass
                st.caption(f"Note: Created empty '{csv_path_for_init}'. Generate dummy data for testing.")

            init_success = active_module_for_init.initialize(csv_path_for_init)
            end_time = time.perf_counter()

            if init_success:
                st.session_state.last_operation_details = {"name": op_name, "time": end_time - start_time}
                show_success(f"{st.session_state.active_backend_name} initialized.")
                st.session_state.backend_initialized = True
                st.session_state.contacts_dirty_app = True
            else:
                st.session_state.last_operation_details = {"name": op_name, "time": "Init Failed"}
                show_error(f"Failed to initialize {st.session_state.active_backend_name} (C lib returned error).")
                st.session_state.backend_initialized = False
        except AttributeError as ae:
            st.error(f"Error calling initialize for {st.session_state.active_backend_name}. Wrapper/C lib issue? Details: {ae}")
            st.session_state.backend_initialized = False
        except Exception as e:
            st.session_state.last_operation_details = {"name": op_name, "time": "Error"}
            show_error(f"Exception during {st.session_state.active_backend_name} init: {e}")
            st.session_state.backend_initialized = False

        if st.session_state.backend_initialized:
            st.rerun()

    # --- Utilities ---
    st.markdown("---")
    st.subheader("Utilities")
    with st.expander("Generate Dummy Contacts", expanded=False):
        st.subheader("Create Dummy CSV")
        num_to_generate = st.number_input(
            "Number of contacts to generate:",
            min_value=0,
            max_value=1_000_000,
            value=40_000,
            step=1_000,
            key="num_dummy_app"
        )
        dummy_csv_filename_input = st.text_input(
            "Output Filename (in data/ folder):",
            value="contacts.csv",
            key="dummy_fname_app"
        )

        if st.button("Generate Dummy CSV File", key="gen_dummy_csv_app"):
            from faker import Faker
            import csv
            import random

            full_dummy_path = os.path.join(project_root, "data", dummy_csv_filename_input)
            os.makedirs(os.path.join(project_root, "data"), exist_ok=True)

            fake = Faker()
            op_name_gen = f"Generate {num_to_generate:,} Dummy Contacts"
            start_time_gen = time.perf_counter()
            try:
                with st.spinner(f"Generating {num_to_generate:,} contacts to '{full_dummy_path}'..."):
                    with open(full_dummy_path, 'w', newline='', encoding='utf-8') as csvfile:
                        fieldnames = ["name", "phone", "email"]
                        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
                        writer.writeheader()
                        for _ in range(num_to_generate):
                            name = fake.name()
                            phone = "".join(str(random.randint(0, 9)) for _ in range(10))
                            try:
                                first_name, last_name = name.split(' ')
                                email_name_part = f"{first_name.lower().replace('.', '')}.{last_name.lower().replace('.', '')}"
                            except ValueError:
                                email_name_part = name.lower().replace(' ', '').replace('.', '')
                            email = f"{email_name_part}{random.randint(1,9999)}@{fake.free_email_domain()}"
                            writer.writerow({"name": name, "phone": phone, "email": email})
                end_time_gen = time.perf_counter()
                st.session_state.last_operation_details = {"name": op_name_gen, "time": end_time_gen - start_time_gen}
                show_success(f"Generated {num_to_generate:,} contacts into '{full_dummy_path}'")
                st.info(f"To use, rename '{dummy_csv_filename_input}' to 'contacts.csv' (in data/) and re-initialize backend.")
            except Exception as e:
                st.session_state.last_operation_details = {"name": op_name_gen, "time": "Error"}
                show_error(f"Failed to generate dummy CSV: {e}")

    st.markdown("---")
    st.subheader("📊 Performance Metrics")
    if 'all_contacts_app' in st.session_state:
        st.metric(label="Contacts Loaded", value=f"{len(st.session_state.all_contacts_app):,}")
    if "last_operation_details" in st.session_state:
        details = st.session_state.last_operation_details
        op_name_display = details['name']
        if isinstance(details['time'], float):
            time_val_ms = details['time'] * 1000
            if details['time'] == 0:
                op_time_str = "~0 ms (very fast)"
            elif time_val_ms < 0.01:
                op_time_str = f"{details['time'] * 1_000_000:.2f} µs"
            elif time_val_ms < 1:
                op_time_str = f"{time_val_ms:.3f} ms"
            elif time_val_ms < 1000:
                op_time_str = f"{time_val_ms:.2f} ms"
            else:
                op_time_str = f"{details['time']:.3f} s"
        else:
            op_time_str = str(details['time'])
        st.markdown("**Last Op:**")
        st.markdown(f"`{op_name_display}`")
        
        # st.markdown("**Time:**")
        # st.markdown(f"`{op_time_str}`")   
        st.metric(label="**Time:**", value=f"{op_time_str}")

    else:
        st.markdown("No operations timed yet.")

    try:
        import psutil
        process = psutil.Process(os.getpid())
        memory_info = process.memory_info()
        st.metric(label="App Memory (RSS)", value=f"{memory_info.rss / (1024 * 1024):.2f} MB")
    except ImportError:
        st.warning("psutil not installed; cannot monitor memory usage.")

# --- Main Content Area ---
if not st.session_state.get("backend_initialized") or not st.session_state.get("active_backend_module"):
    st.warning("Backend not initialized. Please select a version and ensure it loads correctly from the sidebar.")
    if initialization_error_v1 and st.session_state.get("active_backend_name") == "Version 1 (Array + BubbleSort)": st.error(initialization_error_v1)
    if initialization_error_v2 and st.session_state.get("active_backend_name") == "Version 2 (LinkedList + MergeSort)": st.error(initialization_error_v2)
    st.stop()

active_module = st.session_state.active_backend_module
active_backend_name = st.session_state.active_backend_name

if st.session_state.contacts_dirty_app:
    op_name_disp = f"Display List ({active_backend_name})"
    start_time_disp = time.perf_counter()
    st.session_state.all_contacts_app = active_module.get_all_contacts()
    end_time_disp = time.perf_counter()
    st.session_state.last_operation_details = {"name": op_name_disp, "time": end_time_disp - start_time_disp}
    st.session_state.contacts_dirty_app = False

st.header("Manage Your Contacts")

with st.expander("➕ Add New Contact", expanded=False):
    with st.form("add_contact_form_main_app", clear_on_submit=True):
        add_name = st.text_input("Name", key="app_add_name")
        add_phone = st.text_input("Phone (10 digits)", key="app_add_phone", max_chars=10)
        add_email = st.text_input("Email", key="app_add_email")
        add_submitted = st.form_submit_button("Add Contact")
        if add_submitted:
            if not all([add_name, add_phone, add_email]): st.warning("All fields are required.")
            elif not active_module.is_valid_name(add_name): st.warning("Invalid name format.")
            elif not active_module.is_valid_number(add_phone): st.warning("Invalid phone number.")
            elif not active_module.is_valid_email(add_email): st.warning("Invalid email format.")
            else:
                op_name = f"Add Contact ({active_backend_name})"
                try:
                    start_time = time.perf_counter(); response_str = active_module.add_contact(add_name, add_phone, add_email); end_time = time.perf_counter()
                    st.session_state.last_operation_details = {"name": op_name, "time": end_time - start_time}
                    if response_str and "Error:" in response_str: show_error(response_str)
                    elif response_str: show_success(response_str)
                    else: show_error("Operation completed but no message returned.")
                    st.session_state.contacts_dirty_app = True
                except Exception as e: st.session_state.last_operation_details = {"name": op_name, "time": "Error"}; show_error(str(e))

search_exp_open_app = st.session_state.expander_search_results is not None
with st.expander("🔍 Search Contacts", expanded=search_exp_open_app):
    search_query_input_app = st.text_input("Search Query", value=st.session_state.get("expander_last_search_query",""), key="app_search_query")
    search_type_options_map = {"Name": 1, "Phone": 2, "Email": 3}
    search_type_display_app = st.selectbox("Search By", list(search_type_options_map.keys()), key="app_search_type")
    if st.button("Search", key="app_search_btn"):
        if search_query_input_app:
            search_type_int = search_type_options_map[search_type_display_app]
            op_name = f"Search by {search_type_display_app} for '{search_query_input_app}' ({active_backend_name})"
            try:
                start_time = time.perf_counter(); results = active_module.search_contacts(search_query_input_app, search_type_int); end_time = time.perf_counter()
                st.session_state.last_operation_details = {"name": op_name, "time": end_time - start_time}
                st.session_state.expander_search_results = results if results else []; st.session_state.expander_last_search_query = search_query_input_app
            except Exception as e: st.session_state.last_operation_details = {"name": op_name, "time": "Error"}; show_error(str(e))
            st.rerun()
        else: st.warning("Please enter a search query.")
    if st.session_state.expander_search_results is not None:
        st.markdown("---"); display_contacts(st.session_state.expander_search_results, f"Search Results")
        if st.button("Clear Search Results", key="app_clear_search_btn"):
            st.session_state.expander_search_results = None; st.session_state.expander_last_search_query = ""
            st.session_state.last_operation_details = {"name": f"Clear Search ({active_backend_name})", "time": 0.0}; st.rerun()

manage_expander_open_app = st.session_state.selected_email_for_manage_expander is not None or \
                           st.session_state.show_edit_form_in_manage_expander or \
                           st.session_state.show_confirm_delete_individual_dialog
with st.expander("⚙️ Manage Individual Contacts", expanded=manage_expander_open_app):
    if not st.session_state.all_contacts_app:
        st.info("No contacts available to manage."); 
        if st.button("Refresh List for Management", key="refresh_manage_app_btn"): st.session_state.contacts_dirty_app = True; st.rerun()
    else:
        contact_options_app = ["-- Select a contact --"] + [f"{c.get('name', 'N/A')} ({c.get('email', 'N/A')})" for c in st.session_state.all_contacts_app]
        selected_email_manage_exp = st.session_state.selected_email_for_manage_expander
        current_selection_display = "-- Select a contact --"
        if selected_email_manage_exp:
            try:
                contact_obj = next(c for c in st.session_state.all_contacts_app if c.get("email") == selected_email_manage_exp)
                current_selection_display = f"{contact_obj.get('name','N/A')} ({contact_obj.get('email','N/A')})"
            except StopIteration: selected_email_manage_exp = None 
        
        idx = 0
        if current_selection_display in contact_options_app: idx = contact_options_app.index(current_selection_display)
        else: st.session_state.selected_email_for_manage_expander = None 

        selected_contact_display_app = st.selectbox("Select a contact to manage:", contact_options_app, index=idx, key="app_sb_manage_contact", disabled=st.session_state.show_confirm_delete_individual_dialog)

        if not st.session_state.show_confirm_delete_individual_dialog and selected_contact_display_app != current_selection_display :
            if selected_contact_display_app == "-- Select a contact --": st.session_state.selected_email_for_manage_expander = None
            else: st.session_state.selected_email_for_manage_expander = selected_contact_display_app.split('(')[-1].split(')')[0]
            st.session_state.show_edit_form_in_manage_expander = False; st.rerun()

        if st.session_state.selected_email_for_manage_expander and not st.session_state.show_confirm_delete_individual_dialog:
            email_to_act_on = st.session_state.selected_email_for_manage_expander
            contact_name_for_button = "Selected Contact"
            try:
                contact_obj_for_button = next(c for c in st.session_state.all_contacts_app if c.get("email") == email_to_act_on)
                contact_name_for_button = contact_obj_for_button.get('name', 'N/A')
            except StopIteration: pass 

            manage_cols_app = st.columns(2)
            with manage_cols_app[0]:
                if st.button(f"✏️ Edit {contact_name_for_button}", key=f"app_edit_btn_{email_to_act_on.replace('@','_').replace('.','_')}", use_container_width=True):
                    st.session_state.show_edit_form_in_manage_expander = True; st.rerun()
            with manage_cols_app[1]:
                if st.button(f"🗑️ Delete {contact_name_for_button}", key=f"app_delete_trigger_btn_{email_to_act_on.replace('@','_').replace('.','_')}", type="secondary", use_container_width=True):
                    st.session_state.individual_contact_to_delete_email = email_to_act_on
                    st.session_state.individual_contact_to_delete_name = contact_name_for_button
                    st.session_state.show_confirm_delete_individual_dialog = True; st.rerun()
    
    if st.session_state.show_confirm_delete_individual_dialog and st.session_state.individual_contact_to_delete_email:
        st.markdown("---"); st.warning(f"**Confirm Deletion**\n\nAre you sure you want to delete: **{st.session_state.individual_contact_to_delete_name}** ({st.session_state.individual_contact_to_delete_email})?")
        confirm_cols_app = st.columns(2)
        with confirm_cols_app[0]:
            if st.button("✔️ Yes, Delete This Contact", key="app_confirm_del_ind_yes", use_container_width=True, type="primary"):
                email_del = st.session_state.individual_contact_to_delete_email; name_del = st.session_state.individual_contact_to_delete_name
                op_name = f"Delete Individual ({active_backend_name})"
                try:
                    start_time = time.perf_counter(); del_success = active_module.delete_contact_by_email(email_del); end_time = time.perf_counter()
                    st.session_state.last_operation_details = {"name": op_name, "time": end_time - start_time}
                    if del_success: show_success(f"Contact '{name_del}' deleted."); st.session_state.contacts_dirty_app = True; st.session_state.selected_email_for_manage_expander = None
                    else: show_error(f"Failed to delete '{name_del}'.")
                except Exception as e: st.session_state.last_operation_details = {"name": op_name, "time": "Error"}; show_error(str(e))
                finally: st.session_state.show_confirm_delete_individual_dialog = False; st.session_state.individual_contact_to_delete_email = None; st.session_state.individual_contact_to_delete_name = None; st.rerun()
        with confirm_cols_app[1]:
            if st.button("❌ No, Cancel", key="app_confirm_del_ind_no", use_container_width=True, type="secondary"):
                st.session_state.show_confirm_delete_individual_dialog = False; st.session_state.individual_contact_to_delete_email = None; st.session_state.individual_contact_to_delete_name = None; st.rerun()

    if st.session_state.show_edit_form_in_manage_expander and st.session_state.selected_email_for_manage_expander and not st.session_state.show_confirm_delete_individual_dialog:
        email_being_edited = st.session_state.selected_email_for_manage_expander
        contact_to_edit = next((c for c in st.session_state.all_contacts_app if c.get("email") == email_being_edited), None)
        if contact_to_edit:
            st.markdown("---")
            with st.form(key=f"app_edit_form_{email_being_edited.replace('@','_').replace('.','_')}"):
                st.subheader(f"Editing: {contact_to_edit.get('name', 'N/A')}")
                edit_name = st.text_input("New Name", value=contact_to_edit.get('name',''), key=f"app_edit_name_{email_being_edited}")
                edit_phone = st.text_input("New Phone", value=contact_to_edit.get('phone',''), key=f"app_edit_phone_{email_being_edited}", max_chars=10)
                edit_email_new = st.text_input("New Email", value=contact_to_edit.get('email',''), key=f"app_edit_email_{email_being_edited}")
                st.caption(f"Original Email ID: {email_being_edited}")
                form_cols_app_edit = st.columns(2)
                with form_cols_app_edit[0]:
                    if st.form_submit_button("💾 Save Changes", type="primary", use_container_width=True):
                        if not all([edit_name, edit_phone, edit_email_new]): st.warning("All fields required.")
                        elif not active_module.is_valid_name(edit_name): st.warning("Invalid name.")
                        elif not active_module.is_valid_number(edit_phone): st.warning("Invalid phone.")
                        elif not active_module.is_valid_email(edit_email_new): st.warning("Invalid email.")
                        else:
                            op_name = f"Edit Contact ({active_backend_name})"
                            try:
                                start_time = time.perf_counter(); response_str = active_module.edit_contact(email_being_edited, edit_name, edit_phone, edit_email_new); end_time = time.perf_counter()
                                st.session_state.last_operation_details = {"name": op_name, "time": end_time - start_time}
                                if response_str and "Error:" in response_str: show_error(response_str)
                                elif response_str: show_success(response_str)
                                else: show_error("Edit operation status unknown.")
                                st.session_state.contacts_dirty_app = True; st.session_state.show_edit_form_in_manage_expander = False
                                st.session_state.selected_email_for_manage_expander = edit_email_new 
                            except Exception as e: st.session_state.last_operation_details = {"name": op_name, "time": "Error"}; show_error(str(e))
                            st.rerun()
                with form_cols_app_edit[1]:
                    if st.form_submit_button("❌ Cancel Edit", use_container_width=True, type="secondary"):
                        st.session_state.show_edit_form_in_manage_expander = False; st.rerun()
        else: st.warning("Contact to edit not found. It might have been deleted."); st.session_state.show_edit_form_in_manage_expander = False

with st.expander("📖 View Full Contact List", expanded=False):
    st.header("Full Contact List")
    sort_cols_app_view = st.columns(3); sort_options_app_view = {"Name": 1, "Phone": 2, "Email": 3}
    for i, (sort_label, sort_type_val) in enumerate(sort_options_app_view.items()):
        with sort_cols_app_view[i]:
            if st.button(f"Sort by {sort_label}", use_container_width=True, key=f"app_sort_{sort_label.lower()}_btn"):
                op_name = f"Sort by {sort_label} ({active_backend_name})"
                try:
                    start_time = time.perf_counter(); sort_success = active_module.sort_contacts(sort_type_val); end_time = time.perf_counter()
                    st.session_state.last_operation_details = {"name": op_name, "time": end_time - start_time}
                    if sort_success: show_success(f"List sorted by {sort_label}.")
                    else: show_error(f"Failed to sort by {sort_label}.")
                    st.session_state.contacts_dirty_app = True
                except Exception as e: st.session_state.last_operation_details = {"name": op_name, "time": "Error"}; show_error(str(e))
                st.rerun()
    display_contacts(st.session_state.all_contacts_app, f"Contact List ({active_backend_name})")

st.markdown("---"); st.subheader("Final Data Operations")
final_col1_app_bottom, final_col2_app_bottom = st.columns(2)
with final_col1_app_bottom:
    st.markdown('<div class="button-wrapper">', unsafe_allow_html=True)
    if st.button("💾 Save All to CSV", use_container_width=True, type="secondary", key="app_save_all_final_btn"):
        op_name = f"Save All ({active_backend_name})"
        try:
            start_time = time.perf_counter(); save_success = active_module.save_contacts(os.path.join(project_root, "data", "contacts.csv")); end_time = time.perf_counter()
            st.session_state.last_operation_details = {"name": op_name, "time": end_time - start_time}
            if save_success: show_success("Contacts saved to data/contacts.csv!")
            else: show_error("Failed to save contacts.")
        except Exception as e: st.session_state.last_operation_details = {"name": op_name, "time": "Error"}; show_error(str(e))
    st.markdown('</div>', unsafe_allow_html=True)
with final_col2_app_bottom:
    st.markdown('<div class="button-wrapper red-button-container">', unsafe_allow_html=True)
    if st.button("🗑️ Delete All Contacts from Memory", use_container_width=True, key="app_delete_all_final_btn", type="secondary"):
        st.session_state.show_delete_all_confirmation = True; st.rerun()
    st.markdown('</div>', unsafe_allow_html=True)

if st.session_state.show_delete_all_confirmation:
    st.markdown("<div style='border: 2px solid #FF4B4B; padding: 15px; border-radius: 5px; margin-top:10px; background-color: #fff0f0;'>", unsafe_allow_html=True)
    st.subheader(f"🚨 Confirm: Delete ALL Contacts from Memory ({active_backend_name}) 🚨")
    st.warning(f"This will delete all contacts in memory for {active_backend_name}."); st.caption("To clear the CSV, 'Save All to CSV' after this.")
    confirm_cb_app_final_del = st.checkbox("Yes, I understand and want to delete ALL contacts from memory.", key="app_confirm_del_all_cb")
    del_confirm_col1_app_final_del, del_confirm_col2_app_final_del = st.columns(2)
    with del_confirm_col1_app_final_del:
        if st.button("Proceed and Delete All", disabled=not confirm_cb_app_final_del, type="primary", use_container_width=True, key="app_proceed_del_all_btn"):
            if confirm_cb_app_final_del:
                op_name = f"Delete All Contacts ({active_backend_name})"
                try:
                    start_time = time.perf_counter(); delete_success = active_module.delete_all_contacts(); end_time = time.perf_counter()
                    st.session_state.last_operation_details = {"name": op_name, "time": end_time - start_time}
                    if delete_success: show_success("All contacts deleted from memory.")
                    else: show_error("Failed to delete all contacts.")
                    st.session_state.contacts_dirty_app = True; st.session_state.show_delete_all_confirmation = False; st.session_state.selected_email_for_manage_expander = None 
                except Exception as e: st.session_state.last_operation_details = {"name": op_name, "time": "Error"}; show_error(str(e))
                st.rerun()
    with del_confirm_col2_app_final_del:
        if st.button("Cancel Deletion", use_container_width=True, type="secondary", key="app_cancel_del_all_btn"):
            st.session_state.show_delete_all_confirmation = False; st.rerun()
    st.markdown("</div>", unsafe_allow_html=True)
