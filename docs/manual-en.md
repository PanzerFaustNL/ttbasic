# TT BASIC Studio 0.5 — English manual

TT BASIC Studio combines a BASIC interpreter with a GEM development environment for the Atari TT. Both the IDE and your BASIC program run on the Atari itself. This version was built with Pure C and tested in Hatari with TOS 3.05, TT Medium (640 × 480, 16 colours), 8 MiB ST-RAM and 256 MiB TT-RAM. Physical TT hardware testing is still pending.

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
| Add an existing file | File → Add existing file, Ctrl+F7 |
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

Drag the grey divider above the output panel up or down to resize it. The panel shows the most recent lines that fit (up to ten). Errors stop execution. Print writes text to this panel. Create native GEM windows with the GUI objects in section 16; standalone compilation is not supported yet.

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

`Let` is optional. Since 0.10, `Integer` and `Long` are both signed 32-bit integers: **−2,147,483,648 to +2,147,483,647**. This dialect does not use VB's 16-bit Integer. `Single` and `Double` support fractional values with a decimal point. See chapter 20 for ranges, conversions and migration.

Numeric variables begin at zero. For compatibility, undeclared A through Z can also be read as zero. Assignment to a previously unknown identifier creates a Variant variable. Reading other unknown names is an error; prefer explicit Dim declarations.

Duplicate Dim declarations in one scope are rejected. Parameters and locally declared variables belong to the current method invocation. A local Dim can shadow a global variable. Assignment uses an existing global variable when no local variable with that name exists.

### Operators

| Operator | Meaning |
| --- | --- |
| `+`, `-` | Addition, subtraction and unary signs |
| `*`, `/` | Multiplication and floating-point division |
| `\`, `Mod` | Integer division and remainder, requiring 32-bit whole operands |
| `^` | Exponentiation |
| `(...)` | Grouping |
| `=`, `<>`, `<`, `>`, `<=`, `>=` | Numeric comparisons |

Precedence: parentheses, powers (right associative), unary signs, `*`/`/`, `\`, `Mod`, `+`/`-`. `7 / 2` returns `3.5`; `7 \ 2` returns `3`. Comparisons return `-1` for true and `0` for false. Division by zero, overflow and underflow produce errors.

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
| `Result exceeds numeric range` | Intermediate results must stay within the numeric range; see chapter 20 |
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
| Numeric range | Integer/Long: −2,147,483,648 to +2,147,483,647; Double: see chapter 20 |
| Characters per output line | 79 |

Not yet implemented: multidimensional arrays, generic collections, overloading, garbage collection, exception handling. See section 18 for the form designer. BASIC programs are not compiled to standalone PRG files. This is its own BASIC dialect rather than a complete VB implementation.

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

String variables, parameters and results are supported. Available helpers are Len, CStr/Str, CInt/CLng/CDbl/CSng/Val, Left, Right, Mid, UCase, LCase and Abs. Mid positions start at 1. Val/CDbl accept complete decimal text, including exponents. CInt/CLng round to a 32-bit integer, with ties to even. Int rounds down; Fix truncates toward zero. Mod returns an integer division remainder.

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


```text
IF THEN ELSE ELSEIF END WHILE WEND FOR EACH IN TO STEP NEXT SELECT CASE IS
ISNOT DO LOOP UNTIL EXIT CONTINUE DIM AS NEW CLASS INTERFACE INHERITS
IMPLEMENTS PUBLIC PRIVATE PROTECTED OVERRIDABLE OVERRIDES SUB FUNCTION RETURN
CALL LET PRINT INPUT REM IMPORTS INTEGER LONG SINGLE DOUBLE BOOLEAN STRING
VARIANT OBJECT COLLECTION TRUE FALSE NOTHING ME MYBASE AND OR NOT XOR MOD
REDIM PRESERVE TYPEOF ARRAY ADDHANDLER REMOVEHANDLER ADDRESSOF STOP
```

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

Select **File → Add existing file**, or Ctrl+F7, and choose a .BAS file in the native GEM selector. A source outside the project directory is copied into it; the original is preserved. A file already inside that directory is registered without copying. Add any source dependencies as well: relative Imports resolve from the copied file's new directory. Conflicting filenames are never overwritten. Use Imports in the entry source to include the module.

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

## 15. Code completion (0.6)

Press **Ctrl+Space** in the editor for suggestions. Keep typing to filter, select with **Up/Down**, and insert with **Enter** or **Tab**. **Escape** closes the list. Completion replaces the current word and preserves the rest of the line. Incomplete or invalid statements do not prevent suggestions.

Typing a dot opens member suggestions automatically. For example, `third.` shows public Reken members after `Dim third As New Reken(10)` and `Imports "REKEN.BAS"`. Inside a class, `Me.` also includes its private fields. Suggestions include inherited public members, method parameters, local variables, classes, BASIC keywords and Collection members. The selected member's type or method signature appears below the list. Completion inserts the name; enter parentheses and arguments yourself, for example `third.GetValue()`. Constructor arguments are also entered manually. Open modules use their unsaved contents; imports do not need to be executed first.

This first version handles simple receivers such as `third.`, `Me.` and `MyBase.`. Chained expressions such as `items(1).` and `factory.Create().`, and type inference from assignments, are not supported yet. Use explicit `Dim ... As ...` declarations. The index supports up to 16 files and 512 declarations, with up to 64 suggestions. Suggestions are suppressed inside strings and comments.

## 16. Native GEM windows and events (0.7)

Choose **Project → Load GEM example**, then F5, or open GEMDEMO.TBP. It creates a native GEM window containing a text field, checkbox, buttons and label. Create controls in BASIC code or with the form designer in section 18.

```basic
Dim form As New Window("My program", 320, 160)
Dim nameBox As New TextBox(form, "Marco", 16, 16, 240, 24)
Dim buttonA As New Button(form, "Greet", 16, 56, 120, 28)
Dim message As New Label(form, "Ready", 16, 112, 280, 24)
AddHandler buttonA.Click, AddressOf Greet
form.Show()
Application.Run()
End

Sub Greet(sender As Button)
    message.Text = "Hello " & nameBox.Text
End Sub
```

`New Window(title, width, height)` takes the client-area size in pixels. Button, Label, TextBox and CheckBox use `(form, text, x, y, width, height)`, with coordinates relative to the parent client area. Prefer creating controls before Show(). Windows have a title bar, close button and can be moved.

| Object | Properties | Methods | Events |
|---|---|---|---|
| Window | Text/Title, Visible | Show(), Close() | Load, Closed |
| Button | Text, Enabled, Visible | — | Click |
| Label | Text, Enabled, Visible | — | — |
| TextBox | Text, Enabled, Visible | — | Change |
| CheckBox | Text, Enabled, Visible, Checked | — | Click |

Read or assign properties, for example `buttonA.Enabled = False`. Text/Title are Strings; the other properties are Boolean. Tab/Shift+Tab move between text fields. Enter activates the first visible enabled button. A checkbox responds to mouse clicks. Escape stops execution.

`AddHandler control.Event, AddressOf Handler` accepts a global Sub or object method, such as `AddressOf controller.OnClick`. A handler takes zero parameters or one sender parameter of the corresponding GUI type, Object or Variant. Functions cannot be handlers. The event source must currently be a simple variable name; use a local variable for a field if necessary. `RemoveHandler control.Event, AddressOf Handler` removes a binding. Identical bindings are registered once. Dispatch uses a snapshot: binding changes affect subsequent events.

`Application.Run()` handles events until every window has closed. `Application.Exit()` leaves that loop; statements after Run() can still execute. Run() cannot be nested, including from handlers. Show() queues Load when opening a closed window. Closed is delivered after closing and cannot cancel it. Assigning Window.Visible=False also closes it. TextBox.Change fires for actual text changes, including BASIC assignments. All BASIC windows are cleaned up on completion, error or stop.

Limits: four allocated windows, 64 GUI objects including windows, and 64 bindings per run. Windows/controls must fit their parent client area. GEM text is limited to 63 characters; a TextBox accepts `Min(63, (width - 8) / 8)` using integer division. This version does not include window resizing, timers, lists, application menus or standalone compilation. Completion recognizes GUI types, properties, methods and events.

## 17. Debugging (0.7)

| Action | Shortcut |
|---|---|
| Run / continue to next breakpoint | F5 |
| Start stepping / step into | F8 |
| Step over a method call | Shift+F8 |
| Step out to the caller | Ctrl+Shift+F8 |
| Toggle current-line breakpoint | F9, or click the left gutter |
| Request pause while running | Ctrl+P, or Project menu |
| Inspect first visible variable | Ctrl+I, or click a variable |
| Stop execution | Escape, or Project → Stop execution |

**F8 is reserved for debugging from 0.7. Add existing file moves to Ctrl+F7.** New file remains F7. F11/F12 are not required on original Atari keyboards.

Open DEBUG.TBP for a small method-call example. Set a breakpoint on `result = calculator.DoubleValue(startValue)`. F5 runs to that line, F8 enters the method, Shift+F8 completes the call, and Ctrl+Shift+F8 runs until the caller continues. Breakpoints still apply while stepping. The yellow line marks the next statement. The BASIC `Stop` statement also requests a debugger pause.

The right panel shows the call stack and variables. Use `<`/`>` to select a frame and navigate to its source line; frame 0 is the main program. The list includes parameters, locals, Me fields and globals. Up/Down or the bottom arrows scroll. Click a variable or press Ctrl+I for its displayed value and type. GUI values include their text. Arrays/collections show item counts; individual items and fields of other objects cannot be expanded yet. Inspection never invokes BASIC methods or changes variables.

Breakpoints also work inside Click/Change handlers: interact with the program window to reach them. Continuing brings the application window forward again. If Application.Run is waiting for input, a pause request takes effect at the next executable BASIC statement; Escape stops immediately while waiting.

Unhandled runtime errors pause before unwinding the stack, allowing local inspection. F5 or Escape then terminates the failed run. Editing and resuming a failed statement is not supported. Sources are read-only while paused. There is no Immediate window, conditional breakpoint or editing of running variables yet.

Up to 64 breakpoints persist in C:\TTBASIC\STUDIO.BRK. They use physical file rows and do not automatically move when lines are inserted/deleted; review them after structural edits. Set them on executable statements: comments, class/method definitions and Imports are skipped.

## 18. Visual form designer (0.8)

### Getting started

1. Choose **File → New form project** or press **Ctrl+Shift+D**.
2. Enter a project name of one to eight characters. A separate project directory and MAIN.BAS are created.
3. Choose Select, Button, Label, TextBox or CheckBox in the left toolbox. Click the white form to place a control.
4. Drag a control to move it. Drag its bottom-right corner to resize it. The form also has a bottom-right sizing handle.
5. Edit names, text, position, size and handlers in the right property panel. Clicking a value opens a prefilled field. Click Enabled, Visible or Checked to toggle them.
6. **Apply** returns to BASIC; **F2/Ctrl+S** applies and saves. **Test/F5** applies and runs the project, without automatically saving it.
7. Reopen with **Ctrl+D** or **Project → Form designer**, including after restarting the IDE.

Open **C:\TTBASIC\DESIGN.TBP** for a complete example, then Ctrl+D. Its greeting and close handlers already work. Existing handwritten programs, including GEMDEMO, are not automatically converted: Ctrl+D offers to create a separate form project and preserves the original sources.

### Controls

| Action | Input |
|---|---|
| Select / move | Select tool; click or drag |
| Resize | Drag bottom-right corner |
| Move selection | Arrow keys, 8 pixels |
| Resize selection | Shift+arrow, 8 pixels |
| Pan a large form | Ctrl+arrow or the four left arrow buttons |
| Delete control | Delete key or Delete button |
| Undo / restore last design change | Ctrl+Z or Undo |
| Open event code | F4 or Event code |
| Apply and save | F2 or Ctrl+S |
| Apply and test | F5 or Test |
| Leave designer | Cancel or Escape; changed designs can be applied or discarded |
| Help | F1 |

Positions and sizes use client-area pixels. Dragging uses an eight-pixel grid; property fields accept exact values. Larger forms can be panned within the canvas. The canvas is a design representation; Test shows the actual GEM rendering.

### Event code

Select the first Button added to a blank form and press F4. The designer creates a missing Sub and places the caret in its body:

```basic
Sub Button1_Click(sender As Button)
    Form1.Close()
End Sub
```

Save with F2 and run with F5. The button now closes the form. TextBox uses Change; Button and CheckBox use Click. Select the form for Load, or choose the Closed field to edit the close handler. Labels have no events. An empty handler name removes the binding; F4 supplies a default name when needed.

The design lives inside MAIN.BAS alongside BASIC code; no separate resource file is needed. The region between `Rem @TTFORM BEGIN 1` and `Rem @TTFORM END` belongs to the designer. **Keep handwritten event code outside this region.** Existing Subs and their bodies survive layout changes. Deleting or renaming a binding does not delete the old Sub. Handler names are independent of object names. Renaming updates generated declarations and bindings; update references in your own BASIC code yourself. Event Subs can call imported procedures or object methods.

### Limits

The initial designer manages one form in the project's entry source, with up to sixteen controls. Form sizes range from 120–600 pixels wide and 64–400 high; controls must fit inside it. Existing text limits and the 256-line source limit apply to generated layout plus handlers. Generation failures preserve the original source and allow you to revise the design. Review breakpoints after layout edits: they still use physical line numbers. Multiple selection, automatic refactoring of user code and importing arbitrary handwritten GUI layouts are not included yet.

## 19. MIDI and the Yamaha DX7 demo (0.9)

Choose **Project → Open DX7 MIDI demo**, then F5, or open C:\TTBASIC\DX7DEMO.TBP. Every run starts in **dry-run mode**: hexadecimal messages appear in the output panel without being transmitted. Select channel 1–16 and internal voice 1–32. Select voice changes the program. Apply sound sends algorithm, feedback and the selected operator's level. Note 60 on/off controls a test note. Panic sends sustain-off plus 128 explicit Note Off messages on the last used channel and remains usable if an input field is invalid. Close exits.

Check **Send MIDI OUT** to transmit. TX indicates bytes were handed to MIDI output, not acknowledgement by a synthesizer. Device detection and MIDI input are not implemented. Fields contain requested values, not settings read back from the DX7.

### Connections

On a physical TT, connect MIDI OUT to DX7 MIDI IN. Sound comes from the DX7. Match its receive channel. For parameter SysEx, follow the original Yamaha manual: System Information available and Memory Protect off. Select the voice before editing. Program selection changes the current sound and may discard unsaved edits; store them on the DX7 if needed.

On macOS, Hatari uses a PortMidi output device. Attach a USB-MIDI interface before starting Hatari and select its output in the MIDI device settings. Restart Hatari to discover newly attached ports. The demo checkbox does not configure Hatari's port. Dry-run mode works without an interface or synth; development tests do not modify user Hatari settings.

### BASIC API

| Call | Meaning |
|---|---|
| Midi.EnableOutput(True/False) | Enable/disable transmission; each F5 starts disabled |
| Midi.Enabled() | Boolean output state |
| Midi.LastMessage() | Last message/status as String |
| Midi.CheckRange(value, minimum, maximum) | Checked Integer; invalid values cause a runtime error |
| Midi.NoteOn(channel, note, velocity) | Channel 1–16; note/velocity 0–127 |
| Midi.NoteOff(channel, note[, velocity]) | Release note; default velocity 0 |
| Midi.ProgramChange(channel, program) | Program 0–127 |
| Midi.ControlChange(channel, controller, value) | Controller/value 0–127 |
| Midi.PitchBend(channel, value) | 0–16383; centre 8192 |
| Midi.SysEx(bytes) | Array of 2–128 bytes including F0/240 and F7/247 |
| Midi.StopNotes(channel) | Release notes/sustain owned by this run |
| Midi.Panic(channel) | Sustain off and all 128 explicit note-offs |

```basic
Midi.EnableOutput(False)
Midi.NoteOn(1, 60, 96)
Print Midi.LastMessage()
Midi.NoteOff(1, 60)
```

The Note On displays `DRY: 90 3C 60`. SysEx requires F0/F7 framing and seven-bit payload data. Running status is not used. Long log messages are shortened, while transmission uses the entire validated packet.

Owned notes and sustain are released on stop, completion, errors and disabling output. Debugger pauses also release them; continuing does not replay them. Velocity-zero Note On acts as Note Off. Tracking covers messages sent by this program, not notes played elsewhere.

### DX7 class

DX7.BAS exposes the seven-byte message construction in BASIC and reuses one array buffer across edits.

```basic
Imports "DX7.BAS"
Dim synth As New DX7(1)
synth.Program(1)
synth.Algorithm(6)
synth.Feedback(3)
synth.OperatorLevel(1, 90)
Print Midi.LastMessage()
```

This stays dry until EnableOutput(True). Voice and algorithm use 1–32 in the class, converted to wire values as needed. Feedback is 0–7, operator 1–6 and level 0–99. Changing channel or program releases owned notes first. Ctrl+D opens the demo in the form designer; handlers remain editable.

The demo targets the original DX7. Its encoding follows Yamaha's MIDI Data Format, pp. 30–32 of the [DX7 manual](https://usa.yamaha.com/files/download/other_assets/9/333979/DX7E1.pdf): algorithm parameter 134, feedback 135, operator level `(6 - operator) * 21 + 16`. No patch banks, storage commands or bulk transfers are sent. Software/emulation tests are provided; validation on a physical DX7 is still pending.

### Arturia DX7 V with MIDI Learn

The demo starts with **Arturia CC18** checked. In DX7 V, assign Algorithm to channel 1, CC 18 using MIDI Learn; select the matching channel in the demo. **Apply sound** maps Algorithm 1–32 to CC values 0–127. Voice, feedback and operator level are disabled in this mode because their Arturia assignments have not been configured. Uncheck Arturia CC18 for original Yamaha SysEx control. MIDI output starts disabled. Configure the mapping in DX7 V yourself; the demo does not create it.

## 20. Numbers and array fields (0.10)

Single uses 32-bit floating-point precision (about 6–7 significant digits). Double uses the C runtime precision, at least 15 significant digits; Pure C uses extended precision internally. PRINT/CStr and the debugger display up to 15 significant digits. Floating-point values are approximate; compare calculated values using a tolerance such as `Abs(a - b) < 0.000001`. This is not exact decimal Currency arithmetic.

The supported Double range is zero or a magnitude from 1e-300 through 1e300. Single supports zero or approximately 1.175494e-38 through 3.402823e38; subnormal values are rejected. Invalid numbers, overflow and underflow cause errors. Literals include `0.5`, `.25`, `1.` and `1.5e-3`. Commas separate arguments.

```vb
Dim price As Double = 12.5
Dim total As Double = price * 1.21
Print total
Print 7 / 2
Print 7 \ 2
Print 2 ^ 3 ^ 2
Print CInt(2.5)
Print CInt(3.5)
```

Output: 15.125, 3.5, 3, 512, 2 and 4. Integer/Long variables, array indices/bounds, MIDI data and screen coordinates require whole numbers; fractions are never silently truncated. Use CInt/CLng (round), Fix (toward zero) or Int (down) explicitly. CDbl, CSng and Val convert numeric values or complete numeric strings. Input also accepts Single/Double and decimal text.

Math helpers: Abs, Sgn, Sqr, Sin, Cos, Tan, Atn, Exp and Log. Angles are radians; Log is the natural logarithm. A negative square root, Log of zero/negative input, or a negative power base with a fractional exponent raises a domain error. For supports Single/Double counters and fractional Step; a step too small to change the counter at its precision raises an error.

Array fields may be fixed or dynamic. Each object gets its own fixed array. Fixed field bounds must be available at load time: use numeric literals. Public access, private access through Me, inheritance and ReDim Preserve work:

```vb
Class Measurements
    Public values(1 To 3) As Double
    Private history() As Double
    Public Sub New()
        ReDim Me.history(2)
        Me.history(1) = 0.5
        ReDim Preserve Me.history(4)
        Me.values(1) = Me.history(1)
    End Sub
End Class
Dim sample As New Measurements
sample.values(2) = sample.values(1) / 2
Print sample.values(2)
```

### Migration from 0.9

`/` now performs floating-point division. Use `\` where integer truncation is intended. Fractional assignments to Integer/Long require explicit conversion. Single and Double are now reserved type names: rename a method named Double to DoubleValue. Updated DEBUG.BAS and MIDI examples already account for this. Existing personal sources are not rewritten automatically.

### Distribution status

TT BASIC 0.10 is a bounded VB-inspired dialect. Documented constructs are tested; this is not the full VB language. Multidimensional arrays, BASIC file access, generic collections, overloads, garbage collection and exception handling remain unimplemented. Source/object/container limits still apply. Arrays have up to 128 elements, with indices bounded to ±1,000,000. Test your projects on their target computer; physical TT validation remains pending.
