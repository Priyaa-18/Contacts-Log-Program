#include <stdio.h>
#include <string.h>

#include "contacts.h"

#define MAX_CMD_LEN 128

/*
 * This is in general *very* similar to the list_main file seen in lab 2
 * One big difference is the notion of switching between contact logs in one
 * run of the program.
 * You have to create or load a contact log from a file before you can do things
 * like add, lookup, or write.
 * The code below has to check if contact log is NULL before the operations
 * occur. Also, the user has to explicitly clear the current contact log before
 * they can create or load in a new one.
 */
int main(int argc, char **argv) {
    contacts_log_t *log = NULL;
    char name[MAX_NAME_LEN];
    unsigned long phn;
    unsigned zip;

    if (argc < 2) { // checking if the number of arguments are valid or not
        printf("\n");
    } else {
        int len = strlen(argv[1]);

        if (len >= 4 && strncmp(argv[1] + len - 4, ".txt", 4) == 0) {   // comparing the file extension
            log = read_contacts_log_from_text(argv[1]);
            if (log == NULL) {
                printf("Failed to read contacts log from text file\n");
            } else {
                printf("Contacts log loaded from text file\n");
            }
        } else if (len >= 4 && strncmp(argv[1] + len - 4, ".bin", 4) == 0) {    // comparing the file extension
            log = read_contacts_log_from_binary(argv[1]);
            if (log == NULL) {
                printf("Failed to read contacts log from binary file\n");
            } else {
                printf("Contacts log loaded from binary file\n");
            }
        } else {
            printf("Error: Unknown contacts log file extension\n");
        }
    }

    printf("CSCI 2021 Contact Log System\n");
    printf("Commands:\n");
    printf("  create <name>:            creates a new log with specified name\n");
    printf("  log:                      shows the name of the active contacts log\n");
    printf("  add <name> <phone> <zip>: adds a new contact\n");
    printf("  lookup <name>:            searches for a phone number by contact name\n");
    printf("  clear:                    resets current contacts log\n");
    printf("  print:                    shows all contacts in active log\n");
    printf("  write_text:               saves all contacts to text file\n");
    printf("  read_text <file_name>:    loads contacts from text file\n");
    printf("  write_bin:                saves all contacts to binary file\n");
    printf("  read_bin <file_name>:     loads contacts from binary file\n");
    printf("  exit:                     exits the program\n");

    char cmd[MAX_CMD_LEN];
    while (1) {
        printf("contacts> ");
        if (scanf("%s", cmd) == EOF) {
            printf("\n");
            break;
        }

        if (strcmp("exit", cmd) == 0) {
            break;
        }

        else if (strcmp("create", cmd) == 0) {
            scanf("%s", cmd); // Read in new log name
            if (log != NULL) {
                printf("Error: You already have an contacts log.\n");
                printf("You can remove it with the \'clear\' command\n");
            } else {
                log = create_contacts_log(cmd);
                if (log == NULL) {
                    printf("Contacts log creation failed\n");
                }
            }
        }


        // TODO Add cases for other commands
        // Read in the command and (possibly) additional arguments with scanf()

        else if (strcmp("log", cmd) == 0) {
            if (log != NULL) {
                const char *log_name = get_contacts_log_name(log);  // Calls the function to get the log name
                if (log_name != NULL) {
                    printf("%s\n", log_name);
                }
            } else {
                printf("Error: You must create or load a contacts log first\n");
            }
        }

        else if (strcmp("add", cmd) == 0) {
            // Read in the new contact information
            scanf("%s %lu %u", name, &phn, &zip);
            if ((phn < 1000000000 || phn > 9999999999) || (zip < 10000 || zip > 99999)) {   // checks for valid range
                printf("Error: Invalid phone number and/or zip code\n");
            } else if (log != NULL) {
                int failure = add_contact(log, name, phn, zip);
                if (failure) {
                    printf("Failed to add the contact to the contact log\n");
                }
            } else {
                printf("Error: You must create or load a contacts log first\n");
            }
        }

        else if (strcmp("lookup", cmd) == 0) {
            scanf("%s", name); // Read in the name of the contact to search for
            if (log != NULL) {
                unsigned long phn = find_phone_number(log, name);
                if (phn != -1) {
                    printf("%s: %lu\n", name, phn);
                } else {
                    printf("No phone number for \'%s\' found\n", name);
                }
            } else {
                printf("Error: You must create or load a contacts log first\n");
            }
        }

        else if (strcmp("clear", cmd) == 0) {
            if (log != NULL) {
                free_contacts_log(log); // free the contact log if it is not already
                log = NULL;
            } else {
                printf("Error: No contacts log to clear\n");
            }
        }

        else if (strcmp("print", cmd) == 0) {
            if (log != NULL) {
                printf("All contacts in %s:\n", log->log_name);
                print_contacts_log(log);    // displays all contacts stored in the log
            } else {
                printf("Error: You must create or load a contacts log first\n");
            }
        }

        else if (strcmp("write_text", cmd) == 0) {
            if (log != NULL) {
                int success = write_contacts_log_to_text(log);  // writes all the contacts in text file
                if (success == 0) {
                    printf("Contacts log successfully written to %s.txt\n", log->log_name);
                } else {
                    printf("Failed to write contacts log to text file\n");
                }
            } else {
                printf("Error: You must create or load a contacts log first\n");
            }
        }

        else if (strcmp("read_text", cmd) == 0) {
            scanf("%s", cmd); // Read in the name of the text file
            if (log != NULL) {
                printf("Error: You must clear current contacts log first\n");
            } else {
                contacts_log_t *success = read_contacts_log_from_text(cmd);
                if (success != NULL) {
                    free_contacts_log(log);
                    log = success;
                    printf("Contacts log loaded from text file\n");
                } else {
                    printf("Failed to read contacts log from text file\n");
                }
            }
        }

        else if (strcmp("write_bin", cmd) == 0) {
            if (log != NULL) {
                int success = write_contacts_log_to_binary(log);    // writes all the contacts in binary file
                if (success == 0) {
                    printf("Contacts log successfully written to %s.bin\n", log->log_name);
                } else {
                    printf("Failed to write contacts log to binary file\n");
                }
            } else {
                printf("Error: You must create or load a contacts log first\n");
            }
        }

        else if (strcmp("read_bin", cmd) == 0) {
            scanf("%s", cmd); // Read in the name of the binary file
            if (log != NULL) {
                printf("Error: You must clear current contacts log first\n");
            } else {
                contacts_log_t *success = read_contacts_log_from_binary(cmd);
                if (success != NULL) {
                    free_contacts_log(log);
                    log = success;
                    printf("Contacts log loaded from binary file\n");
                } else {
                    printf("Failed to read contacts log from binary file\n");
                }
            }
        }

        else {
            printf("Unknown command %s\n", cmd);
        }
    }

    if (log != NULL) {
        free_contacts_log(log);
    }
    return 0;
}
