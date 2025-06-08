from setuptools import setup, Extension
import pybind11
import os

# Get the absolute path to the directory containing this setup.py file
# This helps in locating source files correctly, especially in different build environments
source_dir = os.path.abspath(os.path.dirname(__file__))

contact_module = Extension(
    'contact_manager_c',  # Name of the module as imported in Python: import contact_manager_c
    sources=[
        os.path.join(source_dir, 'wrapper.cpp'),
        os.path.join(source_dir, 'contact.c')
    ],
    include_dirs=[
        pybind11.get_include(),
        source_dir  # To find contact.h
    ],
    language='c++',
    extra_compile_args=['-std=c++11'] # Or -std=c++14, -std=c++17 if needed by your compiler/pybind11 version
    # For Windows with MSVC, you might not need extra_compile_args explicitly for C++ standard,
    # but you might need other flags if you encounter issues.
    # extra_link_args=[] # If any specific linker flags are needed
)

setup(
    name='ContactManagerCBinding',
    version='1.0',
    author='Sujal Ladde & AI Assistant',
    description='Python bindings for Donna C Contact Management Library',
    ext_modules=[contact_module],
    # You might need to specify pybind11 as a setup_requires or install_requires
    # if it's not already available in the build environment.
    # setup_requires=['pybind11>=2.5'], # Example
    python_requires='>=3.7', # Specify your minimum Python version
)