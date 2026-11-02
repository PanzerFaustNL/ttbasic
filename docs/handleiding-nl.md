# TT BASIC Studio 0.3 — Nederlandstalige handleiding

TT BASIC Studio is een BASIC-interpreter en GEM-ontwikkelomgeving voor de Atari TT. De IDE en je BASIC-programma draaien op de Atari zelf. De huidige versie is gebouwd met Pure C en getest in Hatari met TOS 3.06, TT Medium (640 × 480, 16 kleuren), 8 MiB ST-RAM en 256 MiB TT-RAM. Testen op een fysieke TT staat nog open.

Dit is een vroege taalversie. Deze handleiding beschrijft wat **nu werkt**; het is geen documentatie voor Microsoft Visual Basic. BASIC-trefwoorden en interpreterfoutmeldingen zijn Engels, ook wanneer de IDE Nederlands of Duits toont.

[English](manual-en.md) · [Deutsch](handbuch-de.md)

## 1. Installeren en starten

1. Zet de gebouwde `TTBASIC.PRG` in `C:\TTBASIC` op de Atari.
2. Zet de voorbeeldbestanden `MAIN.BAS`, `OODEMO.BAS`, `COUNTER.BAS` en eventueel `MATH.BAS` in dezelfde map. Een bestaand `MAIN.BAS` hoef je niet te vervangen.
3. Start `C:\TTBASIC\TTBASIC.PRG` vanaf het GEM-bureaublad.
4. Gebruik TT Medium of een resolutie van minimaal 640 × 480.

In de ingerichte Hatari-omgeving komt Atari-pad `C:\TTBASIC` overeen met de Mac-map `Atari-TT/Schijf-C/TTBASIC`. De repository bevat de eigen broncode, voorbeelden en buildinstructies; de emulator, TOS en Pure C worden afzonderlijk geïnstalleerd. Zelf bouwen staat in [README](../README.md#building-on-the-atari).

## 2. De ontwikkelomgeving

De menubalk staat bovenaan het scherm. Links staat het project, centraal de editor en onderaan de uitvoer. Het eigenschappenpaneel rechts kun je tonen via **Beeld → Eigenschappen**.

- `MAIN.BAS` is het startbestand van het project.
- Onder **Imports** verschijnen externe bestanden die vanuit het project worden gebruikt.
- Klik op `MAIN.BAS` of een importbestand om het te bewerken. Cursorpositie en wijzigingen blijven per geopend bestand bewaard.
- Klik op `- Demo` of `+ Demo` om de projectboom in of uit te klappen.
- Een sterretje op de bestands-tab betekent dat het actieve bestand is gewijzigd.
- **F5 voert altijd het project vanaf `MAIN.BAS` uit**, ook als je op dat moment `COUNTER.BAS` bewerkt.
- F5 gebruikt de werkversies van geopende bestanden. Je hoeft wijzigingen dus niet eerst op te slaan.
- Een nieuwe uitvoering begint met nieuwe variabelen en objecten; programmatoestand blijft niet tussen uitvoeringen bestaan.

De importlijst wordt opnieuw bepaald bij laden, opslaan en uitvoeren. Nieuwe of gewijzigde `Imports`-regels worden daardoor uiterlijk bij F5 zichtbaar. Een bestand dat buiten de IDE is gewijzigd, laad je opnieuw met F3; een al geopend bestand blijft anders de werkversie in de IDE gebruiken.

### Taal kiezen

Kies **Beeld → Nederlands / English / Deutsch**. Ook de menukoppen veranderen: in het Engels heet Beeld **View**, in het Duits **Ansicht**. De keuze wordt bewaard in `C:\TTBASIC\STUDIO.CFG`. Programmabestanden blijven in alle talen hetzelfde. De oorspronkelijke Atari-letterset wordt gebruikt; Duitse teksten gebruiken daarom bijvoorbeeld `ue` in plaats van `ü`.

### Menu's en toetsen

| Actie | Menu of toets |
| --- | --- |
| Nieuw leeg startbestand | Bestand → Nieuw MAIN.BAS, Ctrl+N |
| Actief bestand opnieuw van schijf laden | F3, Ctrl+O |
| Actief bestand opslaan | F2, Ctrl+S |
| Project uitvoeren | Project → Uitvoeren, F5 |
| OO-voorbeeld in MAIN.BAS laden | Project → OO-voorbeeld laden |
| Laatste bewerking ongedaan maken | Ctrl+Z |
| Hele huidige regel kopiëren / knippen / plakken | Ctrl+C / Ctrl+X / Ctrl+V |
| Zoeken in het actieve bestand | Ctrl+F |
| Volgende zoekresultaat | F6 |
| Eigenschappen tonen of verbergen | F4 |
| Panelen tonen of verbergen | Beeld |
| Sneltoetsen tonen | F1 |
| Afsluiten | Ctrl+Q |

Zoeken is niet hoofdlettergevoelig en gaat na het laatste resultaat verder aan het begin. Het klembord is een intern **regelklembord**, geen tekstselectie of systeemklembord. Plakken voegt een hele regel in; een lege huidige regel wordt vervangen. Undo werkt één bewerking terug en wordt gewist bij opslaan, opnieuw laden of wisselen van bestand. Er is nog geen redo.

### Opslaan en sluiten

Opslaan schrijft eerst een tijdelijk `.TMP`-bestand en bewaart de vorige schijfversie als `.BAK`. Voor `COUNTER.BAS` zijn dat `COUNTER.TMP` en `COUNTER.BAK`. Bewerkte importbestanden sla je afzonderlijk op door ze te openen en F2 te gebruiken. Er is nog geen ‘Alles opslaan’.

Bij afsluiten controleert de IDE alle geopende bestanden op wijzigingen. Annuleer afsluiten om die bestanden eerst op te slaan. ‘Nieuw MAIN.BAS’ en het OO-voorbeeld veranderen de editor; pas bij F2 vervang je het startbestand op schijf. De editor gebruikt momenteel een vast projectpad, `C:\TTBASIC`, en heeft nog geen algemene Openen/Opslaan-als-dialoog.

## 3. Je eerste programma

Voer dit in `MAIN.BAS` in en druk op F5:

```vb
Print "Hallo Atari TT!"
Dim answer As Integer = 6 * 7
Print answer
End
```

Uitvoer:

```text
Hallo Atari TT!
42
Program finished.
```

Het uitvoerpaneel toont de laatste vier regels. Fouten stoppen de uitvoering. De uitvoer is tekst; dit maakt nog geen zelfstandig GEM-programma of formulier.

## 4. Getallen, variabelen en commentaar

Namen zijn niet hoofdlettergevoelig: `answer`, `ANSWER` en `Answer` zijn dezelfde variabele. Namen beginnen met een letter of underscore; daarna mogen letters, cijfers en underscores volgen. Gebruik maximaal 23 tekens en gebruik geen BASIC-trefwoorden als naam.

```vb
Dim count As Integer
Dim total As Long = 40
count = 2
Let total = total + count
Print total
Rem Dit is commentaar
' Dit is ook commentaar
Print total ' Commentaar na een statement
```

`Let` is optioneel. `Integer` en `Long` zijn in deze interpreter dezelfde numerieke soort: gehele getallen van **−1.000.000 tot en met 1.000.000**. Dat is een taalgrens, niet de oorspronkelijke VB-breedte van `Integer` of de volledige 32-bitrange van C.

Numerieke variabelen beginnen op nul. Voor compatibiliteit kun je A tot en met Z ook zonder `Dim` lezen; ze beginnen op nul. Toekennen aan een nog onbekende naam maakt een numerieke variabele. Voor andere onbekende namen geeft lezen een fout; gebruik bij voorkeur altijd `Dim`.

Een dubbele `Dim` in dezelfde scope geeft een fout. Parameters en lokaal gedeclareerde variabelen zijn lokaal aan een methode; een lokale `Dim` kan een globale naam afschermen. Een toekenning aan een al bestaande globale naam gebruikt die globale variabele wanneer er geen lokale naam bestaat.

### Operatoren

| Operator | Betekenis |
| --- | --- |
| `+`, `-` | Optellen, aftrekken; ook unaire tekens |
| `*`, `/` | Vermenigvuldigen, gehele deling |
| `(...)` | Groeperen |
| `=`, `<>`, `<`, `>`, `<=`, `>=` | Numerieke vergelijking |

Vermenigvuldigen en delen gaan vóór optellen en aftrekken. `7 / 2` geeft `3`; `7 / -2` geeft `-3`. Vergelijkingen geven `-1` voor waar en `0` voor onwaar. Delen door nul en resultaten buiten het bereik geven een fout.

```vb
Print 2 + 3 * 4
Print (2 + 3) * 4
Print 42 >= 40
```

Dit geeft `14`, `20` en `-1`.

### Print en tekst

```vb
Print "Waarde: "; total
Print "Som: "; 2 + 3; "!"
Print
```

Een puntkomma verbindt items op **één** uitvoerregel en voegt geen spatie toe. Zet gewenste spaties in het tekstliteral. Iedere `Print` eindigt de uitvoerregel, ook met een laatste puntkomma. De maximale uitvoerregel is 79 tekens. Strings zijn momenteel uitsluitend literals in `Print`: stringvariabelen, concatenatie met `+` en escape-sequenties voor aanhalingstekens zijn nog niet geïmplementeerd.

## 5. Imports: externe bestanden gebruiken

```vb
Imports "COUNTER.BAS"
```

TT BASIC gebruikt `Imports` voor **BASIC-bronbestanden**. Dit is een eigen taalkeuze: Microsoft VB gebruikt Imports normaal voor namespaces.

- Zet Imports buiten een `Class`, `Sub` of `Function`.
- Gebruik een bestandsnaam tussen dubbele aanhalingstekens.
- Een relatief pad wordt opgelost vanuit het bestand waarin de Imports-regel staat.
- Ook een submap, bijvoorbeeld `Imports "LIB\MATH.BAS"`, kan worden gebruikt. Houd op TOS rekening met 8.3-bestandsnamen.
- Klassen en procedures worden vóór uitvoering geregistreerd, zodat ze ook vanuit eerder geschreven code kunnen worden aangeroepen.
- Een bestand wordt per uitvoering één keer geladen. Herhaalde imports en gedeelde afhankelijkheden geven geen dubbele definities.
- Circulaire imports stoppen met `Circular Imports dependency`.
- Statements buiten definities in een importbestand worden één keer uitgevoerd op de plaats van de import. Zet daarom doorgaans alleen definities in een bibliotheekbestand; een los `End` zou het hele project stoppen.

Voorbeeld van een procedurebibliotheek in `MATH.BAS`:

```vb
Function DoubleValue(value As Integer) As Integer
    Return value * 2
End Function
```

Gebruik in `MAIN.BAS`:

```vb
Imports "MATH.BAS"
Print DoubleValue(21)
End
```

## 6. Klassen en objecten

Een klasse beschrijft de gegevens en bewerkingen van een object. Met `New` maak je een afzonderlijke instantie. Sla deze klasse op als `COUNTER.BAS`:

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

### Velden en zichtbaarheid

`value` is een numeriek veld per object. Velden beginnen op nul; een letterlijke standaardwaarde zoals `Public Value As Integer = 10` is ook mogelijk. `Public` maakt een lid van buitenaf toegankelijk. `Private` beperkt toegang tot methoden van dezelfde klasse. Gebruik expliciete zichtbaarheid; zonder aanduiding is een lid in deze versie publiek.

Een directe `first.value` is bij de bovenstaande klasse dus een fout. `first.GetValue()` is de bedoelde toegang. Voor publieke velden werken zowel lezen als schrijven, bijvoorbeeld `first.Value = 42` wanneer `Value` publiek is.

### Constructor en Me

`Public Sub New(...)` is de constructor en wordt automatisch door `New Counter(...)` uitgevoerd. Zonder constructor mag je `New Counter` of `New Counter()` gebruiken zonder argumenten. Heeft de klasse wel een constructor, dan moeten de argumenten overeenkomen.

`Me` verwijst naar het huidige object en is alleen binnen een klassemethode geldig. Gebruik `Me.value` en `Me.MethodName(...)` om leden van dat object expliciet te benaderen.

### Afzonderlijke instanties en referenties

In `MAIN.BAS`:

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

Uitvoer is `First: 42`, `Second: 2`, `Alias: 42`. `first` en `second` zijn afzonderlijke objecten. `alias = first` kopieert een **referentie**: beide namen verwijzen daarna naar hetzelfde object.

Dit voorbeeld staat al in `OODEMO.BAS`. Kies **Project → OO-voorbeeld laden**, daarna F5. Je vorige `MAIN.BAS` op schijf blijft behouden totdat je zelf opslaat.

### Nothing en typen

```vb
Dim counter As Counter
counter = New Counter(5)
counter = Nothing
```

Een objectvariabele zonder `New` begint als `Nothing`. Leden benaderen voordat een object is gemaakt geeft een fout. Een objectvariabele accepteert alleen `Nothing` of een instantie van de gedeclareerde klasse. Je kunt bijvoorbeeld geen `Counter` aan een variabele van een andere klasse toekennen.

Objecten bestaan tot het einde van de uitvoering. `Nothing` verwijdert de referentie, maar maakt in deze versie geen objectslot vrij. F5 begint met een lege objectpool.

## 7. Subs, Functions en parameters

Een `Sub` voert handelingen uit en geeft geen waarde terug. Een `Function` geeft een getal terug met **`Return`**. Beide kunnen klassemethode of globale procedure zijn en in een importbestand staan.

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

`Call` is optioneel: `ShowNumber(36)` werkt ook. Argumenten worden tussen haakjes doorgegeven. Er zijn maximaal vier numerieke parameters; ze worden als waarde doorgegeven. De aantallen moeten precies overeenkomen. Gebruik momenteel `Integer` of `Long`; objectparameters en stringparameters worden niet ondersteund.

Een Function moet met een Return-waarde eindigen. Toekennen aan de functienaam vervangt Return niet. Een Sub mag vroeg eindigen met `Return` zonder waarde of `Exit Sub`. Een Sub gebruiken in een berekening is een fout. Een los `End` beëindigt het hoofdprogramma en is niet toegestaan als vervanging voor Return binnen een methode.

## 8. Fouten begrijpen

Een foutmelding bevat de bestandsnaam, het bronregelnummer en de Engelse oorzaak, bijvoorbeeld:

```text
COUNTER.BAS:9: Division by zero
```

De regel is de fysieke regel in dat bestand, inclusief lege regels en commentaar. De IDE markeert de betrokken regel als dat bestand actief is. Is MAIN.BAS actief en komt de fout uit een import, dan wijst de editor naar de importregel; de uitvoer noemt de werkelijke foutlocatie.

| Melding of fragment | Wat controleren? |
| --- | --- |
| `Cannot open imported file` | Bestandsnaam, map, pad en aanwezigheid van het bestand |
| `Circular Imports dependency` | A importeert B en B importeert direct of indirect A |
| `Unknown Class` | Spelling, Imports en volledige Class-definitie |
| `Private member` | Gebruik een publieke methode in plaats van directe toegang |
| `Object is Nothing` | Maak eerst een instantie met New |
| `Wrong number of method arguments` | Constructor- of methodesignatuur |
| `Object Class does not match` | Object en gedeclareerde klasse moeten overeenkomen |
| `Function finished without Return` | Voeg een numerieke Return toe |
| `Sub cannot be used as an expression` | Gebruik een Function wanneer een resultaat nodig is |
| `Unknown variable` | Declareer de naam en controleer spelling en scope |
| `Division by zero` | Deler mag geen nul zijn |
| `Result exceeds numeric range` | Houd tussenresultaten binnen ±1.000.000 |
| `Maximum ...` of `exceeds ...` | Bekijk de versiegrenzen hieronder |

## 9. Huidige grenzen en ontbrekende functies

| Onderdeel | Grens |
| --- | --- |
| Bestanden per project, inclusief MAIN.BAS | 16 |
| Regels per bestand | 64 |
| Totale geladen bronregels | 512 |
| Tekens per bronregel | 95, ASCII |
| Lengte van een identifier | 23 tekens |
| Klassen | 8 |
| Numerieke velden per klasse | 8 |
| Methoden en globale procedures samen | 32 |
| Objectinstanties per uitvoering | 32 |
| Variabelen per globale of lokale scope | 48 |
| Parameters per methode/procedure | 4, numeriek |
| Geneste methodeaanroepen | 8 |
| Geneste expressies | 16 |
| Numeriek bereik | −1.000.000 tot +1.000.000 |
| Tekens per uitvoerregel | 79 |

Nog niet aanwezig zijn `If/Then`, `While/Wend`, `For/Next`, gebruikersinvoer, arrays, stringvariabelen, overerving, interfaces, polymorfisme, overloads, objectvelden/objectparameters, automatische geheugenopruiming en de visuele formulierontwerper. De taal maakt nog geen zelfstandige PRG van je BASIC-programma. Gebruik geen volledige VB-programma's zonder ze aan deze ondersteunde subset aan te passen.

## 10. Eigen bestanden en broncode

De IDE is geschreven in `src/IDE.C`; de taal zit in `src/BASIC.C`. Native menu's staan in `src/MENU.C`, vertalingen in `src/LANG.C`. De `.PRJ`- en `.LNK`-bestanden staan in `atari/`. BASIC-voorbeelden vind je in `examples/`; kopieer ze naar `C:\TTBASIC` om ze in het Atari-project te gebruiken.

Fouten of uitbreidingen kun je in de GitHub-repository bespreken. Vermeld bij een fout de IDE-versie, het minimale BASIC-voorbeeld, de volledige foutmelding en of je Hatari of fysieke hardware gebruikt.
