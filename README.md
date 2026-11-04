# TT BASIC Studio

A native GEM BASIC IDE for the Atari TT, written in Pure C. Version **0.10** adds Single/Double arithmetic, 32-bit Integer/Long values, numeric conversions, math functions and direct typed array fields. It also includes MIDI output, dry-run hexadecimal logging, owned-note cleanup and a Yamaha DX7 demo with an editable form and BASIC instrument class. The visual form designer supports placing, moving and resizing controls, properties and preserved event handlers. Project management includes New/Open project, New/Add source file, Save All and portable .TBP manifests. The language includes conditions, loops, Select Case, strings, arrays, collections, interactive input, source imports, inheritance, interfaces and polymorphic method calls. The IDE supports Dutch, English and German.

## Manuals

- [Nederlandstalige handleiding](docs/handleiding-nl.md)
- [English manual](docs/manual-en.md)
- [Deutsches Handbuch](docs/handbuch-de.md)

The manuals cover installation, menus, editing, BASIC syntax, Imports, objects, examples, diagnostics and current limits. BASIC keywords and interpreter diagnostics remain English in all UI languages.

## Example

```vb
Imports "COUNTER.BAS"
Dim first As New Counter(10)
Dim second As New Counter(2)
first.Add(32)
Print first.GetValue()
Print second.GetValue()
End
```

Output: `42`, `2`. The imported [Counter class](examples/COUNTER.BAS) has a private field, a constructor, an Add method and a GetValue function.

## Using the IDE

Start `C:\TTBASIC\TTBASIC.PRG` in TT Medium, or at least 640 × 480. Press F5 to run the project from MAIN.BAS. Imported modules appear in the project tree and can be edited directly. Runs use unsaved in-memory sources; F2 saves the active file and keeps a `.BAK` of its previous disk contents. File → New project (Ctrl+N), Open project (Ctrl+O), New file (F7), Add existing file (Ctrl+F7) and Save all (Ctrl+Shift+S) manage project work.

Load examples with **Project → Load language example**, **Load polymorphism example**, **Load input example**, or **Load OO example**. Ctrl+D switches between a managed form design and source. While a form runs, Ctrl+Tab switches between the form and read-only source; the Form toolbar button returns to the form. Press Escape to stop a running program. In the initial Dutch UI this is **Project → OO-voorbeeld laden**. Choose the UI language under **Beeld / View / Ansicht → Nederlands / English / Deutsch**.

## Building on the Atari

Prerequisites: Atari TT or a compatible Hatari setup, TOS 3.05 and a separate Pure C installation at `C:\PUREC`. Pure C, TOS and the emulator are not distributed in this repository.

1. Copy `src/*.C`, `src/*.H`, `atari/TTBASIC.PRJ` and `atari/BUILD.LNK` into `C:\TTBASIC`.
2. Copy the BASIC files from `examples/` into that directory. Preserve any existing user programs.
3. Start `C:\PUREC\PC.PRG`.
4. Select `C:\TTBASIC\TTBASIC.PRJ` as the project.
5. Set the include directory to `C:\PUREC\INCLUDE` and the library directory to `C:\PUREC\LIB` if your Pure C configuration does not already use them.
6. Keep `PCFLTLIB.LIB` before `PCSTDLIB.LIB` in the supplied project: this enables floating-point formatting as well as arithmetic.
7. Use **Project → Make**. The result is `C:\TTBASIC\TTBASIC.PRG`.

The project uses a 512 KiB stack and the compiler option `-P` for absolute calls. Keep `-P` enabled: the program is too large for 16-bit PC-relative inter-module calls. Numeric values use the C floating-point runtime; the supplied library order enables both math and decimal formatting. The source and Atari project files use CRLF line endings for the original compiler. Git's attributes preserve this on checkout.

On a modern host you can prepare a GEMDOS drive directory with:

```sh
python3 tools/install.py --drive-c /path/to/hatari-drive-c
```

The installer copies project sources and supplies missing example files. It preserves existing `.BAS` and `.TBP` files. It does not install Pure C, TOS or Hatari, and does not compile the Atari executable.

## Tests

A host C compiler is enough to test the interpreter:

```sh
./tools/test.sh
SANITIZE=1 ./tools/test.sh
```

The suites cover project creation, manifest round trips, file copying, collision prevention, source validation, the original object/import model, control flow, strings, input, array bounds, collections, inheritance, interface contracts, dynamic dispatch, cancellation and every shipped runnable example. The same interpreter suite has also been compiled with Pure C and run inside the emulated TT. Native GEM interaction, multi-file editing and UI languages are checked separately in Hatari.

## Current scope

- Text editor with code completion, native menus, project/import tree, properties and resizable output panels. Ordinary typing redraws affected lines; vertical scrolling reuses existing screen rows to reduce flicker.
- Separate projects with a default MAIN.BAS entry point, editable source files and persistent .TBP manifests.
- Integer, floating-point and string BASIC, Boolean logic, procedures, functions, typed fields and object references.
- Up to 16 files, 256 lines per file and 2048 loaded source lines.
- Up to 16 classes/interfaces, 64 object instances and 64 methods/procedures.
- BASIC is interpreted; the IDE itself is a native Atari PRG.

Implemented: If/ElseIf/Else, While/Wend/End While, For/Step/Next, For Each, Do/Loop, Select Case, Exit/Continue, typed arrays, ReDim Preserve, ordered collections, string helpers and Input through a GEM dialog. Inherits, Implements, Overridable/Overrides, Protected, MyBase, TypeOf and typed object/interface parameters support polymorphism. Multidimensional arrays, generic collections, overloads, garbage collection, exception handling remain future work. Imports loads source files rather than VB namespaces. This is a bounded VB-inspired dialect, not the complete VB language.

Validated in Hatari with TOS 3.05, TT Medium, 8 MiB ST-RAM and 256 MiB TT-RAM. Physical TT validation is pending.

## Sharing a build

See [release notes](RELEASE.md) for installation, migration and known limits. Create a binary/source/manual bundle after a native build:

```sh
python3 tools/package.py --program /path/to/TTBASIC.PRG --output dist/ttbasic-0.10.zip
```

The bundle contains the executable, clean examples, three manuals, license and corresponding source. It includes no personal projects, TOS ROM or Pure C compiler/libraries.

## Repository layout

| Directory | Contents |
| --- | --- |
| `src/` | Pure C sources, headers and portable project persistence |
| `atari/` | Native project and linker configuration |
| `examples/` | BASIC programs and class library |
| `docs/` | Dutch, English and German manuals |
| `tests/` | Portable interpreter regression tests |
| `tools/` | Host tests and non-destructive drive preparation |

Only project files belong here. ROM images, third-party compiler files, emulator packages, personal programs, local settings and temporary test drives are excluded.

## License

GNU General Public License version 3; see [LICENSE](LICENSE).

Try **Project → Load GEM example**, then F5, or open `GEMDEMO.TBP`. Open `DEBUG.TBP` for a small stepping example. F8 starts/steps into, Shift+F8 steps over, Ctrl+Shift+F8 steps out, F9 toggles a breakpoint, Ctrl+P requests pause, Ctrl+I inspects a variable, and Escape stops execution. GUI event handlers can be debugged. This is an initial debugger: no Immediate window, conditional breakpoints or edits while paused.

Create a form project with **Ctrl+Shift+D** (File → New form project), or open `DESIGN.TBP` and press **Ctrl+D**. The designer has a toolbox, canvas, property editor, drag/resize, undo, F4 event code and F5 test. F2/Ctrl+S applies and saves. Up to 16 controls are supported in one entry-file form. Layout metadata and generated BASIC share a marked source region; handwritten handlers outside it are preserved. Handwritten GUI layouts are not imported automatically.

Choose **Project → Open DX7 MIDI demo**, then F5, or open `DX7DEMO.TBP`. It starts with output disabled. The `DX7.BAS` class exposes Yamaha parameter messages; `Midi` provides reusable MIDI 1.0 calls. Select an actual MIDI output in Hatari before enabling transmission, or use the TT MIDI OUT port. No incoming MIDI, bank transfers or automatic connection detection are included. Tests cover the messages and cleanup; physical DX7 validation is pending. See the manuals for channel/parameter ranges and original DX7 settings.
