
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Enum to identify the data type
typedef enum {
    INT, FLOAT, CHAR, STRING
} DataType;

// Union to store different data types
typedef union {
    int int_value;
    float float_value;
    char char_value;
    char *string_value;
} Data;

// Structure combining union and type information
typedef struct {
    Data data;
    DataType type;
    int initialized;
} Container;

// Function declarations
void init_container(Container *c);
int set_int(Container *c, int value);
int set_float(Container *c, float value);
int set_char(Container *c, char value);
int set_string(Container *c, const char *value);

int get_int(const Container *c, int *result);
int get_float(const Container *c, float *result);
int get_char(const Container *c, char *result);
int get_string(const Container *c, const char **result);

void free_container(Container *c);

// Initialize container
void init_container(Container *c) {
    if (c != NULL) {
        c->initialized = 0;
        c->type = INT;
        c->data.int_value = 0;
    }
}

// Free previously allocated memory
void free_container(Container *c) {
    if (c == NULL)
        return;

    if (c->initialized && c->type == STRING) {
        free(c->data.string_value);
    }

    c->initialized = 0;
}

// Set integer value
int set_int(Container *c, int value) {
    if (c == NULL)
        return 0;

    free_container(c);

    c->data.int_value = value;
    c->type = INT;
    c->initialized = 1;

    return 1;
}

// Set float value
int set_float(Container *c, float value) {
    if (c == NULL)
        return 0;

    free_container(c);

    c->data.float_value = value;
    c->type = FLOAT;
    c->initialized = 1;

    return 1;
}

// Set character value
int set_char(Container *c, char value) {
    if (c == NULL)
        return 0;

    free_container(c);

    c->data.char_value = value;
    c->type = CHAR;
    c->initialized = 1;

    return 1;
}

// Set string value
int set_string(Container *c, const char *value) {
    if (c == NULL || value == NULL)
        return 0;

    char *temp = malloc(strlen(value) + 1);

    if (temp == NULL)
        return 0;

    strcpy(temp, value);

    free_container(c);

    c->data.string_value = temp;
    c->type = STRING;
    c->initialized = 1;

    return 1;
}

// Get integer value
int get_int(const Container *c, int *result) {
    if (c == NULL || result == NULL ||
        !c->initialized || c->type != INT)
        return 0;

    *result = c->data.int_value;
    return 1;
}

// Get float value
int get_float(const Container *c, float *result) {
    if (c == NULL || result == NULL ||
        !c->initialized || c->type != FLOAT)
        return 0;

    *result = c->data.float_value;
    return 1;
}

// Get character value
int get_char(const Container *c, char *result) {
    if (c == NULL || result == NULL ||
        !c->initialized || c->type != CHAR)
        return 0;

    *result = c->data.char_value;
    return 1;
}

// Get string value
int get_string(const Container *c, const char **result) {
    if (c == NULL || result == NULL ||
        !c->initialized || c->type != STRING)
        return 0;

    *result = c->data.string_value;
    return 1;
}

// Main function
int main() {
    Container c;
    int i;
    float f;
    char ch;
    const char *str;

    init_container(&c);

    if (set_int(&c, 10) && get_int(&c, &i))
        printf("Int: %d\n", i);

    if (set_float(&c, 3.14f) && get_float(&c, &f))
        printf("Float: %f\n", f);

    if (set_char(&c, 'A') && get_char(&c, &ch))
        printf("Char: %c\n", ch);

    if (set_string(&c, "Hello, World!") &&
        get_string(&c, &str))
        printf("String: %s\n", str);

    free_container(&c);

    return 0;
}
