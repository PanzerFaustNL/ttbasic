# TT BASIC Studio 0.3 — Deutsches Handbuch

TT BASIC Studio verbindet einen BASIC-Interpreter mit einer GEM-Entwicklungsumgebung für den Atari TT. Sowohl die IDE als auch das BASIC-Programm laufen auf dem Atari selbst. Diese Version wurde mit Pure C erstellt und in Hatari mit TOS 3.06, TT Medium (640 × 480, 16 Farben), 8 MiB ST-RAM und 256 MiB TT-RAM getestet. Tests auf einem physischen Atari TT stehen noch aus.

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
- Unter **Imports** stehen die vom Projekt eingebundenen Dateien.
- MAIN.BAS oder eine Importdatei anklicken, um sie zu bearbeiten. Änderungen und Cursorposition bleiben für jede geöffnete Datei erhalten.
- Mit `- Demo` / `+ Demo` lässt sich der Projektbaum ein- und ausklappen.
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
| Neue leere Startdatei | Datei → Neu MAIN.BAS, Strg+N |
| Aktive Datei erneut von Datenträger laden | F3, Strg+O |
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

Speichern erzeugt zunächst eine `.TMP`-Datei und bewahrt die vorherige Datenträgerversion als `.BAK`. Zu COUNTER.BAS gehören also COUNTER.TMP und COUNTER.BAK. Geänderte Importdateien einzeln auswählen und mit F2 speichern; „Alles speichern“ gibt es noch nicht.

Beim Beenden werden alle geöffneten Dateien auf Änderungen geprüft. Das Beenden abbrechen, um diese Dateien zuerst zu speichern. Neu MAIN.BAS und das OO-Beispiel verändern zunächst den Editor; erst Speichern ersetzt die Startdatei auf dem Datenträger. Der Projektpfad ist derzeit fest auf C:\TTBASIC eingestellt. Allgemeine Öffnen- und Speichern-unter-Dialoge fehlen noch.

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

Das Ausgabefenster zeigt die letzten vier Zeilen. Fehler stoppen die Ausführung. Die Ausgabe ist textbasiert; eigene GEM-Anwendungen oder Formulare entstehen dadurch noch nicht.

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

Let ist optional. Integer und Long bezeichnen derzeit denselben begrenzten numerischen Typ: ganze Zahlen von **−1.000.000 bis +1.000.000**. Dies ist eine Grenze des Interpreters, nicht die ursprüngliche VB-Integer-Breite und nicht der vollständige 32-Bit-Bereich von C.

Numerische Variablen beginnen mit null. Aus Kompatibilitätsgründen können A bis Z auch ohne Dim als null gelesen werden. Eine Zuweisung an einen bisher unbekannten Namen erzeugt eine numerische Variable. Andere unbekannte Namen zu lesen ist ein Fehler; explizite Dim-Deklarationen sind empfehlenswert.

Eine zweite Dim-Deklaration desselben Namens im selben Gültigkeitsbereich wird abgewiesen. Parameter und lokal deklarierte Variablen gehören zum aktuellen Methodenaufruf. Eine lokale Dim-Deklaration kann einen globalen Namen verdecken. Wenn keine lokale Variable existiert, verwendet eine Zuweisung eine bereits vorhandene globale Variable dieses Namens.

### Operatoren

| Operator | Bedeutung |
| --- | --- |
| `+`, `-` | Addition, Subtraktion und Vorzeichen |
| `*`, `/` | Multiplikation und ganzzahlige Division |
| `(...)` | Gruppierung |
| `=`, `<>`, `<`, `>`, `<=`, `>=` | Numerische Vergleiche |

Multiplikation und Division werden vor Addition und Subtraktion ausgewertet. 7 / 2 ergibt 3, 7 / -2 ergibt -3. Vergleiche liefern -1 für wahr und 0 für falsch. Division durch null und Ergebnisse außerhalb des Zahlenbereichs verursachen Fehler.

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

Semikolons verbinden Elemente auf **einer Ausgabezeile**, ohne Leerzeichen hinzuzufügen. Gewünschte Leerzeichen gehören in das Textliteral. Jede Print-Anweisung beendet die Zeile, auch bei abschließendem Semikolon. Eine Ausgabezeile enthält höchstens 79 Zeichen. Zeichenketten sind bisher nur als Literale in Print verfügbar. Stringvariablen, Verkettung mit + und Escape-Sequenzen für Anführungszeichen fehlen noch.

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

value ist ein numerisches Feld, das in jeder Instanz getrennt gespeichert wird. Felder beginnen mit null oder mit einem ganzzahligen Literal wie `Public Value As Integer = 10`. Public erlaubt Zugriff von außerhalb der Klasse. Private erlaubt Zugriff nur innerhalb von Methoden derselben Klasse. Sichtbarkeit ausdrücklich angeben; ohne Angabe ist ein Mitglied in dieser Version öffentlich.

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

Eine Objektvariable ohne New beginnt als Nothing. Mitglieder vor der Objekterzeugung anzusprechen ist ein Fehler. Eine Objektvariable akzeptiert nur Nothing oder eine Instanz der deklarierten Klasse. Eine Instanz einer anderen Klasse kann nicht zugewiesen werden.

Objekte bleiben bis zum Ende eines Laufs reserviert. Nothing entfernt eine Referenz, gibt in dieser Version aber keinen Objektplatz frei. Jeder F5-Lauf beginnt mit einem leeren Objektpool.

## 7. Subs, Functions und Parameter

Eine Sub führt Aktionen aus und gibt keinen Wert zurück. Eine Function liefert mit **Return** eine Zahl zurück. Beide sind als Klassenmethoden oder globale Prozeduren möglich, auch in Importdateien.

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

Call ist optional; ShowNumber(36) funktioniert ebenfalls. Argumente stehen in Klammern. Maximal vier numerische Parameter werden als Werte übergeben. Die Anzahl muss exakt passen. Integer oder Long verwenden; Objekt- und Stringparameter fehlen noch.

Eine Function muss einen numerischen Wert mit Return zurückgeben. Eine Zuweisung an den Funktionsnamen ersetzt Return nicht. Eine Sub kann mit Return ohne Wert oder Exit Sub vorzeitig beendet werden. Eine Sub in einer Berechnung zu verwenden ist ein Fehler. Ein alleinstehendes End beendet das Hauptprogramm und darf innerhalb einer Methode nicht als Ersatz für Return verwendet werden.

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
| `Function finished without Return` | Numerische Return-Anweisung ergänzen |
| `Sub cannot be used as an expression` | Für einen Rückgabewert eine Function verwenden |
| `Unknown variable` | Deklaration, Schreibweise und Gültigkeitsbereich |
| `Division by zero` | Divisor darf nicht null sein |
| `Result exceeds numeric range` | Zwischenergebnisse innerhalb ±1.000.000 halten |
| `Maximum ...` / `exceeds ...` | Grenzen dieser Version beachten |

## 9. Grenzen und noch fehlende Funktionen

| Bestandteil | Grenze |
| --- | --- |
| Projektdateien einschließlich MAIN.BAS | 16 |
| Zeilen pro Datei | 64 |
| Insgesamt geladene Quellzeilen | 512 |
| Zeichen pro Quellzeile | 95, ASCII |
| Länge eines Bezeichners | 23 Zeichen |
| Klassen | 8 |
| Numerische Felder pro Klasse | 8 |
| Klassenmethoden und globale Prozeduren zusammen | 32 |
| Objektinstanzen pro Lauf | 32 |
| Variablen pro globalem/lokalem Bereich | 48 |
| Parameter pro Methode/Prozedur | 4, numerisch |
| Verschachtelte Methodenaufrufe | 8 |
| Verschachtelte Ausdrücke | 16 |
| Zahlenbereich | −1.000.000 bis +1.000.000 |
| Zeichen pro Ausgabezeile | 79 |

Noch nicht implementiert sind If/Then, While/Wend, For/Next, interaktive Eingaben, Arrays, Stringvariablen, Vererbung, Interfaces, Polymorphie, Überladung, Objektfelder/Objektparameter, automatische Speicherbereinigung und der visuelle Formular-Designer. Der Interpreter erzeugt noch keine eigenständigen PRG-Dateien aus BASIC-Programmen. Vorhandene VB-Programme müssen an diese unterstützte Teilmenge angepasst werden.

## 10. Projektquellcode

Die IDE steht in src/IDE.C, der Interpreter in src/BASIC.C. Native Menüs werden in src/MENU.C und Übersetzungen in src/LANG.C implementiert. Pure-C-Projekt- und Linkerdateien liegen unter atari/. BASIC-Beispiele aus examples/ nach C:\TTBASIC kopieren, um sie auf dem Atari zu verwenden.

Bei einer Fehlermeldung im GitHub-Repository bitte die Studio-Version, ein minimales BASIC-Beispiel, die vollständige Meldung und die verwendete Plattform angeben: Hatari oder physische Atari-Hardware.
