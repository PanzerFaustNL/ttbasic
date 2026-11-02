# TT BASIC Studio 0.5 — English manual

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
- The project tree lists project files and discovered Imports dependencies.
- Click MAIN.BAS or an imported file to edit it. Each open file retains its edits and cursor position.
- Click the project name to collapse or expand the tree. The arrows at the bottom scroll a long file list.
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
| New project | File → New project, Ctrl+N |
| Open project | File → Open project, Ctrl+O |
| New source file | File → New file, F7 |
| Add an existing file | File → Add existing file, F8 |
| Save all files and project | File → Save all, Ctrl+Shift+S |
| Reload active file from disk | F3 |
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

Saving first writes a `.TMP` file and preserves the previous disk version as `.BAK`. For COUNTER.BAS these are COUNTER.TMP and COUNTER.BAK. Save modified modules individually by selecting them and pressing F2; File → Save all saves all modified open files and the project layout.

On exit the IDE checks all open files for unsaved changes. Cancel closing to save them first. New project creates a separate directory with a fresh MAIN.BAS. Open project loads a .TBP file. Examples use the installed DEFAULT project. General source-file Save As and rename operations are not implemented.

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

Semicolons combine items on **one output line**, without adding spaces. Include required spaces inside string literals. Each Print ends the line, even with a trailing semicolon. Output lines are limited to 79 characters. Declare string variables with `Dim text As String`. Concatenate two strings with `&` or `+`. Double embedded quotes, for example `"A ""quote"""`. Strings hold at most 127 characters.

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

`value` is a numeric field stored separately in each instance. Fields can also contain strings, booleans and object or collection references. Fields start at zero, or at an integer literal default such as `Public Value As Integer = 10`. Public members are accessible from outside the class. Private members can only be accessed from methods of the same class. Declare visibility explicitly; members default to public in this implementation.

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

An object variable declared without New starts as Nothing. Accessing its members before construction is an error. An object variable accepts Nothing or an instance compatible with its declared class or interface, including derived classes. Assigning an object of another class is rejected.

Objects remain allocated until the end of the run. Assigning Nothing removes a reference but does not reclaim an object slot in this version. Each F5 run starts with a fresh object pool.

## 7. Subs, Functions and parameters

A Sub performs actions without returning a value. A Function returns a typed value using **Return**. Both can be class methods or global procedures, including definitions in imported files.

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

Call is optional: `ShowNumber(36)` also works. Arguments must be enclosed in parentheses. Up to eight typed parameters are supported. Numbers and strings are passed by value; objects, arrays and collections share references. Argument types and counts must match. Types include Integer/Long, Boolean, String, Variant, Object, Collection, classes and interfaces. Declare array parameters as `items() As Integer`.

Functions must return a value matching their declared result type. Assigning to the function name is not a substitute for Return. A Sub can return early with a bare Return or Exit Sub. Using a Sub as part of an expression is an error. A standalone End terminates the main program and cannot replace Return inside a method.

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
| `Function finished without Return` | Add a Return matching the result type |
| `Sub cannot be used as an expression` | Use a Function when a result is required |
| `Unknown variable` | Declaration, spelling and scope |
| `Division by zero` | Divisor must not be zero |
| `Result exceeds numeric range` | Intermediate results must stay within ±1,000,000 |
| `Maximum ...` / `exceeds ...` | Version limits below |

## 9. Limits and features not yet implemented

| Item | Limit |
| --- | --- |
| Project files, including MAIN.BAS | 16 |
| Lines per file | 256 |
| Total loaded source lines | 2048 |
| Characters per source line | 159, ASCII |
| Identifier length | 23 characters |
| Classes | 16 |
| Numeric fields per class | 16 |
| Class methods plus global procedures | 64 |
| Object instances per run | 64 |
| Variables per global/local scope | 64 |
| Parameters per method/procedure | 8, typed |
| Nested method calls | 8 |
| Nested expressions | 16 |
| Numeric range | −1,000,000 to +1,000,000 |
| Characters per output line | 79 |

Not yet implemented: multidimensional arrays, generic collections, overloading, garbage collection, exception handling and the visual form designer. BASIC programs are not compiled to standalone PRG files. This is its own BASIC dialect rather than a complete VB implementation.

## 10. Project source layout

`src/IDE.C` implements the IDE; `src/BASIC.C` implements the interpreter. Native menus live in `src/MENU.C`, translations in `src/LANG.C`. Pure C project and linker files are in `atari/`. Copy BASIC examples from `examples/` into C:\TTBASIC for use on the Atari.

When reporting a problem in the GitHub repository, include the Studio version, a minimal BASIC example, the complete diagnostic and whether you use Hatari or physical Atari hardware.

## 11. Version 0.4: control flow and data

Use **Project → Load language example**, **Load polymorphism example** or **Load input example**, followed by F5. These load FLOW.BAS, POLY.BAS and INPUT.BAS. The polymorphism example also uses ANIMAL.BAS, DOG.BAS and ISHOW.BAS.

### If, ElseIf and Else

```vb
Dim age As Integer = 21
If age < 12 Then
    Print "Child"
ElseIf age < 18 Then
    Print "Junior"
Else
    Print "Adult"
End If
```

Blocks can be nested. An inline form supports one statement per branch: `If age >= 18 Then Print "Adult" Else Print "Junior"`. Use block syntax for nested conditions.

Boolean values are True (-1) and False (0). And, Or, Xor and Not operate on truth values. Both operands of And/Or are evaluated; use nested If blocks when the second expression is only safe under a condition.

### While and Do

```vb
Dim t As Integer
While t < 3
    Print t
    t = t + 1
Wend

Do
    t = t - 1
Loop Until t = 0
```

End While is an alternative to Wend. Do supports a While or Until condition either on the Do line or on the Loop line, but not both. A precondition may skip the first iteration; a postcondition is checked after an iteration. A plain Do/Loop can be terminated with Exit Do.

### For and For Each

```vb
Dim total As Integer
For i As Integer = 6 To 1 Step -1
    total = total + i
Next i
Print total
```

The end value is inclusive. Step defaults to 1 and may be negative, but not zero. Start, end and step are evaluated on entry. Next may include the matching variable name. Use a numeric counter.

```vb
Dim names(1) As String
names(0) = "Atari"
names(1) = "TT"
For Each name As String In names
    Print name
Next name
```

For Each iterates over arrays and collections. A new iterator without As is Variant; a typed iterator checks every element. Exit For/While/Do exits a loop; Continue For/While/Do advances to its next iteration. These also work inside nested If blocks and cannot escape from a different method invocation.

### Select Case

```vb
Dim score As Integer = 7
Select Case score
Case 0, 1
    Print "Low"
Case 2 To 7
    Print "Middle"
Case Is >= 8
    Print "High"
Case Else
    Print "Other"
End Select
```

The selector is evaluated once. Only the first matching branch runs; there is no fall-through. Case Else must be last. Numeric and case-sensitive string comparisons are supported. Exit Select leaves the current Select block.

### Arrays and collections

`Dim values(2) As Integer` allocates three elements indexed 0, 1 and 2. `Dim values(1 To 3) As Integer` uses bounds 1 through 3. Read or assign using `values(1)`. LBound and UBound return bounds; Count or `.Count` returns the number of elements.

```vb
Dim values() As Integer
ReDim values(1)
values(1) = 42
ReDim Preserve values(3)
Print values(1)
```

ReDim replaces a dynamic array. Preserve retains existing elements and cannot change the lower bound. `Array(1, 2, 3)` creates a Variant array. Arrays are one-dimensional and hold at most 128 elements. Assignment and parameter passing share their reference. ReDim assigns a new array to its variable; other references still point to the old array.

```vb
Dim values As New Collection
values.Add("TT")
values.Add(42)
Print values.Item(1)
Print values(2)
Print values.Count
values.Remove(1)
values.Clear()
```

Collections are ordered, untyped and indexed from 1. Add appends, Remove removes an index, and Clear empties the collection. Up to 128 values, including objects, are supported. Structurally modifying a collection during For Each raises an error; exit the loop first. Each run permits 32 array/collection allocations, including ReDim; storage is not reclaimed during a run.

### Strings and input

String variables, parameters and results are supported. Available helpers are Len, CStr/Str, CInt/Val, Left, Right, Mid, UCase, LCase and Abs. Mid positions start at 1. CInt/Val require an entirely valid integer within the supported range. Mod returns an integer division remainder.

```vb
Dim name As String
Input "Your name?"; name
Dim age As Integer
Input "Your age?"; age
Print "Hello " & name
Print age + 1
```

Input opens a GEM dialog in the IDE. Its field accepts 63 characters; language String values can contain 127. Invalid numeric input or Cancel stops execution with a diagnostic. A Variant target becomes numeric when conversion succeeds, otherwise String; a Variant already holding a String continues to receive text.

## 12. Inheritance, interfaces and polymorphism

A class may have one base class, using Inherits Animal inside the class or on its Class header. Public members are accessible externally, Private members only inside their defining class, and Protected members also inside derived classes.

```vb
Class Animal
    Public Overridable Function Speak() As String
        Return "animal"
    End Function
End Class

Class Dog
    Inherits Animal
    Public Overrides Function Speak() As String
        Return "woof"
    End Function
End Class

Dim actor As Animal = New Dog
Print actor.Speak()
```

This prints woof. The variable is declared as Animal but refers to a Dog. Mark the base method Overridable and its replacement Overrides. Names, parameter types and result types must match. Public visibility cannot be reduced. MyBase calls the base implementation directly.

A derived constructor may call `MyBase.New(...)` as its first executable statement. Otherwise the parameterless base constructor is called automatically. If the base constructor requires arguments, the derived constructor must pass them explicitly. POLY.BAS provides a complete working example.

```vb
Interface IShow
    Function Speak() As String
End Interface
```

A class declares Implements IShow and provides a public method with the same name and signature. Interfaces contain signatures only: no End Function/End Sub per member. End Interface closes the contract. Interfaces cannot be instantiated with New. An interface variable can hold any implementing class, and calls dispatch to the concrete implementation.

Explicit mapping to another method name is also supported:

```vb
Public Function Display() As String Implements IShow.Speak
    Return "shown"
End Function
```

This method belongs inside a class declaring Implements IShow. A class can directly implement up to four comma-separated interfaces. Inherited contracts are included and validated. An interface can inherit one other interface. Missing or mismatched implementations are rejected before execution.

`TypeOf actor Is Dog` checks the runtime type. `actor Is other` and `actor IsNot Nothing` compare object references. Classes and interfaces can be parameter types, result types and array element types. Object is a general object-reference type. Variant can contain numbers, strings, objects and containers.

## 13. Reserved words and execution limits

A central registry rejects implemented language keywords as declared names. It covers structural keywords, types, visibility, object keywords, logical operators, True/False/Nothing and Input. New is allowed only as a constructor name. Helpers such as Len and Count are contextual function names, not language keywords.

Additional 0.4 limits: 16 classes/interfaces total, 16 fields per class including inherited fields, 64 methods/procedures, 64 object instances, 64 variables per scope, eight parameters and 32 nested blocks. Arrays/collections allow 32 allocations of up to 128 elements; strings hold 127 characters. Execution stops after 1,000,000 steps to bound runaway programs. Press **Escape** while a program runs to cancel sooner; use Cancel in an input dialog. The Pure C build uses a 512 KiB stack.

## 14. Managing projects and source files (0.5)

### Starting a new project

1. Select **File → New project**, or Ctrl+N.
2. Enter one to eight letters/digits, for example `HELLO`.
3. If files have changes, choose Save, Discard or Cancel.
4. Studio creates `C:\TTBASIC\PROJECTS\HELLO`, containing `HELLO.TBP` and a fresh `MAIN.BAS`.
5. The project opens immediately. Press F5 to run the initial Hello program.

An existing directory with the same name is never overwritten. Eight-character names fit TOS filename constraints. You may move the complete project directory later and open its .TBP at the new location.

### Adding a new file

Select **File → New file**, or F7. Enter `MODEL` or `MODEL.BAS`. The source is created inside the active project directory, recorded in the .TBP and opened in the editor. Existing names are rejected. A new file initially contains a comment.

Project membership organizes sources; **Imports determines which modules participate in execution**. To use a Model class from MODEL.BAS, write this in MAIN.BAS:

```vb
Imports "MODEL.BAS"
Dim item As New Model
Print item.GetValue()
End
```

MODEL.BAS could contain:

```vb
Class Model
    Public Function GetValue() As Integer
        Return 42
    End Function
End Class
```

### Adding an existing source

Select **File → Add existing file**, or F8, and choose a .BAS file in the native GEM selector. A source outside the project directory is copied into it; the original is preserved. A file already inside that directory is registered without copying. Add any source dependencies as well: relative Imports resolve from the copied file's new directory. Conflicting filenames are never overwritten. Use Imports in the entry source to include the module.

### Saving and reopening

- F2 saves only the active source.
- **File → Save all**, Ctrl+Shift+S, saves every modified open source and the project layout.
- **File → Open project**, Ctrl+O, selects a .TBP file. This is different from the Pure C build project TTBASIC.PRJ.
- F3 reloads only the active source file.
- Switching projects prompts for pending edits.
- The last project is restored on the next application start. If unavailable, Studio tries the DEFAULT project.

The tree contains registered files and discovered Imports dependencies. Click a filename to switch editors. Bottom arrows scroll the list. F5 always runs the active project's entry source, using in-memory versions of open files.

### Manifest format

.TBP files are plain text with a format version, project name, entry source and members:

```text
TTBASIC_PROJECT 1
NAME=HELLO
ENTRY=MAIN.BAS
FILE=MAIN.BAS
FILE=MODEL.BAS
```

Paths inside the project directory are stored relatively, allowing the directory to be moved as a unit. The entry source must also be a member. At most sixteen files may be registered. Opening validates registered sources; a missing or unreadable file prevents replacing the current workspace.

Membership changes save immediately. `.PTM` is a temporary manifest and `.PBK` preserves the previous layout. Source files keep their own `.TMP` and `.BAK`. Loading examples switches safely to the installed DEFAULT project before opening the example.
