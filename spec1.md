# Awk‑Lite Modular Architecture

This document decomposes **awk‑lite** into **independent, buildable modules**.
Each module can be implemented, tested, and reasoned about **in isolation**, then composed into a working interpreter.

The intent is to guide **incremental development** and avoid monolithic design.

---

## 1. High‑Level Module Map

```
┌─────────────┐
│ Input Reader│
└──────┬──────┘
       ↓
┌─────────────┐
│ Line Context│  ($0, NR)
└──────┬──────┘
       ↓
┌─────────────┐
│ Field Split │  ($1..$NF)
└──────┬──────┘
       ↓
┌─────────────┐
│ Rule Engine │
└──────┬──────┘
       ↓
┌─────────────┐
│ Action Exec │
└─────────────┘
```

Supporting modules:

* Lexer
* Expression Parser / Evaluator
* Regex Engine
* Variable Store

---

## 2. Input Reader Module

### Responsibility

* Read input line‑by‑line from file or stdin

### Interface

* `next_line() -> string | EOF`

### Guarantees

* Returned line ends with `\n` stripped
* Preserves original text exactly

### Notes

* No parsing here
* No global state

---

## 3. Line Context Module

### Responsibility

* Maintain per‑line built‑ins

### State

* `$0` → full line
* `NR` → line counter

### Interface

* `set_line(string)`
* `get_NR()`

### Notes

* Updated once per input line

---

## 4. Field Splitter Module

### Responsibility

* Split `$0` into whitespace‑separated fields

### State

* `$1 .. $NF`
* `NF`

### Interface

* `split_fields(string)`
* `get_field(i)`

### Notes

* Whitespace = space or tab
* Empty fields ignored

---

## 5. Lexer Module

### Responsibility

* Convert program text into tokens

### Produces Tokens

* Keywords (`BEGIN`, `END`, `if`, `print`)
* Identifiers
* Numbers
* Strings
* Regex literals
* Operators
* Punctuation

### Key Rule

* Regex literals only allowed:

  * at rule start
  * after `~` or `!~`

### Output

* Token stream

---

## 6. Parser Module

### Responsibility

* Build internal representation of program rules

### Parses

* Rules: `pattern { action }`
* BEGIN / END blocks
* Expressions
* Statements

### Output

* Rule list (AST or structured nodes)

### Notes

* No execution here

---

## 7. Rule Engine Module

### Responsibility

* Control execution order

### Behavior

1. Execute BEGIN rules
2. For each input line:

   * Evaluate patterns
   * Dispatch matching actions
3. Execute END rules

### Interface

* `run(program)`

---

## 8. Regex Engine Module

### Responsibility

* Match regex patterns against strings

### Interface

* `regex_match(regex, string) -> bool`

### Supported Features

* Literals, `.`, character classes
* `* + ?`
* `^ $`
* Grouping and alternation (optional)

### Notes

* Stateless
* Line‑local only

---

## 9. Expression Parser Module

### Responsibility

* Parse arithmetic and boolean expressions

### Handles

* Arithmetic: `+ - * /`
* Comparisons: `== != < > <= >=`
* Regex match: `~ !~`

### Output

* Expression tree

---

## 10. Expression Evaluator Module

### Responsibility

* Evaluate expressions at runtime

### Inputs

* Expression tree
* Variable store
* Field values

### Outputs

* Numeric or string value

### Notes

* Dynamic typing
* Automatic string → number conversion

---

## 11. Variable Store Module

### Responsibility

* Store global variables

### Behavior

* Variables created on first assignment
* Default value = `0`

### Interface

* `get(name)`
* `set(name, value)`

### Notes

* Hash map or dictionary

---

## 12. Action Executor Module

### Responsibility

* Execute statements in action blocks

### Handles

* `print`
* assignment
* `if` statements

### Interface

* `execute(action)`

---

## 13. Output Module

### Responsibility

* Handle printing and formatting

### Behavior

* Space‑separated fields
* Newline‑terminated output

---

## 14. Error Handling Module

### Responsibility

* Centralize error reporting

### Handles

* Syntax errors
* Invalid regex
* Division by zero

---

## 15. Suggested Build Order

1. Input Reader
2. Line Context + Field Splitter
3. Variable Store
4. Regex Engine (or library binding)
5. Lexer
6. Expression Parser + Evaluator
7. Action Executor
8. Rule Engine

---

## 16. Dependency Summary

```
Lexer → Parser → Rule Engine
                    ↓
     Regex Engine ← Evaluator → Variable Store
```

---

## 17. Design Principle

> **Each module should be testable without running the full interpreter.**

This keeps the system understandable and debuggable.

---

## 18. Completion Criteria

You have a working awk‑lite when:

* BEGIN / END execute correctly
* Regex filters lines
* Arithmetic accumulates state
* Output matches reference behavior

---

This document is intended as a **development roadmap**, not a language spec.
