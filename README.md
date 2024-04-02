**Contacts Log Program**

**Overview:**

This project explores several fundamental aspects of getting work done in C:

- Dynamic memory management with malloc() and free()
- Reading and writing files in both text and binary formats
- Displaying information to the screen
- Reading commands from the user in interactive programs
- Building data structures with the C struct primitive and pointers

It is an implementation of a simple Contacts Log Program (i.e., a system for recording contact information about a group of people) based on a hash table data structure. It also has a command-line interface that allows a user to interact with this data structure by adding new contacts, searching for existing contact information, printing all contacts, and reading/writing contact data to/from both text and binary files.

**Features:**

The contacts log stores the following information for each contact:

- A name (e.g., Louise)
- A phone number (e.g., 6126254002)
- A zip code (e.g., 55455)
  
Note that for phone numbers we store only the digits of the number and not any other symbols sometimes used in written representations of phone numbers. For example, we store 6126254002 and not 612-625-4002. This allows use to use a numerical data type for each phone number rather than a string. Also assume that no phone numbers start with 0.

Zip codes are stored similarly, as a numerical data type and not as a string.

Assume that no zip codes start with 0.

**Prerequisites**
- C compiler (e.g., GCC)
- Git (optional)

**Usage**
1. Run the compiled program using the command: ./contacts_main
2. Follow the on-screen prompts to perform various actions such as adding, searching, or deleting contacts.
