#include <stdio.h>
#include <string.h>

#define MAX 100

struct Patient {
    char id[10];
    char name[50];
    int age;
    char symptoms[100];
    char doctor[50];
    int priority;
    char status[30];
    int discharged;
};

struct Patient patients[MAX];

int count = 0;
int nextID = 101;

/* Normal queue */
int normalQueue[MAX];
int normalFront = 0, normalRear = 0;

/* Emergency priority queue */
int emergencyQueue[MAX];
int emergencyCount = 0;

/* Recent actions stack */
char history[MAX][100];
int top = -1;

/* Save recent action */
void addHistory(char action[]) {
    if (top < MAX - 1) {
        top++;
        strcpy(history[top], action);
    }
}

/* Convert priority number to name */
char *priorityName(int p) {
    if (p == 1) return "Critical";
    if (p == 2) return "Serious";
    if (p == 3) return "Moderate";
    return "Normal";
}

/* Search patient by ID */
int searchPatient(char id[]) {
    int i;

    for (i = 0; i < count; i++) {
        if (strcmp(patients[i].id, id) == 0)
            return i;
    }

    return -1;
}

/* Register patient */
void registerPatient() {
    if (count >= MAX) {
        printf("Patient records are full!\n");
        return;
    }

    int i = count;

    sprintf(patients[i].id, "P%d", nextID++);

    printf("Enter patient name: ");
    scanf(" %49[^\n]", patients[i].name);

    printf("Enter age: ");
    scanf("%d", &patients[i].age);

    printf("Enter symptoms: ");
    scanf(" %99[^\n]", patients[i].symptoms);

    printf("Enter doctor name: ");
    scanf(" %49[^\n]", patients[i].doctor);

    printf("Priority (1=Critical, 2=Serious, "
           "3=Moderate, 4=Normal): ");
    scanf("%d", &patients[i].priority);

    if (patients[i].priority < 1 ||
        patients[i].priority > 4) {
        printf("Invalid priority. Registration cancelled.\n");
        return;
    }

    strcpy(patients[i].status, "Registered");
    patients[i].discharged = 0;

    /* Add patient to the appropriate queue */
    if (patients[i].priority == 4) {
        normalQueue[normalRear++] = i;
    } else {
        emergencyQueue[emergencyCount++] = i;
    }

    count++;

    char action[100];
    sprintf(action, "Registered patient %s",
            patients[i].id);
    addHistory(action);

    printf("\nPatient registered successfully!\n");
    printf("Patient ID: %s\n", patients[i].id);
}

/* Display one patient */
void displayPatient(int i) {
    printf("\nPatient ID: %s\n", patients[i].id);
    printf("Name: %s\n", patients[i].name);
    printf("Age: %d\n", patients[i].age);
    printf("Symptoms: %s\n", patients[i].symptoms);
    printf("Doctor: %s\n", patients[i].doctor);
    printf("Priority: %s\n",
           priorityName(patients[i].priority));
    printf("Status: %s\n", patients[i].status);
    printf("Discharge: %s\n",
           patients[i].discharged ? "Yes" : "No");
}

/* Search and display */
void searchMenu() {
    char id[10];

    printf("Enter patient ID: ");
    scanf("%9s", id);

    int i = searchPatient(id);

    if (i == -1)
        printf("Patient not found!\n");
    else
        displayPatient(i);
}

/* Display all patients */
void displayAll() {
    if (count == 0) {
        printf("No patient records available.\n");
        return;
    }

    int i;

    for (i = 0; i < count; i++)
        displayPatient(i);
}

/* Display waiting queues */
void displayQueues() {
    int i, index;

    printf("\n--- Emergency Queue ---\n");

    for (int p = 1; p <= 3; p++) {
        for (i = 0; i < emergencyCount; i++) {
            index = emergencyQueue[i];

            if (patients[index].priority == p &&
                !patients[index].discharged &&
                strcmp(patients[index].status,
                       "Registered") == 0) {
                printf("%s - %s (%s)\n",
                       patients[index].id,
                       patients[index].name,
                       priorityName(p));
            }
        }
    }

    printf("\n--- Normal Queue (FIFO) ---\n");

    for (i = normalFront; i < normalRear; i++) {
        index = normalQueue[i];

        if (!patients[index].discharged &&
            strcmp(patients[index].status,
                   "Registered") == 0) {
            printf("%s - %s\n",
                   patients[index].id,
                   patients[index].name);
        }
    }
}

/* Serve next patient */
void servePatient() {
    int i, selected = -1;

    /* Emergency patients get priority.
       Lower number means higher priority. */
    for (int p = 1; p <= 3 && selected == -1; p++) {
        for (i = 0; i < emergencyCount; i++) {
            int index = emergencyQueue[i];

            if (patients[index].priority == p &&
                !patients[index].discharged &&
                strcmp(patients[index].status,
                       "Registered") == 0) {
                selected = index;
                break;
            }
        }
    }

    /* If no emergency is waiting, serve normal FIFO */
    if (selected == -1) {
        while (normalFront < normalRear) {
            int index = normalQueue[normalFront++];

            if (!patients[index].discharged &&
                strcmp(patients[index].status,
                       "Registered") == 0) {
                selected = index;
                break;
            }
        }
    }

    if (selected == -1) {
        printf("No patients are waiting.\n");
        return;
    }

    strcpy(patients[selected].status, "In Consultation");

    printf("Now serving: %s - %s\n",
           patients[selected].id,
           patients[selected].name);

    char action[100];
    sprintf(action, "Consultation started for %s",
            patients[selected].id);
    addHistory(action);
}

/* Update patient */
void updatePatient() {
    char id[10];

    printf("Enter patient ID: ");
    scanf("%9s", id);

    int i = searchPatient(id);

    if (i == -1) {
        printf("Patient not found!\n");
        return;
    }

    printf("1. Update symptoms\n");
    printf("2. Update doctor\n");
    printf("3. Update treatment status\n");

    int choice;
    printf("Enter choice: ");
    scanf("%d", &choice);

    if (choice == 1) {
        printf("Enter new symptoms: ");
        scanf(" %99[^\n]", patients[i].symptoms);
    }
    else if (choice == 2) {
        printf("Enter new doctor: ");
        scanf(" %49[^\n]", patients[i].doctor);
    }
    else if (choice == 3) {
        printf("1. Registered\n");
        printf("2. In Consultation\n");
        printf("3. Treatment Complete\n");

        int s;
        printf("Enter status: ");
        scanf("%d", &s);

        if (s == 1)
            strcpy(patients[i].status, "Registered");
        else if (s == 2)
            strcpy(patients[i].status, "In Consultation");
        else if (s == 3)
            strcpy(patients[i].status, "Treatment Complete");
        else {
            printf("Invalid status!\n");
            return;
        }
    }
    else {
        printf("Invalid choice!\n");
        return;
    }

    addHistory("Patient record updated");
    printf("Record updated successfully.\n");
}

/* Discharge patient */
void dischargePatient() {
    char id[10];

    printf("Enter patient ID: ");
    scanf("%9s", id);

    int i = searchPatient(id);

    if (i == -1) {
        printf("Patient not found!\n");
        return;
    }

    if (patients[i].discharged) {
        printf("Patient already discharged.\n");
        return;
    }

    patients[i].discharged = 1;
    strcpy(patients[i].status, "Discharged");

    addHistory("Patient discharged");

    printf("Patient discharged successfully.\n");
}

/* Display action history */
void displayHistory() {
    if (top == -1) {
        printf("No actions recorded.\n");
        return;
    }

    printf("\n--- Recent Actions ---\n");

    for (int i = top; i >= 0; i--)
        printf("%s\n", history[i]);
}

/* Main program */
int main() {
    int choice;

    do {
        printf("\n===== HOSPITAL PATIENT MANAGEMENT SYSTEM =====\n");
        printf("1. Register Patient\n");
        printf("2. Search Patient\n");
        printf("3. Display All Patients\n");
        printf("4. View Waiting Queues\n");
        printf("5. Serve Next Patient\n");
        printf("6. Update Patient\n");
        printf("7. Discharge Patient\n");
        printf("8. View Action History\n");
        printf("0. Exit\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: registerPatient(); break;
            case 2: searchMenu(); break;
            case 3: displayAll(); break;
            case 4: displayQueues(); break;
            case 5: servePatient(); break;
            case 6: updatePatient(); break;
            case 7: dischargePatient(); break;
            case 8: displayHistory(); break;
            case 0: printf("Exiting system...\n"); break;
            default: printf("Invalid choice!\n");
        }

    } while (choice != 0);

    return 0;
}