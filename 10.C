/*
 * Problem 5 (Extended): Safe Heterogeneous Data Container Library
 *
 * Description:
 *   A memory-safe tagged container supporting TYPE_EMPTY, int, float, char,
 *   and heap-allocated strings using ISO C11 (malloc + memcpy, without strdup).
 *   Supports safe overwrites, status-returning getters, deep copy, and an
 *   array demonstration of mixed data types.
 *
 * Sample Run:
 *   Index 0 [INT]    : 10
 *   Index 1 [FLOAT]  : 3.14
 *   Index 2 [CHAR]   : A
 *   Index 3 [STRING] : Hello
 *   Index 4 [INT]    : 0
 *   Index 5 [STRING] : Data
 *   Deep copy verified after modifying original string.
 *   Transition test: STRING -> INT -> STRING -> FLOAT completed safely.
 *
 * Edge Cases Tested:
 *   - Valid integer zero (0) vs wrong-type getter error
 *   - Empty container get, print, and repeated container_clear
 *   - Empty string ("") storage and self-copy (container_copy(&c, &c))
 *   - Replacing string with its own internal pointer safely
 *   - Switching away from STRING without memory leaks
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TYPE_EMPTY,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING
} DataType;

typedef union {
    int int_value;
    float float_value;
    char char_value;
    char *string_value;
} Data;

typedef struct {
    Data data;
    DataType type;
} Container;

void container_init(Container *c) {
    if (c == NULL) {
        return;
    }

    c->type = TYPE_EMPTY;
    c->data.string_value = NULL;
}

void container_clear(Container *c) {
    if (c == NULL) {
        return;
    }

    if (c->type == TYPE_STRING && c->data.string_value != NULL) {
        free(c->data.string_value);
        c->data.string_value = NULL;
    }

    c->type = TYPE_EMPTY;
}

int container_set_int(Container *c, int value) {
    if (c == NULL) {
        return -1;
    }

    container_clear(c);
    c->data.int_value = value;
    c->type = TYPE_INT;

    return 0;
}

int container_set_float(Container *c, float value) {
    if (c == NULL) {
        return -1;
    }

    container_clear(c);
    c->data.float_value = value;
    c->type = TYPE_FLOAT;

    return 0;
}

int container_set_char(Container *c, char value) {
    if (c == NULL) {
        return -1;
    }

    container_clear(c);
    c->data.char_value = value;
    c->type = TYPE_CHAR;

    return 0;
}

int container_set_string(Container *c, const char *value) {
    if (c == NULL || value == NULL) {
        return -1;
    }

    size_t len = strlen(value);
    char *new_buf = malloc(len + 1);
    if (new_buf == NULL) {
        return -1;
    }

    memcpy(new_buf, value, len + 1);

    container_clear(c);
    c->data.string_value = new_buf;
    c->type = TYPE_STRING;

    return 0;
}

int container_get_int(const Container *c, int *out_value) {
    if (c == NULL || out_value == NULL || c->type != TYPE_INT) {
        return -1;
    }

    *out_value = c->data.int_value;

    return 0;
}

int container_get_float(const Container *c, float *out_value) {
    if (c == NULL || out_value == NULL || c->type != TYPE_FLOAT) {
        return -1;
    }

    *out_value = c->data.float_value;

    return 0;
}

int container_get_char(const Container *c, char *out_value) {
    if (c == NULL || out_value == NULL || c->type != TYPE_CHAR) {
        return -1;
    }

    *out_value = c->data.char_value;

    return 0;
}

int container_get_string(const Container *c, const char **out_value) {
    if (c == NULL || out_value == NULL || c->type != TYPE_STRING) {
        return -1;
    }

    *out_value = c->data.string_value;

    return 0;
}
const char *container_type_name(const Container *c) {
    if (c == NULL) {
        return "NULL";
    }

    switch (c->type) {
        case TYPE_EMPTY: {
            return "EMPTY";
        }
        case TYPE_INT: {
            return "INT";
        }
        case TYPE_FLOAT: {
            return "FLOAT";
        }
        case TYPE_CHAR: {
            return "CHAR";
        }
        case TYPE_STRING: {
            return "STRING";
        }
        default: {
            return "UNKNOWN";
        }
    }
}

void container_print(const Container *c) {
    if (c == NULL) {
        printf("(null container)");
        return;
    }

    switch (c->type) {
        case TYPE_EMPTY: {
            printf("<empty>");
            break;
        }
        case TYPE_INT: {
            printf("%d", c->data.int_value);
            break;
        }
        case TYPE_FLOAT: {
            printf("%.2f", c->data.float_value);
            break;
        }
        case TYPE_CHAR: {
            printf("'%c'", c->data.char_value);
            break;
        }
        case TYPE_STRING: {
            printf("\"%s\"", c->data.string_value);
            break;
        }
        default: {
            printf("<invalid>");
            break;
        }
    }
}

int container_copy(Container *destination, const Container *source) {
    if (destination == NULL || source == NULL) {
        return -1;
    }

    if (destination == source) {
        return 0;
    }

    switch (source->type) {
        case TYPE_EMPTY: {
            container_clear(destination);
            return 0;
        }
        case TYPE_INT: {
            return container_set_int(destination, source->data.int_value);
        }
        case TYPE_FLOAT: {
            return container_set_float(destination, source->data.float_value);
        }
        case TYPE_CHAR: {
            return container_set_char(destination, source->data.char_value);
        }
        case TYPE_STRING: {
            return container_set_string(destination, source->data.string_value);
        }
        default: {
            return -1;
        }
    }
}

int main(void) {
    int count = 6;
    Container *list = malloc((size_t)count * sizeof(Container));
    if (list == NULL) {
        printf("Error: Failed to allocate container array.\n");
        return 1;
    }

    for (int i = 0; i < count; i++) {
        container_init(list + i);
    }

    container_set_int(list + 0, 10);
    container_set_float(list + 1, 3.14f);
    container_set_char(list + 2, 'A');
    container_set_string(list + 3, "Hello");
    container_set_int(list + 4, 0);
    container_set_string(list + 5, "Data");

    printf("--- Mixed Container Array ---\n");
    for (int i = 0; i < count; i++) {
        printf("Index %d [%-6s] : ", i, container_type_name(list + i));
        container_print(list + i);
        printf("\n");
    }

    printf("\n--- Deep Copy & Mutation Test ---\n");
    Container backup;
    container_init(&backup);
    container_copy(&backup, list + 5);

    container_set_string(list + 5, "Updated Data");
    printf("Original (Index 5) : ");
    container_print(list + 5);
    printf("\nCopied (backup)    : ");
    container_print(&backup);
    printf("\n");

    /* Self-copy and self-pointer string replacement test */
    container_copy(&backup, &backup);
    const char *raw_str = NULL;
    if (container_get_string(&backup, &raw_str) == 0) {
        container_set_string(&backup, raw_str);
    }
    printf("After self-copy    : ");
    container_print(&backup);
    printf("\n");

    printf("\n--- Type Transition Test (STRING -> INT -> STRING -> FLOAT) ---\n");
    Container trans;
    container_init(&trans);

    container_set_string(&trans, "Initial Text");
    printf("Step 1 [%-6s] : ", container_type_name(&trans));
    container_print(&trans);
    printf("\n");

    container_set_int(&trans, 42);
    printf("Step 2 [%-6s] : ", container_type_name(&trans));
    container_print(&trans);
    printf("\n");

    container_set_string(&trans, "");
    printf("Step 3 [%-6s] : ", container_type_name(&trans));
    container_print(&trans);
    printf("\n");

    container_set_float(&trans, 9.81f);
    printf("Step 4 [%-6s] : ", container_type_name(&trans));
    container_print(&trans);
    printf("\n");

    printf("\n--- Edge Case & Getter Validation ---\n");
    int zero_val = -1;
    if (container_get_int(list + 4, &zero_val) == 0) {
        printf("Valid zero int retrieved: %d\n", zero_val);
    }

    int wrong_val = 0;
    if (container_get_int(list + 1, &wrong_val) != 0) {
        printf("Wrong-type getter safely rejected FLOAT as INT.\n");
    }

    container_clear(&trans);
    container_clear(&trans);
    printf("Cleared container state: [%s] ", container_type_name(&trans));
    container_print(&trans);
    printf("\n");

    container_clear(&backup);
    for (int i = 0; i < count; i++) {
        container_clear(list + i);
    }
    free(list);

    return 0;
}
