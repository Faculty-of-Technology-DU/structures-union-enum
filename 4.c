/*
 * Problem 1 (Extended): Student Records and Academic Analytics System
 *
 * Description:
 *   A menu-driven dynamic student records manager supporting creation,
 *   insertion with automatic realloc growth, search, update, deletion,
 *   GPA analytics (highest, lowest, average), and sorting by GPA descending
 *   (tie-broken by ascending roll number).
 *
 * Sample Run:
 *   Enter initial capacity: 2
 *   1. Add Student -> Roll: 101, Name: "Bruce Wayne", Age: 21, GPA: 3.70
 *   1. Add Student -> Roll: 102, Name: "Selina Kyle", Age: 22, GPA: 3.85
 *   1. Add Student -> Roll: 103, Name: "Clark Kent", Age: 20, GPA: 3.85 (Resized 2 -> 4)
 *   3. Search Roll 102 -> Found: Selina Kyle
 *   4. Update Roll 102 -> Age: 23, GPA: 3.90
 *   5. Delete Roll 101 -> Deleted successfully
 *   6. Display Sorted -> 102 (3.90), 103 (3.85)
 *   7. GPA Summary -> Highest: 102 (3.90), Lowest: 103 (3.85), Average: 3.88
 *   0. Exit -> Memory freed cleanly.
 *
 * Edge Cases Tested:
 *   - Zero, negative, or oversized initial capacity
 *   - Empty collection operations (display, search, update, delete, sort, summary)
 *   - Duplicate roll number rejection
 *   - Invalid age (< 16 or > 100) and invalid GPA (< 0.00 or > 4.00)
 *   - Names with spaces and rejection of names exceeding 49 characters
 *   - Search / update / delete of non-existent roll numbers
 *   - Dynamic capacity growth via realloc
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LEN 50
#define MAX_CAPACITY 10000
#define BUF_SIZE 256

typedef struct {
    int roll_number;
    char name[MAX_NAME_LEN];
    int age;
    float gpa;
} Student;

typedef struct {
    Student *records;
    int count;
    int capacity;
} StudentList;

void clear_extra_input(void) {
    int ch = 0;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

int read_int(const char *prompt, int *out_val) {
    char buf[BUF_SIZE];
    printf("%s", prompt);
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        return -1;
    }

    if (strchr(buf, '\n') == NULL) {
        clear_extra_input();
        return -2;
    }

    char *endptr = NULL;
    long val = strtol(buf, &endptr, 10);
    while (*endptr == ' ' || *endptr == '\t' || *endptr == '\n' || *endptr == '\r') {
        endptr++;
    }

    if (endptr == buf || *endptr != '\0' || val < -1000000000L || val > 1000000000L) {
        return -2;
    }

    *out_val = (int)val;

    return 0;
}

int read_float(const char *prompt, float *out_val) {
    char buf[BUF_SIZE];
    printf("%s", prompt);
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        return -1;
    }

    if (strchr(buf, '\n') == NULL) {
        clear_extra_input();
        return -2;
    }

    char *endptr = NULL;
    float val = strtof(buf, &endptr);
    while (*endptr == ' ' || *endptr == '\t' || *endptr == '\n' || *endptr == '\r') {
        endptr++;
    }

    if (endptr == buf || *endptr != '\0' || !isfinite(val)) {
        return -2;
    }

    *out_val = val;

    return 0;
}

int read_name(const char *prompt, char *out_name, int max_len) {
    char buf[BUF_SIZE];
    printf("%s", prompt);
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        return -1;
    }

    if (strchr(buf, '\n') == NULL) {
        clear_extra_input();
        printf("Error: Input line is too long.\n");
        return -2;
    }

    int len = (int)strcspn(buf, "\r\n");
    buf[len] = '\0';

    if (len == 0) {
        printf("Error: Name cannot be empty.\n");
        return -2;
    }

    if (len >= max_len) {
        printf("Error: Name exceeds %d characters.\n", max_len - 1);
        return -2;
    }

    strcpy(out_name, buf);

    return 0;
}

int init_list(StudentList *list, int cap) {
    if (cap <= 0 || cap > MAX_CAPACITY) {
        return -1;
    }

    list->records = malloc((size_t)cap * sizeof(Student));
    if (list->records == NULL) {
        return -1;
    }

    list->count = 0;
    list->capacity = cap;

    return 0;
}

void free_list(StudentList *list) {
    free(list->records);
    list->records = NULL;
    list->count = 0;
    list->capacity = 0;
}

int find_by_roll(const StudentList *list, int roll) {
    for (int i = 0; i < list->count; i++) {
        if (list->records[i].roll_number == roll) {
            return i;
        }
    }

    return -1;
}

int add_student(StudentList *list, int roll, const char *name, int age, float gpa) {
    if (roll <= 0) {
        printf("Error: Roll number must be positive.\n");
        return -1;
    }

    if (find_by_roll(list, roll) != -1) {
        printf("Error: Roll number %d already exists.\n", roll);
        return -1;
    }

    if (age < 16 || age > 100) {
        printf("Error: Age must be between 16 and 100.\n");
        return -1;
    }

    if (!isfinite(gpa) || gpa < 0.0f || gpa > 4.0f) {
        printf("Error: GPA must be between 0.00 and 4.00.\n");
        return -1;
    }

    if (list->count == list->capacity) {
        int new_cap = list->capacity * 2;
        Student *resized = realloc(list->records, (size_t)new_cap * sizeof(Student));
        if (resized == NULL) {
            printf("Error: Memory reallocation failed.\n");
            return -1;
        }
        list->records = resized;
        list->capacity = new_cap;
        printf("Info: Capacity resized to %d.\n", new_cap);
    }

    Student *s = &list->records[list->count];
    s->roll_number = roll;
    strcpy(s->name, name);
    s->age = age;
    s->gpa = gpa;
    list->count++;

    return 0;
}
int update_student(StudentList *list, int roll, const char *name, int age, float gpa) {
    int idx = find_by_roll(list, roll);
    if (idx == -1) {
        printf("Error: Roll number %d not found.\n", roll);
        return -1;
    }

    if (age < 16 || age > 100) {
        printf("Error: Age must be between 16 and 100.\n");
        return -1;
    }

    if (!isfinite(gpa) || gpa < 0.0f || gpa > 4.0f) {
        printf("Error: GPA must be between 0.00 and 4.00.\n");
        return -1;
    }

    strcpy(list->records[idx].name, name);
    list->records[idx].age = age;
    list->records[idx].gpa = gpa;

    return 0;
}

int delete_student(StudentList *list, int roll) {
    int idx = find_by_roll(list, roll);
    if (idx == -1) {
        printf("Error: Roll number %d not found.\n", roll);
        return -1;
    }

    for (int i = idx; i < list->count - 1; i++) {
        list->records[i] = list->records[i + 1];
    }
    list->count--;

    return 0;
}

void print_student(const Student *s) {
    printf("Roll: %-5d | Name: %-25s | Age: %-3d | GPA: %.2f\n",
           s->roll_number, s->name, s->age, s->gpa);
}

void display_all(const StudentList *list) {
    if (list->count == 0) {
        printf("No student records available.\n");
        return;
    }

    printf("\n--- Student List (%d / %d) ---\n", list->count, list->capacity);
    for (int i = 0; i < list->count; i++) {
        print_student(&list->records[i]);
    }
}

/* Sorts on a temporary copy so original insertion order stays unchanged */
void display_sorted(const StudentList *list) {
    if (list->count == 0) {
        printf("No student records available to sort.\n");
        return;
    }

    Student *copy = malloc((size_t)list->count * sizeof(Student));
    if (copy == NULL) {
        printf("Error: Memory allocation failed for sorting.\n");
        return;
    }

    memcpy(copy, list->records, (size_t)list->count * sizeof(Student));

    for (int i = 0; i < list->count - 1; i++) {
        for (int j = 0; j < list->count - i - 1; j++) {
            int swap = 0;
            if (copy[j].gpa < copy[j + 1].gpa) {
                swap = 1;
            } else if (copy[j].gpa == copy[j + 1].gpa &&
                       copy[j].roll_number > copy[j + 1].roll_number) {
                swap = 1;
            }

            if (swap) {
                Student temp = copy[j];
                copy[j] = copy[j + 1];
                copy[j + 1] = temp;
            }
        }
    }

    printf("\n--- Students Sorted by GPA (Descending) ---\n");
    for (int i = 0; i < list->count; i++) {
        print_student(&copy[i]);
    }

    free(copy);
}

void display_summary(const StudentList *list) {
    if (list->count == 0) {
        printf("Student collection is empty.\n");
        return;
    }

    int high = 0;
    int low = 0;
    float sum = 0.0f;

    for (int i = 0; i < list->count; i++) {
        sum += list->records[i].gpa;
        if (list->records[i].gpa > list->records[high].gpa) {
            high = i;
        }
        if (list->records[i].gpa < list->records[low].gpa) {
            low = i;
        }
    }

    printf("\n--- GPA Summary ---\n");
    printf("Highest GPA : %.2f (%s, Roll %d)\n",
           list->records[high].gpa, list->records[high].name, list->records[high].roll_number);
    printf("Lowest GPA  : %.2f (%s, Roll %d)\n",
           list->records[low].gpa, list->records[low].name, list->records[low].roll_number);
    printf("Average GPA : %.2f\n", sum / (float)list->count);
}

void print_menu(void) {
    printf("\n=== Student Management Menu ===\n");
    printf("1. Add Student\n");
    printf("2. Display All Students\n");
    printf("3. Search by Roll Number\n");
    printf("4. Update Student\n");
    printf("5. Delete Student\n");
    printf("6. Display Sorted by GPA\n");
    printf("7. Display GPA Summary\n");
    printf("0. Exit\n");
}

int main(void) {
    StudentList list = {NULL, 0, 0};
    int cap = 0;

    while (1) {
        int res = read_int("Enter initial capacity (1 - 10000): ", &cap);
        if (res == -1) {
            return 0;
        }
        if (res == 0 && init_list(&list, cap) == 0) {
            break;
        }
        printf("Error: Enter a valid capacity between 1 and %d.\n", MAX_CAPACITY);
    }

    while (1) {
        int choice = -1;
        print_menu();
        int res = read_int("Enter choice: ", &choice);
        if (res == -1) {
            break;
        }
        if (res == -2) {
            printf("Error: Invalid choice.\n");
            continue;
        }

        if (choice == 0) {
            printf("Exiting and freeing memory.\n");
            break;
        }

        switch (choice) {
            case 1: {
                int roll = 0;
                int age = 0;
                float gpa = 0.0f;
                char name[MAX_NAME_LEN];

                if (read_int("Enter roll number: ", &roll) != 0) {
                    printf("Error: Invalid roll number.\n");
                    break;
                }
                if (read_name("Enter name: ", name, MAX_NAME_LEN) != 0) {
                    break;
                }
                if (read_int("Enter age (16 - 100): ", &age) != 0) {
                    printf("Error: Invalid age.\n");
                    break;
                }
                if (read_float("Enter GPA (0.00 - 4.00): ", &gpa) != 0) {
                    printf("Error: Invalid GPA.\n");
                    break;
                }

                if (add_student(&list, roll, name, age, gpa) == 0) {
                    printf("Success: Student %d added.\n", roll);
                }
                break;
            }
            case 2: {
                display_all(&list);
                break;
            }
            case 3: {
                int roll = 0;
                if (read_int("Enter roll number to search: ", &roll) != 0) {
                    printf("Error: Invalid roll number.\n");
                    break;
                }
                int idx = find_by_roll(&list, roll);
                if (idx == -1) {
                    printf("Student with roll %d not found.\n", roll);
                } else {
                    print_student(&list.records[idx]);
                }
                break;
            }
            case 4: {
                int roll = 0;
                int age = 0;
                float gpa = 0.0f;
                char name[MAX_NAME_LEN];

                if (read_int("Enter roll number to update: ", &roll) != 0) {
                    printf("Error: Invalid roll number.\n");
                    break;
                }
                if (find_by_roll(&list, roll) == -1) {
                    printf("Error: Roll number %d not found.\n", roll);
                    break;
                }
                if (read_name("Enter new name: ", name, MAX_NAME_LEN) != 0) {
                    break;
                }
                if (read_int("Enter new age (16 - 100): ", &age) != 0) {
                    printf("Error: Invalid age.\n");
                    break;
                }
                if (read_float("Enter new GPA (0.00 - 4.00): ", &gpa) != 0) {
                    printf("Error: Invalid GPA.\n");
                    break;
                }

                if (update_student(&list, roll, name, age, gpa) == 0) {
                    printf("Success: Student %d updated.\n", roll);
                }
                break;
            }
            case 5: {
                int roll = 0;
                if (read_int("Enter roll number to delete: ", &roll) != 0) {
                    printf("Error: Invalid roll number.\n");
                    break;
                }
                if (delete_student(&list, roll) == 0) {
                    printf("Success: Student %d deleted.\n", roll);
                }
                break;
            }
            case 6: {
                display_sorted(&list);
                break;
            }
            case 7: {
                display_summary(&list);
                break;
            }
            default: {
                printf("Error: Unknown option.\n");
                break;
            }
        }
    }

    free_list(&list);

    return 0;
}
