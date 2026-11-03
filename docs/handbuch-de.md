# TT BASIC Studio 0.5 — Deutsches Handbuch

TT BASIC Studio verbindet einen BASIC-Interpreter mit einer GEM-Entwicklungsumgebung für den Atari TT. Sowohl die IDE als auch das BASIC-Programm laufen auf dem Atari selbst. Diese Version wurde mit Pure C erstellt und in Hatari mit TOS 3.05, TT Medium (640 × 480, 16 Farben), 8 MiB ST-RAM und 256 MiB TT-RAM getestet. Tests auf einem physischen Atari TT stehen noch aus.

Dies ist eine frühe Sprachversion. Dieses Handbuch beschreibt die **bereits implementierten Funktionen**, nicht die vollständige Sprache Microsoft Visual Basic. BASIC-Schlüsselwörter und Fehlermeldungen des Interpreters bleiben in allen Oberflächensprachen Englisch.

[Nederlands](handleiding-nl.md) · [English](manual-en.md)

## 1. Installation und Start

1. Die erstellte `TTBASIC.PRG` nach `C:\TTBASIC` auf dem Atari kopieren.
2. Die Beispieldateien `MAIN.BAS`, `OODEMO.BAS`, `COUNTER.BAS` und gegebenenfalls `MATH.BAS` in denselben Ordner kopieren. Eine vorhandene MAIN.BAS mit eigenem Programmcode muss nicht ersetzt werden.
3. `C:\TTBASIC\TTBASIC.PRG` vom GEM-Desktop starten.
4. TT Medium oder eine Bildschirmauflösung von mindestens 640 × 480 verwenden.

In der eingerichteten Hatari-Umgebung entspricht Atari-Verzeichnis C:\TTBASIC dem Mac-Verzeichnis `Atari-TT/Schijf-C/TTBASIC`. Das Repository enthält den eigenen Quellcode, Beispiele und Bauanweisungen. Hatari, TOS und Pure C werden getrennt installiert. Siehe [Erstellen auf dem Atari](../README.md#building-on-the-atari).

## 2. Die Entwicklungsumgebung

Die native GEM-Menüleiste befindet sich am oberen Bildschirmrand. Links steht der Projektbaum, in der Mitte der Editor und unten die Ausgabe. Das rechte Eigenschaftenfenster wird über **Ansicht → Eigenschaften** eingeblendet.

- `MAIN.BAS` ist die Startdatei des Projekts.
- Der Projektbaum zeigt Projektdateien und erkannte Imports-Abhängigkeiten.
- MAIN.BAS oder eine Importdatei anklicken, um sie zu bearbeiten. Änderungen und Cursorposition bleiben für jede geöffnete Datei erhalten.
- Den Projektnamen zum Ein- oder Ausklappen anklicken. Die Pfeile unten blättern durch eine längere Dateiliste.
- Ein Sternchen im Dateireiter bedeutet ungespeicherte Änderungen.
- **F5 startet immer das Projekt ab MAIN.BAS**, auch wenn gerade eine Importdatei bearbeitet wird.
- F5 verwendet die Arbeitsfassungen der geöffneten Dateien. Vorheriges Speichern ist nicht erforderlich.
- Jeder Lauf beginnt mit neuen Variablen und Objekten. Der Programmzustand bleibt zwischen Ausführungen nicht erhalten.

Die Importliste wird beim Laden, Speichern und Ausführen aktualisiert. Neue Imports-Anweisungen erscheinen somit spätestens nach F5 im Baum. Bereits geöffnete Dateien behalten ihre Arbeitsfassung. Änderungen durch einen externen Editor werden mit F3 neu geladen.

### Sprache auswählen

Unter **Ansicht → Nederlands / English / Deutsch** wird die Oberflächensprache geändert. Ansicht heißt auf Niederländisch **Beeld**, auf Englisch **View**. Die Auswahl wird in `C:\TTBASIC\STUDIO.CFG` gespeichert. Programmdateien funktionieren unverändert in allen Sprachen. Die Atari-Oberfläche verwendet die ursprüngliche Systemschrift und deshalb ASCII-Umschreibungen wie `ue` statt `ü`.

### Menüs und Tastenkombinationen

| Aktion | Menü oder Taste |
| --- | --- |
| Neues Projekt | Datei → Neues Projekt, Strg+N |
| Projekt öffnen | Datei → Projekt öffnen, Strg+O |
| Neue Quelldatei | Datei → Neue Datei, F7 |
| Bestehende Datei hinzufügen | Datei → Datei hinzufügen, Ctrl+F7 |
| Alle Dateien und Projekt speichern | Datei → Alles speichern, Strg+Shift+S |
| Aktive Datei erneut von Datenträger laden | F3 |
| Aktive Datei speichern | F2, Strg+S |
| Projekt ausführen | Projekt → Projekt starten, F5 |
| OO-Beispiel in den Starteditor laden | Projekt → OO-Beispiel laden |
| Letzte Bearbeitung rückgängig machen | Strg+Z |
| Ganze aktuelle Zeile kopieren / ausschneiden / einfügen | Strg+C / Strg+X / Strg+V |
| In der aktiven Datei suchen | Strg+F |
| Weitersuchen | F6 |
| Eigenschaften ein-/ausblenden | F4 |
| Fensterbereiche ein-/ausblenden | Ansicht |
| Tastenübersicht | F1 |
| Beenden | Strg+Q |

Die Suche unterscheidet nicht zwischen Groß- und Kleinschreibung und beginnt nach dem letzten Treffer wieder am Anfang. Die Zwischenablage enthält **eine ganze Zeile** und ist intern; eine freie Textauswahl und die Systemzwischenablage sind noch nicht vorhanden. Einfügen erzeugt eine ganze Zeile oder ersetzt eine leere aktuelle Zeile. Rückgängig stellt genau eine Bearbeitung wieder her. Speichern, Neuladen und Dateiwechsel löschen diesen Schritt; Wiederholen/Redo fehlt noch.

### Speichern und Beenden

Speichern erzeugt zunächst eine `.TMP`-Datei und bewahrt die vorherige Datenträgerversion als `.BAK`. Zu COUNTER.BAS gehören also COUNTER.TMP und COUNTER.BAK. Geänderte Importdateien einzeln auswählen und mit F2 speichern; Datei → Alles speichern speichert alle geänderten offenen Dateien und die Projektstruktur.

Beim Beenden werden alle geöffneten Dateien auf Änderungen geprüft. Das Beenden abbrechen, um diese Dateien zuerst zu speichern. Neues Projekt erzeugt einen eigenen Ordner mit neuer MAIN.BAS. Projekt öffnen lädt eine .TBP-Datei. Beispiele verwenden das installierte DEFAULT-Projekt. Allgemeines Speichern unter und Umbenennen von Quelldateien fehlen noch.

## 3. Das erste Programm

Diesen Text in MAIN.BAS eingeben und F5 drücken:

```vb
Print "Hallo Atari TT!"
Dim answer As Integer = 6 * 7
Print answer
End
```

Ausgabe:

```text
Hallo Atari TT!
42
Program finished.
```

Ziehen Sie die graue Trennleiste oberhalb der Ausgabe nach oben oder unten, um deren Hoehe zu aendern. Es werden die letzten passenden Zeilen angezeigt (maximal zehn). Fehler stoppen die Ausführung. Print schreibt Text in diesen Bereich. Eigene GEM-Fenster werden mit den GUI-Objekten in Kapitel 16 erzeugt; eigenstaendige Kompilierung fehlt noch.

## 4. Zahlen, Variablen und Kommentare

Namen unterscheiden nicht zwischen Groß- und Kleinschreibung: answer, ANSWER und Answer bezeichnen dieselbe Variable. Ein Name beginnt mit einem Buchstaben oder Unterstrich; danach sind Buchstaben, Ziffern und Unterstriche erlaubt. Maximal 23 Zeichen verwenden und BASIC-Schlüsselwörter nicht als Namen benutzen.

```vb
Dim count As Integer
Dim total As Long = 40
count = 2
Let total = total + count
Print total
Rem Ein Kommentar
' Noch ein Kommentar
Print total ' Kommentar nach einer Anweisung
```

Let ist optional. Seit 0.10 sind Integer und Long beide vorzeichenbehaftete 32-Bit-Ganzzahlen: **−2.147.483.648 bis +2.147.483.647**. Dieser Dialekt verwendet keinen 16-Bit-VB-Integer. Single und Double unterstützen Nachkommastellen mit einem Punkt im Quelltext. Bereiche, Konvertierungen und Migration stehen in Kapitel 20.

Numerische Variablen beginnen mit null. Aus Kompatibilitätsgründen können A bis Z auch ohne Dim als null gelesen werden. Eine Zuweisung an einen bisher unbekannten Namen erzeugt eine Variant-Variable. Andere unbekannte Namen zu lesen ist ein Fehler; explizite Dim-Deklarationen sind empfehlenswert.

Eine zweite Dim-Deklaration desselben Namens im selben Gültigkeitsbereich wird abgewiesen. Parameter und lokal deklarierte Variablen gehören zum aktuellen Methodenaufruf. Eine lokale Dim-Deklaration kann einen globalen Namen verdecken. Wenn keine lokale Variable existiert, verwendet eine Zuweisung eine bereits vorhandene globale Variable dieses Namens.

### Operatoren

| Operator | Bedeutung |
| --- | --- |
| `+`, `-` | Addition, Subtraktion und Vorzeichen |
| `*`, `/` | Multiplikation und Gleitkommadivision |
| `\`, `Mod` | Ganzzahldivision und Rest mit ganzen 32-Bit-Operanden |
| `^` | Potenzieren |
| `(...)` | Gruppierung |
| `=`, `<>`, `<`, `>`, `<=`, `>=` | Numerische Vergleiche |

Rangfolge: Klammern, Potenzen (rechtsassoziativ), Vorzeichen, `*`/`/`, `\`, `Mod`, `+`/`-`. `7 / 2` ergibt `3.5`, `7 \ 2` ergibt `3`. Vergleiche liefern -1 für wahr und 0 für falsch. Division durch null, Überlauf und Unterlauf verursachen Fehler.

```vb
Print 2 + 3 * 4
Print (2 + 3) * 4
Print 42 >= 40
```

Die Ergebnisse sind 14, 20 und -1.

### Print und Text

```vb
Print "Wert: "; total
Print "Summe: "; 2 + 3; "!"
Print
```

Semikolons verbinden Elemente auf **einer Ausgabezeile**, ohne Leerzeichen hinzuzufügen. Gewünschte Leerzeichen gehören in das Textliteral. Jede Print-Anweisung beendet die Zeile, auch bei abschließendem Semikolon. Eine Ausgabezeile enthält höchstens 79 Zeichen. Stringvariablen werden mit `Dim text As String` deklariert. Zwei Strings lassen sich mit `&` oder `+` verketten. Eingebettete Anführungszeichen verdoppeln: `"Ein ""Zitat"""`. Ein String enthält höchstens 127 Zeichen.

## 5. Imports: externe Dateien einbinden

```vb
Imports "COUNTER.BAS"
```

TT BASIC verwendet Imports zum Laden von **BASIC-Quelldateien**. Das ist eine eigene Dialektentscheidung; Microsoft VB verwendet Imports normalerweise für Namensräume.

- Imports außerhalb von Class-, Sub- und Function-Definitionen verwenden.
- Dateinamen in doppelte Anführungszeichen setzen.
- Relative Pfade beziehen sich auf die Datei, in der die Imports-Anweisung steht.
- Unterverzeichnisse funktionieren, etwa `Imports "LIB\MATH.BAS"`. TOS-kompatible 8.3-Dateinamen verwenden.
- Klassen und Prozeduren werden vor der Ausführung registriert. Dadurch sind auch Vorwärtsaufrufe möglich.
- Jede Datei wird pro Lauf einmal geladen, auch bei mehrfacher Einbindung oder gemeinsam genutzten Abhängigkeiten.
- Zirkuläre Abhängigkeiten verursachen `Circular Imports dependency`.
- Anweisungen außerhalb von Definitionen in Importdateien werden einmal an der Importstelle ausgeführt. Bibliotheksdateien sollten deshalb normalerweise nur Definitionen enthalten: Ein alleinstehendes End würde das gesamte Projekt beenden.

Beispielbibliothek MATH.BAS:

```vb
Function DoubleValue(value As Integer) As Integer
    Return value * 2
End Function
```

Verwendung in MAIN.BAS:

```vb
Imports "MATH.BAS"
Print DoubleValue(21)
End
```

## 6. Klassen und Objekte

Eine Klasse beschreibt Daten und Operationen eines Objekts. New erzeugt eine eigenständige Instanz. Diese Definition als COUNTER.BAS speichern:

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

### Felder und Sichtbarkeit

value ist ein numerisches Feld, das in jeder Instanz getrennt gespeichert wird. Felder können auch Strings, Boolean-Werte sowie Objekt- und Collection-Referenzen enthalten. Felder beginnen mit null oder mit einem ganzzahligen Literal wie `Public Value As Integer = 10`. Public erlaubt Zugriff von außerhalb der Klasse. Private erlaubt Zugriff nur innerhalb von Methoden derselben Klasse. Sichtbarkeit ausdrücklich angeben; ohne Angabe ist ein Mitglied in dieser Version öffentlich.

Direkter Zugriff auf first.value ist bei dieser Klasse somit ein Fehler. Stattdessen first.GetValue() verwenden. Öffentliche Felder können gelesen und beschrieben werden, beispielsweise first.Value = 42 bei einem öffentlichen Value-Feld.

### Konstruktor und Me

Public Sub New(...) ist der Konstruktor und wird automatisch durch New Counter(...) ausgeführt. Ohne Konstruktor akzeptiert eine Klasse New Counter oder New Counter() ohne Argumente. Ist ein Konstruktor vorhanden, muss die Argumentzahl passen.

Me verweist auf das aktuelle Objekt und ist nur innerhalb einer Klassenmethode gültig. Mit Me.value und Me.MethodName(...) werden seine Mitglieder ausdrücklich angesprochen.

### Unabhängige Instanzen und Referenzen

In MAIN.BAS:

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

Die Ausgabe lautet First: 42, Second: 2 und Alias: 42. first und second sind unabhängige Instanzen. alias = first kopiert eine **Referenz**; beide Variablen verweisen danach auf dasselbe Objekt.

Das Beispiel ist als OODEMO.BAS enthalten. **Projekt → OO-Beispiel laden** wählen und F5 drücken. Die vorherige MAIN.BAS auf dem Datenträger bleibt erhalten, bis das Beispiel ausdrücklich gespeichert wird.

### Nothing und Typprüfung

```vb
Dim counter As Counter
counter = New Counter(5)
counter = Nothing
```

Eine Objektvariable ohne New beginnt als Nothing. Mitglieder vor der Objekterzeugung anzusprechen ist ein Fehler. Eine Objektvariable akzeptiert Nothing oder eine mit der deklarierten Klasse bzw. dem Interface kompatible Instanz, einschließlich abgeleiteter Klassen. Eine Instanz einer anderen Klasse kann nicht zugewiesen werden.

Objekte bleiben bis zum Ende eines Laufs reserviert. Nothing entfernt eine Referenz, gibt in dieser Version aber keinen Objektplatz frei. Jeder F5-Lauf beginnt mit einem leeren Objektpool.

## 7. Subs, Functions und Parameter

Eine Sub führt Aktionen aus und gibt keinen Wert zurück. Eine Function liefert mit **Return** einen typisierten Wert zurück. Beide sind als Klassenmethoden oder globale Prozeduren möglich, auch in Importdateien.

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

Call ist optional; ShowNumber(36) funktioniert ebenfalls. Argumente stehen in Klammern. Maximal acht typisierte Parameter werden unterstützt. Zahlen und Strings werden als Werte übergeben, Objekte, Arrays und Collections als gemeinsame Referenzen. Anzahl und Typen müssen passen. Typen sind Integer/Long, Boolean, String, Variant, Object, Collection sowie Klassen und Interfaces. Arrayparameter werden als `items() As Integer` deklariert.

Eine Function muss mit Return einen Wert des deklarierten Ergebnistyps zurückgeben. Eine Zuweisung an den Funktionsnamen ersetzt Return nicht. Eine Sub kann mit Return ohne Wert oder Exit Sub vorzeitig beendet werden. Eine Sub in einer Berechnung zu verwenden ist ein Fehler. Ein alleinstehendes End beendet das Hauptprogramm und darf innerhalb einer Methode nicht als Ersatz für Return verwendet werden.

## 8. Fehlermeldungen verstehen

Eine Meldung nennt Quelldatei, physische Zeilennummer und englische Ursache:

```text
COUNTER.BAS:9: Division by zero
```

Leere Zeilen und Kommentare zählen mit. Der Editor markiert die betroffene Zeile, wenn die Datei aktiv ist. Ist MAIN.BAS aktiv und der Fehler liegt in einem Import, wird die Imports-Zeile markiert; die Ausgabe nennt den tatsächlichen Fehlerort.

| Meldung oder Ausschnitt | Prüfen |
| --- | --- |
| `Cannot open imported file` | Dateiname, Ordner und Pfad |
| `Circular Imports dependency` | Direkter oder indirekter Importzyklus |
| `Unknown Class` | Schreibweise, Imports und vollständige Klassendefinition |
| `Private member` | Öffentliche Methode statt direktem Zugriff verwenden |
| `Object is Nothing` | Objekt zuerst mit New erzeugen |
| `Wrong number of method arguments` | Konstruktor- oder Methodensignatur |
| `Object Class does not match` | Objekt muss zur deklarierten Klasse passen |
| `Function finished without Return` | Return des Ergebnistyps ergänzen |
| `Sub cannot be used as an expression` | Für einen Rückgabewert eine Function verwenden |
| `Unknown variable` | Deklaration, Schreibweise und Gültigkeitsbereich |
| `Division by zero` | Divisor darf nicht null sein |
| `Result exceeds numeric range` | Zwischenergebnisse innerhalb des Zahlenbereichs halten; siehe Kapitel 20 |
| `Maximum ...` / `exceeds ...` | Grenzen dieser Version beachten |

## 9. Grenzen und noch fehlende Funktionen

| Bestandteil | Grenze |
| --- | --- |
| Projektdateien einschließlich MAIN.BAS | 16 |
| Zeilen pro Datei | 256 |
| Insgesamt geladene Quellzeilen | 2048 |
| Zeichen pro Quellzeile | 159, ASCII |
| Länge eines Bezeichners | 23 Zeichen |
| Klassen | 16 |
| Numerische Felder pro Klasse | 16 |
| Klassenmethoden und globale Prozeduren zusammen | 64 |
| Objektinstanzen pro Lauf | 64 |
| Variablen pro globalem/lokalem Bereich | 64 |
| Parameter pro Methode/Prozedur | 8, typisiert |
| Verschachtelte Methodenaufrufe | 8 |
| Verschachtelte Ausdrücke | 16 |
| Zahlenbereich | Integer/Long: −2.147.483.648 bis +2.147.483.647; Double: Kapitel 20 |
| Zeichen pro Ausgabezeile | 79 |

Noch nicht implementiert sind mehrdimensionale Arrays, generische Collections, Überladung, automatische Speicherbereinigung, und Ausnahmebehandlung. Der Formularentwurf wird in Kapitel 18 beschrieben. BASIC wird noch nicht in eigenständige PRG-Dateien übersetzt. Dies ist ein eigener BASIC-Dialekt, keine vollständige VB-Implementierung.

## 10. Projektquellcode

Die IDE steht in src/IDE.C, der Interpreter in src/BASIC.C. Native Menüs werden in src/MENU.C und Übersetzungen in src/LANG.C implementiert. Pure-C-Projekt- und Linkerdateien liegen unter atari/. BASIC-Beispiele aus examples/ nach C:\TTBASIC kopieren, um sie auf dem Atari zu verwenden.

Bei einer Fehlermeldung im GitHub-Repository bitte die Studio-Version, ein minimales BASIC-Beispiel, die vollständige Meldung und die verwendete Plattform angeben: Hatari oder physische Atari-Hardware.

## 11. Version 0.4: Kontrollstrukturen und Daten

**Projekt → Sprachbeispiel laden**, **Polymorphie-Beispiel laden** oder **Eingabebeispiel laden** wählen und F5 drücken. Die Dateien heißen FLOW.BAS, POLY.BAS und INPUT.BAS. POLY verwendet zusätzlich ANIMAL.BAS, DOG.BAS und ISHOW.BAS.

### If, ElseIf und Else

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

Blöcke dürfen verschachtelt werden. Die Kurzform erlaubt eine Anweisung pro Zweig: `If age >= 18 Then Print "Adult" Else Print "Junior"`. Für verschachtelte Bedingungen die Blockform verwenden.

Boolean kennt True (-1) und False (0). And, Or, Xor und Not arbeiten mit Wahrheitswerten. Beide Seiten von And/Or werden ausgewertet. Wenn die zweite Bedingung nur unter einer Voraussetzung sicher ist, verschachtelte If-Blöcke benutzen.

### While und Do

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

End While kann Wend ersetzen. Bei Do darf eine While- oder Until-Bedingung entweder auf der Do- oder auf der Loop-Zeile stehen, nicht an beiden Stellen. Eine Bedingung am Anfang kann die erste Ausführung verhindern; eine am Ende wird nach dem Durchlauf geprüft. Eine einfache Do/Loop-Schleife lässt sich mit Exit Do verlassen.

### For und For Each

```vb
Dim total As Integer
For i As Integer = 6 To 1 Step -1
    total = total + i
Next i
Print total
```

Der Endwert ist eingeschlossen. Step ist standardmäßig 1 und darf negativ sein, aber nicht null. Start, Ende und Schritt werden beim Eintritt berechnet. Next darf den passenden Variablennamen enthalten. Der Zähler muss numerisch sein.

```vb
Dim names(1) As String
names(0) = "Atari"
names(1) = "TT"
For Each name As String In names
    Print name
Next name
```

For Each durchläuft Arrays und Collections. Eine neue Schleifenvariable ohne As ist Variant. Eine typisierte Variable prüft jedes Element. Exit For/While/Do verlässt die jeweilige Schleife; Continue For/While/Do geht zum nächsten Durchlauf. Dies funktioniert auch innerhalb verschachtelter If-Blöcke, kann aber nicht aus einem anderen Methodenaufruf herausführen.

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

Der Auswahlwert wird einmal berechnet. Nur der erste passende Zweig wird ausgeführt; es gibt kein Durchfallen zum nächsten Case. Case Else muss zuletzt stehen. Zahlen und Strings werden unterstützt; Stringvergleiche unterscheiden Groß- und Kleinschreibung. Exit Select verlässt den aktuellen Select-Block.

### Arrays und Collections

`Dim values(2) As Integer` erzeugt drei Elemente mit den Indizes 0, 1 und 2. `Dim values(1 To 3) As Integer` verwendet Grenzen 1 bis 3. Lesen und Zuweisen erfolgen mit `values(1)`. LBound und UBound liefern die Grenzen, Count oder `.Count` die Elementzahl.

```vb
Dim values() As Integer
ReDim values(1)
values(1) = 42
ReDim Preserve values(3)
Print values(1)
```

ReDim ersetzt ein dynamisches Array. Preserve erhält bestehende Elemente; die Untergrenze darf dabei nicht geändert werden. `Array(1, 2, 3)` erzeugt ein Variant-Array. Arrays sind eindimensional und enthalten maximal 128 Elemente. Zuweisungen und Parameterübergaben teilen die Referenz. ReDim weist der Variablen ein neues Array zu; andere Referenzen zeigen weiter auf das alte.

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

Collections sind geordnet, untypisiert und beginnen bei Index 1. Add hängt an, Remove entfernt einen Index und Clear leert den Inhalt. Maximal 128 Werte einschließlich Objekten sind möglich. Die Größe während For Each zu verändern verursacht einen Fehler; die Schleife zuerst verlassen. Pro Lauf stehen 32 Array-/Collection-Allokationen einschließlich ReDim zur Verfügung. Speicher wird während eines Laufs noch nicht freigegeben.

### Strings und Eingabe

Stringvariablen, Stringparameter und Stringergebnisse werden unterstützt. Hilfsfunktionen: Len, CStr/Str, CInt/CLng/CDbl/CSng/Val, Left, Right, Mid, UCase, LCase und Abs. Mid zählt ab Position 1. Val/CDbl akzeptieren vollständigen Dezimaltext mit Exponenten. CInt/CLng runden auf eine 32-Bit-Ganzzahl, bei genau halben Werten zur geraden Zahl. Int rundet ab; Fix schneidet Richtung null ab. Mod liefert den Rest einer ganzzahligen Division.

```vb
Dim name As String
Input "Your name?"; name
Dim age As Integer
Input "Your age?"; age
Print "Hello " & name
Print age + 1
```

Input öffnet in der IDE einen GEM-Dialog. Das Eingabefeld akzeptiert 63 Zeichen, Stringwerte in der Sprache bis zu 127. Ungültige Zahleneingaben oder Abbrechen beenden den Lauf mit einer Meldung. Eine Variant-Zielvariable erhält nach Möglichkeit eine Zahl, sonst einen String; enthält sie bereits einen String, wird weiter Text eingelesen.

## 12. Vererbung, Interfaces und Polymorphie

Eine Klasse kann eine Basisklasse besitzen. Inherits Animal steht innerhalb der Klasse oder auf der Class-Kopfzeile. Public-Mitglieder sind von außen zugänglich, Private nur in ihrer definierenden Klasse und Protected zusätzlich in abgeleiteten Klassen.

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

Die Ausgabe ist woof. Die Variable ist als Animal deklariert, enthält jedoch einen Dog. Die Basismethode benötigt Overridable, die Ersetzung Overrides. Name, Parameter- und Ergebnistypen müssen passen. Die öffentliche Sichtbarkeit darf nicht eingeschränkt werden. MyBase ruft die Basisimplementierung direkt auf.

Ein abgeleiteter Konstruktor kann als erste ausführbare Anweisung `MyBase.New(...)` verwenden. Andernfalls wird der parameterlose Basiskonstruktor automatisch aufgerufen. Benötigt dieser Argumente, müssen sie ausdrücklich übergeben werden. POLY.BAS zeigt ein vollständiges Beispiel.

```vb
Interface IShow
    Function Speak() As String
End Interface
```

Eine Klasse erklärt Implements IShow und stellt eine öffentliche Methode mit gleichem Namen und gleicher Signatur bereit. Interfaces enthalten nur Signaturen, ohne End Function/End Sub pro Mitglied. End Interface schließt den Vertrag ab. Ein Interface kann nicht mit New erzeugt werden. Eine Interfacevariable kann Objekte beliebiger implementierender Klassen enthalten; Aufrufe erreichen die konkrete Implementierung.

Eine ausdrückliche Zuordnung zu einem anderen Methodennamen ist ebenfalls möglich:

```vb
Public Function Display() As String Implements IShow.Speak
    Return "shown"
End Function
```

Diese Methode gehört in eine Klasse mit Implements IShow. Eine Klasse kann bis zu vier durch Kommas getrennte Interfaces direkt implementieren. Geerbte Verträge werden übernommen und geprüft. Ein Interface kann ein anderes Interface erben. Fehlende oder abweichende Implementierungen werden vor der Ausführung abgewiesen.

`TypeOf actor Is Dog` prüft den tatsächlichen Typ. `actor Is other` und `actor IsNot Nothing` vergleichen Objektreferenzen. Klassen und Interfaces sind als Parameter-, Ergebnis- und Arrayelementtypen möglich. Object ist ein allgemeiner Objektreferenztyp. Variant kann Zahlen, Strings, Objekte und Container enthalten.

## 13. Reservierte Wörter und Ausführungsgrenzen

Eine zentrale Liste verhindert, dass implementierte Sprachschlüsselwörter als Namen deklariert werden. Dazu gehören Kontrollstrukturen, Typen, Sichtbarkeit, Objektwörter, logische Operatoren, True/False/Nothing und Input. New ist nur als Konstruktorname eine Ausnahme. Hilfsfunktionen wie Len und Count sind kontextabhängige Funktionsnamen und keine Sprachschlüsselwörter.


```text
IF THEN ELSE ELSEIF END WHILE WEND FOR EACH IN TO STEP NEXT SELECT CASE IS
ISNOT DO LOOP UNTIL EXIT CONTINUE DIM AS NEW CLASS INTERFACE INHERITS
IMPLEMENTS PUBLIC PRIVATE PROTECTED OVERRIDABLE OVERRIDES SUB FUNCTION RETURN
CALL LET PRINT INPUT REM IMPORTS INTEGER LONG SINGLE DOUBLE BOOLEAN STRING
VARIANT OBJECT COLLECTION TRUE FALSE NOTHING ME MYBASE AND OR NOT XOR MOD
REDIM PRESERVE TYPEOF ARRAY ADDHANDLER REMOVEHANDLER ADDRESSOF STOP
```

Zusätzliche Grenzen in 0.4: zusammen 16 Klassen/Interfaces, 16 Felder je Klasse einschließlich geerbter Felder, 64 Methoden/Prozeduren, 64 Objektinstanzen, 64 Variablen pro Bereich, acht Parameter und 32 verschachtelte Blöcke. Arrays/Collections: 32 Allokationen mit jeweils höchstens 128 Elementen. Strings: 127 Zeichen. Nach 1.000.000 Schritten wird die Ausführung begrenzt. Während eines laufenden Programms mit **Escape** vorzeitig abbrechen; im Eingabedialog Abbrechen verwenden. Die Pure-C-Version verwendet einen Stack von 512 KiB.

## 14. Projekte und Quelldateien verwalten (0.5)

### Neues Projekt

1. **Datei → Neues Projekt** oder Strg+N wählen.
2. Einen Namen mit ein bis acht Buchstaben/Ziffern eingeben, beispielsweise `HELLO`.
3. Bei Änderungen zwischen Speichern, Verwerfen und Abbrechen wählen.
4. Die IDE erzeugt `C:\TTBASIC\PROJECTS\HELLO` mit `HELLO.TBP` und einer neuen `MAIN.BAS`.
5. Das Projekt öffnet sich sofort. F5 startet das erste Hello-Programm.

Ein vorhandener Ordner gleichen Namens wird nicht überschrieben. Die acht Zeichen entsprechen den TOS-Dateinamen. Der vollständige Projektordner kann später verschoben werden; anschließend die .TBP-Datei am neuen Ort öffnen.

### Neue Datei hinzufügen

**Datei → Neue Datei** oder F7 wählen. `MODEL` oder `MODEL.BAS` eingeben. Die Datei wird im aktuellen Projektordner erzeugt, im .TBP registriert und im Editor geöffnet. Vorhandene Dateinamen werden abgewiesen. Eine neue Datei beginnt mit einem Kommentar.

Die Projektmitgliedschaft organisiert Dateien; **Imports bestimmt die für die Ausführung eingebundenen Module**. Für eine Klasse Model aus MODEL.BAS steht in MAIN.BAS:

```vb
Imports "MODEL.BAS"
Dim item As New Model
Print item.GetValue()
End
```

MODEL.BAS kann so aussehen:

```vb
Class Model
    Public Function GetValue() As Integer
        Return 42
    End Function
End Class
```

### Vorhandene Datei hinzufügen

**Datei → Datei hinzufügen** oder Ctrl+F7 wählen und im GEM-Dateiauswahldialog eine .BAS-Datei auswählen. Eine Datei außerhalb des Projektordners wird hineinkopiert; das Original bleibt erhalten. Eine Datei im Projektordner wird ohne Kopie registriert. Abhängige Quelldateien ebenfalls hinzufügen; relative Imports beziehen sich auf den neuen Ordner der kopierten Datei. Namenskonflikte überschreiben keine Dateien. Das Modul mit Imports in der Startdatei einbinden.

### Speichern und erneut öffnen

- F2 speichert nur die aktive Quelldatei.
- **Datei → Alles speichern**, Strg+Shift+S, speichert alle geänderten offenen Dateien und die Projektstruktur.
- **Datei → Projekt öffnen**, Strg+O, wählt eine .TBP-Datei. Dies ist ein anderes Format als das Pure-C-Buildprojekt TTBASIC.PRJ.
- F3 lädt nur die aktive Quelldatei neu.
- Vor einem Projektwechsel fragt die IDE nach ungespeicherten Änderungen.
- Beim nächsten Programmstart wird das zuletzt geöffnete Projekt wiederhergestellt. Ist es nicht verfügbar, wird DEFAULT versucht.

Der Projektbaum enthält registrierte Dateien und erkannte Imports-Abhängigkeiten. Dateinamen zum Wechseln anklicken, die unteren Pfeile zum Blättern verwenden. F5 startet immer die Startdatei des aktiven Projekts mit den Arbeitsfassungen geöffneter Dateien.

### Projektdateiformat

.TBP-Dateien sind einfacher Text mit Formatversion, Projektname, Startdatei und Mitgliedern:

```text
TTBASIC_PROJECT 1
NAME=HELLO
ENTRY=MAIN.BAS
FILE=MAIN.BAS
FILE=MODEL.BAS
```

Pfade innerhalb des Projektordners werden relativ gespeichert. Dadurch lässt sich der ganze Ordner verschieben. Die Startdatei muss auch Mitglied sein. Maximal sechzehn Dateien sind registrierbar. Beim Öffnen werden die Quellen geprüft; eine fehlende oder unlesbare Datei verhindert, dass das aktuelle Projekt ersetzt wird.

Änderungen der Mitgliedschaft werden sofort gespeichert. `.PTM` ist das temporäre Manifest, `.PBK` enthält die vorherige Struktur. Quelldateien behalten eigene `.TMP`- und `.BAK`-Dateien. Beispiele wechseln nach Rückfrage sicher in das installierte DEFAULT-Projekt.

## 15. Code-Vervollstaendigung (0.6)

Mit **Strg+Leertaste** zeigt der Editor Vorschlaege. Weiterschreiben filtert die Liste. Mit **Pfeil auf/ab** auswaehlen und mit **Enter** oder **Tab** einfuegen; **Escape** schliesst die Liste. Das aktuelle Wort wird ersetzt, der Rest der Zeile bleibt erhalten. Unvollstaendige oder fehlerhafte Anweisungen verhindern die Vorschlaege nicht.

Nach einem Punkt erscheint die Liste automatisch. Beispielsweise zeigt `third.` die oeffentlichen Mitglieder von Reken nach `Dim third As New Reken(10)` und `Imports "REKEN.BAS"`. Innerhalb einer Klasse zeigt `Me.` auch deren private Felder. Geerbte oeffentliche Mitglieder, Methodenparameter, lokale Variablen, Klassen, BASIC-Schluesselwoerter und Collection-Mitglieder werden beruecksichtigt. Unter der Liste steht der Typ oder die Methodensignatur. Nur der Name wird eingefuegt; Klammern und Argumente selbst eingeben, beispielsweise `third.GetValue()`. Auch Konstruktorargumente werden von Hand eingegeben. Geoeffnete Module werden mit ihren ungespeicherten Aenderungen analysiert; eine Ausfuehrung ist nicht notwendig.

Diese erste Version unterstuetzt einfache Empfaenger wie `third.`, `Me.` und `MyBase.`. Verkettete Ausdruecke wie `items(1).` und `factory.Create().` sowie Typinferenz aus Zuweisungen fehlen noch. Verwenden Sie ausdrueckliche `Dim ... As ...` Deklarationen. Maximal 16 Dateien, 512 Deklarationen und 64 Vorschlaege je Liste werden verarbeitet. Innerhalb von Strings und Kommentaren erscheint keine Liste.

## 16. Eigene GEM-Fenster und Ereignisse (0.7)

**Projekt → GEM-Beispiel laden**, danach F5, oder GEMDEMO.TBP oeffnen. Das Beispiel erzeugt ein echtes GEM-Fenster mit Textfeld, Checkbox, Schaltflaechen und Beschriftung. Elemente werden in BASIC oder mit dem Formularentwurf in Kapitel 18 erzeugt.

```basic
Dim form As New Window("Mein Programm", 320, 160)
Dim nameBox As New TextBox(form, "Marco", 16, 16, 240, 24)
Dim buttonA As New Button(form, "Begruessen", 16, 56, 120, 28)
Dim message As New Label(form, "Bereit", 16, 112, 280, 24)
AddHandler buttonA.Click, AddressOf Greet
form.Show()
Application.Run()
End

Sub Greet(sender As Button)
    message.Text = "Hallo " & nameBox.Text
End Sub
```

`New Window(title, width, height)` verwendet die Groesse des Arbeitsbereichs in Pixeln. Button, Label, TextBox und CheckBox verwenden `(form, text, x, y, width, height)` relativ zum Arbeitsbereich des Elternfensters. Elemente vorzugsweise vor Show() erzeugen. Fenster besitzen Titel, Schliessknopf und koennen verschoben werden.

| Objekt | Eigenschaften | Methoden | Ereignisse |
|---|---|---|---|
| Window | Text/Title, Visible | Show(), Close() | Load, Closed |
| Button | Text, Enabled, Visible | — | Click |
| Label | Text, Enabled, Visible | — | — |
| TextBox | Text, Enabled, Visible | — | Change |
| CheckBox | Text, Enabled, Visible, Checked | — | Click |

Eigenschaften lesen oder zuweisen, etwa `buttonA.Enabled = False`. Text/Title sind Strings, die anderen Eigenschaften Boolean. Tab/Shift+Tab wechseln Textfelder. Enter aktiviert die erste sichtbare aktive Schaltflaeche. Checkboxen reagieren auf Mausklicks. Escape stoppt das Programm.

`AddHandler control.Event, AddressOf Handler` bindet eine globale Sub oder Objektmethode, etwa `AddressOf controller.OnClick`. Der Handler hat keinen Parameter oder einen sender-Parameter des passenden GUI-Typs, Object oder Variant. Functions sind als Handler unzulaessig. Die Ereignisquelle muss derzeit eine einfache Variable sein; fuer ein Feld gegebenenfalls eine lokale Variable verwenden. `RemoveHandler control.Event, AddressOf Handler` entfernt die Bindung. Identische Bindungen werden einmal registriert. Aenderungen der Bindungsliste wirken fuer nachfolgende Ereignisse.

`Application.Run()` verarbeitet Ereignisse, bis alle Fenster geschlossen sind. `Application.Exit()` verlaesst diese Schleife; anschliessender Code kann noch laufen. Run() darf nicht verschachtelt werden, auch nicht aus Handlern. Show() erzeugt Load beim Oeffnen. Closed folgt nach dem Schliessen und kann es nicht verhindern. Window.Visible=False schliesst ebenfalls. TextBox.Change wird bei wirklicher Textaenderung erzeugt, auch durch BASIC-Zuweisungen. Bei Programmende, Fehler oder Abbruch werden alle BASIC-Fenster aufgeraeumt.

Grenzen: vier angelegte Fenster, 64 GUI-Objekte einschliesslich Fenster und 64 Ereignisbindungen je Lauf. Fenster/Elemente muessen in ihren Elternarbeitsbereich passen. GEM-Texte maximal 63 Zeichen; TextBox maximal `Min(63, (width - 8) / 8)` mit ganzzahliger Division. Fenstergroesse aendern, Timer, Listen, eigene Anwendungsmenues und eigenstaendige Kompilierung fehlen noch. Die Code-Vervollstaendigung kennt GUI-Typen, Eigenschaften, Methoden und Ereignisse.

## 17. Debugger (0.7)

| Aktion | Bedienung |
|---|---|
| Start / bis zum naechsten Haltepunkt weiter | F5 |
| Mit Schritten starten / Einzelschritt | F8 |
| Methodenaufruf vollstaendig ausfuehren | Shift+F8 |
| Methode verlassen | Strg+Shift+F8 |
| Haltepunkt der aktuellen Zeile umschalten | F9, oder linker Rand |
| Pause anfordern | Strg+P, oder Projektmenue |
| Erste sichtbare Variable ansehen | Strg+I, oder Variable anklicken |
| Ausfuehrung stoppen | Escape, oder Projektmenue |

**F8 ist ab 0.7 fuer den Debugger reserviert. Datei hinzufuegen liegt auf Strg+F7.** Neue Datei bleibt F7. F11/F12 werden auf der originalen Atari-Tastatur nicht benoetigt.

DEBUG.TBP zeigt einen kurzen Methodenaufruf. Einen Haltepunkt auf `result = calculator.DoubleValue(startValue)` setzen. F5 laeuft bis dorthin; F8 betritt die Methode, Shift+F8 fuehrt sie vollstaendig aus, Strg+Shift+F8 laeuft bis zum Aufrufer weiter. Haltepunkte gelten auch waehrend Schritten. Die gelbe Zeile wird als Naechstes ausgefuehrt. Auch die BASIC-Anweisung `Stop` fordert eine Debuggerpause an.

Rechts stehen Aufrufliste und Variablen. Mit `<`/`>` den Stackframe waehlen; der Editor zeigt seine Quellzeile. Frame 0 ist das Hauptprogramm. Parameter, lokale Variablen, Me-Felder und globale Variablen werden angezeigt. Pfeil auf/ab oder die unteren Pfeile scrollen. Anklicken oder Strg+I zeigt den dargestellten Wert und Typ. GUI-Werte enthalten ihren Text. Arrays/Collections zeigen die Elementzahl; einzelne Elemente und Felder anderer Objekte sind noch nicht aufklappbar. Die Ansicht ruft keine BASIC-Methoden auf und aendert keine Werte.

Haltepunkte funktionieren auch in Click-/Change-Handlern. Die entsprechende Aktion im Programmfenster loest die Pause aus. Beim Fortsetzen erscheint das Anwendungsfenster wieder vorne. Wartet Application.Run auf Eingaben, wird eine angeforderte Pause bei der naechsten ausfuehrbaren BASIC-Zeile wirksam; Escape stoppt auch beim Warten.

Unbehandelte Laufzeitfehler pausieren vor dem Abbau des Aufrufstapels. Lokale Werte bleiben damit sichtbar. F5/Escape beendet anschliessend den fehlerhaften Lauf. Fehlerhafte Zeilen koennen noch nicht geaendert und fortgesetzt werden. Quellcode ist waehrend der Pause schreibgeschuetzt. Immediate-Fenster, bedingte Haltepunkte und das Aendern laufender Variablen fehlen noch.

Maximal 64 Haltepunkte werden in C:\TTBASIC\STUDIO.BRK gespeichert. Sie verweisen auf physische Dateizeilen und verschieben sich bei Einfuegen/Loeschen noch nicht automatisch. Danach kontrollieren. Kommentare, Klassen-/Methodendefinitionen und Imports werden uebersprungen; ausfuehrbare Zeilen verwenden.

## 18. Grafischer Formularentwurf (0.8)

### Einstieg

1. **Datei → Neues Formularprojekt** waehlen oder **Strg+Shift+D** druecken.
2. Einen Projektnamen mit ein bis acht Zeichen eingeben. Eine eigene Projektmappe mit MAIN.BAS wird erstellt.
3. Links Auswahl, Button, Label, TextBox oder CheckBox waehlen und in das weisse Formular klicken.
4. Elemente durch Ziehen bewegen; die rechte untere Ecke aendert die Groesse. Auch das Formular besitzt dort einen Groessengriff.
5. Rechts Namen, Text, Position, Groesse und Handler bearbeiten. Die aktuellen Werte sind vorausgefuellt. Aktiv, Sichtbar und Markiert werden durch Anklicken umgeschaltet.
6. **Anwenden** kehrt zu BASIC zurueck; **F2/Strg+S** wendet an und speichert. **Testen/F5** wendet an und startet, speichert aber nicht automatisch.
7. **Strg+D** oder **Projekt → Formular entwerfen** oeffnet den Entwurf erneut, auch nach einem Neustart.

Das Beispiel **C:\TTBASIC\DESIGN.TBP** oeffnen und Strg+D druecken. Begruessungs- und Schliesscode sind bereits enthalten. Handgeschriebene Programme wie GEMDEMO werden nicht automatisch umgewandelt: Strg+D bietet ein eigenes Formularprojekt an und erhaelt den bisherigen Quellcode.

### Bedienung

| Aktion | Bedienung |
|---|---|
| Auswaehlen / bewegen | Auswahlwerkzeug; klicken oder ziehen |
| Groesse aendern | Rechte untere Ecke ziehen |
| Auswahl bewegen | Pfeiltasten, 8 Pixel |
| Groesse der Auswahl | Shift+Pfeil, 8 Pixel |
| Grosses Formular verschieben | Strg+Pfeil oder die vier linken Pfeilschaltflaechen |
| Element loeschen | Delete oder Loeschen |
| Letzte Entwurfsaenderung zurueck / wiederherstellen | Strg+Z oder Zurueck |
| Ereigniscode oeffnen | F4 oder Ereignis |
| Anwenden und speichern | F2 oder Strg+S |
| Anwenden und testen | F5 oder Testen |
| Entwurf verlassen | Abbrechen/Escape; geaenderte Entwuerfe anwenden oder verwerfen |
| Hilfe | F1 |

Position und Groesse sind Pixel im Arbeitsbereich des Formulars. Ziehen verwendet ein Acht-Pixel-Raster; Eigenschaften erlauben genaue Werte. Grosse Formulare lassen sich im Entwurfsbereich verschieben. Die Vorschau ist eine Entwurfsdarstellung; Testen zeigt die wirkliche GEM-Darstellung.

### Ereigniscode

Den ersten Button eines leeren Formulars markieren und F4 druecken. Eine fehlende Sub wird angelegt und der Cursor in ihren Rumpf gesetzt:

```basic
Sub Button1_Click(sender As Button)
    Form1.Close()
End Sub
```

Mit F2 speichern und F5 starten. Die Schaltflaeche schliesst nun das Formular. TextBox verwendet Change, Button und CheckBox Click. Das Formular selbst bietet Load; fuer den Schliess-Handler das Feld Closed waehlen. Labels haben keine Ereignisse. Ein leerer Handlername entfernt die Bindung; F4 erzeugt bei Bedarf einen Standardnamen.

Der Entwurf steht mit dem BASIC-Code in MAIN.BAS; eine separate Ressourcendatei ist nicht erforderlich. Den Bereich zwischen `Rem @TTFORM BEGIN 1` und `Rem @TTFORM END` verwaltet der Designer. **Eigenen Ereigniscode ausserhalb dieses Bereichs schreiben.** Vorhandene Subs und ihre Inhalte bleiben bei Layoutaenderungen erhalten. Entfernte oder umbenannte Bindungen loeschen die alte Sub nicht. Handlernamen sind von Objektnamen unabhaengig. Eine Umbenennung aktualisiert erzeugte Deklarationen und Bindungen; Verweise im eigenen BASIC-Code selbst anpassen. Ereignis-Subs koennen importierte Prozeduren oder Objektmethoden aufrufen.

### Grenzen

Die erste Version verwaltet ein Formular in der Startdatei des Projekts mit maximal sechzehn Elementen. Breite 120–600, Hoehe 64–400 Pixel; alle Elemente muessen hineinpassen. Textgrenzen und maximal 256 Quellzeilen gelten auch fuer Layout und Handler. Bei Erzeugungsfehlern bleibt die Quelle erhalten; der Entwurf kann angepasst werden. Haltepunkte nach Layoutaenderungen kontrollieren: sie verwenden physische Zeilennummern. Mehrfachauswahl, automatische Refaktorierung eigenen Codes und das Einlesen beliebiger handgeschriebener GUI-Layouts fehlen noch.

## 19. MIDI und Yamaha-DX7-Demo (0.9)

**Projekt → DX7 MIDI-Demo oeffnen**, danach F5, oder C:\TTBASIC\DX7DEMO.TBP oeffnen. Jeder Lauf beginnt im **Trockenlauf**: Hex-Nachrichten erscheinen in der Ausgabe, werden aber nicht gesendet. Kanal 1–16 und interne Voice 1–32 waehlen. Select voice wechselt das Programm; Apply sound sendet Algorithmus, Feedback und den Pegel des gewaehlten Operators. Note 60 on/off steuert einen Testton. Panic sendet Sustain-Aus und 128 ausdrueckliche Note-Off-Nachrichten auf dem zuletzt verwendeten Kanal; auch bei ungueltiger Eingabe bleibt es nutzbar. Close beendet das Fenster.

**Send MIDI OUT** aktiviert die Uebertragung. TX bedeutet, dass Bytes an den MIDI-Ausgang uebergeben wurden, nicht dass ein Synthesizer sie bestaetigt hat. Geraeteerkennung und MIDI-Empfang fehlen noch. Die Felder zeigen gewuenschte Werte, keine vom DX7 ausgelesenen Einstellungen.

### Anschliessen

Am echten TT MIDI OUT mit DX7 MIDI IN verbinden; der Ton kommt vom DX7. Den Empfangskanal angleichen. Fuer Parameter-SysEx die originale Yamaha-Anleitung beachten: System Information verfuegbar und Memory Protect aus. Vor dem Bearbeiten die Voice waehlen. Programmwechsel kann ungespeicherte Aenderungen verwerfen; bei Bedarf zuvor am DX7 speichern.

Unter macOS verwendet Hatari einen PortMidi-Ausgang. USB-MIDI-Interface vor Hatari anschliessen und in den MIDI-Geraeteeinstellungen auswaehlen. Fuer neu angeschlossene Ports Hatari neu starten. Die Checkbox der Demo konfiguriert den Hatari-Port nicht. Trockenlauf funktioniert ohne Interface/Synthesizer; Entwicklungstests aendern die Benutzerkonfiguration nicht.

### BASIC-API

| Aufruf | Bedeutung |
|---|---|
| Midi.EnableOutput(True/False) | Senden ein/aus; jeder F5-Lauf beginnt ausgeschaltet |
| Midi.Enabled() | Boolean-Ausgabestatus |
| Midi.LastMessage() | Letzte Nachricht/Status als String |
| Midi.CheckRange(value, minimum, maximum) | Gepruefter Integer; sonst Laufzeitfehler |
| Midi.NoteOn(channel, note, velocity) | Kanal 1–16; Note/Velocity 0–127 |
| Midi.NoteOff(channel, note[, velocity]) | Note loslassen; Velocity standardmaessig 0 |
| Midi.ProgramChange(channel, program) | Programm 0–127 |
| Midi.ControlChange(channel, controller, value) | Controller/Wert 0–127 |
| Midi.PitchBend(channel, value) | 0–16383; Mitte 8192 |
| Midi.SysEx(bytes) | Array mit 2–128 Bytes einschliesslich F0/240 und F7/247 |
| Midi.StopNotes(channel) | Eigene Noten/Sustain dieses Laufs loslassen |
| Midi.Panic(channel) | Sustain aus und alle 128 Note-Off-Nachrichten |

```basic
Midi.EnableOutput(False)
Midi.NoteOn(1, 60, 96)
Print Midi.LastMessage()
Midi.NoteOff(1, 60)
```

Note On erscheint als `DRY: 90 3C 60`. SysEx verlangt F0/F7 und sieben Bit breite Nutzdaten. Running Status wird nicht verwendet. Lange Logzeilen werden gekuerzt, das gepruefte Paket jedoch vollstaendig gesendet.

Bei Stop, Ende, Fehler oder Abschalten werden eigene Noten und Sustain losgelassen. Auch Debuggerpausen lassen sie los; Fortsetzen spielt sie nicht automatisch erneut. Note On mit Velocity 0 gilt als Note Off. Die Verwaltung umfasst nur Nachrichten dieses Programms.

### DX7-Klasse

DX7.BAS zeigt den Aufbau der sieben Byte langen Nachrichten in BASIC. Ein Arraypuffer wird fuer alle Aenderungen wiederverwendet.

```basic
Imports "DX7.BAS"
Dim synth As New DX7(1)
synth.Program(1)
synth.Algorithm(6)
synth.Feedback(3)
synth.OperatorLevel(1, 90)
Print Midi.LastMessage()
```

Ohne EnableOutput(True) bleibt es beim Trockenlauf. Voice/Algorithmus verwenden 1–32, Feedback 0–7, Operator 1–6 und Pegel 0–99. Kanal-/Programmwechsel lassen eigene Noten vorher los. Strg+D oeffnet die Demo im Formularentwurf; Ereigniscode bleibt bearbeitbar.

Die Demo zielt auf den originalen DX7. Grundlage ist Yamaha MIDI Data Format, S. 30–32 der [DX7-Anleitung](https://usa.yamaha.com/files/download/other_assets/9/333979/DX7E1.pdf): Algorithmus 134, Feedback 135, Operatorpegel `(6 - operator) * 21 + 16`. Keine Patchbanken, Speicherbefehle oder Bulk-Transfers. Software/Emulation werden getestet; die Pruefung mit einem echten DX7 steht noch aus.

### Arturia DX7 V mit MIDI Learn

Beim Start ist **Arturia CC18** aktiviert. Ordnen Sie in DX7 V Algorithm per MIDI Learn Kanal 1, CC 18 zu und wählen Sie denselben Kanal in der Demo. **Apply sound** setzt Algorithm 1–32 in CC-Werte 0–127 um. Voice, Feedback und Operatorpegel sind in diesem Modus deaktiviert, da ihre Arturia-Zuordnungen noch nicht eingerichtet sind. Deaktivieren Sie Arturia CC18 für die ursprüngliche Yamaha-SysEx-Steuerung. MIDI OUT ist beim Start ausgeschaltet. Die Zuordnung muss in DX7 V eingerichtet werden; die Demo erstellt sie nicht selbst.

## 20. Zahlen und Arrayfelder (0.10)

Single verwendet 32-Bit-Gleitkommagenauigkeit mit etwa 6–7 signifikanten Stellen. Double nutzt die Genauigkeit der C-Laufzeit, mindestens 15 signifikante Stellen; Pure C rechnet intern mit erweiterter Genauigkeit. PRINT/CStr und Debugger zeigen maximal 15 signifikante Stellen. Gleitkommazahlen sind Näherungen; berechnete Werte mit einer Toleranz vergleichen, etwa `Abs(a - b) < 0.000001`. Dies ist keine exakte dezimale Currency-Arithmetik.

Double unterstützt null oder Beträge von 1e-300 bis 1e300. Single unterstützt null oder ungefähr 1.175494e-38 bis 3.402823e38; subnormale Werte werden abgewiesen. Ungültige Zahlen, Überlauf und Unterlauf ergeben Fehler. Literale: `0.5`, `.25`, `1.`, `1.5e-3`. Kommas trennen Argumente.

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

Ausgabe: 15.125, 3.5, 3, 512, 2 und 4. Integer/Long, Arrayindizes/-grenzen, MIDI-Daten und Bildschirmkoordinaten verlangen ganze Zahlen; Brüche werden nicht still abgeschnitten. CInt/CLng runden, Fix schneidet Richtung null ab, Int rundet ab. CDbl, CSng und Val konvertieren Zahlen oder vollständige numerische Texte. Input unterstützt Single/Double und Dezimaleingaben.

Mathematik: Abs, Sgn, Sqr, Sin, Cos, Tan, Atn, Exp und Log. Winkel im Bogenmaß; Log ist der natürliche Logarithmus. Negative Quadratwurzeln, Log von null/negativen Zahlen und negative Potenzbasen mit nicht-ganzzahligen Exponenten verursachen Bereichsfehler. For unterstützt Single/Double-Zähler und gebrochene Step-Werte; zu kleine Schritte, die den Zähler nicht ändern können, werden abgewiesen.

Arrayfelder dürfen fest oder dynamisch sein. Jede Instanz erhält ein eigenes festes Array. Feste Feldgrenzen müssen beim Laden verfügbar sein: numerische Literale verwenden. Öffentlicher Zugriff, privater Zugriff über Me, Vererbung und ReDim Preserve funktionieren:

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

### Migration von 0.9

`/` ist jetzt Gleitkommadivision. Für ganzzahliges Abschneiden `\` verwenden. Gebrochene Werte für Integer/Long brauchen eine explizite Konvertierung. Single und Double sind reservierte Typnamen: eine Methode Double beispielsweise in DoubleValue umbenennen. DEBUG.BAS und die MIDI-Beispiele wurden angepasst. Eigene vorhandene Quellen werden nicht automatisch geändert.

### Weitergabe

TT BASIC 0.10 ist ein begrenzter, von VB inspirierter Dialekt. Dokumentierte Konstrukte werden getestet; es ist nicht die vollständige VB-Sprache. Mehrdimensionale Arrays, BASIC-Dateizugriff, generische Collections, Überladungen, Garbage Collection und Ausnahmebehandlung fehlen weiterhin. Grenzen für Quellen, Objekte und Container gelten weiterhin. Arrays maximal 128 Elemente; Indizes innerhalb ±1.000.000. Projekte auf dem Zielrechner testen; eine Prüfung auf einem physischen TT steht noch aus.
