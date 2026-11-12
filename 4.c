// Structures - 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_CAPACITY 10000

typedef struct {
    int roll_no;
    char name[50];
    int age;
    float gpa;
} Student;

int search_student(Student *students, int size, int roll_no);
int expand_student_list(Student **students, int *capacity);

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

//student* series of recored
Student* create_student_list(int n) {
    Student *studs = (Student*) malloc(n * sizeof(Student));
    return studs;
}



//check wheather the studnet is has valid roll no and unique
void add_student(Student **studs, int *count, int *capacity) {

    if (*count >= *capacity) {
        if (!expand_student_list(studs, capacity)) {
            return;
        }
    }

    int roll_no, age;
    float gpa;
    char name[52];

    printf("Enter roll number: ");
    if (scanf("%d", &roll_no) != 1) {
        printf("Invalid roll number!\n");
        clear_input_buffer();
        return;
    }

 
    if (roll_no <= 0) {
        printf("Invalid roll number!\n");
        return;
    }

    if (search_student(*studs, *count, roll_no) != -1) {
        printf("Roll number already exists!\n");
        return;
    }

    printf("Enter name: ");
    clear_input_buffer();  // Remove leftover newline
    if (fgets(name, sizeof(name), stdin) == NULL) {
        printf("Error reading name!\n");
        return;
    }

    if (strchr(name, '\n') == NULL) {
        printf("Name exceeds storage limit (max 49 characters)!\n");
        clear_input_buffer();
        return;
    }
//replacd index with 0
    name[strcspn(name, "\n")] = '\0';

    if (strlen(name) == 0) {
        printf("Name cannot be empty!\n");
        return;
    }

    printf("Enter age: ");
    if (scanf("%d", &age) != 1 || age < 16 || age > 100) {
        printf("Invalid age!\n");
        clear_input_buffer();
        return;
    }

    printf("Enter GPA: ");
    if (scanf("%f", &gpa) != 1 || gpa < 0.0f || gpa > 4.0f) {
        printf("Invalid GPA!\n");
        clear_input_buffer();
        return;
    }

    // Store student details
    (*studs)[*count].roll_no = roll_no;
    strcpy((*studs)[*count].name, name);
    (*studs)[*count].age = age;
    (*studs)[*count].gpa = gpa;

    (*count)++;

    printf("Student added successfully\n");
} 



//updating student in list

void update_student(Student *studs, int count) {

    int roll_no;
    int age;
    float gpa;
    char name[52];

    printf("Enter roll number to update: ");
    if (scanf("%d", &roll_no) != 1) {
        printf("Invalid roll number!\n");
        clear_input_buffer();
        return;
    }

    // Find student
    int index = search_student(studs, count, roll_no);

    if (index == -1) {
        printf("Student not found!\n");
        return;
    }

    printf("Enter new name: ");
    clear_input_buffer();
    if (fgets(name, sizeof(name), stdin) == NULL) {
        printf("Error reading name!\n");
        return;
    }

    if (strchr(name, '\n') == NULL) {
        printf("Name exceeds storage limit (max 49 characters)!\n");
        clear_input_buffer();
        return;
    }

    name[strcspn(name, "\n")] = '\0';

    if (strlen(name) == 0) {
        printf("Name cannot be empty!\n");
        return;
    }

    printf("Enter new age: ");
    if (scanf("%d", &age) != 1 || age < 16 || age > 100) {
        printf("Invalid age!\n");
        clear_input_buffer();
        return;
    }

    printf("Enter new GPA: ");
    if (scanf("%f", &gpa) != 1 || gpa < 0.0f || gpa > 4.0f) {
        printf("Invalid GPA!\n");
        clear_input_buffer();
        return;
    }

    // Update existing record
    strcpy(studs[index].name, name);
    studs[index].age = age;
    studs[index].gpa = gpa;

    printf("Student update successful \n");
}



//deleting student records from the list

void delete_student(Student *studs, int *count) {

    int roll_no;

    printf("Enter roll number to delete: ");
    if (scanf("%d", &roll_no) != 1) {
        printf("Invalid roll number!\n");
        clear_input_buffer();
        return;
    }

    // Search for student
    int index = search_student(studs, *count, roll_no);

    if (index == -1) {
        printf("Student not found!\n");
        return;
    }

    // Shift remaining records to the left
    for (int i = index; i < *count - 1; i++) {

        studs[i] = studs[i + 1];

    }

    
    (*count)--;

    printf("Student deleted successfully!\n");
}



//for expanding student list we are using realloc



int expand_student_list(Student **students, int *capacity) {
    if (*capacity < 0 || *capacity >= MAX_CAPACITY) {
        printf("Maximum capacity reached!\n");
        return 0;
    }

    int new_capacity;
    if (*capacity == 0) {
        new_capacity = 1;
    } else if (*capacity > MAX_CAPACITY / 2) {
        new_capacity = MAX_CAPACITY;
    } else {
        new_capacity = *capacity * 2;
    }

    Student *temp = realloc(*students, new_capacity * sizeof(Student));
    if (temp == NULL) {
        printf("Memory expansion failed!\n");
        return 0;
    }

    *students = temp;
    *capacity = new_capacity;
    printf("Memory expanded to %d.\n", *capacity);

    return 1;
}


//for finding highest lowest average gpa
void gpa_statistics(Student *students, int count) {
    if (count == 0) {
        printf("No student records available!\n");
        return;
    }

    float highest = students[0].gpa;
    float lowest = students[0].gpa;
    float total = 0;

    for (int i = 0; i < count; i++) {

        if (students[i].gpa > highest) {
            highest = students[i].gpa;
        }

        if (students[i].gpa < lowest) {
            lowest = students[i].gpa;
        }

        total = total + students[i].gpa;
    }

    printf("Highest GPA: %.2f\n", highest);
    printf("Lowest GPA: %.2f\n", lowest);
    printf("Average GPA: %.2f\n", total / count);
}
//for sorting our list
void sort_students(Student *students, int count) {

    for (int i = 0; i < count - 1; i++) {

        for (int j = 0; j < count - i - 1; j++) {

            if ((students[j].gpa < students[j + 1].gpa )||(students[j].gpa == students[j + 1].gpa && students[j].roll_no > students[j + 1].roll_no)) {

                Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }

    printf("Students sorted successfully!\n");
}


void display_students(Student *students, int size) {
    if (size == 0) {
        printf("No student records to display.\n");
        return;
    }
    for (int i = 0; i < size; i++) {
        printf("Roll No: %d, Name: %s, Age: %d, GPA: %.2f\n", students[i].roll_no, students[i].name, students[i].age, students[i].gpa);
    }
}




void free_student_list(Student *students) {
    free(students);
}



//for searching the student 
//if roll no not found return -1
//if roll no found it will retun the index
int search_student(Student *students, int size, int roll_no) {

    for (int i = 0; i < size; i++) {

        if (students[i].roll_no == roll_no) {
            return i;
        }
    }

    return -1;
}




int main() {

    int capacity;
    int count = 0;
    int choice;

    printf("Enter no student u want to fit in list  total capcity of memory: ");

    if (scanf("%d", &capacity) != 1 || capacity <= 0 || capacity > MAX_CAPACITY) {
        printf("Invalid capacity!\n");
        return 1;
    }

    Student *student_list = create_student_list(capacity);

    if (student_list == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    while (1) {

        printf(" STUDENT MANAGEMENT SYSTEM \n");
        printf("1. Add Student\n");
        printf("2. Display Students\n");
        printf("3. Search Student\n");
        printf("4. Update Student\n");
        printf("5. Delete Student\n");
        printf("6. GPA Statistics\n");
        printf("7. Sort Students\n");
        printf("8. Exit\n");

        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            clear_input_buffer();
            continue;
        }

        switch (choice) {

            case 1:
                add_student(&student_list, &count, &capacity);
                break;

            case 2:
                display_students(student_list, count);
                break;

            case 3: {
                int roll_no;

                printf("Enter roll number: ");

                if (scanf("%d", &roll_no) != 1) {
                    printf("Invalid roll number!\n");
                    clear_input_buffer();
                    break;
                }

                int index = search_student(student_list, count, roll_no);

                if (index == -1) {
                    printf("Student not found!\n");
                }
                else {
                    printf("\nStudent found!\n");
                    printf("Roll No: %d\n", student_list[index].roll_no);
                    printf("Name: %s\n", student_list[index].name);
                    printf("Age: %d\n", student_list[index].age);
                    printf("GPA: %.2f\n", student_list[index].gpa);
                }

                break;
            }

            case 4:
                update_student(student_list, count);
                break;

            case 5:
                delete_student(student_list, &count);
                break;

            case 6:
                gpa_statistics(student_list, count);
                break;

            case 7:
                sort_students(student_list, count);
                break;

            case 8:
                free_student_list(student_list);
                printf("Memory released successfully!\n");
                printf("Exiting program...\n");
                return 0;

            default:
                printf("Invalid choice! Try again.\n");
        }
    }

    free_student_list(student_list);
    return 0;
}
