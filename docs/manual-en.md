# TT BASIC Studio 0.3 — English manual

TT BASIC Studio combines a BASIC interpreter with a GEM development environment for the Atari TT. Both the IDE and your BASIC program run on the Atari itself. This version was built with Pure C and tested in Hatari with TOS 3.06, TT Medium (640 × 480, 16 colours), 8 MiB ST-RAM and 256 MiB TT-RAM. Physical TT hardware testing is still pending.

This is an early language implementation. This manual describes **currently implemented features**, not the complete Microsoft Visual Basic language. BASIC keywords and interpreter diagnostics remain English in every UI language.

[Nederlands](handleiding-nl.md) · [Deutsch](handbuch-de.md)

## 1. Installation

1. Place the built `TTBASIC.PRG` in `C:\TTBASIC` on the Atari.
2. Copy the examples `MAIN.BAS`, `OODEMO.BAS`, `COUNTER.BAS` and optionally `MATH.BAS` to that directory. Keep an existing MAIN.BAS if it contains your own program.
3. Launch `C:\TTBASIC\TTBASIC.PRG` from the GEM desktop.
4. Use TT Medium or a screen resolution of at least 640 × 480.

In the configured Hatari installation, Atari directory `C:\TTBASIC` maps to the Mac directory `Atari-TT/Schijf-C/TTBASIC`. The repository contains this project's sources, examples and build instructions. Install Hatari, TOS and Pure C separately. See [building on the Atari](../README.md#building-on-the-atari).

## 2. Using the IDE

The native GEM menu bar is at the top of the screen. The project tree is on the left, the editor in the centre, and output at the bottom. Show the properties panel using **View → Properties**.

- `MAIN.BAS` is the project entry point.
- The **Imports** group lists files loaded by the project.
- Click MAIN.BAS or an imported file to edit it. Each open file retains its edits and cursor position.
- Click `- Demo` / `+ Demo` to collapse or expand the project tree.
- An asterisk on the file tab means that file has unsaved changes.
- **F5 always runs MAIN.BAS and its dependencies**, even when the active editor displays an imported file.
- F5 uses the in-memory versions of open files; saving first is not required.
- Every run starts with fresh variables and objects. Runtime state does not survive between runs.

The dependency list is refreshed when loading, saving and running. After changing an Imports statement, F5 updates the tree. Files already open in the IDE keep their in-memory contents; use F3 to reload changes made by an external editor.

### Switching languages

Choose **View → Nederlands / English / Deutsch**. View is called **Beeld** in Dutch and **Ansicht** in German. The selected language is stored in `C:\TTBASIC\STUDIO.CFG`. Source programs remain identical in all languages. German UI text uses ASCII spellings such as `ue` because the original Atari system font is used.

### Menus and shortcuts

| Action | Command |
| --- | --- |
| New empty entry file | File → New MAIN.BAS, Ctrl+N |
| Reload active file from disk | F3, Ctrl+O |
| Save active file | F2, Ctrl+S |
| Run project | Project → Run project, F5 |
| Load the OO example into the entry editor | Project → Load OO example |
| Undo last edit | Ctrl+Z |
| Copy / cut / paste entire current line | Ctrl+C / Ctrl+X / Ctrl+V |
| Find in active file | Ctrl+F |
| Find next | F6 |
| Show/hide properties | F4 |
| Show/hide panels | View |
| Shortcut reference | F1 |
| Quit | Ctrl+Q |

Search ignores case and wraps to the beginning. The clipboard holds **a whole line** and is internal to the IDE; there is no arbitrary text selection or system clipboard integration. Pasting inserts a line, or replaces the current line if it is empty. Undo restores one edit and is cleared by saving, reloading or switching documents. Redo is not implemented.

### Saving and closing

Saving first writes a `.TMP` file and preserves the previous disk version as `.BAK`. For COUNTER.BAS these are COUNTER.TMP and COUNTER.BAK. Save modified modules individually by selecting them and pressing F2; there is no Save All yet.

On exit the IDE checks all open files for unsaved changes. Cancel closing to save them first. New MAIN.BAS and Load OO example change the editor; the disk entry file is replaced only when you save it. The project directory is currently fixed at `C:\TTBASIC`; general Open and Save As dialogs are not implemented.

## 3. Your first program

Enter this in MAIN.BAS and press F5:

```vb
Print "Hello Atari TT!"
Dim answer As Integer = 6 * 7
Print answer
End
```

Output:

```text
Hello Atari TT!
42
Program finished.
```

The panel shows the last four output lines. Errors stop execution. Output is textual; the interpreter does not yet create a standalone GEM application or a visual form.

## 4. Numbers, variables and comments

Identifiers are case-insensitive: `answer`, `ANSWER` and `Answer` refer to the same variable. Names start with a letter or underscore and continue with letters, digits or underscores. Use at most 23 characters and avoid BASIC keywords as identifiers.

```vb
Dim count As Integer
Dim total As Long = 40
count = 2
Let total = total + count
Print total
Rem A comment
' Another comment
Print total ' A trailing comment
```

`Let` is optional. `Integer` and `Long` currently represent the same bounded numeric type: whole numbers from **−1,000,000 to +1,000,000**. This is an interpreter limit, not the original VB Integer width or C's entire 32-bit range.

Numeric variables begin at zero. For compatibility, undeclared A through Z can also be read as zero. Assignment to a previously unknown identifier creates a numeric variable. Reading other unknown names is an error; prefer explicit Dim declarations.

Duplicate Dim declarations in one scope are rejected. Parameters and locally declared variables belong to the current method invocation. A local Dim can shadow a global variable. Assignment uses an existing global variable when no local variable with that name exists.

### Operators

| Operator | Meaning |
| --- | --- |
| `+`, `-` | Addition, subtraction and unary signs |
| `*`, `/` | Multiplication and integer division |
| `(...)` | Grouping |
| `=`, `<>`, `<`, `>`, `<=`, `>=` | Numeric comparisons |

Multiplication and division take precedence over addition and subtraction. `7 / 2` returns `3`; `7 / -2` returns `-3`. Comparisons return `-1` for true and `0` for false. Division by zero and results outside the numeric range produce errors.

```vb
Print 2 + 3 * 4
Print (2 + 3) * 4
Print 42 >= 40
```

Results: `14`, `20`, `-1`.

### Print and text

```vb
Print "Value: "; total
Print "Sum: "; 2 + 3; "!"
Print
```

Semicolons combine items on **one output line**, without adding spaces. Include required spaces inside string literals. Each Print ends the line, even with a trailing semicolon. Output lines are limited to 79 characters. Strings currently exist only as literals in Print; string variables, concatenation using `+`, and escaped quote sequences are not implemented.

## 5. Imports and external source files

```vb
Imports "COUNTER.BAS"
```

In TT BASIC, Imports loads **BASIC source files**. This is an intentional dialect choice; Microsoft VB normally uses Imports for namespaces.

- Put Imports outside Class, Sub and Function definitions.
- Quote the filename with double quotes.
- Relative paths are resolved from the file containing the Imports statement.
- Subdirectories work, for example `Imports "LIB\MATH.BAS"`. Use TOS-compatible 8.3 filenames.
- Classes and procedures are registered before execution, allowing forward calls.
- Each file is loaded once per run, even when imported repeatedly or shared by multiple dependencies.
- Circular dependencies produce `Circular Imports dependency`.
- Top-level statements in an imported file execute once at the import position. Library files should normally contain definitions only: a standalone End in a library would stop the entire project.

A small library, MATH.BAS:

```vb
Function DoubleValue(value As Integer) As Integer
    Return value * 2
End Function
```

MAIN.BAS:

```vb
Imports "MATH.BAS"
Print DoubleValue(21)
End
```

## 6. Classes and objects

A class defines the data and operations of an object. New creates an independent instance. Save this definition in COUNTER.BAS:

```vb
Class Counter
    Private value As Integer

    Public Sub New(start As Integer)
        Me.value = start
    End Sub

    Public Sub Add(amount As Integer)
        Me.value = Me.value + amount
    End Sub

    Public Function GetValue() As Integer
        Return Me.value
    End Function
End Class
```

### Fields and visibility

`value` is a numeric field stored separately in each instance. Fields start at zero, or at an integer literal default such as `Public Value As Integer = 10`. Public members are accessible from outside the class. Private members can only be accessed from methods of the same class. Declare visibility explicitly; members default to public in this implementation.

Direct access to `first.value` is therefore an error for the example class. Use `first.GetValue()`. Public fields support both reads and assignments, such as `first.Value = 42` for a public Value field.

### Constructors and Me

`Public Sub New(...)` is a constructor, automatically invoked by `New Counter(...)`. A class without a constructor accepts `New Counter` or `New Counter()` without arguments. If a constructor exists, its argument count must match.

Me refers to the current object and is only valid inside a class method. Use `Me.value` and `Me.MethodName(...)` to explicitly access its members.

### Independent objects and shared references

MAIN.BAS:

```vb
Imports "COUNTER.BAS"
Dim first As New Counter(10)
Dim second As New Counter(2)
Dim alias As Counter
alias = first

first.Add(5)
alias.Add(27)

Print "First: "; first.GetValue()
Print "Second: "; second.GetValue()
Print "Alias: "; alias.GetValue()
End
```

The results are `First: 42`, `Second: 2`, `Alias: 42`. First and second are independent instances. `alias = first` copies a **reference**, so those two variables subsequently refer to the same object.

This example is included as OODEMO.BAS. Select **Project → Load OO example**, then F5. Your previous MAIN.BAS on disk remains intact until you explicitly save the example over it.

### Nothing and type checking

```vb
Dim counter As Counter
counter = New Counter(5)
counter = Nothing
```

An object variable declared without New starts as Nothing. Accessing its members before construction is an error. An object variable accepts only Nothing or an instance of its declared class. Assigning an object of another class is rejected.

Objects remain allocated until the end of the run. Assigning Nothing removes a reference but does not reclaim an object slot in this version. Each F5 run starts with a fresh object pool.

## 7. Subs, Functions and parameters

A Sub performs actions without returning a value. A Function returns a number using **Return**. Both can be class methods or global procedures, including definitions in imported files.

```vb
Sub ShowNumber(value As Integer)
    Print "Number: "; value
End Sub

Function Square(value As Integer) As Integer
    Dim result As Integer = value * value
    Return result
End Function

Call ShowNumber(Square(6))
End
```

Call is optional: `ShowNumber(36)` also works. Arguments must be enclosed in parentheses. Up to four numeric parameters are passed by value, and argument counts must match exactly. Use Integer or Long; object and string parameters are not supported yet.

Functions must return a numeric value. Assigning to the function name is not a substitute for Return. A Sub can return early with a bare Return or Exit Sub. Using a Sub as part of an expression is an error. A standalone End terminates the main program and cannot replace Return inside a method.

## 8. Understanding errors

Errors identify the source file, physical line number and English cause:

```text
COUNTER.BAS:9: Division by zero
```

Line numbers include blank lines and comments. The editor highlights the relevant line when that file is active. If MAIN.BAS is active and an imported file fails, the editor points to the Imports statement while output identifies the actual failing file and line.

| Diagnostic fragment | Check |
| --- | --- |
| `Cannot open imported file` | Filename, directory and path |
| `Circular Imports dependency` | Direct or indirect import cycle |
| `Unknown Class` | Spelling, Imports and complete Class definition |
| `Private member` | Call a public method instead of accessing a private member |
| `Object is Nothing` | Construct the object with New first |
| `Wrong number of method arguments` | Constructor or method signature |
| `Object Class does not match` | Object must match the declared class |
| `Function finished without Return` | Add a numeric Return |
| `Sub cannot be used as an expression` | Use a Function when a result is required |
| `Unknown variable` | Declaration, spelling and scope |
| `Division by zero` | Divisor must not be zero |
| `Result exceeds numeric range` | Intermediate results must stay within ±1,000,000 |
| `Maximum ...` / `exceeds ...` | Version limits below |

## 9. Limits and features not yet implemented

| Item | Limit |
| --- | --- |
| Project files, including MAIN.BAS | 16 |
| Lines per file | 64 |
| Total loaded source lines | 512 |
| Characters per source line | 95, ASCII |
| Identifier length | 23 characters |
| Classes | 8 |
| Numeric fields per class | 8 |
| Class methods plus global procedures | 32 |
| Object instances per run | 32 |
| Variables per global/local scope | 48 |
| Parameters per method/procedure | 4, numeric |
| Nested method calls | 8 |
| Nested expressions | 16 |
| Numeric range | −1,000,000 to +1,000,000 |
| Characters per output line | 79 |

Not yet implemented: If/Then, While/Wend, For/Next, interactive input, arrays, string variables, inheritance, interfaces, polymorphism, overloading, object fields/object parameters, garbage collection and the visual form designer. The interpreter does not compile BASIC programs into standalone PRG files. Existing VB programs must be adapted to this supported subset.

## 10. Project source layout

`src/IDE.C` implements the IDE; `src/BASIC.C` implements the interpreter. Native menus live in `src/MENU.C`, translations in `src/LANG.C`. Pure C project and linker files are in `atari/`. Copy BASIC examples from `examples/` into C:\TTBASIC for use on the Atari.

When reporting a problem in the GitHub repository, include the Studio version, a minimal BASIC example, the complete diagnostic and whether you use Hatari or physical Atari hardware.
