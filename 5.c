/*
 * Problem 2 (Extended): Rectangle Inventory and Geometry Analysis
 *
 * Description:
 *   An interactive geometry manager storing dynamically allocated Rectangle
 *   records with unique IDs, finite positive dimensions, derived area and
 *   perimeter fields, epsilon-based square detection, ascending area sorting
 *   (tie-broken by ID), threshold filtering, and area summary statistics.
 *
 * Sample Run:
 *   Enter initial capacity (1 - 1000): 2
 *   1. Add -> ID: 201, W: 5.0, H: 4.0 (Area: 20.00, Perim: 18.00, Non-square)
 *   1. Add -> ID: 202, W: 6.5, H: 3.5 (Area: 22.75, Perim: 20.00, Non-square)
 *   1. Add -> ID: 203, W: 4.0, H: 4.0 (Resized 2 -> 4; Square)
 *   6. Sorted -> 203 (16.00), 201 (20.00), 202 (22.75)
 *   4. Update ID 201 -> W: 6.0, H: 6.0 (Area: 36.00, Perim: 24.00, Square)
 *   7. Filter > 20.0 -> 202 (22.75), 201 (36.00)
 *   8. Summary -> Largest: 201 (36.00), Smallest: 203 (16.00), Total: 74.75, Avg: 24.92
 *   0. Exit -> Memory freed cleanly.
 *
 * Edge Cases Tested:
 *   - Capacity outside 1..1000
 *   - Zero, negative, NaN, and infinity dimensions
 *   - Duplicate rectangle ID rejection
 *   - Square check with floating-point tolerance (1e-5f)
 *   - Equal areas sorted by ascending ID
 *   - Empty collection after deletion and realloc growth
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CAPACITY 1000
#define SQUARE_EPSILON 1e-5f
#define BUF_SIZE 256

typedef struct {
    int id;
    float width;
    float height;
    float area;
    float perimeter;
} Rectangle;

typedef struct {
    Rectangle *items;
    int count;
    int capacity;
} RectangleList;

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

void compute_geometry(Rectangle *r) {
    r->area = r->width * r->height;
    r->perimeter = 2.0f * (r->width + r->height);
}

int is_square(const Rectangle *r) {
    return (fabsf(r->width - r->height) <= SQUARE_EPSILON);
}

int init_list(RectangleList *list, int cap) {
    if (cap < 1 || cap > MAX_CAPACITY) {
        return -1;
    }

    list->items = malloc((size_t)cap * sizeof(Rectangle));
    if (list->items == NULL) {
        return -1;
    }

    list->count = 0;
    list->capacity = cap;

    return 0;
}

void free_list(RectangleList *list) {
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

int find_by_id(const RectangleList *list, int id) {
    for (int i = 0; i < list->count; i++) {
        if (list->items[i].id == id) {
            return i;
        }
    }

    return -1;
}

int add_rectangle(RectangleList *list, int id, float width, float height) {
    if (id <= 0) {
        printf("Error: ID must be a positive integer.\n");
        return -1;
    }

    if (find_by_id(list, id) != -1) {
        printf("Error: Rectangle ID %d already exists.\n", id);
        return -1;
    }

    if (!isfinite(width) || width <= 0.0f || !isfinite(height) || height <= 0.0f) {
        printf("Error: Width and height must be finite positive numbers.\n");
        return -1;
    }

    if (list->count == list->capacity) {
        int new_cap = list->capacity * 2;
        Rectangle *resized = realloc(list->items, (size_t)new_cap * sizeof(Rectangle));
        if (resized == NULL) {
            printf("Error: Memory reallocation failed.\n");
            return -1;
        }
        list->items = resized;
        list->capacity = new_cap;
        printf("Info: Capacity resized to %d.\n", new_cap);
    }

    Rectangle *r = &list->items[list->count];
    r->id = id;
    r->width = width;
    r->height = height;
    compute_geometry(r);
    list->count++;

    return 0;
}
int update_rectangle(RectangleList *list, int id, float width, float height) {
    int idx = find_by_id(list, id);
    if (idx == -1) {
        printf("Error: Rectangle ID %d not found.\n", id);
        return -1;
    }

    if (!isfinite(width) || width <= 0.0f || !isfinite(height) || height <= 0.0f) {
        printf("Error: Width and height must be finite positive numbers.\n");
        return -1;
    }

    list->items[idx].width = width;
    list->items[idx].height = height;
    compute_geometry(&list->items[idx]);

    return 0;
}

int delete_rectangle(RectangleList *list, int id) {
    int idx = find_by_id(list, id);
    if (idx == -1) {
        printf("Error: Rectangle ID %d not found.\n", id);
        return -1;
    }

    for (int i = idx; i < list->count - 1; i++) {
        list->items[i] = list->items[i + 1];
    }
    list->count--;

    return 0;
}

void print_rectangle(const Rectangle *r) {
    const char *kind = is_square(r) ? "Square" : "Non-square";
    printf("ID: %-5d | W: %-7.2f | H: %-7.2f | Area: %-9.2f | Perim: %-9.2f | %s\n",
           r->id, r->width, r->height, r->area, r->perimeter, kind);
}

void display_all(const RectangleList *list) {
    if (list->count == 0) {
        printf("No rectangles stored.\n");
        return;
    }

    printf("\n--- Rectangle List (%d / %d) ---\n", list->count, list->capacity);
    for (int i = 0; i < list->count; i++) {
        print_rectangle(&list->items[i]);
    }
}

void display_sorted(const RectangleList *list) {
    if (list->count == 0) {
        printf("No rectangles available to sort.\n");
        return;
    }

    Rectangle *copy = malloc((size_t)list->count * sizeof(Rectangle));
    if (copy == NULL) {
        printf("Error: Memory allocation failed for sorting.\n");
        return;
    }

    memcpy(copy, list->items, (size_t)list->count * sizeof(Rectangle));

    for (int i = 0; i < list->count - 1; i++) {
        for (int j = 0; j < list->count - i - 1; j++) {
            int swap = 0;
            if (copy[j].area > copy[j + 1].area) {
                swap = 1;
            } else if (copy[j].area == copy[j + 1].area &&
                       copy[j].id > copy[j + 1].id) {
                swap = 1;
            }

            if (swap) {
                Rectangle temp = copy[j];
                copy[j] = copy[j + 1];
                copy[j + 1] = temp;
            }
        }
    }

    printf("\n--- Rectangles Sorted by Area (Ascending) ---\n");
    for (int i = 0; i < list->count; i++) {
        print_rectangle(&copy[i]);
    }

    free(copy);
}

void display_filtered(const RectangleList *list, float threshold) {
    if (list->count == 0) {
        printf("No rectangles stored.\n");
        return;
    }

    int found = 0;
    printf("\n--- Rectangles with Area > %.2f ---\n", threshold);
    for (int i = 0; i < list->count; i++) {
        if (list->items[i].area > threshold) {
            print_rectangle(&list->items[i]);
            found++;
        }
    }

    if (found == 0) {
        printf("No rectangles have area greater than %.2f.\n", threshold);
    }
}

void display_summary(const RectangleList *list) {
    if (list->count == 0) {
        printf("Rectangle collection is empty.\n");
        return;
    }

    int big = 0;
    int small = 0;
    float total = 0.0f;

    for (int i = 0; i < list->count; i++) {
        total += list->items[i].area;
        if (list->items[i].area > list->items[big].area) {
            big = i;
        }
        if (list->items[i].area < list->items[small].area) {
            small = i;
        }
    }

    printf("\n--- Area Summary ---\n");
    printf("Largest Area  : %.2f (ID %d)\n", list->items[big].area, list->items[big].id);
    printf("Smallest Area : %.2f (ID %d)\n", list->items[small].area, list->items[small].id);
    printf("Total Area    : %.2f\n", total);
    printf("Average Area  : %.2f\n", total / (float)list->count);
}

void print_menu(void) {
    printf("\n=== Rectangle Geometry Menu ===\n");
    printf("1. Add Rectangle\n");
    printf("2. Display All Rectangles\n");
    printf("3. Search by ID\n");
    printf("4. Update Dimensions\n");
    printf("5. Delete Rectangle\n");
    printf("6. Display Sorted by Area\n");
    printf("7. Filter by Minimum Area\n");
    printf("8. Display Area Summary\n");
    printf("0. Exit\n");
}

int main(void) {
    RectangleList list = {NULL, 0, 0};
    int cap = 0;

    while (1) {
        int res = read_int("Enter initial capacity (1 - 1000): ", &cap);
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
                int id = 0;
                float w = 0.0f;
                float h = 0.0f;

                if (read_int("Enter rectangle ID: ", &id) != 0) {
                    printf("Error: Invalid ID.\n");
                    break;
                }
                if (read_float("Enter width: ", &w) != 0) {
                    printf("Error: Invalid width.\n");
                    break;
                }
                if (read_float("Enter height: ", &h) != 0) {
                    printf("Error: Invalid height.\n");
                    break;
                }

                if (add_rectangle(&list, id, w, h) == 0) {
                    printf("Success: Rectangle %d added.\n", id);
                }
                break;
            }
            case 2: {
                display_all(&list);
                break;
            }
            case 3: {
                int id = 0;
                if (read_int("Enter rectangle ID to search: ", &id) != 0) {
                    printf("Error: Invalid ID.\n");
                    break;
                }
                int idx = find_by_id(&list, id);
                if (idx == -1) {
                    printf("Rectangle ID %d not found.\n", id);
                } else {
                    print_rectangle(&list.items[idx]);
                }
                break;
            }
            case 4: {
                int id = 0;
                float w = 0.0f;
                float h = 0.0f;

                if (read_int("Enter rectangle ID to update: ", &id) != 0) {
                    printf("Error: Invalid ID.\n");
                    break;
                }
                if (find_by_id(&list, id) == -1) {
                    printf("Error: Rectangle ID %d not found.\n", id);
                    break;
                }
                if (read_float("Enter new width: ", &w) != 0) {
                    printf("Error: Invalid width.\n");
                    break;
                }
                if (read_float("Enter new height: ", &h) != 0) {
                    printf("Error: Invalid height.\n");
                    break;
                }

                if (update_rectangle(&list, id, w, h) == 0) {
                    printf("Success: Rectangle %d updated.\n", id);
                }
                break;
            }
            case 5: {
                int id = 0;
                if (read_int("Enter rectangle ID to delete: ", &id) != 0) {
                    printf("Error: Invalid ID.\n");
                    break;
                }
                if (delete_rectangle(&list, id) == 0) {
                    printf("Success: Rectangle %d deleted.\n", id);
                }
                break;
            }
            case 6: {
                display_sorted(&list);
                break;
            }
            case 7: {
                float thresh = 0.0f;
                if (read_float("Enter minimum area threshold: ", &thresh) != 0) {
                    printf("Error: Invalid threshold.\n");
                    break;
                }
                display_filtered(&list, thresh);
                break;
            }
            case 8: {
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
