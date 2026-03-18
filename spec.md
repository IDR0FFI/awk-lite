# Awk‑Lite Specification

This document defines a **small, implementable subset of awk** ("awk‑lite").
The goal is clarity and practicality: this is **not POSIX awk**, but a clean core you can realistically implement in C (and re‑implement in another language for practice).

---

## 1. Conceptual Overview

**Awk‑lite execution model**:

```
input stream (lines)
   ↓
split into fields ($0, $1, $2, ...)
   ↓
for each rule:
    if pattern matches → run action
   ↓
maintain global state (variables)
```

Key ideas:

* Input is processed **line by line**
* Regex decides **when code runs**, not what code means
* Arithmetic and variables provide **stateful computation**

---

## 2. Program Structure

An awk‑lite program is a list of **rules**:

```
pattern { action }
```

Special rules:

```
BEGIN { action }
END   { action }
```

Execution order:

1. Run all `BEGIN` actions (once)
2. For each input line:

   * Set `$0`
   * Split fields → `$1 .. $NF`
   * Increment `NR`
   * Execute rules top‑down
3. Run all `END` actions (once)

---

## 3. Input and Fields

* Input is a sequence of newline‑terminated lines
* Each line is split using **whitespace** (space or tab)

Built‑in fields:

* `$0` → full line
* `$1`, `$2`, ... → fields (1‑indexed)

Built‑in variables:

| Name | Meaning                           |
| ---- | --------------------------------- |
| `NR` | current line number (starts at 1) |
| `NF` | number of fields in current line  |

---

## 4. Patterns

Patterns control **whether an action executes**.

### 4.1 Regex Pattern

```
/regex/
```

* Matches if regex matches `$0`

Example:

```
/error/ { print }
```

---

### 4.2 Boolean Expression Pattern

```
$1 > 10
$2 == "foo"
```

* Evaluated per line
* If expression is true → action runs

---

### 4.3 Empty Pattern

```
{ action }
```

* Always matches

---

## 5. Actions

Actions are blocks of statements:

```
{ statement; statement; ... }
```

If a rule has **no action**, the default action is:

```
print $0
```

---

## 6. Statements

### 6.1 Print

```
print
print $1
print $1, $2
```

* Output fields separated by a space
* Ends with newline

---

### 6.2 Assignment

```
x = 5
sum = sum + $1
```

* Variables are global
* Created on first use
* Default value = `0`

---

### 6.3 Arithmetic

Supported operators:

```
+  -  *  /
```

Rules:

* Fields auto‑convert to numbers when used arithmetically
* Division by zero → runtime error

---

### 6.4 Comparisons

Operators:

```
==  !=  <  >  <=  >=
```

Rules:

* Numeric comparison if both operands numeric
* Otherwise string comparison

---

### 6.5 Conditionals

```
if (condition) statement
if (condition) { statements }
```

* No `else` in awk‑lite

---

## 7. Regex Usage Semantics

Regex literals **must be written explicitly**:

```
/regex/
```

Allowed contexts:

1. Rule patterns
2. Regex match expressions

```
$1 ~ /foo/
$0 !~ /error/
```

Regex is **never implicit** and is **not a string**.

---

## 8. Regex Specification (Minimal)

This is the **supported regex subset**.

### 8.1 Atoms

```
a        literal character
.        any single character
```

---

### 8.2 Character Classes

```
[abc]
[a-z]
[0-9]
```

* No negated classes (`[^ ]`)

---

### 8.3 Repetition

```
*    zero or more
+    one or more
?    zero or one
```

Applies to the immediately preceding atom or group.

---

### 8.4 Anchors

```
^    start of string
$    end of string
```

---

### 8.5 Grouping and Alternation (Optional)

```
(abc)
a|b
```

* Grouping only, no capturing

---

### 8.6 Explicitly Unsupported

* Backreferences
* Lookahead / lookbehind
* Multiline matching

Regex always matches **entirely within one line**.

---

## 9. Arithmetic Semantics (Clarified)

* Arithmetic **syntax** is parsed using expressions
* Arithmetic **evaluation** uses runtime state

Example:

```
{ sum = sum + $1 }
END { print sum }
```

* Variables persist across lines
* This is **stateful computation**, not grammar recognition

---

## 10. Lexer Token Specification

### 10.1 Keywords

```
BEGIN
END
if
print
```

---

### 10.2 Identifiers

```
[a-zA-Z_][a-zA-Z0-9_]*
```

Used for variables and built‑ins (`NR`, `NF`).

---

### 10.3 Numbers

```
[0-9]+(\.[0-9]+)?
```

---

### 10.4 Strings

```
"characters except double‑quote"
```

* No escape sequences required

---

### 10.5 Field References

```
$0
$1
$2
...
```

Tokenized as:

* `$` operator
* NUMBER

---

### 10.6 Operators

Arithmetic:

```
+  -  *  /
```

Assignment:

```
=
```

Comparison:

```
==  !=  <  >  <=  >=
```

Regex match:

```
~  !~
```

---

### 10.7 Punctuation

```
{  }
(  )
,  ;
```

---

### 10.8 Regex Literals

```
/regex/
```

Lexer rules:

* Regex literal is a **single token**
* Allowed only:

  * at rule start
  * after `~` or `!~`

This avoids ambiguity with division (`/`).

---

## 11. Error Handling (Minimal)

* Undefined variable → value `0`
* Invalid regex → compile error
* Division by zero → runtime error

---

## 12. Minimal Feature Boundary

Included:

* Regex filtering
* Arithmetic and variables
* BEGIN / END
* if statements

Excluded:

* Arrays
* Functions
* Loops (`while`, `for`)
* User‑defined functions

---

## 13. Implementation Hint (Non‑Normative)

Core components you need:

* Line reader
* Field splitter
* Lexer
* Expression evaluator
* Regex engine or library
* Global variable table
* Rule dispatcher

This document is **sufficient to start coding**.




# Awk-Lite CLI Structure

This document defines the **command-line interface (CLI)** for awk-lite.
It describes how the interpreter is invoked, how programs are supplied,
and how input files are handled.

This is a simplified model inspired by real awk, but intentionally smaller and clearer.

---

# 1. Basic Invocation

```
awk-lite [options] 'program' [file1 file2 ...]
```

If no files are provided, input is read from **stdin**.

---

# 2. Core Usage Modes

## 2.1 Inline Program

```
awk-lite '{ print $1 }' data.txt
```

* The program is passed directly as a string argument
* Files following the program are treated as input sources

---

## 2.2 Program From File

```
awk-lite -f program.awk data.txt
```

* `-f` specifies a file containing the awk-lite program
* Multiple `-f` flags may be allowed (optional feature)

---

## 2.3 Reading From Stdin

```
cat data.txt | awk-lite '{ print $1 }'
```

If no input files are given:

* Read from standard input

---

# 3. Supported Options

Minimal option set:

| Option          | Meaning                       |
| --------------- | ----------------------------- |
| `-f file`       | Read program from file        |
| `-v name=value` | Set variable before execution |
| `--help`        | Print usage info              |
| `--version`     | Print version info            |

---

# 4. Variable Initialization (-v)

Example:

```
awk-lite -v threshold=10 '$1 > threshold { print }' data.txt
```

Behavior:

* Variables defined with `-v` are set **before BEGIN runs**
* Values are parsed as numbers if numeric, otherwise strings

Multiple variables allowed:

```
awk-lite -v x=5 -v name="test" 'BEGIN { print x, name }'
```

---

# 5. Input File Handling

Rules:

* Files are processed **in order provided**
* `NR` continues increasing across files
* If a file cannot be opened → error and exit

Example:

```
awk-lite '{ print NR, $0 }' file1.txt file2.txt
```

---

# 6. Exit Codes

| Code | Meaning       |
| ---- | ------------- |
| 0    | Success       |
| 1    | Runtime error |
| 2    | Syntax error  |
| 3    | File error    |

---

# 7. Usage Message

Example help output:

```
Usage: awk-lite [options] 'program' [file ...]

Options:
  -f FILE        Read program from FILE
  -v NAME=VALUE  Set variable before execution
  --help         Show this message
  --version      Show version information
```

---

# 8. Argument Parsing Rules

1. Parse options first
2. Determine program source:

   * If `-f` used → load program from file
   * Else → first non-option argument is program string
3. Remaining arguments are input files
4. If no files → read stdin

---

# 9. Implementation Structure

Suggested CLI module responsibilities:

## 9.1 Argument Parser

* Parse options
* Validate argument combinations

## 9.2 Program Loader

* Load inline program OR file contents

## 9.3 Variable Preloader

* Apply `-v` assignments to variable store

## 9.4 Input Dispatcher

* Iterate over input files or stdin

---

# 10. Minimal Implementation Order

1. Inline program only
2. Add stdin support
3. Add file arguments
4. Add `-f`
5. Add `-v`
6. Add help/version

---

# 11. Completion Criteria

Your CLI is complete when:

* Inline programs work
* Program files work
* Multiple input files work
* Variables via `-v` work
* Proper exit codes are returned

---

This document defines the CLI boundary of awk-lite.
It is intentionally minimal but structurally complete.
