#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_FILE   "students.dat"
#define TEMP_FILE   "students.tmp"
#define BACKUP_FILE "students.bak"
#define NAME_LEN    50
#define DEPT_LEN    50
#define INPUT_LEN   128

typedef struct {
    int id;
    char name[NAME_LEN];
    int age;
    char department[DEPT_LEN];
    double cgpa;
} Student;

static void trim(char *s)
{
    char *start = s;
    size_t n;

    while (isspace((unsigned char)*start)) start++;
    if (start != s) memmove(s, start, strlen(start) + 1);

    n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1]))
        s[--n] = '\0';
}

static int readLine(const char *prompt, char *buf, size_t size)
{
    for (;;) {
        char *newline;
        int ch;
        size_t n;

        printf("%s", prompt);
        fflush(stdout);

        if (fgets(buf, (int)size, stdin) == NULL)
            return 0;

        newline = strchr(buf, '\n');

        if (newline != NULL) {
            *newline = '\0';
        } else {
            ch = getchar();

            if (ch != '\n' && ch != EOF) {
                while (ch != '\n' && ch != EOF)
                    ch = getchar();

                puts("Input too long. Please try again.");
                continue;
            }
        }

        n = strlen(buf);
        if (n > 0 && buf[n - 1] == '\r')
            buf[n - 1] = '\0';

        trim(buf);
        return 1;
    }
}

static int readInt(const char *prompt, int *out)
{
    char line[INPUT_LEN], *end;
    long value;

    for (;;) {
        if (!readLine(prompt, line, sizeof line))
            return 0;

        errno = 0;
        value = strtol(line, &end, 10);

        if (line[0] == '\0' || end == line || *end != '\0' ||
            errno == ERANGE || value < INT_MIN || value > INT_MAX) {
            puts("Please enter a valid whole number.");
            continue;
        }

        *out = (int)value;
        return 1;
    }
}

static int readIntRange(const char *prompt, int min, int max, int *out)
{
    int value;

    for (;;) {
        if (!readInt(prompt, &value))
            return 0;

        if (value < min || value > max) {
            printf("Enter a value from %d to %d.\n", min, max);
            continue;
        }

        *out = value;
        return 1;
    }
}

static int readDoubleRange(const char *prompt, double min,
                           double max, double *out)
{
    char line[INPUT_LEN], *end;
    double value;

    for (;;) {
        if (!readLine(prompt, line, sizeof line))
            return 0;

        errno = 0;
        value = strtod(line, &end);

        if (line[0] == '\0' || end == line || *end != '\0' ||
            errno == ERANGE || !isfinite(value)) {
            puts("Please enter a valid numeric value.");
            continue;
        }

        if (value < min || value > max) {
            printf("Enter a value from %.2f to %.2f.\n", min, max);
            continue;
        }

        *out = value;
        return 1;
    }
}

static int readText(const char *prompt, char *out, size_t size)
{
    for (;;) {
        if (!readLine(prompt, out, size))
            return 0;

        if (out[0] == '\0') {
            puts("This field cannot be empty.");
            continue;
        }

        return 1;
    }
}

static int readYesNo(const char *prompt, int *answer)
{
    char line[INPUT_LEN];

    for (;;) {
        if (!readLine(prompt, line, sizeof line))
            return 0;

        if ((line[0] == 'y' || line[0] == 'Y') &&
            line[1] == '\0') {
            *answer = 1;
            return 1;
        }

        if ((line[0] == 'n' || line[0] == 'N') &&
            line[1] == '\0') {
            *answer = 0;
            return 1;
        }

        puts("Please enter Y or N.");
    }
}

/* 1 = record read, 0 = clean end of file,
   -1 = read error or corrupted record. */
static int readRecord(FILE *fp, Student *s)
{
    size_t n = fread(s, 1, sizeof *s, fp);

    if (n == sizeof *s)
        return 1;

    if (n == 0 && feof(fp) && !ferror(fp))
        return 0;

    if (ferror(fp))
        perror("Error reading student database");
    else
        puts("Error: student database contains an incomplete record.");

    return -1;
}

static int writeRecord(FILE *fp, const Student *s)
{
    if (fwrite(s, sizeof *s, 1, fp) != 1) {
        perror("Error writing student database");
        return 0;
    }

    return 1;
}

/* Returns 1 when found, 0 when not found,
   and -1 on a file error. */
static int findStudent(int id, Student *result)
{
    FILE *fp = fopen(DATA_FILE, "rb");
    Student current;
    int status, found = 0;

    if (fp == NULL) {
        perror("Could not open student database");
        return -1;
    }

    while ((status = readRecord(fp, &current)) == 1) {
        if (current.id == id) {
            *result = current;
            found = 1;
            break;
        }
    }

    if (status == -1)
        found = -1;

    if (fclose(fp) != 0) {
        perror("Could not close student database");
        return -1;
    }

    return found;
}

static void printStudent(const Student *s)
{
    printf("\n------ Student Record ------\n");
    printf("ID         : %d\n", s->id);
    printf("Name       : %s\n", s->name);
    printf("Age        : %d\n", s->age);
    printf("Department : %s\n", s->department);
    printf("CGPA       : %.2f / 4.00\n", s->cgpa);
}

static int inputDetails(Student *s)
{
    return readText("Enter student name: ",
                    s->name, sizeof s->name) &&
           readIntRange("Enter age (1-120): ",
                        1, 120, &s->age) &&
           readText("Enter department: ",
                    s->department, sizeof s->department) &&
           readDoubleRange("Enter CGPA (0.00-4.00): ",
                           0.0, 4.0, &s->cgpa);
}

static int displayAll(void)
{
    FILE *fp = fopen(DATA_FILE, "rb");
    Student s;
    int status, count = 0;

    if (fp == NULL) {
        perror("Could not open student database");
        return 1;
    }

    puts("\n====================== STUDENT RECORDS ======================");
    printf("%-8s %-25s %-6s %-22s %s\n",
           "ID", "NAME", "AGE", "DEPARTMENT", "CGPA");
    puts("-------------------------------------------------------------");

    while ((status = readRecord(fp, &s)) == 1) {
        printf("%-8d %-25.25s %-6d %-22.22s %.2f\n",
               s.id, s.name, s.age, s.department, s.cgpa);
        count++;
    }

    if (fclose(fp) != 0) {
        perror("Could not close student database");
        return 1;
    }

    if (status == -1)
        return 1;

    if (count == 0)
        puts("No student records found.");
    else
        printf("Total records: %d\n", count);

    return 1;
}

/* Keep a backup until the replacement file is installed. */
static int commitTempFile(void)
{
    if (remove(BACKUP_FILE) != 0 && errno != ENOENT) {
        perror("Could not remove old backup");
        return 0;
    }

    if (rename(DATA_FILE, BACKUP_FILE) != 0) {
        perror("Could not back up student database");
        return 0;
    }

    if (rename(TEMP_FILE, DATA_FILE) != 0) {
        perror("Could not install updated database");

        if (rename(BACKUP_FILE, DATA_FILE) != 0)
            perror("CRITICAL: could not restore database backup");

        return 0;
    }

    if (remove(BACKUP_FILE) != 0 && errno != ENOENT)
        puts("Warning: updated database, but could not remove backup file.");

    return 1;
}

/* update=1 replaces a record; update=0 deletes it. */
static int rewriteDatabase(int id, const Student *replacement,
                           int update)
{
    FILE *source = fopen(DATA_FILE, "rb");
    FILE *temp;
    Student current;
    int status = 0, found = 0, ok = 1;

    if (source == NULL) {
        perror("Could not open student database");
        return -1;
    }

    temp = fopen(TEMP_FILE, "wb");
    if (temp == NULL) {
        perror("Could not create temporary database");
        fclose(source);
        return -1;
    }

    while (ok && (status = readRecord(source, &current)) == 1) {
        if (current.id == id) {
            found = 1;

            if (update && !writeRecord(temp, replacement))
                ok = 0;
        } else if (!writeRecord(temp, &current)) {
            ok = 0;
        }
    }

    if (status == -1)
        ok = 0;

    if (fclose(source) != 0) {
        perror("Could not close original database");
        ok = 0;
    }

    if (fclose(temp) != 0) {
        perror("Could not close temporary database");
        ok = 0;
    }

    if (!ok) {
        remove(TEMP_FILE);
        return -1;
    }

    if (!found) {
        remove(TEMP_FILE);
        return 0;
    }

    if (!commitTempFile()) {
        remove(TEMP_FILE);
        return -1;
    }

    return 1;
}

static int addStudent(void)
{
    Student s;
    FILE *fp;
    int status;

    if (!readIntRange("Enter student ID (positive number): ",
                      1, INT_MAX, &s.id))
        return 0;

    status = findStudent(s.id, &s);

    if (status == 1) {
        puts("That student ID already exists.");
        return 1;
    }

    if (status == -1)
        return 1;

    if (!inputDetails(&s))
        return 0;

    fp = fopen(DATA_FILE, "ab");
    if (fp == NULL) {
        perror("Could not open student database for appending");
        return 1;
    }

    status = writeRecord(fp, &s);

    if (fclose(fp) != 0) {
        perror("Could not close student database");
        return 1;
    }

    if (status)
        puts("Student added successfully.");

    return 1;
}

static int searchStudent(void)
{
    int id, status;
    Student s;

    if (!readIntRange("Enter student ID to search: ",
                      1, INT_MAX, &id))
        return 0;

    status = findStudent(id, &s);

    if (status == 1)
        printStudent(&s);
    else if (status == 0)
        puts("Student not found.");

    return 1;
}

static int updateStudent(void)
{
    int id, status;
    Student s;

    if (!readIntRange("Enter student ID to update: ",
                      1, INT_MAX, &id))
        return 0;

    status = findStudent(id, &s);

    if (status == -1)
        return 1;

    if (status == 0) {
        puts("Student not found.");
        return 1;
    }

    printStudent(&s);
    puts("Enter the new details. Student ID will remain unchanged.");

    if (!inputDetails(&s))
        return 0;

    status = rewriteDatabase(id, &s, 1);

    if (status == 1)
        puts("Student updated successfully.");
    else if (status == 0)
        puts("Student not found during update.");

    return 1;
}

static int deleteStudent(void)
{
    int id, status, confirm;
    Student s;

    if (!readIntRange("Enter student ID to delete: ",
                      1, INT_MAX, &id))
        return 0;

    status = findStudent(id, &s);

    if (status == -1)
        return 1;

    if (status == 0) {
        puts("Student not found.");
        return 1;
    }

    printStudent(&s);

    if (!readYesNo("Delete this record? (Y/N): ", &confirm))
        return 0;

    if (!confirm) {
        puts("Deletion cancelled.");
        return 1;
    }

    status = rewriteDatabase(id, NULL, 0);

    if (status == 1)
        puts("Student deleted successfully.");
    else if (status == 0)
        puts("Student not found during deletion.");

    return 1;
}

static int initializeDatabase(void)
{
    FILE *fp = fopen(DATA_FILE, "rb");

    if (fp != NULL) {
        if (fclose(fp) != 0) {
            perror("Could not close student database");
            return 0;
        }

        if (remove(BACKUP_FILE) != 0 && errno != ENOENT)
            puts("Warning: old backup file could not be removed.");

        if (remove(TEMP_FILE) != 0 && errno != ENOENT)
            puts("Warning: old temporary file could not be removed.");

        return 1;
    }

    if (errno != ENOENT) {
        perror("Could not access student database");
        return 0;
    }

    if (rename(BACKUP_FILE, DATA_FILE) == 0) {
        puts("Recovered student records from backup.");
        remove(TEMP_FILE);
        return 1;
    }

    if (errno != ENOENT) {
        perror("Could not recover backup file");
        return 0;
    }

    fp = fopen(DATA_FILE, "ab");
    if (fp == NULL) {
        perror("Could not create student database");
        return 0;
    }

    if (fclose(fp) != 0) {
        perror("Could not close student database");
        return 0;
    }

    return 1;
}

int main(void)
{
    int choice, running = 1;

    if (!initializeDatabase())
        return EXIT_FAILURE;

    puts("\n===== CodeAlpha Student Management System =====");

    while (running) {
        puts("\n============== MAIN MENU ==============");
        puts("1. Add Student");
        puts("2. Delete Student");
        puts("3. Update Student");
        puts("4. Search Student");
        puts("5. Display All Students");
        puts("0. Exit");

        if (!readInt("Choose an option: ", &choice))
            break;

        switch (choice) {
            case 1:
                running = addStudent();
                break;

            case 2:
                running = deleteStudent();
                break;

            case 3:
                running = updateStudent();
                break;

            case 4:
                running = searchStudent();
                break;

            case 5:
                (void)displayAll();
                break;

            case 0:
                running = 0;
                puts("Goodbye!");
                break;

            default:
                puts("Invalid option. Choose a number from 0 to 5.");
        }
    }

    return EXIT_SUCCESS;
}
