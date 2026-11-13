/*
 * Problem 4 (Extended): Interactive Command-Driven Record Manager
 *
 * Description:
 *   An in-memory task manager controlled by case-insensitive commands
 *   (ADD, DELETE, VIEW, UPDATE, COMPLETE, FIND, HELP, EXIT) backed by a
 *   dynamically resized Task array and bounded fgets input handling.
 *
 * Sample Run:
 *   > add        -> ID: 101, Desc: "Finish C assignment" -> Task 101 added.
 *   > ADD        -> ID: 102, Desc: "Push code to GitHub" -> Task 102 added.
 *   > view       -> Lists task 101 [PENDING] and task 102 [PENDING]
 *   > complete   -> ID: 101 -> Task 101 marked DONE.
 *   > find       -> ID: 101 -> [101] Finish C assignment (DONE)
 *   > delete     -> ID: 102 -> Task 102 deleted.
 *   > help       -> Shows all recognized commands.
 *   > exit       -> Frees memory and exits.
 *
 * Edge Cases Tested:
 *   - Mixed-case commands (add, Add, ADD) and unknown commands
 *   - Excessive command or description line length
 *   - Duplicate task ID and nonexistent task ID
 *   - Descriptions containing spaces and empty descriptions
 *   - Empty task list operations, EOF termination, and realloc growth
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DESC_LEN 100
#define LINE_BUF_SIZE 256

typedef enum {
    STATUS_PENDING,
    STATUS_DONE
} TaskStatus;

typedef enum {
    CMD_ADD,
    CMD_DELETE,
    CMD_VIEW,
    CMD_UPDATE,
    CMD_COMPLETE,
    CMD_FIND,
    CMD_HELP,
    CMD_EXIT,
    CMD_UNKNOWN
} CommandType;

typedef struct {
    int id;
    char description[MAX_DESC_LEN];
    TaskStatus status;
} Task;

typedef struct {
    Task *items;
    int count;
    int capacity;
} TaskList;

void clear_extra_input(void) {
    int ch = 0;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

int read_bounded_line(const char *prompt, char *buf, int buf_size) {
    printf("%s", prompt);
    if (fgets(buf, buf_size, stdin) == NULL) {
        return -1;
    }

    if (strchr(buf, '\n') == NULL) {
        clear_extra_input();
        printf("Error: Input line is too long.\n");
        return -2;
    }

    size_t len = strcspn(buf, "\r\n");
    buf[len] = '\0';

    return 0;
}

int read_positive_id(const char *prompt, int *out_id) {
    char line[LINE_BUF_SIZE];
    int status = read_bounded_line(prompt, line, sizeof(line));
    if (status != 0) {
        return status;
    }

    char *endptr = NULL;
    long val = strtol(line, &endptr, 10);
    while (*endptr == ' ' || *endptr == '\t') {
        endptr++;
    }

    if (endptr == line || *endptr != '\0' || val <= 0 || val > 1000000000L) {
        printf("Error: Please enter a valid positive numeric ID.\n");
        return -2;
    }

    *out_id = (int)val;

    return 0;
}

void to_upper_text(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = (char)toupper((unsigned char)str[i]);
    }
}

void trim_spaces(char *str) {
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t') {
        start++;
    }

    int end = (int)strlen(str) - 1;
    while (end >= start && (str[end] == ' ' || str[end] == '\t')) {
        end--;
    }

    int len = (end >= start) ? (end - start + 1) : 0;
    if (start > 0 && len > 0) {
        memmove(str, str + start, (size_t)len);
    }
    str[len] = '\0';
}

CommandType get_command_type(const char *cmd_str) {
    char temp[LINE_BUF_SIZE];
    if (strlen(cmd_str) >= sizeof(temp)) {
        return CMD_UNKNOWN;
    }

    strcpy(temp, cmd_str);
    trim_spaces(temp);
    to_upper_text(temp);

    if (strcmp(temp, "ADD") == 0) {
        return CMD_ADD;
    }
    if (strcmp(temp, "DELETE") == 0) {
        return CMD_DELETE;
    }
    if (strcmp(temp, "VIEW") == 0) {
        return CMD_VIEW;
    }
    if (strcmp(temp, "UPDATE") == 0) {
        return CMD_UPDATE;
    }
    if (strcmp(temp, "COMPLETE") == 0) {
        return CMD_COMPLETE;
    }
    if (strcmp(temp, "FIND") == 0) {
        return CMD_FIND;
    }
    if (strcmp(temp, "HELP") == 0) {
        return CMD_HELP;
    }
    if (strcmp(temp, "EXIT") == 0) {
        return CMD_EXIT;
    }

    return CMD_UNKNOWN;
}

int find_task_index(const TaskList *list, int id) {
    for (int i = 0; i < list->count; i++) {
        if (list->items[i].id == id) {
            return i;
        }
    }

    return -1;
}
int add_task(TaskList *list) {
    int id = 0;
    int id_res = read_positive_id("Enter task ID: ", &id);
    if (id_res != 0) {
        return id_res;
    }

    if (find_task_index(list, id) != -1) {
        printf("Error: Task ID %d already exists.\n", id);
        return -2;
    }

    char desc_buf[LINE_BUF_SIZE];
    int desc_res = read_bounded_line("Enter description (max 99 chars): ", desc_buf, sizeof(desc_buf));
    if (desc_res != 0) {
        return desc_res;
    }

    trim_spaces(desc_buf);
    if (strlen(desc_buf) == 0) {
        printf("Error: Task description cannot be empty.\n");
        return -2;
    }
    if (strlen(desc_buf) >= MAX_DESC_LEN) {
        printf("Error: Description exceeds %d characters.\n", MAX_DESC_LEN - 1);
        return -2;
    }

    if (list->count == list->capacity) {
        int new_cap = (list->capacity == 0) ? 2 : list->capacity * 2;
        Task *resized = realloc(list->items, (size_t)new_cap * sizeof(Task));
        if (resized == NULL) {
            printf("Error: Memory allocation failed.\n");
            return -2;
        }
        list->items = resized;
        list->capacity = new_cap;
    }

    Task *t = &list->items[list->count];
    t->id = id;
    strcpy(t->description, desc_buf);
    t->status = STATUS_PENDING;
    list->count++;

    printf("Success: Task %d added.\n", id);

    return 0;
}

void view_tasks(const TaskList *list) {
    if (list->count == 0) {
        printf("Task list is empty.\n");
        return;
    }

    printf("\n--- Task List (%d) ---\n", list->count);
    for (int i = 0; i < list->count; i++) {
        const char *st = (list->items[i].status == STATUS_DONE) ? "DONE" : "PENDING";
        printf("ID: %-5d | Status: %-7s | Description: %s\n",
               list->items[i].id, st, list->items[i].description);
    }
}

int find_task(const TaskList *list) {
    if (list->count == 0) {
        printf("Task list is empty.\n");
        return 0;
    }

    int id = 0;
    int id_res = read_positive_id("Enter task ID to find: ", &id);
    if (id_res != 0) {
        return id_res;
    }

    int idx = find_task_index(list, id);
    if (idx == -1) {
        printf("Error: Task ID %d not found.\n", id);
        return -2;
    }

    const char *st = (list->items[idx].status == STATUS_DONE) ? "DONE" : "PENDING";
    printf("Found -> ID: %d | Status: %s | Description: %s\n",
           list->items[idx].id, st, list->items[idx].description);

    return 0;
}

int update_task(TaskList *list) {
    if (list->count == 0) {
        printf("Task list is empty.\n");
        return 0;
    }

    int id = 0;
    int id_res = read_positive_id("Enter task ID to update: ", &id);
    if (id_res != 0) {
        return id_res;
    }

    int idx = find_task_index(list, id);
    if (idx == -1) {
        printf("Error: Task ID %d not found.\n", id);
        return -2;
    }

    char desc_buf[LINE_BUF_SIZE];
    int desc_res = read_bounded_line("Enter new description (max 99 chars): ", desc_buf, sizeof(desc_buf));
    if (desc_res != 0) {
        return desc_res;
    }

    trim_spaces(desc_buf);
    if (strlen(desc_buf) == 0) {
        printf("Error: Task description cannot be empty.\n");
        return -2;
    }
    if (strlen(desc_buf) >= MAX_DESC_LEN) {
        printf("Error: Description exceeds %d characters.\n", MAX_DESC_LEN - 1);
        return -2;
    }

    strcpy(list->items[idx].description, desc_buf);
    printf("Success: Task %d updated.\n", id);

    return 0;
}

int complete_task(TaskList *list) {
    if (list->count == 0) {
        printf("Task list is empty.\n");
        return 0;
    }

    int id = 0;
    int id_res = read_positive_id("Enter task ID to mark complete: ", &id);
    if (id_res != 0) {
        return id_res;
    }

    int idx = find_task_index(list, id);
    if (idx == -1) {
        printf("Error: Task ID %d not found.\n", id);
        return -2;
    }

    list->items[idx].status = STATUS_DONE;
    printf("Success: Task %d marked as DONE.\n", id);

    return 0;
}

int delete_task(TaskList *list) {
    if (list->count == 0) {
        printf("Task list is empty.\n");
        return 0;
    }

    int id = 0;
    int id_res = read_positive_id("Enter task ID to delete: ", &id);
    if (id_res != 0) {
        return id_res;
    }

    int idx = find_task_index(list, id);
    if (idx == -1) {
        printf("Error: Task ID %d not found.\n", id);
        return -2;
    }

    for (int i = idx; i < list->count - 1; i++) {
        list->items[i] = list->items[i + 1];
    }
    list->count--;

    printf("Success: Task %d deleted.\n", id);

    return 0;
}

void print_help(void) {
    printf("\nAvailable Commands (case-insensitive):\n");
    printf("  ADD      - Add a new task with unique ID and description\n");
    printf("  DELETE   - Delete an existing task by ID\n");
    printf("  VIEW     - Display all tasks\n");
    printf("  UPDATE   - Change the description of an existing task\n");
    printf("  COMPLETE - Mark an existing task as DONE\n");
    printf("  FIND     - Search and display a task by ID\n");
    printf("  HELP     - Show this help message\n");
    printf("  EXIT     - Free memory and quit the program\n");
}

int main(void) {
    TaskList list = {NULL, 0, 0};
    char input[LINE_BUF_SIZE];

    printf("Task Manager Ready. Type HELP for commands.\n");

    while (1) {
        int line_res = read_bounded_line("\nEnter command: ", input, sizeof(input));
        if (line_res == -1) {
            printf("\nEOF reached. Exiting program.\n");
            break;
        }
        if (line_res == -2) {
            continue;
        }

        CommandType cmd = get_command_type(input);

        switch (cmd) {
            case CMD_ADD: {
                if (add_task(&list) == -1) {
                    free(list.items);
                    return 0;
                }
                break;
            }
            case CMD_DELETE: {
                if (delete_task(&list) == -1) {
                    free(list.items);
                    return 0;
                }
                break;
            }
            case CMD_VIEW: {
                view_tasks(&list);
                break;
            }
            case CMD_UPDATE: {
                if (update_task(&list) == -1) {
                    free(list.items);
                    return 0;
                }
                break;
            }
            case CMD_COMPLETE: {
                if (complete_task(&list) == -1) {
                    free(list.items);
                    return 0;
                }
                break;
            }
            case CMD_FIND: {
                if (find_task(&list) == -1) {
                    free(list.items);
                    return 0;
                }
                break;
            }
            case CMD_HELP: {
                print_help();
                break;
            }
            case CMD_EXIT: {
                printf("Exiting program.\n");
                free(list.items);
                return 0;
            }
            default: {
                printf("Error: Unknown command. Type HELP for available commands.\n");
                break;
            }
        }
    }

    free(list.items);

    return 0;
}
