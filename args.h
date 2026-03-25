#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <str.h>

// -f, -v, -h [files ...]
// -f and -v are multiple values means create a list
// no -f than take the first string as program
// no [files ...] than take stdin as file
// make a proper header file to do so

// -f is list of program files name
// -v is a list of key:value pair or variable:value pair,
// thus it is self defined struct with self defined
// function to populate value

// But it does not make sense to supply special func and struct
// cli are constant, only need to define this one style

// parsing type ./<program> [FLAGS] -- [ARGS]
// [ARGS] → default: program [files ...]
//        → -f prog: [files ...]
//        → stdin  : program

// check till there is '-' character, check against flags
// if no -f then first args is program
// rest of args are files if none take stdin

// Make list flag and list of dict flag
// -f → list flag store as String(str.h)
// -v → list of dict flag store as variable:value pair with dynamic type info

// Flag Context
// Array of all flags made
// Array of the rest of flags
// Error handling if if

typedef enum {
    Obj_String = 0,
    Obj_Int,
    Obj_Float
} Obj_Type;

// List that stores value of same type
typedef struct {
    void** items;
    size_t count;
    size_t capacity;
} List;

// Single key-value pair
typedef struct {
    Obj_Type type;
    
    String var;
    void* value;
} Pair;

typedef enum {
    Flag_Pair = 0,
    Flag_IList,
    Flag_FList,
    Flag_SList,
    Flag_Dict
} Flag_Type;

typedef union {
    List *as_list;
    Pair *as_pair;
} Flag_Value;

typedef struct {
    Flag_Type type;
    
    String name;
    String desc;

    Flag_Value val;
} Flag;

typedef struct {
    Flag flags[FLAG_CAPACITY];
    size_t count;

    String program_name;

    char** rest_argv;
    int rest_argc;
} Flag_Context;

// specify the flags(function)
// store in flag_context
// return the val..
// parser goes through ctx and does work
// also stores the rest args

Flag *__add_flag(Flag_Context *c, Flag_Type type, String name, String desc);
Pair *flag_add_pair(Flag_Context *c, String name, String desc);
List *flag_add_list(Flag_Context *c, String name, String desc, Flag_type type);
List *flag_add_dict(Flag_Context *c, String name, String desc);

Flag *__add_flag(Flag_Context *c, Flag_Type type, String name, String desc) {
    c→flags.type = type;
    c→flags.name = name;
    c→flags.desc = desc;
    
    c→count++;

    return c→flags;
}
Pair *flag_add_pair(Flag_Context *c, String name, String desc) {
    Flag* f = __add_flag(c, Flag_Pair, name, desc);
    f→val.as_pair = NULL;
    
    return f→val.as_pair;
}
List *flag_add_list(Flag_Context *c, String name, String desc, Flag_type type) {
    Flag* f = __add_flag(c, type, name, desc);
    f→val.as_list = NULL;

    return f→val.as_list;
}
List *flag_add_dict(Flag_Context *c, String name, String desc) {
    Flag* f = __add_flag(c, Flag_Dict, name, desc);
    f→val.as_list = NULL;

    return f→val.as_list;
}

String flag_shift_args(int* argc, char** argv) {
    String arg = fromCSTR(*argv);
    
    *argc -= 1;
    *argv += 1;

    return arg;
}

bool parse_args(Flag_Context *c, int argc, char** argv) {
    // take global args passed
    // loop through the flag context added flags
    // store the appropriate values

    Flag_Context *fc = (Flag_Context*)c;

    if (isNullString(fc→program_name) {
	char *name = flag_shift_args(argc, argv);
	fc→program_name = fromCSTR(name);
    }

    while (argc > 0) {
	String flag = flag_shift_args(argc, argv);

	if (!cmpString(flag, fromCSTR("-")) || cmpString(flag, fromCSTR("--"))) {
	    // no flags push args back and return
	    fc→rest_argc = argc + 1;
	    fc→rest_argv = *argv - 1;

	    return true;
	}
    }
}
