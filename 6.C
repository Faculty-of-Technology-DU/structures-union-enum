/*
 * Problem 3 (Extended): Memory-Efficient Typed Records and Safe Union Access
 *
 * Description:
 *   Prints size, alignment, member addresses, and struct offsets for
 *   StructData and UnionData, compares 100 records, and demonstrates a
 *   tagged union (enum ValueType) that safely accesses only the active member.
 *
 * Sample Run:
 *   StructData: size = 28, align = 4 | Offsets: i = 0, f = 4, str = 8
 *   UnionData : size = 20, align = 4
 *   100 Structs = 2800 bytes | 100 Unions = 2000 bytes
 *   Record 1 [INT]: 10 | Record 2 [FLOAT]: 220.50 | Record 3 [TEXT]: Union Programming
 *
 * Edge Cases Tested:
 *   - 19-character maximum string and overlength string rejection
 *   - Switching a single record from INT -> TEXT -> FLOAT
 *   - Invalid tag handling
 */

#include <stdalign.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define STR_CAP 20
#define REC_COUNT 5

typedef union {
    int i;
    float f;
    char str[STR_CAP];
} UnionData;

typedef struct {
    int i;
    float f;
    char str[STR_CAP];
} StructData;

typedef enum {
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_TEXT
} ValueType;

typedef struct {
    ValueType tag;
    UnionData data;
} TaggedRecord;

int set_int(TaggedRecord *rec, int val) {
    if (rec == NULL) {
        return -1;
    }

    rec->data.i = val;
    rec->tag = VALUE_INT;

    return 0;
}

int set_float(TaggedRecord *rec, float val) {
    if (rec == NULL) {
        return -1;
    }

    rec->data.f = val;
    rec->tag = VALUE_FLOAT;

    return 0;
}

int set_text(TaggedRecord *rec, const char *text) {
    if (rec == NULL || text == NULL) {
        return -1;
    }

    if (strlen(text) >= STR_CAP) {
        printf("Error: String exceeds %d characters.\n", STR_CAP - 1);
        return -1;
    }

    strcpy(rec->data.str, text);
    rec->tag = VALUE_TEXT;

    return 0;
}

void display_record(const TaggedRecord *rec, int num) {
    if (rec == NULL) {
        return;
    }

    switch (rec->tag) {
        case VALUE_INT: {
            printf("Record %d [INT]   : %d\n", num, rec->data.i);
            break;
        }
        case VALUE_FLOAT: {
            printf("Record %d [FLOAT] : %.2f\n", num, rec->data.f);
            break;
        }
        case VALUE_TEXT: {
            printf("Record %d [TEXT]  : %s\n", num, rec->data.str);
            break;
        }
        default: {
            printf("Record %d [ERROR] : Invalid tag\n", num);
            break;
        }
    }
}

int main(void) {
    StructData sdata = {10, 220.5f, "Struct Programming"};
    UnionData udata;
    udata.i = 10;

    printf("--- Memory Layout & Alignment ---\n");
    printf("StructData: size = %zu, align = %zu\n", sizeof(StructData), alignof(StructData));
    printf("UnionData : size = %zu, align = %zu\n", sizeof(UnionData), alignof(UnionData));
    printf("Struct offsets: i = %zu, f = %zu, str = %zu\n",
           offsetof(StructData, i), offsetof(StructData, f), offsetof(StructData, str));

    printf("Struct addresses: i=%p, f=%p, str=%p\n",
           (void *)&sdata.i, (void *)&sdata.f, (void *)sdata.str);
    printf("Union addresses : i=%p, f=%p, str=%p\n",
           (void *)&udata.i, (void *)&udata.f, (void *)udata.str);

    printf("100 Structs = %zu bytes | 100 Unions = %zu bytes\n\n",
           100 * sizeof(StructData), 100 * sizeof(UnionData));

    printf("--- Tagged Records ---\n");
    TaggedRecord records[REC_COUNT];
    set_int(records + 0, 10);
    set_float(records + 1, 220.5f);
    set_text(records + 2, "Union Programming");
    set_int(records + 3, 42);
    set_text(records + 4, "19_Char_Max_String!");

    for (int i = 0; i < REC_COUNT; i++) {
        display_record(records + i, i + 1);
    }

    printf("\n--- Edge Case Tests ---\n");
    set_text(records + 4, "This string is way too long for 20 bytes");
    display_record(records + 4, 5);

    TaggedRecord temp;
    set_int(&temp, 100);
    display_record(&temp, 6);
    set_text(&temp, "Switched");
    display_record(&temp, 6);
    set_float(&temp, 55.5f);
    display_record(&temp, 6);

    temp.tag = (ValueType)99;
    display_record(&temp, 7);

    printf("\nTaggedRecord size = %zu bytes\n", sizeof(TaggedRecord));
    printf("Observation: A union overlaps all members at the same address, saving memory\n");
    printf("when only one type is stored at a time, unlike a struct which stores all fields separately.\n");

    return 0;
}
