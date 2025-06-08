# setup.py
import os
import platform
import shutil
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext as _build_ext

# Define the C extension for Version 1
ext_v1 = Extension(
    name="contact_v1_lib",
    sources=["version1/contact_v1_lib.c"],
    include_dirs=["version1"],
    # This tells setuptools not to expect PyInit_contact_v1_lib,
    # which is crucial for building a generic DLL/SO for ctypes with MSVC.
    export_symbols=[], 
    # For MSVC, setuptools usually adds /DLL. If not, you might need:
    # extra_link_args = ["/DLL"] if platform.system() == "Windows" else []
)

# Define the C extension for Version 2
ext_v2 = Extension(
    name="contact_v2_lib",
    sources=["version2/contact_v2_lib.c"],
    include_dirs=["version2"],
    export_symbols=[],
    # extra_link_args = ["/DLL"] if platform.system() == "Windows" else []
)

class build_ext_subclass(_build_ext):
    def get_ext_filename(self, ext_name):
        # This method is called by distutils to determine the filename
        # of the extension. We want to ensure it has the correct suffix
        # for a shared library (.dll, .so, .dylib).
        from distutils.sysconfig import get_config_var
        
        # ext_name here is "contact_v1_lib" or "contact_v2_lib"
        # We want the output name to be exactly that, plus the shared lib suffix.
        if platform.system() == "Windows":
            # For ctypes, we want .dll. Python extensions are .pyd (which are DLLs).
            # Let's aim for .dll directly.
            return ext_name + ".dll"
        else:
            # For Linux/macOS, get_config_var('SHLIB_SUFFIX') gives .so or .dylib
            return ext_name + (get_config_var('SHLIB_SUFFIX') or "")

    def run(self):
        # Ensure the final target directory exists
        os.makedirs("compiled_libs", exist_ok=True)
        
        # Call the original build_ext run method
        _build_ext.run(self)

        # After building, copy the files from the build directory to compiled_libs/
        # build_lib is the default output directory for build_ext (e.g., build/lib.win-amd64-cpython-311)
        # The self.extensions contains the Extension objects we defined.
        for ext in self.extensions:
            # self.get_ext_fullpath(ext.name) gives the path where build_ext placed the file
            built_path = self.get_ext_fullpath(ext.name)
            
            # Our desired final filename (e.g., contact_v1_lib.dll)
            # Use the name attribute of the extension directly for the base.
            final_filename = self.get_ext_filename(ext.name) # This now just gives "name.suffix"
            
            # Construct the final destination path
            dest_path = os.path.join("compiled_libs", os.path.basename(final_filename))

            if os.path.exists(built_path):
                print(f"Copying {built_path} to {dest_path}")
                shutil.copy(built_path, dest_path)
            else:
                print(f"Warning: Expected built file {built_path} not found for extension {ext.name}.")


setup(
    name="ContactManagerBackends",
    version="0.1.0",
    description="C backends for Contact Manager (Array and LinkedList versions)",
    ext_modules=[ext_v1, ext_v2],
    cmdclass={
        'build_ext': build_ext_subclass
    }
)
