
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>

#define ID_LEN 16
#define TITLE_LEN 80
#define NAME_LEN 60
#define EMAIL_LEN 60
#define PHONE_LEN 16
#define DATE_LEN 11
#define ADDRESS_LEN 100
#define MAX_INPUT 256

typedef struct {
    char city[40];
    char street[60];
} Address;

typedef struct {
    char book_id[ID_LEN];
    char title[TITLE_LEN];
    char author[NAME_LEN];
    int year;
    int available;
    union {
        unsigned int accession_no;
        char short_code[12];
    } reference;
    int reference_is_number;
} Book;

typedef struct {
    char book_id[ID_LEN];
    char issue_date[DATE_LEN];
    int returned;
} BorrowRecord;

typedef struct {
    char member_id[ID_LEN];
    char name[NAME_LEN];
    char email[EMAIL_LEN];
    char phone[PHONE_LEN];
    Address address;
    BorrowRecord *history;
    size_t history_count;
} Member;

static size_t total_books = 0;
static int operation_count = 0; /* file-scope object: static storage duration, internal linkage */


static int read_line(const char *prompt, char *buffer, size_t capacity) {
    int ch;
    size_t n;
    printf("%s", prompt);
    if (fgets(buffer, (int)capacity, stdin) == NULL) return 0;
    n = strlen(buffer);
    if (n > 0 && buffer[n - 1] == '\n') {
        buffer[n - 1] = '\0';
    } else {
        ch = getchar();
        if (ch != '\n' && ch != EOF) {
            while (ch != '\n' && ch != EOF) ch = getchar();
            fprintf(stderr, "Input too long (maximum %zu characters). Try again.\\n", capacity - 1);
            buffer[0] = '\0';
            return 0;
        }
    }
    return 1;
}

static int parse_positive_int(const char *s, int *value) {
    char *end = NULL;
    long v;
    errno = 0;
    if (s == NULL || *s == '\0') return 0;
    v = strtol(s, &end, 10);
    if (errno != 0 || *end != '\0' || v < 1 || v > 1000) return 0;
    *value = (int)v;
    return 1;
}

static void count_characters(const char *s, int *vowels, int *consonants,
                             int *digits, int *spaces) {
    const char *p = s;
    *vowels = *consonants = *digits = *spaces = 0;
    while (*p != '\0') {
        unsigned char c = (unsigned char)*p;
        if (isalpha(c)) {
            c = (unsigned char)tolower(c);
            if (c=='a'||c=='e'||c=='i'||c=='o'||c=='u') ++*vowels;
            else ++*consonants;
        } else if (isdigit(c)) ++*digits;
        else if (c == ' ') ++*spaces;
        ++p;
    }
}

static void reverse_string(char *s) {
    size_t left = 0, right = strlen(s);
    if (right == 0) return;
    --right;
    while (left < right) {
        char temp = s[left];
        s[left++] = s[right];
        s[right--] = temp;
    }
}

static void print_book(const Book *b) {
    printf("%-6s | %-24s | %-18s | %4d | %s\n",
           b->book_id, b->title, b->author, b->year,
           b->available ? "Available" : "Issued");
}

static int find_book(const Book *books, size_t count, const char *id) {
    size_t i;
    for (i = 0; i < count; ++i)
        if (strcmp(books[i].book_id, id) == 0) return (int)i;
    return -1;
}

static void update_availability(Book *book, int available) {
    if (book != NULL) book->available = available;
}

static int add_books(Book **books, size_t *count, size_t add_count) {
    size_t i;
    Book *temporary;
    if (add_count > (SIZE_MAX / sizeof(Book)) - *count) return 0;
    temporary = realloc(*books, (*count + add_count) * sizeof(Book));
    if (temporary == NULL) return 0;
    *books = temporary;
    for (i = 0; i < add_count; ++i) {
        Book *b = &(*books)[*count + i];
        memset(b, 0, sizeof(*b));
        if (!read_line("Book ID: ", b->book_id, sizeof b->book_id) ||
            strlen(b->book_id) == 0 || find_book(*books, *count + i, b->book_id) >= 0) {
            fprintf(stderr, "Invalid or duplicate book ID; record not accepted.\n");
            return 0;
        }
        if (!read_line("Title: ", b->title, sizeof b->title) || strlen(b->title) == 0 ||
            !read_line("Author: ", b->author, sizeof b->author) || strlen(b->author) == 0) {
            fprintf(stderr, "Title and author must not be empty.\n");
            return 0;
        }
        {
            char year_text[16];
            int year;
            if (!read_line("Publication year: ", year_text, sizeof year_text) ||
                !parse_positive_int(year_text, &year) || year < 1000 || year > 2100) {
                fprintf(stderr, "Invalid publication year.\n");
                return 0;
            }
            b->year = year;
        }
        b->available = 1;
        b->reference_is_number = 1;
        b->reference.accession_no = (unsigned int)(*count + i + 1001);
    }
    *count += add_count;
    total_books = *count;
    return 1;
}

static void list_books(const Book *books, size_t count) {
    size_t i;
    if (count == 0) { puts("No books in inventory."); return; }
    puts("ID     | TITLE                    | AUTHOR             | YEAR | STATUS");
    puts("-------+--------------------------+--------------------+------+-----------");
    for (i = 0; i < count; ++i) print_book(&books[i]);
}

static void show_summary(const Member *m) {
    size_t i;
    if (m == NULL) return;
    printf("Borrowing summary for %s (%s)\n", m->name, m->member_id);
    if (m->history_count == 0) { puts("No borrowing history."); return; }
    for (i = 0; i < m->history_count; ++i)
        printf("%s | Issued: %s | %s\n", m->history[i].book_id,
               m->history[i].issue_date,
               m->history[i].returned ? "Returned" : "Not returned");
}

static int run_string_demo(void) {
    char title[TITLE_LEN] = "The C Programming Language";
    char author[NAME_LEN] = "Kernighan";
    char label[ TITLE_LEN + NAME_LEN + 4 ];
    char sample[] = "Library 2026";
    char *p = sample;
    int v, c, d, s;
    snprintf(label, sizeof label, "%s - %s", title, author);
    printf("Display label: %s\n", label);
    printf("Title length: %zu\n", strlen(title));
    printf("strcmp(B101, B101): %d\n", strcmp("B101", "B101"));
    printf("strcmp(B101, B102): %d (non-zero means different)\n", strcmp("B101", "B102"));
    count_characters(sample, &v, &c, &d, &s);
    printf("Counts: vowels=%d consonants=%d digits=%d spaces=%d\n", v,c,d,s);
    printf("Pointer traversal: ");
    while (*p != '\0') { putchar(*p); ++p; }
    putchar('\n');
    reverse_string(sample);
    printf("Reversed: %s\n", sample);
    {
        struct RefStruct { unsigned int number; char code[12]; };
        union RefUnion { unsigned int number; char code[12]; };
        printf("sizeof(struct RefStruct) = %zu bytes\n", sizeof(struct RefStruct));
        printf("sizeof(union RefUnion) = %zu bytes\n", sizeof(union RefUnion));
    }
    return 0;
}

static void usage(const char *program) {
    printf("Usage:\n");
    printf("  %s list\n", program);
    printf("  %s search <book_id>\n", program);
    printf("  %s summary <member_id>\n", program);
    printf("  %s add <count>   (interactive; records are session-only)\n", program);
    printf("  %s strings\n", program);
    printf("  %s              (interactive menu)\n", program);
}

int main(int argc, char *argv[]) {
    Book *books = NULL;
    Member member;
    Book sample = {"B101", "C Programming Basics", "E. Balagurusamy", 2022, 1,
                   {.accession_no = 5001}, 1};
    int result = 0;
    memset(&member, 0, sizeof member);
    strcpy(member.member_id, "M001");
    strcpy(member.name, "Ananya Rao");
    strcpy(member.email, "ananya@example.edu");
    strcpy(member.phone, "9876543210");
    strcpy(member.address.city, "Chittoor");
    strcpy(member.address.street, "College Road");
    member.history = calloc(1, sizeof(BorrowRecord));
    if (member.history == NULL) {
        fprintf(stderr, "Unable to allocate borrowing history.\n");
        return EXIT_FAILURE;
    }
    strcpy(member.history[0].book_id, "B101");
    strcpy(member.history[0].issue_date, "2026-11-01");
    member.history[0].returned = 0;
    member.history_count = 1;

    if (argc == 1) {
        char choice[16];
        puts("LIBRARY INVENTORY AND MEMBERSHIP SYSTEM");
        puts("1. List books  2. Search B101  3. Borrowing summary  4. String demo  5. Mark B101 issued  6. Exit");
        if (read_line("Choose: ", choice, sizeof choice)) {
            if (strcmp(choice,"1")==0) list_books(&sample,1);
            else if (strcmp(choice,"2")==0) {
                int index = find_book(&sample,1,"B101");
                if (index >= 0) print_book(&sample); else puts("Book not found.");
            } else if (strcmp(choice,"3")==0) show_summary(&member);
            else if (strcmp(choice,"4")==0) run_string_demo();
            else if (strcmp(choice,"5")==0) {
                update_availability(&sample, 0);
                puts("B101 availability updated: Issued");
            } else puts("Exiting.");
        }
    } else if (strcmp(argv[1], "list") == 0 && argc == 2) {
        list_books(&sample, 1);
    } else if (strcmp(argv[1], "search") == 0 && argc == 3) {
        int index = find_book(&sample, 1, argv[2]);
        if (index >= 0) print_book(&sample);
        else { puts("Book not found."); result = 2; }
    } else if (strcmp(argv[1], "summary") == 0 && argc == 3) {
        if (strcmp(argv[2], member.member_id) == 0) show_summary(&member);
        else { puts("Member not found."); result = 2; }
    } else if (strcmp(argv[1], "add") == 0 && argc == 3) {
        int count;
        if (!parse_positive_int(argv[2], &count)) {
            fprintf(stderr, "Count must be an integer from 1 to 1000.\n");
            usage(argv[0]); result = 1;
        } else if (!add_books(&books, &total_books, (size_t)count)) {
            fprintf(stderr, "Book entry failed; check input and available memory.\n");
            result = 1;
        } else list_books(books, total_books);
    } else if (strcmp(argv[1], "strings") == 0 && argc == 2) {
        run_string_demo();
    } else {
        usage(argv[0]);
        result = 1;
    }
    free(books);
    free(member.history);
    books = NULL;
    member.history = NULL;
    ++operation_count;
    return result;
}
