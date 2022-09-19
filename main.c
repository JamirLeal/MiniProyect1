#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdbool.h"

#define MAX_BUFFER_SIZE 100
#define MAX_N_CONTACTS 10

size_t zBytes = 0;

struct Contact_s {
    char name[MAX_BUFFER_SIZE];
    char gender[MAX_BUFFER_SIZE];
    char contact_number[MAX_N_CONTACTS][MAX_BUFFER_SIZE];
    char email[MAX_N_CONTACTS][MAX_BUFFER_SIZE];
} Contact_default = {
    NULL,
    NULL,
    NULL,
    NULL
};

typedef struct Contact_s Contact;

enum Operation {
    Add, 
    Search,
    Edit,
    Delete
};

enum DataField {
    Name,
    Gender,
    ContactNumber,
    Email
};

char* stringToBinary(char* s) {
    if(s == NULL) return 0; /* no input string */
    size_t len = strlen(s);
    char *binary = malloc(len*8 + 1); // each char is one byte (8 bits) and + 1 at the end for null terminator
    binary[0] = '\0';
    for(size_t i = 0; i < len; ++i) {
        char ch = s[i];
        for(int j = 7; j >= 0; --j){
            if(ch & (1 << j)) {
                strcat(binary,"1");
            } else {
                strcat(binary,"0");
            }
        }
    }
    return binary;
}

int getOperationCode(char *operation) {
    if (strcmp(operation, "Add") == 0) {
        return Add;
    } else if (strcmp(operation, "Edit") == 0) {
        return Edit;
    } else if (strcmp(operation, "Search") == 0) {
        return Search;
    } else if (strcmp(operation, "Delete") == 0) {
        return Delete;
    }
}

int getDataField(char *dataField) {
    if (strcmp(dataField, "Name") == 0) {
        return Name;
    } else if (strcmp(dataField, "Gender") == 0) {
        return Gender;
    } else if (strcmp(dataField, "Phone") == 0) {
        return ContactNumber;
    } else if (strcmp(dataField, "Email") == 0) {
        return Email;
    }
}

Contact* searchContact(char *numberToFound, bool printDetails) {
    FILE *fp;

    bool contactFound = false;

    Contact *std_reader = (Contact *) malloc(sizeof(Contact));
    fp = fopen("phonebook.bin", "rb");
    while(fread(std_reader, sizeof(Contact), 1, fp) == 1) {
        if(strcmp(numberToFound, std_reader->contact_number[0]) == 0) {
            contactFound = true;
            if (printDetails) {
                printf("Contact found! Printing details:\n");
                printf(" Name %s  \n Gender: %s\n", 
                std_reader->name, 
                std_reader->contact_number[0], 
                std_reader->gender, 
                std_reader->email);
            }

            printf(" Phone numbers: \n");
            for (int i = 0; i < 10; i++) {
                if (std_reader->contact_number[i][0] == '\0') 
                    break;
                printf("  %s\n", std_reader->contact_number[i]);
            }

            printf(" Emails: \n");
            for (int i = 0; i < 10; i++) {
                if (std_reader->email[i][0] == '\0') 
                    break;
                printf("  %s\n", std_reader->email[i]);
            }
            
            fclose(fp);
            free(std_reader);
            
            return std_reader;
        }
    }

    if (!contactFound) {
        if (printDetails) printf("There was not contact found with that phone number! \n");
        fclose(fp);
        
        return NULL;
    }
  
}

void addContact() {
    Contact *contactToAdd = (Contact *) malloc(sizeof(Contact));
    char *tmpName = NULL, *tmpGender = NULL;
    printf("Please introduce the data of the contact that you want to add:\n");
    printf("Introduce the name: ");
    getline(&tmpName, &zBytes, stdin);
    tmpName[strcspn(tmpName, "\r\n")] = 0;    
    printf("You have entered %s as name.\n", tmpName);
    strcpy(contactToAdd->name, tmpName);

    printf("Introduce the gender (Male/Female): ");
    getline(&tmpGender, &zBytes, stdin);
    tmpGender[strcspn(tmpGender, "\r\n")] = 0;
    printf("\nYou have entered %s as gender.\n", tmpGender);
    strcpy(contactToAdd->gender, tmpGender);

    char *addMoreContacts = NULL;
    int i = 0;

    while (i < 10) {
        char *tmpContactNumber = NULL;
        printf("Introduce the contact number: ");
        getline(&tmpContactNumber, &zBytes, stdin);
        tmpContactNumber[strcspn(tmpContactNumber, "\r\n")] = 0;
        printf("\nYou have entered %s as contact number.\n", tmpContactNumber);
        strcpy(contactToAdd->contact_number[i], tmpContactNumber);
        printf("Do you want to add another contact number (Yes/No): ");
        getline(&addMoreContacts, &zBytes, stdin);
        addMoreContacts[strcspn(addMoreContacts, "\r\n")];
        if (strcmp(addMoreContacts, "No\n") == 0) {
            break;
        }
        i++;
    }

    i = 0;
    char *addMoreEmails = NULL;

    while (i < 10) {
        char *tmpEmail = NULL;
        printf("Introduce the email of the contact: ");
        getline(&tmpEmail, &zBytes, stdin);
        tmpEmail[strcspn(tmpEmail, "\r\n")] = 0;
        printf("\nYou have entered %s as email.\n", tmpEmail);
        strcpy(contactToAdd->email[i], tmpEmail);
        printf("Do you want to add another email (Yes/No): ");
        getline(&addMoreEmails, &zBytes, stdin);
        addMoreEmails[strcspn(addMoreEmails, "\r\n")];
        if (strcmp(addMoreEmails, "No\n") == 0) {
            break;
        }
        i++;
    }

    if (searchContact(contactToAdd->contact_number[0], false) != NULL) {
        printf("Error! A contact with that phone number already exists.\n");
    } else {
        FILE *fp = fopen("phonebook.bin", "ab+");
        fwrite(contactToAdd, sizeof(Contact), 1, fp);
        fclose(fp);
        printf("Contact was successfully added!\n");
    }
}

void editContact() {
    char *numberToFound = NULL, *fieldToUpdate = NULL, *newFieldContent = NULL;

    printf("Introduce the contact number: ");
    getline(&numberToFound, &zBytes, stdin);
    numberToFound[strcspn(numberToFound, "\r\n")] = 0;

    if (searchContact(numberToFound, false) == NULL) {
        printf("Error! The contact number that you want to update does not exist.");
        return;
    }

    printf("Introduce the field that you want to change: (Name/Gender/Phone/Email)\n");
    getline(&fieldToUpdate, &zBytes, stdin);
    fieldToUpdate[strcspn(fieldToUpdate, "\r\n")] = 0;

    printf("Introduce the new value for that field\n");
    getline(&newFieldContent, &zBytes, stdin);
    newFieldContent[strcspn(newFieldContent, "\r\n")] = 0;

    FILE *fp = fopen("phonebook.bin", "r+b");
    Contact *std_reader = (Contact *) malloc(sizeof(Contact));

    while(fread(std_reader, sizeof(Contact), 1, fp) == 1) {
        if(strcmp(numberToFound, std_reader->contact_number[0]) == 0) {
            printf("Contact found! Printing details:\n");
            printf(" Name: %s  \n Phone: %s\n Gender: %s\n Email: %s\n", 
            std_reader->name, 
            std_reader->gender);

            switch (getDataField(fieldToUpdate)) {
                case Name:
                    memset(std_reader->name, 0, sizeof(std_reader->name));
                    strcpy(std_reader->name, newFieldContent);
                    break;
                case Gender:
                    memset(std_reader->gender, 0, sizeof(std_reader->gender));
                    strcpy(std_reader->gender, newFieldContent);
                    break;
                case ContactNumber:
                    memset(std_reader->contact_number[0], 0, sizeof(std_reader->contact_number[0]));
                    strcpy(std_reader->contact_number[0], newFieldContent);
                    break;
                case Email:
                    memset(std_reader->email[0], 0, sizeof(std_reader->email[0]));
                    strcpy(std_reader->email[0], newFieldContent);
                    break;
            }

            fseek(fp, -(unsigned long)sizeof(Contact), SEEK_CUR);
            fwrite(std_reader, sizeof(Contact), 1, fp);
            fflush(fp);
            fclose(fp);

            free(std_reader);
            free(numberToFound);
            free(fieldToUpdate);
            free(newFieldContent);

            printf("Contact was successfully updated!\n");
            
            return;    
        }
    }
}

void deleteContact() {
    char *numberToFound = NULL;

    printf("Introduce the contact number: ");
    getline(&numberToFound, &zBytes, stdin);
    numberToFound[strcspn(numberToFound, "\r\n")] = 0;

    if (searchContact(numberToFound, false) == NULL) {
        printf("Error! The contact number that you want to update does not exist.");
        return;
    }

    FILE *fp = fopen("phonebook.bin", "r+b");
    Contact *std_reader = (Contact *) malloc(sizeof(Contact));

    while(fread(std_reader, sizeof(Contact), 1, fp) == 1) {
        if(strcmp(numberToFound, std_reader->contact_number[0]) == 0) {
            printf("Contact to be deleted found! Printing details:\n");
            printf(" Name %s  \n Phone : %s\n Gender: %s\n Email: %s\n", 
            std_reader->name, 
            std_reader->contact_number, 
            std_reader->gender, 
            std_reader->email);

            char emptyField[1] = {'\0'};

            memset(std_reader->name, 0, sizeof(std_reader->name));
            strcpy(std_reader->name, emptyField);
            memset(std_reader->gender, 0, sizeof(std_reader->gender));
            strcpy(std_reader->gender, emptyField);
            for (int i = 0; i < 10; i++) {
                memset(std_reader->contact_number[i], 0, sizeof(std_reader->contact_number[i]));
                strcpy(std_reader->contact_number[i], emptyField);

                memset(std_reader->email[i], 0, sizeof(std_reader->email[i]));
                strcpy(std_reader->email[i], emptyField);
            }

            fseek(fp, -(unsigned long)sizeof(Contact), SEEK_CUR);
            fwrite(std_reader, sizeof(Contact), 1, fp);
            fflush(fp);
            fclose(fp);
            free(std_reader);
            free(numberToFound);
            
            return;
        }
    }
}

int main() {
    char *operation, *password = NULL;

    FILE *fp = fopen("password.bin", "r");

    if (fp) {
        printf("Hi, to access to this phonebook you need to introduce your password!\n");
        getline(&password, &zBytes, stdin);
        password[strcspn(password, "\r\n")] = 0;

        char *std_reader = (char *) malloc(sizeof(char));

        while(fread(std_reader, sizeof(Contact), 1, fp) == 1) {
            if(strcmp(password, std_reader) == 0) {
                
            }
        }

        fclose(fp);
    } else {
        printf("Please create a password that will be needed to access this phonebook in the future: ");
        getline(&password, &zBytes, stdin);
        password[strcspn(password, "\r\n")] = 0;


    }

    

    

    char *std_reader = (char *) malloc(sizeof(char) * 100);

    if (fread(std_reader, sizeof(Contact), 1, fp) == 1) {
       printf("std reader: %s\n", std_reader);
    }

    

    // printf("Write the operation type that you want to do:\n\t- Add\n\t- Search\n\t- Edit\n\t- Delete\n");   

    // getline(&operation, &zBytes, stdin);
    // operation[strcspn(operation, "\r\n")] = 0;

    // int operationToDo = getOperationCode(operation);

    // char *numberToFound = NULL;

    // switch (operationToDo) {
    //     case Add:
    //         addContact();
    //         break;
    //     case Search:
    //         printf("Introduce the contact number: ");
    //         getline(&numberToFound, &zBytes, stdin);
    //         numberToFound[strcspn(numberToFound, "\r\n")] = 0;
    //         searchContact(numberToFound, true);
    //         break;
    //     case Edit:
    //         editContact();
    //         break;
    //     case Delete:
    //         deleteContact();
    //         break;
    // }

}
