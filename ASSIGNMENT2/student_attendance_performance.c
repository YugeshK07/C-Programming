#include <stdio.h>
#include <stdlib.h>

#define MAX_STUDENTS 100
#define MIN_ATTENDANCE 75
#define MIN_MARK 40

int readInt(const char *prompt) {
    int value, status;
    char ch;
    while (1) {
        printf("%s", prompt);
        status = scanf("%d", &value);
        if (status == 1) {
            while ((ch = getchar()) != '\n' && ch != EOF) { }
            return value;
        }
        if (status == EOF) exit(0);
        printf("Invalid input. Enter an integer value.\n");
        while ((ch = getchar()) != '\n' && ch != EOF) { }
    }
}

int validID(int ids[], int n, int id) {
    int i;
    for (i = 0; i < n; i++)
        if (ids[i] == id) return 0;
    return 1;
}

const char *performance(int mark) {
    if (mark >= 90) return "Outstanding";
    else if (mark >= 80) return "Very Good";
    else if (mark >= 70) return "Good";
    else if (mark >= 50) return "Satisfactory";
    else return "Needs Improvement";
}

void displayRecords(int ids[], int attendance[], int marks[], int n) {
    int i;
    printf("\nID\tAttendance\tMark\tEligibility\tPerformance\tAttendance status\n");
    for (i = 0; i < n; i++) {
        const char *status = attendance[i] < MIN_ATTENDANCE ? "Shortage" :
                             (attendance[i] <= 79 ? "Near threshold" : "Satisfactory");
        printf("%d\t%d%%\t\t%d\t%s\t\t%s\t%s\n", ids[i], attendance[i], marks[i],
               (attendance[i] >= MIN_ATTENDANCE && marks[i] >= MIN_MARK) ? "Eligible" : "Not eligible",
               performance(marks[i]), status);
    }
}

void checkEligibility(int ids[], int attendance[], int marks[], int n) {
    int id = readInt("Enter student ID: "), i;
    for (i = 0; i < n; i++) {
        if (ids[i] == id) {
            if (attendance[i] >= MIN_ATTENDANCE) {
                if (marks[i] >= MIN_MARK)
                    printf("Student %d: Eligible for examination.\n", id);
                else
                    printf("Student %d: Attendance sufficient, but internal mark is below %d.\n", id, MIN_MARK);
            } else {
                printf("Student %d: Attendance shortage (%.0f%% required).\n",
                       id, (double)MIN_ATTENDANCE);
            }
            return;
        }
    }
    printf("Student ID %d not found.\n", id);
}

void statistics(int ids[], int attendance[], int marks[], int n) {
    (void)ids;
    int i, total = 0, high = marks[0], low = marks[0], eligible = 0;
    int above = 0, below = 0;
    double avg;
    for (i = 0; i < n; i++) {
        total += marks[i];
        if (marks[i] > high) high = marks[i];
        if (marks[i] < low) low = marks[i];
        if (attendance[i] >= MIN_ATTENDANCE && marks[i] >= MIN_MARK) eligible++;
    }
    avg = (double)total / n;
    for (i = 0; i < n; i++) {
        if (marks[i] > avg) above++;
        else if (marks[i] < avg) below++;
    }
    printf("\nTotal marks: %d\nAverage marks: %.2f\nHighest mark: %d\nLowest mark: %d\n",
           total, avg, high, low);
    printf("Eligible students: %d\nAbove average: %d\nBelow average: %d\n",
           eligible, above, below);
}

void searchStudent(int ids[], int attendance[], int marks[], int n) {
    int key = readInt("Enter ID to search: "), i;
    for (i = 0; i < n; i++) {
        if (ids[i] == key) {
            printf("Found at position %d: ID=%d, Attendance=%d%%, Mark=%d\n",
                   i + 1, ids[i], attendance[i], marks[i]);
            return;
        }
    }
    printf("Student ID %d not found.\n", key);
}

void meritList(int ids[], int attendance[], int marks[], int n) {
    (void)attendance;
    int idCopy[MAX_STUDENTS], markCopy[MAX_STUDENTS], i, j, temp;
    for (i = 0; i < n; i++) {
        idCopy[i] = ids[i];
        markCopy[i] = marks[i];
    }
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - 1 - i; j++) {
            /* Descending marks; for equal marks, smaller ID comes first. */
            if (markCopy[j] < markCopy[j + 1] ||
                (markCopy[j] == markCopy[j + 1] && idCopy[j] > idCopy[j + 1])) {
                temp = markCopy[j]; markCopy[j] = markCopy[j + 1]; markCopy[j + 1] = temp;
                temp = idCopy[j]; idCopy[j] = idCopy[j + 1]; idCopy[j + 1] = temp;
            }
        }
    }
    printf("\nMERIT LIST (descending marks; ties by ascending ID)\n");
    printf("Rank\tID\tMark\n");
    for (i = 0; i < n; i++)
        printf("%d\t%d\t%d\n", i + 1, idCopy[i], markCopy[i]);
}

int main(void) {
    int ids[MAX_STUDENTS], attendance[MAX_STUDENTS], marks[MAX_STUDENTS];
    int n, i, choice;
    do {
        n = readInt("Enter number of students (1-100): ");
        if (n < 1 || n > MAX_STUDENTS)
            printf("Student count must be between 1 and %d.\n", MAX_STUDENTS);
    } while (n < 1 || n > MAX_STUDENTS);

    for (i = 0; i < n; i++) {
        printf("\nStudent %d\n", i + 1);
        do {
            ids[i] = readInt("Enter unique positive student ID: ");
            if (ids[i] <= 0 || !validID(ids, i, ids[i]))
                printf("ID must be positive and unique.\n");
        } while (ids[i] <= 0 || !validID(ids, i, ids[i]));

        do {
            attendance[i] = readInt("Enter attendance percentage (0-100): ");
            if (attendance[i] < 0 || attendance[i] > 100)
                printf("Attendance must be in the range 0-100.\n");
        } while (attendance[i] < 0 || attendance[i] > 100);

        do {
            marks[i] = readInt("Enter internal mark (0-100): ");
            if (marks[i] < 0 || marks[i] > 100)
                printf("Mark must be in the range 0-100.\n");
        } while (marks[i] < 0 || marks[i] > 100);
    }

    do {
        printf("\n===== STUDENT ATTENDANCE AND PERFORMANCE SYSTEM =====\n");
        printf("1. Display all records\n");
        printf("2. Check one student's eligibility\n");
        printf("3. Display class statistics\n");
        printf("4. Search student by ID\n");
        printf("5. Display merit list\n");
        printf("6. Exit\n");
        choice = readInt("Enter choice: ");
        switch (choice) {
            case 1: displayRecords(ids, attendance, marks, n); break;
            case 2: checkEligibility(ids, attendance, marks, n); break;
            case 3: statistics(ids, attendance, marks, n); break;
            case 4: searchStudent(ids, attendance, marks, n); break;
            case 5: meritList(ids, attendance, marks, n); break;
            case 6: printf("Exiting system. Thank you.\n"); break;
            default: printf("Invalid menu choice. Select 1-6.\n");
        }
    } while (choice != 6);
    return 0;
}
