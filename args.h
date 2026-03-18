#pragma once

typedef enum {
    TYPE_STRING = 0,
    TYPE_INT,
    TYPE_FLOAT
} Type;

typedef struct {
    String name;
    String value;
    Type   type;
} Variable;

void display_usage();
int parse_arg(int argc, char* argv[], String progfile[], String infile[], Variable var[])
