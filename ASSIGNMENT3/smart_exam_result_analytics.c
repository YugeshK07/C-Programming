#include <stdio.h>
#include <string.h>

#define MAX_STUDENTS 10
#define SUBJECTS 3
#define ASSESSMENTS 2
#define MAX_SECTION_RECORDS (2 * MAX_STUDENTS)

typedef struct {
    int id;
    int marks[SUBJECTS];
    int total;
    float average;
} Student;

int readMarks(int marks[][SUBJECTS], int n);
void displayMatrix(int marks[][SUBJECTS], int n);
int calculateTotal(const int marks[], int count);
float calculateAverage(int total, int count);
void calculateStudentResults(int marks[][SUBJECTS], int n,
                             int totals[], float averages[]);
int searchStudent(const int ids[], int n, int key);
void searchMark(int marks[][SUBJECTS], int n, int key);
void subjectToppers(int marks[][SUBJECTS], int n);
void bubbleSortStudents(Student s[], int n);
void displayRanking(const Student s[], int n);
void mergeResults(const Student a[], int na, const Student b[], int nb,
                  Student merged[]);
int recursiveSum(const int a[], int n);
int recursiveMax(const int a[], int n);
void updateValue(int *value);
void calculateByPointer(const int marks[], int count, int *total, float *average);
void printSection(const char *name, const Student s[], int n);

int main(void) {
    int nA, nB, i, j, key, pos;
    int marksA[MAX_STUDENTS][SUBJECTS] = {{0}};
    int marksB[MAX_STUDENTS][SUBJECTS] = {{0}};
    int idsA[MAX_STUDENTS], idsB[MAX_STUDENTS];
    int totalsA[MAX_STUDENTS], totalsB[MAX_STUDENTS];
    float averagesA[MAX_STUDENTS], averagesB[MAX_STUDENTS];
    Student sectionA[MAX_STUDENTS], sectionB[MAX_STUDENTS];
    Student merged[MAX_SECTION_RECORDS];
    int mergedCount;
    int sample[SUBJECTS] = {78, 85, 91};
    int pointerDemo = 10;

    printf("SMART EXAMINATION AND RESULT ANALYTICS SYSTEM\n");
    printf("Subjects: Mathematics, Physics, Programming\n");
    printf("Enter number of students in Section A (1-%d): ", MAX_STUDENTS);
    if (scanf("%d", &nA) != 1 || nA < 1 || nA > MAX_STUDENTS) {
        printf("Invalid Section A size. Program terminated.\n");
        return 1;
    }

    for (i = 0; i < nA; i++) {
        printf("Enter unique ID for Section A student %d: ", i + 1);
        if (scanf("%d", &idsA[i]) != 1) return 1;
        for (j = 0; j < i; j++) {
            if (idsA[j] == idsA[i]) {
                printf("Duplicate student ID. Program terminated.\n");
                return 1;
            }
        }
    }
    printf("Enter marks for Section A (0-100), subject order shown above:\n");
    if (!readMarks(marksA, nA)) {
        printf("Invalid Section A marks. Program terminated.\\n");
        return 1;
    }

    printf("Enter number of students in Section B (0-%d): ", MAX_STUDENTS);
    if (scanf("%d", &nB) != 1 || nB < 0 || nB > MAX_STUDENTS) {
        printf("Invalid Section B size. Program terminated.\n");
        return 1;
    }
    for (i = 0; i < nB; i++) {
        printf("Enter unique ID for Section B student %d: ", i + 1);
        if (scanf("%d", &idsB[i]) != 1) return 1;
        for (j = 0; j < nA; j++) {
            if (idsA[j] == idsB[i]) {
                printf("Duplicate student ID across sections. Program terminated.\n");
                return 1;
            }
        }
        for (j = 0; j < i; j++) {
            if (idsB[j] == idsB[i]) {
                printf("Duplicate student ID. Program terminated.\n");
                return 1;
            }
        }
    }
    printf("Enter marks for Section B (0-100), subject order shown above:\n");
    if (!readMarks(marksB, nB)) {
        printf("Invalid Section B marks. Program terminated.\\n");
        return 1;
    }

    printf("\nSECTION A MARKS\n");
    displayMatrix(marksA, nA);
    calculateStudentResults(marksA, nA, totalsA, averagesA);
    for (i = 0; i < nA; i++) {
        sectionA[i].id = idsA[i];
        sectionA[i].total = totalsA[i];
        sectionA[i].average = averagesA[i];
        for (j = 0; j < SUBJECTS; j++) sectionA[i].marks[j] = marksA[i][j];
    }

    printf("\nSECTION B MARKS\n");
    displayMatrix(marksB, nB);
    calculateStudentResults(marksB, nB, totalsB, averagesB);
    for (i = 0; i < nB; i++) {
        sectionB[i].id = idsB[i];
        sectionB[i].total = totalsB[i];
        sectionB[i].average = averagesB[i];
        for (j = 0; j < SUBJECTS; j++) sectionB[i].marks[j] = marksB[i][j];
    }

    printSection("SECTION A RESULTS", sectionA, nA);
    printSection("SECTION B RESULTS", sectionB, nB);

    printf("\nSearch student ID in Section A: ");
    if (scanf("%d", &key) != 1) return 1;
    pos = searchStudent(idsA, nA, key);
    if (pos >= 0) {
        printf("Student %d found at row %d. Marks: ", key, pos + 1);
        for (j = 0; j < SUBJECTS; j++) printf("%d ", marksA[pos][j]);
        printf("\n");
    } else {
        printf("Student %d not found in Section A.\n", key);
    }

    printf("Search a mark in Section A: ");
    if (scanf("%d", &key) != 1) return 1;
    searchMark(marksA, nA, key);
    subjectToppers(marksA, nA);

    bubbleSortStudents(sectionA, nA);
    bubbleSortStudents(sectionB, nB);
    printf("\nRANKING - SECTION A (descending total; ID ascending for ties)\n");
    displayRanking(sectionA, nA);
    printf("\nRANKING - SECTION B (descending total; ID ascending for ties)\n");
    displayRanking(sectionB, nB);

    mergedCount = nA + nB;
    mergeResults(sectionA, nA, sectionB, nB, merged);
    printf("\nCONSOLIDATED MERIT LIST\n");
    displayRanking(merged, mergedCount);

    printf("\nRECURSIVE SUM of sample {78, 85, 91}: %d\n",
           recursiveSum(sample, SUBJECTS));
    printf("RECURSIVE MAX of sample {78, 85, 91}: %d\n",
           recursiveMax(sample, SUBJECTS));
    printf("Pointer demo: before=%d, ", pointerDemo);
    updateValue(&pointerDemo);
    printf("after=%d\n", pointerDemo);

    return 0;
}

int readMarks(int marks[][SUBJECTS], int n) {
    int i, j, value;
    for (i = 0; i < n; i++) {
        for (j = 0; j < SUBJECTS; j++) {
            printf("Student row %d, subject %d: ", i + 1, j + 1);
            if (scanf("%d", &value) != 1 || value < 0 || value > 100) {
                printf("Invalid mark. Marks must be between 0 and 100.\\n");
                return 0;
            }
            marks[i][j] = value;
        }
    }
    return 1;
}

void displayMatrix(int marks[][SUBJECTS], int n) {
    int i, j;
    printf("Student");
    for (j = 0; j < SUBJECTS; j++) printf("\tS%d", j + 1);
    printf("\n");
    for (i = 0; i < n; i++) {
        printf("%d", i + 1);
        for (j = 0; j < SUBJECTS; j++) printf("\t%d", marks[i][j]);
        printf("\n");
    }
}

int calculateTotal(const int marks[], int count) {
    int i, total = 0;
    for (i = 0; i < count; i++) total += marks[i];
    return total;
}

float calculateAverage(int total, int count) {
    return count > 0 ? (float)total / count : 0.0f;
}

void calculateStudentResults(int marks[][SUBJECTS], int n,
                             int totals[], float averages[]) {
    int i;
    for (i = 0; i < n; i++) {
        totals[i] = calculateTotal(marks[i], SUBJECTS);
        averages[i] = calculateAverage(totals[i], SUBJECTS);
    }
}

int searchStudent(const int ids[], int n, int key) {
    int i;
    for (i = 0; i < n; i++) if (ids[i] == key) return i;
    return -1;
}

void searchMark(int marks[][SUBJECTS], int n, int key) {
    int i, j, found = 0;
    for (i = 0; i < n; i++) {
        for (j = 0; j < SUBJECTS; j++) {
            if (marks[i][j] == key) {
                printf("Mark %d found at student row %d, subject column %d.\n",
                       key, i + 1, j + 1);
                found = 1;
            }
        }
    }
    if (!found) printf("Mark %d not found.\n", key);
}

void subjectToppers(int marks[][SUBJECTS], int n) {
    int j, i, max, winner;
    printf("Subject-wise highest marks:\n");
    for (j = 0; j < SUBJECTS; j++) {
        max = marks[0][j];
        winner = 0;
        for (i = 1; i < n; i++) {
            if (marks[i][j] > max) {
                max = marks[i][j];
                winner = i;
            }
        }
        printf("Subject %d: %d (student row %d)\n", j + 1, max, winner + 1);
    }
}

void bubbleSortStudents(Student s[], int n) {
    int i, j;
    Student temp;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (s[j].total < s[j + 1].total ||
                (s[j].total == s[j + 1].total && s[j].id > s[j + 1].id)) {
                temp = s[j];
                s[j] = s[j + 1];
                s[j + 1] = temp;
            }
        }
    }
}

void displayRanking(const Student s[], int n) {
    int i;
    printf("Rank\tID\tTotal\tAverage\n");
    for (i = 0; i < n; i++)
        printf("%d\t%d\t%d\t%.2f\n", i + 1, s[i].id, s[i].total, s[i].average);
}

void mergeResults(const Student a[], int na, const Student b[], int nb,
                  Student merged[]) {
    int i = 0, j = 0, k = 0;
    while (i < na && j < nb) {
        if (a[i].total > b[j].total ||
            (a[i].total == b[j].total && a[i].id < b[j].id))
            merged[k++] = a[i++];
        else
            merged[k++] = b[j++];
    }
    while (i < na) merged[k++] = a[i++];
    while (j < nb) merged[k++] = b[j++];
}

int recursiveSum(const int a[], int n) {
    if (n <= 0) return 0;
    return a[n - 1] + recursiveSum(a, n - 1);
}

int recursiveMax(const int a[], int n) {
    int previousMax;
    if (n == 1) return a[0];
    previousMax = recursiveMax(a, n - 1);
    return a[n - 1] > previousMax ? a[n - 1] : previousMax;
}

void updateValue(int *value) {
    if (value != NULL) *value = *value + 5;
}

void calculateByPointer(const int marks[], int count, int *total, float *average) {
    *total = calculateTotal(marks, count);
    *average = calculateAverage(*total, count);
}

void printSection(const char *name, const Student s[], int n) {
    printf("\n%s\n", name);
    displayRanking(s, n);
}
