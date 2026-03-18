#include "main.h"

int main(int argc, char* argv[])
{
    if (argc < 2) {
	fprintf(stderr, "Program used without options.\n");
	display_usage();

	return -1;
    }

    String progfile[10];
    Variable var[24];
    String infile[10];

    int exit = parse_arg(argc, argv, progfile, infile, var);
    if (exit == -1) {
	return -1;
    }
    if (exit == 1) {
	printf("Successful argument parsing\n");
    }

    printf("Program File\n");
    for (int i=0; i<10; i++) {
	if (progfile[i].start != NULL) printf("\t%-*s\n", progfile[i].len, progfile[i].start);
    }
    printf("Input File\n");
    if (infile[0].start == NULL) printf("Read from stdin\n");
    for (int i=0; i<10; i++) {
	if (infile[i].start != NULL) printf("\t%-*s\n", infile[i].len, infile[i].start);
    }
    printf("Variable\n");
    for (int i=0; i<24; i++) {
	if (var[i].name.start != NULL) {
	    switch (var[i].type) {

	    case TYPE_STRING:
		printf("\t%.*s = %-*s(STRING)\n", var[i].name.len, var[i].name.start, var[i].value.len, var[i].value.start);
		break;
	    case TYPE_INT:
		{
		    char buf[50];
		    strncpy(buf, var[i].value.start, var[i].value.len);
		    int value = atoi(buf);
		    if (!value) {
			fprintf(stderr, "Not a valid integer type → %s", buf);
			display_usage();
		    }
		    printf("\t%.*s = %d(INT)\n", var[i].name.len, var[i].name.start, value);
		    break;
		}
	    case TYPE_FLOAT:
		{
		    char buf[50];
		    strncpy(buf, var[i].value.start, var[i].value.len);
		    float value = (float)atof(buf);
		    printf("\t%.*s = %f(FLOAT)\n", var[i].name.len, var[i].name.start, value);
		    break;
		}
	    default:
		break;
	    }
	}
    }
    
    
    return 0;
}


void display_usage()
{
    printf("Usage: awk-lite [options] 'program' [file1 file2 ...]\n"
    "Options:\n"
    "\t-f progfile   : File containing awk code\n"
    "\t-v name=value : Sets variable value before execution\n"
    "\tfile*         : Input file to operate on\n"
    "\t--help | -h   : Prints this\n"
    "\n"
    "program must be present\n"
    "If no input file stdin is considered\n");
}

int parse_arg(int argc, char* argv[], String progfile[], String infile[], Variable var[])
{
    memset(progfile, 0, 10*sizeof(String));
    memset(infile, 0, 10*sizeof(String));
    memset(var, 0, 24*sizeof(Variable));

    char is_program = 1;
    int pf = 0;
    int v = 0;
    int inf = 0;
    
    for (int i=1; i<argc; i++) {
	if (strcmp("--help", argv[i]) == 0) {
	    display_usage();
	    return -1;
	}
	else if (argv[i][0] == '-') {
	    
	    switch (argv[i][1]) {

	    case 'h':
		display_usage();
		return -1;
	    case 'f':
		i++;
		progfile[pf].start = &(argv[i][0]);
		progfile[pf++].len = strlen(&argv[i][0]);
		is_program = 0;
		break;
	    case 'v':
		i++;
		int name_addr = 0;
		int str_len = strlen(&argv[i][0]);
		while (argv[i][name_addr] != '=' && name_addr < str_len) name_addr++;
		var[v].name = (String){ &(argv[i][0]), name_addr};

		if (isalpha(argv[i][name_addr+1])) {
		    var[v].type = TYPE_STRING;
		    var[v].value = (String){ .start = &argv[i][name_addr+1], .len = str_len - name_addr - 1};
		    v++;
		}
		else if (isdigit(argv[i][name_addr+1])) {
		    int is_float = 0;
		    int k=name_addr+2;
		    while (k < str_len) {
			if (argv[i][k] == '.') {
			    is_float = 1;
			    break;
			}
			k++;
		    }

		    if (is_float) {
			var[v].type = TYPE_FLOAT;
			var[v].value = (String){ .start = &argv[i][name_addr+1], .len = str_len - name_addr - 1};
		    }
		    else {
			var[v].type = TYPE_INT;
			var[v].value = (String){ .start = &argv[i][name_addr+1], .len = str_len - name_addr - 1};
		    }

		    v++;
		}
		else {
		    fprintf(stderr, "Invalid argument for -v %s\n", argv[i]);
		    display_usage();
		    return -1;
		}
		break;
	    default:
		fprintf(stderr, "Invalid flag type. See Usage →\n");
		display_usage();
		return -1;
	    }
	 }
	 else {
	     if (is_program) {
		 progfile[pf].start = &argv[i][0];
		 progfile[pf++].len = strlen(&argv[i][0]);

		 is_program = 0;
	     }
	     else {
		 infile[inf].start = &argv[i][0];
		 infile[inf++].len = strlen(&argv[i][0]);
	     }
	 }
     }

     return 1;
}

