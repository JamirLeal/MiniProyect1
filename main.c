#include <stdio.h>
#include <stdlib.h>

int getOperationCode(char *operation) {
    if (operation == "Add") {
        return 1;
    } else if (operation == "Edit") {
        return 2;
    } else if (operation == "Search") {
        return 3;
    } else if (operation == "Delete") {
        return 4;
    }
}

int main() {
    FILE *fp = fopen("phonebook.dat", "ab+");

    // add, search, edit, delete
    printf("Write the operation type that you want to do:\nAdd\nSearch\nEdit\nDelete\n");   

    char *operation;

    scanf("%s", operation);

    switch (operation) {
        case "Add":
            continue;
        case ""
    }

}
