# Awk-Lite Specification Sheet

## Awk-Lite CLI 

```bash
awk-lite [options] 'program' [file1 file2 ...] 
```

Complete Definition →
```bash
awk-lite [-f progfle] [-v name=value] program [file1 file2 ...]
```
- Fields as `[]` are optional.
- If no `file*` are provided default reads from `stdin`.

| Option          | Meaning                         |
|-----------------|---------------------------------|
| `-f progfile`   | File that contains awk code     |
| `-v name=value` | Sets variable value before exec |
| `file*`         | Multiple input file to process  |
| `--help` `-h`   | Help for usage                  |
	
- There must be either an input file or input from `stdin`.

### Sample Code

```bash
awk-lite '{print $1} read.txt
```
- Prints the 1st word from read.txt for every line.

```bash
awk-lite -f prog.awk -v a=5 -v b=10 read.txt
```
- Choose a file for awk program.
- Allows for multiple variable assignment

```bash
ls -la | awk-lite '{print}' 
```
- Pipe data from stdin of another command into awk-lite.

## Awk-lite Functionality Definition

### Program Structure

- `#` is for comment.

```
pattern { action }
```

`pattern` contains special keyword:
	- `BEGIN` → Executed once at beginning.
	- `END`   → Executed once at end.
	
Rest all `pattern` statement are executed for every line.

Other `pattern` includes:
	- `\regex\`
	- `Arithmetic/Boolean`
	
Keywords in the scope:
   - `$0`     → Includes the complete line.
   - `$1-$NF` → Each word stored based on index.
   - `NF`     → Number of Last word.
   - `NR`     → Number of lines.
- Remember using `$` means the value of the word referred by the index following it.

- Program code must either have a `pattern` or an `action`.
- If no `pattern` is specified, executed for all lines.
- If no `action` is specified, default is `{print $0}`.

### Patterns

#### Regex

- Must always be enclosed within `/regex/`.
- Matches for `$0`; complete line.

Match expression in regex:
	- `$1 ~ /foo/`  → `~` means return true if `$1` matches `/regex/` expression.
	- `$1 !~ /bar/` → `!~` means return true if `$1` does not matches `/regex/`.
	
```
# Atoms
a        match specific character
.        match any single character

# Character Class
[abc]    match any character in bracket
[a-z]    match any character from range
[0-9]    ...
[A-Z]    ...

# Repetition
*        match zero or more of above
+        match one or more of above
?        match zero or one of above

# Anchors
^        match if in start of string
$        match if in end of string
```

#### Boolean
- Boolean logic pattern are valid in action.
- `$1 == "foo"`

### Action

- Must always be enclosed within `{}`.

1. `print`:
	- Default if no action provided.
	- `print` just refers to print complete line.
	- `print $1, $2` multiple value are whitespace seperated.
	- Every `print` ends with a newline.

2. Arithmetic:
   - Variables are global scoped.
   - Initialised with `0` value.
   - Created on first use.
   - Basic operators `+ - * /`.
   
3. Boolean:
   - Operator `== != < > <= >=`.
   - Used along side with `if if-else`.

4. Conditional block:
   - Format → `if (condition) statement`.
	          `if (condition) statement else statement`.
   - Statement can be enclosed in `{}` but consider it optional.
   - For initial stage `if-else` is also optional.

## Awk-lite Program Module

### 1. Argument Parser(Main Module)

```bash
awk-lite [-f progfle] [-v name=value] program [file1 file2 ...]
```
- Parse file based on this cli defination.
- Store progfile and then load to create pattern and action.
- Read program and create pattern and action.
- Store file path for later parse exec.
- Store variable for usage.
- `-h` give help hint.
