
// Structures - 2
#include <stdio.h>
#include <stdlib.h>

// Structure definition
typedef struct {
    float width;
    float height;
    float area;
} Rectangle;

// Function to calculate area
void calculate_area(Rectangle *r) {
    r->area = r->width * r->height;
}

// Function to create rectangle array
Rectangle* create_rectangle_array(int size) {
    if (size <= 0) {
        return NULL;
    }

    Rectangle *rectangles =
        malloc(size * sizeof(Rectangle));

    if (rectangles == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    return rectangles;
}

// Function to add rectangle details
void add_rectangle(Rectangle *rectangles, int index,
                    int size, float width, float height) {

    if (rectangles == NULL || index < 0 || index >= size ||
        width < 0 || height < 0) {
        return;
    }

    rectangles[index].width = width;
    rectangles[index].height = height;

    calculate_area(&rectangles[index]);
}

// Function to display rectangles
void display_rectangles(const Rectangle *rectangles, int size) {
    if (rectangles == NULL) {
        return;
    }

    for (int i = 0; i < size; i++) {
        printf("Rectangle %d: Width = %.2f, Height = %.2f, Area = %.2f\n",
               i + 1,
               rectangles[i].width,
               rectangles[i].height,
               rectangles[i].area);
    }
}

// Function to free memory
void free_rectangle_array(Rectangle *rectangles) {
    free(rectangles);
}

// Main function
int main() {
    int number_of_rectangles = 2;

    Rectangle *rectangles =
        create_rectangle_array(number_of_rectangles);

    if (rectangles == NULL) {
        return 1;
    }

    add_rectangle(rectangles, 0, number_of_rectangles, 5.0, 4.0);
    add_rectangle(rectangles, 1, number_of_rectangles, 6.5, 3.5);

    printf("Rectangle Details:\n\n");

    display_rectangles(rectangles, number_of_rectangles);

    free_rectangle_array(rectangles);

    return 0;
}
