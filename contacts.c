#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "contacts.h"

// This is the (somewhat famous) djb2 hash
unsigned hash(const char *str) {
    unsigned hash_val = 5381;
    int i = 0;
    while (str[i] != '\0') {
        hash_val = ((hash_val << 5) + hash_val) + str[i];
        i++;
    }
    return hash_val % NUM_BUCKETS;
}

contacts_log_t *create_contacts_log(const char *log_name) {
    contacts_log_t *new_log = malloc(sizeof(contacts_log_t));
    if (new_log == NULL) {
        return NULL;
    }

    strcpy(new_log->log_name, log_name);
    for (int i = 0; i < NUM_BUCKETS; i++) {
        new_log->buckets[i] = NULL;
    }
    new_log->size = 0;

    return new_log;
}

const char *get_contacts_log_name(const contacts_log_t *log) {
    return log->log_name;   // returns the name of the log
}

int add_contact(contacts_log_t *log, const char *name, unsigned long phone_number, unsigned zip_code) {
    if (log == NULL) {  // Checks whether the log exists or not
        return -1;
    }

    unsigned index = hash(name);    // extracting the index of the bucket associated with the name

    node_t *new_node = malloc(sizeof(node_t));  // allocates address to the new node
    if (new_node == NULL) {
        return -1;
    }
    // adds the new contact information to the new node
    strcpy(new_node->name, name);
    new_node->phone_number = phone_number;
    new_node->zip_code = zip_code;
    new_node->next = NULL;

    node_t *current = log->buckets[index];  // pointer to the current node

    if (current == NULL) {
        log->buckets[index] = new_node; // point the new node to the empty bucket
    } else {
        while (current->next != NULL) { // traverse to the end of the linked list
            current = current->next;
        }
        current->next = new_node;
    }

    log->size += 1; // updates the size of the linked list

    return 0;
}

long find_phone_number(const contacts_log_t *log, const char *name) {
    unsigned index = hash(name);    // extracts the index of the bucket associated with the name
    node_t *current = log->buckets[index];

    if (current == NULL) {  // returns -1 if the node doesn't exist
        return -1;
    }

    while (current != NULL) {   // Traverses linked list to find matching contact name
        if (strcmp(current->name, name) != 0) {
            current = current->next;
        } else {
            return current->phone_number;   // returns the phone number if found
        }
    }

    return -1;
}

void print_contacts_log(const contacts_log_t *log) {
    if (log == NULL) {
        return;
    }

    for (int i = 0; i < NUM_BUCKETS; i++) { // iterates through the linked list to print contact info
        node_t *current = log->buckets[i];
        while (current != NULL) {   // prints all the stored contact information
            printf("Name: %s, Phone Number: %lu, Zip Code: %u\n", current->name, current->phone_number, current->zip_code);
            current = current->next;
        }
    }
}

void free_contacts_log(contacts_log_t *log) {
    if (log == NULL) {
        return;
    }

    for (int i = 0; i < NUM_BUCKETS; i++) { // iterates through the linked list to free every bucket
        node_t *ptr = log->buckets[i];
        while (ptr != NULL) {
            node_t *temp = ptr; // a temporary pointer
            ptr = ptr->next;
            free(temp);
        }
        log->buckets[i] = NULL; // initializes every bucket back to NULL
    }

    free(log);  // frees the contact log
}

int write_contacts_log_to_text(const contacts_log_t *log) {
    char file_name[MAX_NAME_LEN + strlen(".txt")];
    strcpy(file_name, log->log_name);
    strcat(file_name, ".txt");
    FILE *f = fopen(file_name, "w");
    if (f == NULL) {
        return -1;
    }

    fprintf(f, "%u\n", log->size);
    for (int i = 0; i < NUM_BUCKETS; i++) {
        node_t *current = log->buckets[i];
        while (current != NULL) {
            fprintf(f, "%s %lu %u\n", current->name, current->phone_number, current->zip_code);
            current = current->next;
        }
    }
    fclose(f);
    return 0;
}

contacts_log_t *read_contacts_log_from_text(const char *file_name) {
    FILE *f = fopen(file_name, "r");
    if (f == NULL) {
        return NULL;
    }

    char log_name[MAX_NAME_LEN];
    strncpy(log_name, file_name, sizeof(log_name)); // duplicates the name of the file
    int len = strlen(log_name);
    if (len > 4) {
        log_name[len - 4] = '\0';   // add null terminators starting from the '.'
    }

    contacts_log_t *log = create_contacts_log(log_name);    // call to create the log
    if (log == NULL) {
        fclose(f);
        return NULL;
    }
    
    unsigned size;
    fscanf(f, "%d", &size); // reads the size of the log from the file

    char name[MAX_NAME_LEN];
    unsigned long phone;
    unsigned zip;

    for (int i = 0; i < size; i++) {
        fscanf(f, "%s %lu %u", name, &phone, &zip);
        int success = add_contact(log, name, phone, zip);   // adds the contact info to the log
        if (success == -1) {
            fclose(f);
            return NULL;
        }
    }

    fclose(f);
    return log;
}

int write_contacts_log_to_binary(const contacts_log_t *log) {
    char file_name[MAX_NAME_LEN + strlen(".bin")];
    strcpy(file_name, log->log_name);
    strcat(file_name, ".bin");  // concatenates the file extension to the file name

    FILE *f = fopen(file_name, "w");
    if (f == NULL) {
        return -1;
    }

    fwrite(&log->size, sizeof(unsigned), 1, f); // writes the size of the log to the file

    for (int i = 0; i < NUM_BUCKETS; i++) {
        node_t *current = log->buckets[i];  // gets the current bucket

        while (current != NULL) {   // writes the contact info to the file
            unsigned length = strlen(current->name);
            fwrite(&length, sizeof(unsigned), 1, f);
            fwrite(current->name, sizeof(char), length, f);
            fwrite(&current->phone_number, sizeof(unsigned long), 1, f);
            fwrite(&current->zip_code, sizeof(unsigned), 1, f);
            current = current->next;
        }
    }
    fclose(f);
    return 0;
}

contacts_log_t *read_contacts_log_from_binary(const char *file_name) {
    FILE *f = fopen(file_name, "rb");
    if (f == NULL) {
        return NULL;
    }

    char log_name[MAX_NAME_LEN];
    strcpy(log_name, file_name);    // duplicates the name of the file
    int len = strlen(log_name);
    if (len >= 4) {
        log_name[len - 4] = '\0';   // add null terminators starting from the '.'
    }

    contacts_log_t *log = create_contacts_log(log_name);    // call to create the log
    if (log == NULL) {
        fclose(f);
        return NULL;
    }

    unsigned size;
    fread(&size, sizeof(unsigned), 1, f);   // reads the size of the log from the file

    unsigned length;
    char name[MAX_NAME_LEN] = "";
    unsigned long phone;
    unsigned zip;

    for (int i = 0; i < size; i++) {    // reads the contact info from the file
        fread(&length, sizeof(unsigned), 1, f);
        fread(name, sizeof(char), length, f);
        fread(&phone, sizeof(unsigned long), 1, f);
        fread(&zip, sizeof(unsigned), 1, f);
        
        name[length] = '\0';    // add the null terminator at the end of the contact's name

        int success = add_contact(log, name, phone, zip);   // adds the contact info to the log
        if (success == -1) {
            fclose(f);
            return NULL;
        }
    }

    fclose(f);
    return log;
}
