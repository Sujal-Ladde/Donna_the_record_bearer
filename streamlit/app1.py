import streamlit as st
import contact_manager_c # This is your compiled C module
import pandas as pd

# 1. SET PAGE CONFIG MUST BE THE VERY FIRST STREAMLIT COMMAND
st.set_page_config(page_title="Donna Contact Manager", layout="wide")

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
st.title("📖 Donna Contact Manager")
st.caption(f"Powered by C & Python. Developed by Sujal Ladde.")

# --- Initialization and Session State ---
if 'initialized' not in st.session_state:
    try:
        contact_manager_c.initialize(); st.session_state.initialized = True; st.session_state.contacts_dirty = True
    except Exception as e: show_error(f"Initialization failed: {e}"); st.stop()

default_states = {
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

if st.session_state.contacts_dirty or (not st.session_state.all_contacts and st.session_state.initialized): # Ensure initial load happens
    st.session_state.all_contacts = contact_manager_c.get_all_contacts()
    st.session_state.contacts_dirty = False

# --- Primary Actions Area ---
st.header("Manage Your Contacts")

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
                try: 
                    response = contact_manager_c.add_contact(add_name, add_phone, add_email)
                    show_success(response) 
                    st.session_state.contacts_dirty = True
                    # REMOVED st.rerun() - state change will trigger it
                except Exception as e: 
                    show_error(str(e))
                    # No explicit rerun needed here either.

search_expander_open = st.session_state.expander_search_results is not None
with st.expander("🔍 Search Contacts", expanded=search_expander_open):
    search_query_input = st.text_input("Search Query",value=st.session_state.expander_last_search_query, key="search_query_exp")
    search_type_option_input = st.selectbox("Search By", ("Name", "Phone", "Email"), key="search_type_exp")
    if st.button("Search", key="search_btn_exp"): 
        if search_query_input:
            results = contact_manager_c.search_contacts(search_query_input, {"Name": 1, "Phone": 2, "Email": 3}[search_type_option_input])
            st.session_state.expander_search_results = results if results is not None else []
            st.session_state.expander_last_search_query = search_query_input
            st.rerun() # Rerun here is okay to show search results immediately
        else: st.warning("Please enter a search query.")
    if st.session_state.expander_search_results is not None:
        st.markdown("---"); display_contacts(st.session_state.expander_search_results, f"Search results for '{st.session_state.expander_last_search_query}'")
        if st.button("Clear Search Results", key="clear_search_btn_exp"):
            st.session_state.expander_search_results = None; st.session_state.expander_last_search_query = ""
            st.rerun() # Rerun here is okay to clear search results immediately

manage_expander_open = st.session_state.selected_email_for_manage_expander is not None or \
                       st.session_state.show_edit_form_in_manage_expander or \
                       st.session_state.show_confirm_delete_individual_dialog
with st.expander("⚙️ Manage Individual Contacts", expanded=manage_expander_open):
    if not st.session_state.all_contacts:
        st.info("No contacts available to manage. Add some contacts first.")
        if st.button("Refresh Contact List", key="refresh_manage_exp"):
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
                        st.session_state.show_confirm_delete_individual_dialog = True
                        st.rerun() # Rerun to show confirmation dialog
        elif not st.session_state.show_confirm_delete_individual_dialog and selected_contact_display == "-- Select a contact --" and st.session_state.selected_email_for_manage_expander is not None:
             st.session_state.selected_email_for_manage_expander = None; st.session_state.show_edit_form_in_manage_expander = False; st.rerun()

    if st.session_state.get('show_confirm_delete_individual_dialog') and st.session_state.get('individual_contact_to_delete_email'):
        st.markdown("---")
        st.warning(f"**Confirm Deletion**\n\nAre you sure you want to delete: **{st.session_state.individual_contact_to_delete_name}** ({st.session_state.individual_contact_to_delete_email})?")
        confirm_cols = st.columns(2)
        with confirm_cols[0]:
            if st.button("✔️ Yes, Delete This Contact", key="confirm_delete_individual_yes", use_container_width=True, type="primary"):
                email_to_delete = st.session_state.individual_contact_to_delete_email; name_deleted = st.session_state.individual_contact_to_delete_name
                try:
                    if contact_manager_c.delete_contact_by_email(email_to_delete):
                        show_success(f"Contact '{name_deleted}' deleted.")
                        st.session_state.contacts_dirty = True
                        st.session_state.selected_email_for_manage_expander = None 
                        st.session_state.show_edit_form_in_manage_expander = False
                    else: show_error(f"Failed to delete '{name_deleted}'.")
                except Exception as e: show_error(str(e))
                finally:
                    st.session_state.show_confirm_delete_individual_dialog = False; st.session_state.individual_contact_to_delete_email = None; st.session_state.individual_contact_to_delete_name = None
                    # REMOVED st.rerun() - state change will trigger it
        with confirm_cols[1]:
            if st.button("❌ No, Cancel", key="confirm_delete_individual_no", use_container_width=True, type="secondary"):
                st.session_state.show_confirm_delete_individual_dialog = False; st.session_state.individual_contact_to_delete_email = None; st.session_state.individual_contact_to_delete_name = None
                # REMOVED st.rerun() - state change will trigger it
    
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
                            try:
                                response = contact_manager_c.edit_contact(email_being_edited, edit_name, edit_phone, edit_email_new)
                                show_success(response) 
                                st.session_state.contacts_dirty = True; st.session_state.show_edit_form_in_manage_expander = False
                                st.session_state.selected_email_for_manage_expander = edit_email_new 
                                # REMOVED st.rerun() - state change will trigger it
                            except Exception as e: show_error(str(e))
                with form_cols[1]:
                    if st.form_submit_button("❌ Cancel Edit", use_container_width=True, type="secondary"):
                        st.session_state.show_edit_form_in_manage_expander = False
                        # REMOVED st.rerun() - state change will trigger it (or form clear will)
        else: st.warning("Contact to edit not found (it may have been deleted). Please re-select."); st.session_state.show_edit_form_in_manage_expander = False

with st.expander("📖 View Full Contact List", expanded=False):
    st.header("Full Contact List")
    sort_col1, sort_col2, sort_col3 = st.columns(3)
    def sort_and_rerun(sort_func): sort_func(); st.session_state.contacts_dirty = True; st.rerun() # Rerun for sort is fine
    with sort_col1:
        if st.button("Sort by Name", use_container_width=True, key="sort_name_table"): sort_and_rerun(contact_manager_c.sort_contacts_by_name)
    with sort_col2:
        if st.button("Sort by Phone", use_container_width=True, key="sort_phone_table"): sort_and_rerun(contact_manager_c.sort_contacts_by_phone)
    with sort_col3:
        if st.button("Sort by Email", use_container_width=True, key="sort_email_table"): sort_and_rerun(contact_manager_c.sort_contacts_by_email)
    if st.session_state.contacts_dirty or not st.session_state.all_contacts and st.session_state.initialized:
        st.session_state.all_contacts = contact_manager_c.get_all_contacts(); st.session_state.contacts_dirty = False
    display_contacts(st.session_state.all_contacts, "Currently Stored Contacts")

st.markdown("---"); st.subheader("Final Data Operations") 
final_col1, final_col2 = st.columns(2)
with final_col1:
    st.markdown('<div class="button-wrapper">', unsafe_allow_html=True) 
    if st.button("💾 Save All to CSV", use_container_width=True, type="secondary", key="save_all_final_btn"): 
        try: 
            contact_manager_c.save_contacts()
            show_success("Contacts saved to contacts.csv!") # This should work as no immediate rerun follows
        except Exception as e: show_error(str(e)) 
    st.markdown('</div>', unsafe_allow_html=True)
with final_col2: 
    st.markdown('<div class="button-wrapper red-button-container">', unsafe_allow_html=True) 
    if st.button("🗑️ Delete All Contacts from Memory", use_container_width=True, key="delete_all_final_btn", type="secondary"):
        st.session_state.show_delete_all_confirmation = True; st.rerun() # Rerun to show confirmation is fine
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
                try: 
                    contact_manager_c.delete_all_contacts()
                    show_success("All contacts deleted from memory.") 
                    st.session_state.contacts_dirty = True; st.session_state.show_delete_all_confirmation = False; st.session_state.selected_email_for_manage_expander = None; 
                    # REMOVED st.rerun()
                except Exception as e: show_error(str(e)) 
    with del_confirm_col2:
        if st.button("Cancel Deletion", use_container_width=True, type="secondary", key="cancel_del_all_btn_final"):
            st.session_state.show_delete_all_confirmation = False
            # REMOVED st.rerun()
    st.markdown("</div>", unsafe_allow_html=True)