# TT BASIC Studio 0.5 — Nederlandstalige handleiding

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
- De projectboom toont de bestanden uit het project en de gevonden Imports-afhankelijkheden.
- Klik op `MAIN.BAS` of een importbestand om het te bewerken. Cursorpositie en wijzigingen blijven per geopend bestand bewaard.
- Klik op de projectnaam om de projectboom in of uit te klappen. Met de pijltjes onderaan blader je door een lange bestandslijst.
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
| Nieuw project | Bestand → Nieuw project, Ctrl+N |
| Project openen | Bestand → Project openen, Ctrl+O |
| Nieuw bronbestand | Bestand → Nieuw bestand, F7 |
| Bestaand bestand toevoegen | Bestand → Bestand toevoegen, Ctrl+F7 |
| Alle bestanden en project opslaan | Bestand → Alles opslaan, Ctrl+Shift+S |
| Actief bestand opnieuw van schijf laden | F3 |
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

Opslaan schrijft eerst een tijdelijk `.TMP`-bestand en bewaart de vorige schijfversie als `.BAK`. Voor `COUNTER.BAS` zijn dat `COUNTER.TMP` en `COUNTER.BAK`. Bewerkte importbestanden sla je afzonderlijk op door ze te openen en F2 te gebruiken. Met Bestand → Alles opslaan worden alle geopende gewijzigde bestanden en de projectindeling opgeslagen.

Bij afsluiten controleert de IDE alle geopende bestanden op wijzigingen. Annuleer afsluiten om die bestanden eerst op te slaan. Nieuw project maakt een eigen map met een nieuw MAIN.BAS. Project openen gebruikt een .TBP-bestand. Voorbeelden gebruiken het geïnstalleerde DEFAULT-project. Er is nog geen algemene Opslaan-als- of hernoemfunctie voor bronbestanden.

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

Sleep de grijze scheidingsbalk boven het uitvoerpaneel omhoog of omlaag om het groter of kleiner te maken. Het paneel toont de laatste regels die passen (maximaal tien). Fouten stoppen de uitvoering. Print schrijft tekst naar dit paneel. Eigen GEM-vensters maak je met de GUI-objecten uit hoofdstuk 16; zelfstandig compileren wordt nog niet ondersteund.

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

Een puntkomma verbindt items op **één** uitvoerregel en voegt geen spatie toe. Zet gewenste spaties in het tekstliteral. Iedere `Print` eindigt de uitvoerregel, ook met een laatste puntkomma. De maximale uitvoerregel is 79 tekens. Stringvariabelen zijn beschikbaar met `Dim tekst As String`. Gebruik `&` of `+` voor concatenatie van twee strings. Verdubbel aanhalingstekens binnen een literal: `"Een ""quote"""`. Een string bevat maximaal 127 tekens.

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

`value` is hier een numeriek veld per object. Velden kunnen ook String, Boolean, object- of collectiereferenties bevatten. Velden beginnen op nul; een letterlijke standaardwaarde zoals `Public Value As Integer = 10` is ook mogelijk. `Public` maakt een lid van buitenaf toegankelijk. `Private` beperkt toegang tot methoden van dezelfde klasse. Gebruik expliciete zichtbaarheid; zonder aanduiding is een lid in deze versie publiek.

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

Een objectvariabele zonder `New` begint als `Nothing`. Leden benaderen voordat een object is gemaakt geeft een fout. Een objectvariabele accepteert `Nothing` of een instantie die compatibel is met de gedeclareerde klasse of interface, inclusief afgeleide klassen. Je kunt bijvoorbeeld geen `Counter` aan een variabele van een andere klasse toekennen.

Objecten bestaan tot het einde van de uitvoering. `Nothing` verwijdert de referentie, maar maakt in deze versie geen objectslot vrij. F5 begint met een lege objectpool.

## 7. Subs, Functions en parameters

Een `Sub` voert handelingen uit en geeft geen waarde terug. Een `Function` geeft een getypeerde waarde terug met **`Return`**. Beide kunnen klassemethode of globale procedure zijn en in een importbestand staan.

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

`Call` is optioneel: `ShowNumber(36)` werkt ook. Argumenten worden tussen haakjes doorgegeven. Er zijn maximaal acht getypeerde parameters. Getallen en strings worden als waarde doorgegeven; objecten, arrays en collecties delen hun referentie. De aantallen en typen moeten overeenkomen. Ondersteund zijn Integer/Long, Boolean, String, Variant, Object, Collection en namen van klassen en interfaces. Een arrayparameter schrijf je als `items() As Integer`.

Een Function moet met een Return-waarde van het opgegeven type eindigen. Toekennen aan de functienaam vervangt Return niet. Een Sub mag vroeg eindigen met `Return` zonder waarde of `Exit Sub`. Een Sub gebruiken in een berekening is een fout. Een los `End` beëindigt het hoofdprogramma en is niet toegestaan als vervanging voor Return binnen een methode.

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
| `Function finished without Return` | Voeg een Return van het resultaat-type toe |
| `Sub cannot be used as an expression` | Gebruik een Function wanneer een resultaat nodig is |
| `Unknown variable` | Declareer de naam en controleer spelling en scope |
| `Division by zero` | Deler mag geen nul zijn |
| `Result exceeds numeric range` | Houd tussenresultaten binnen ±1.000.000 |
| `Maximum ...` of `exceeds ...` | Bekijk de versiegrenzen hieronder |

## 9. Huidige grenzen en ontbrekende functies

| Onderdeel | Grens |
| --- | --- |
| Bestanden per project, inclusief MAIN.BAS | 16 |
| Regels per bestand | 256 |
| Totale geladen bronregels | 2048 |
| Tekens per bronregel | 159, ASCII |
| Lengte van een identifier | 23 tekens |
| Klassen | 16 |
| Numerieke velden per klasse | 16 |
| Methoden en globale procedures samen | 64 |
| Objectinstanties per uitvoering | 64 |
| Variabelen per globale of lokale scope | 64 |
| Parameters per methode/procedure | 8, getypeerd |
| Geneste methodeaanroepen | 8 |
| Geneste expressies | 16 |
| Numeriek bereik | −1.000.000 tot +1.000.000 |
| Tekens per uitvoerregel | 79 |

Nog niet aanwezig zijn multidimensionale arrays, generieke collecties, overloads, automatische geheugenopruiming, uitzonderingsafhandeling en de visuele formulierontwerper. De taal maakt nog geen zelfstandige PRG van je BASIC-programma. Dit is een eigen BASIC-dialect en geen volledige VB-implementatie.

## 10. Eigen bestanden en broncode

De IDE is geschreven in `src/IDE.C`; de taal zit in `src/BASIC.C`. Native menu's staan in `src/MENU.C`, vertalingen in `src/LANG.C`. De `.PRJ`- en `.LNK`-bestanden staan in `atari/`. BASIC-voorbeelden vind je in `examples/`; kopieer ze naar `C:\TTBASIC` om ze in het Atari-project te gebruiken.

Fouten of uitbreidingen kun je in de GitHub-repository bespreken. Vermeld bij een fout de IDE-versie, het minimale BASIC-voorbeeld, de volledige foutmelding en of je Hatari of fysieke hardware gebruikt.

## 11. Versie 0.4: besturingsstructuren en gegevens

Gebruik **Project → Taalvoorbeeld laden**, **Polymorfisme-voorbeeld** of **Invoervoorbeeld laden**, gevolgd door F5. De bestanden heten FLOW.BAS, POLY.BAS en INPUT.BAS. FLOW toont structuren, arrays en collecties; POLY gebruikt ANIMAL.BAS, DOG.BAS en ISHOW.BAS.

### If, ElseIf en Else

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

Blokken mogen genest zijn. Een korte vorm met één instructie per tak werkt ook: `If age >= 18 Then Print "Adult" Else Print "Junior"`. Gebruik voor geneste voorwaarden de blokvorm.

Boolean heeft de waarden True (-1) en False (0). `And`, `Or`, `Xor` en `Not` werken op de waarheidswaarde. Beide kanten van And/Or worden geëvalueerd; gebruik geneste If-blokken wanneer een tweede expressie alleen onder een voorwaarde veilig is.

### While en Do

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

In plaats van Wend mag End While worden gebruikt. Do ondersteunt `Do While ... / Loop`, `Do Until ... / Loop`, `Do / Loop While ...` en `Do / Loop Until ...`. Een voorwaarde bovenaan kan de eerste uitvoering overslaan; een voorwaarde onderaan test na de eerste uitvoering. Gebruik één testpositie per Do-blok. Een Do/Loop zonder voorwaarde eindigt via Exit Do.

### For en For Each

```vb
Dim total As Integer
For i As Integer = 6 To 1 Step -1
    total = total + i
Next i
Print total
```

De eindwaarde is inclusief; Step is standaard 1 en mag negatief zijn, maar niet nul. Beginwaarde, eindwaarde en stap worden bij het binnengaan bepaald. Next mag de bijbehorende variabelenaam bevatten. Gebruik een numerieke teller.

```vb
Dim names(1) As String
names(0) = "Atari"
names(1) = "TT"
For Each name As String In names
    Print name
Next name
```

For Each werkt met arrays en collecties. Een nieuwe lusvariabele zonder As is Variant. Met As wordt ieder element op het opgegeven type gecontroleerd. `Exit For`, `Exit While` en `Exit Do` verlaten een lus; `Continue For`, `Continue While` en `Continue Do` gaan verder met de volgende iteratie. Deze instructies werken ook binnen geneste If-blokken. Ze kunnen niet uit een andere methode ontsnappen.

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

De selector wordt één keer berekend. De eerste passende Case wordt uitgevoerd; er is geen doorval naar de volgende Case. Case Else staat als laatste. Getallen en strings worden ondersteund; stringvergelijkingen zijn hoofdlettergevoelig. Met Exit Select verlaat je het huidige Select-blok.

### Arrays en collecties

`Dim values(2) As Integer` maakt drie elementen met indices 0, 1 en 2. `Dim values(1 To 3) As Integer` gebruikt grenzen 1 tot en met 3. Indexeren en toekennen werken met `values(1)`. LBound en UBound geven de grenzen; Count of `.Count` geeft het aantal elementen.

```vb
Dim values() As Integer
ReDim values(1)
values(1) = 42
ReDim Preserve values(3)
Print values(1)
```

ReDim vervangt een dynamische array. Preserve bewaart bestaande elementen; de ondergrens mag daarbij niet veranderen. `Array(1, 2, 3)` maakt een Variant-array. Arrays zijn één-dimensionaal en bevatten maximaal 128 elementen. Arrays delen referenties bij toekennen en doorgeven aan procedures. ReDim geeft de variabele een nieuwe array; andere referenties blijven naar de oude array wijzen.

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

Collecties zijn geordend, ongetypeerd en beginnen bij index 1. Add voegt achteraan toe; Remove verwijdert een index; Clear wist de inhoud. Ze bevatten maximaal 128 waarden, inclusief objecten. Het aantal elementen veranderen tijdens For Each veroorzaakt een fout; verlaat de lus eerst. Er zijn maximaal 32 array-/collectieallocaties per uitvoering, inclusief ReDim. Er is nog geen automatische opruiming tijdens een uitvoering.

### Strings en invoer

Stringvariabelen, stringparameters en stringresultaten werken. Beschikbare functies: Len, CStr/Str, CInt/Val, Left, Right, Mid, UCase, LCase en Abs. Mid gebruikt een startpositie vanaf 1. CInt/Val verwachten een volledig geldig geheel getal in het ondersteunde bereik. `Mod` geeft de rest van een gehele deling.

```vb
Dim name As String
Input "Your name?"; name
Dim age As Integer
Input "Your age?"; age
Print "Hello " & name
Print age + 1
```

Input opent in de IDE een GEM-dialoog. De invoerveldlengte is daar 63 tekens; String-waarden in de taal mogen 127 tekens bevatten. Een ongeldige numerieke invoer of Annuleren stopt de uitvoering met een foutmelding. Een Variant-doel neemt een getal aan wanneer mogelijk en anders een string; een al als string gebruikte Variant blijft tekst invoeren.

## 12. Overerving, interfaces en polymorfisme

Een klasse kan één basisklasse hebben, met `Inherits Animal` in de klasse of op de Class-regel. Public-leden zijn toegankelijk, Private-leden alleen vanuit de definiërende klasse en Protected-leden ook vanuit afgeleide klassen.

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

Dit geeft `woof`: de variabele is gedeclareerd als Animal, maar het werkelijke object is Dog. Overridable staat op de basismethode; Overrides op de vervanging. Naam, parameter- en resultaattypen moeten overeenkomen. Een publieke methode mag niet minder toegankelijk worden gemaakt. MyBase roept de basismethode rechtstreeks aan.

Een constructor van een afgeleide klasse kan op de eerste uitvoerbare regel `MyBase.New(...)` gebruiken. Anders wordt automatisch de parameterloze basisconstructor aangeroepen. Heeft die argumenten nodig, dan moet de afgeleide constructor ze expliciet doorgeven. POLY.BAS demonstreert dit volledig.

```vb
Interface IShow
    Function Speak() As String
End Interface
```

Een klasse verklaart `Implements IShow` en levert een publieke methode met dezelfde naam en signatuur. Een interface bevat alleen signaturen, zonder End Function of End Sub per lid. End Interface sluit het contract af. Een interface kan niet met New worden gemaakt. Een interfacevariabele kan een object van iedere implementerende klasse bevatten; aanroepen worden naar de concrete implementatie gestuurd.

Expliciete koppeling met een andere methodenaam werkt ook:

```vb
Public Function Display() As String Implements IShow.Speak
    Return "shown"
End Function
```

Deze methode hoort binnen een klasse met Implements IShow. Een klasse kan maximaal vier interfaces rechtstreeks implementeren, met komma's gescheiden. Geërfde interfacecontracten worden meegenomen en gecontroleerd. Een interface kan één andere interface erven. Ontbrekende of afwijkende implementaties worden vóór de programma-uitvoering afgekeurd.

`TypeOf actor Is Dog` controleert het werkelijke type; `actor Is other` en `actor IsNot Nothing` vergelijken objectreferenties. Klassen en interfaces kunnen als parametertype, resultaat en array-elementtype worden gebruikt. Object werkt als algemeen objectreferentietype; Variant kan getallen, strings, objecten en containers bevatten.

## 13. Gereserveerde woorden en uitvoergrenzen

De interpreter gebruikt een centrale lijst van geïmplementeerde taalwoorden en weigert die als gedeclareerde naam. Daaronder vallen de structuurwoorden, typen, zichtbaarheid, objectwoorden, logische operatoren, True/False/Nothing en Input. New is alleen als constructornaam een uitzondering. Namen van hulpfuncties zoals Len en Count zijn contextuele functienamen en geen taalkeywords.

Extra grenzen in 0.4: 16 klassen/interfaces samen, 16 velden inclusief geërfde velden per klasse, 64 methoden/procedures, 64 objectinstanties, 64 variabelen per scope, acht parameters en 32 geneste blokken. Arrays/collecties: 32 allocaties van maximaal 128 elementen. Strings: 127 tekens. Een uitvoering stopt na 1.000.000 stappen om eindeloze programma's te begrenzen. Druk tijdens een lopend programma op **Escape** om eerder af te breken; bij een invoervenster gebruik je Annuleren. De Pure C-build gebruikt een stack van 512 KiB.

## 14. Projecten en bronbestanden beheren (0.5)

### Een nieuw project starten

1. Kies **Bestand → Nieuw project** of druk Ctrl+N.
2. Geef een naam van één tot acht letters/cijfers, bijvoorbeeld `HELLO`.
3. Bij gewijzigde bestanden kun je Opslaan, Verwerpen of Annuleren kiezen.
4. De IDE maakt `C:\TTBASIC\PROJECTS\HELLO` met `HELLO.TBP` en een eigen `MAIN.BAS`.
5. Het nieuwe project wordt geopend. Druk F5 voor het eerste Hello-programma.

Een bestaande map met dezelfde projectnaam wordt niet overschreven. De beperking tot acht tekens past bij de TOS-bestandsnamen. Je kunt een projectmap later als geheel verplaatsen en het .TBP-bestand op de nieuwe locatie openen.

### Een nieuw bestand toevoegen

Kies **Bestand → Nieuw bestand** of F7. Geef bijvoorbeeld `MODEL` of `MODEL.BAS` op. De IDE maakt het bestand in de huidige projectmap, registreert het in het .TBP-bestand en opent het in de editor. Dubbele namen worden geweigerd. Het nieuwe bestand begint met een commentaarregel.

Bestanden toevoegen organiseert het project; **Imports bepaalt welke modules deel van de uitvoering zijn**. Voor een klasse Model in MODEL.BAS zet je in MAIN.BAS:

```vb
Imports "MODEL.BAS"
Dim item As New Model
Print item.GetValue()
End
```

MODEL.BAS kan bijvoorbeeld bevatten:

```vb
Class Model
    Public Function GetValue() As Integer
        Return 42
    End Function
End Class
```

### Een bestaand bestand toevoegen

Kies **Bestand → Bestand toevoegen** of Ctrl+F7. Selecteer een .BAS-bestand in de GEM-bestandskiezer. Een bestand van buiten de projectmap wordt gekopieerd, zodat het project zelfstandig blijft. Het origineel blijft intact. Een bestand dat al in de map staat wordt alleen geregistreerd. Voeg eventuele afhankelijke bronbestanden ook toe; relatieve Imports worden vanuit de nieuwe bestandsmap opgelost. Een botsende bestandsnaam wordt niet overschreven. Ook hier gebruik je Imports om de code vanuit het startbestand te laden.

### Opslaan en opnieuw openen

- F2 slaat alleen het actieve bestand op.
- **Bestand → Alles opslaan**, Ctrl+Shift+S, slaat alle gewijzigde geopende bestanden en de projectindeling op.
- **Bestand → Project openen**, Ctrl+O, opent een .TBP-bestand via de bestandskiezer. Dit is een ander formaat dan het Pure C-buildproject TTBASIC.PRJ.
- F3 herlaadt alleen het actieve bronbestand.
- Bij een projectwisseling vraagt de IDE wat er met wijzigingen moet gebeuren.
- Het laatst geopende project wordt bij de volgende start automatisch heropend. Is dat niet beschikbaar, dan wordt het DEFAULT-project geprobeerd.

De projectboom toont de geregistreerde bestanden én gevonden Imports-afhankelijkheden. Klik op een naam om te wisselen. De pijltjes onderaan bladeren door de lijst. F5 gebruikt altijd het startbestand uit het actieve project en de werkversies van de geopende bestanden.

### Het projectbestand

Een .TBP is gewone tekst met een formaatversie, naam, startbestand en bronbestanden:

```text
TTBASIC_PROJECT 1
NAME=HELLO
ENTRY=MAIN.BAS
FILE=MAIN.BAS
FILE=MODEL.BAS
```

Paden binnen de projectmap worden relatief opgeslagen, zodat de hele map verplaatsbaar is. Het startbestand is ook een projectlid. Er kunnen maximaal zestien geregistreerde bestanden zijn. Bij het openen worden de geregistreerde bronbestanden gecontroleerd; een onleesbaar of ontbrekend bestand voorkomt dat het huidige project wordt vervangen.

Projectwijzigingen worden meteen opgeslagen. `.PTM` is het tijdelijke manifest en `.PBK` de vorige projectindeling. Bronbestanden behouden hun eigen `.TMP` en `.BAK`. Voorbeelden worden in het geïnstalleerde DEFAULT-project geladen; je huidige project wordt daarbij eerst veilig verlaten.

## 15. Code aanvullen (0.6)

Druk in de editor op **Ctrl+Spatie** voor suggesties. Typ verder om de lijst te filteren, kies met **pijl omhoog/omlaag** en voeg in met **Enter** of **Tab**. **Escape** sluit de lijst. Het huidige woord wordt vervangen; de rest van de regel blijft behouden. Fouten of onvolledige statements blokkeren de aanvulling niet.

Na een punt verschijnt de lijst automatisch: bijvoorbeeld `third.` toont de beschikbare openbare leden van Reken, wanneer `Dim third As New Reken(10)` en `Imports "REKEN.BAS"` aanwezig zijn. Binnen een klasse toont `Me.` ook de eigen private velden. Geërfde openbare leden, methodeparameters, lokale variabelen, klassen, BASIC-sleutelwoorden en Collection-leden worden meegenomen. Onder de lijst staat het type of de methodesignatuur. Alleen de naam wordt ingevoegd; typ bij een methode zelf de haakjes en argumenten, bijvoorbeeld `third.GetValue()`. Voeg na een constructor ook zelf de argumenten toe. Gewijzigde geopende modules worden vanuit het geheugen gelezen; imports hoeven niet eerst te worden uitgevoerd.

Deze eerste versie ondersteunt eenvoudige ontvangers zoals `third.`, `Me.` en `MyBase.`. Samengestelde expressies zoals `items(1).` en `factory.Create().` en type-inferentie uit toewijzingen worden nog niet ondersteund. Gebruik een expliciete `Dim ... As ...` declaratie. De index omvat maximaal 16 bestanden en 512 declaraties, met maximaal 64 suggesties per lijst. Er verschijnt geen lijst binnen strings of commentaar.

## 16. Eigen GEM-vensters en events (0.7)

Open **Project → GEM-voorbeeld laden** en druk op F5, of open GEMDEMO.TBP via Bestand → Project openen. Dit maakt een echt GEM-venster met een tekstveld, checkbox, knoppen en een label. De bedieningselementen worden in BASIC-code aangemaakt; er is nog geen visuele formulierontwerper.

```basic
Dim form As New Window("Mijn programma", 320, 160)
Dim nameBox As New TextBox(form, "Marco", 16, 16, 240, 24)
Dim buttonA As New Button(form, "Groet", 16, 56, 120, 28)
Dim message As New Label(form, "Klaar", 16, 112, 280, 24)
AddHandler buttonA.Click, AddressOf Greet
form.Show()
Application.Run()
End

Sub Greet(sender As Button)
    message.Text = "Hallo " & nameBox.Text
End Sub
```

`New Window(title, width, height)` gebruikt de breedte en hoogte van het **werkgebied**, in pixels. Andere constructors gebruiken `New Button(form, text, x, y, width, height)`, met dezelfde argumenten voor Label, TextBox en CheckBox. Posities zijn relatief aan het werkgebied van het oudervenster. Maak de bedieningselementen bij voorkeur aan vóór Show(). Vensters hebben een titelbalk, een sluitknop en kunnen worden verplaatst.

| Object | Eigenschappen | Methoden | Events |
|---|---|---|---|
| Window | Text of Title, Visible | Show(), Close() | Load, Closed |
| Button | Text, Enabled, Visible | — | Click |
| Label | Text, Enabled, Visible | — | — |
| TextBox | Text, Enabled, Visible | — | Change |
| CheckBox | Text, Enabled, Visible, Checked | — | Click |

Eigenschappen kun je lezen en toewijzen: `message.Text = "Welkom"`, `buttonA.Enabled = False`, `upperBox.Checked = True`. Text/Title zijn strings; de andere eigenschappen zijn Boolean. Tab en Shift+Tab kiezen tekstvelden. Enter activeert de eerste zichtbare ingeschakelde knop; Escape stopt het programma. Een checkbox reageert op een muisklik.

`AddHandler control.Event, AddressOf Handler` koppelt een event aan een globale Sub of een objectmethode, bijvoorbeeld `AddressOf controller.OnClick`. De Sub heeft geen parameters, of één sender-parameter met het passende GUI-type, Object of Variant. Een Function is geen eventhandler. De eventbron is in deze versie een eenvoudige variabelenaam; gebruik voor een veld eventueel een lokale variabele. Verwijderen kan met `RemoveHandler control.Event, AddressOf Handler`. Een identieke koppeling wordt slechts eenmaal geregistreerd. Tijdens een event wordt de lijst van handlers gekopieerd; wijzigingen aan koppelingen gelden voor volgende events.

`Application.Run()` verwerkt events tot alle vensters gesloten zijn. `Application.Exit()` verlaat deze lus; code na Run() kan daarna nog worden uitgevoerd. Run() mag niet genest worden, ook niet vanuit een handler. Show() genereert Load wanneer een gesloten venster wordt geopend. Closed volgt na het sluiten; dit event kan het sluiten nog niet annuleren. Window.Visible=False sluit het venster eveneens. TextBox.Change wordt verstuurd wanneer tekst werkelijk verandert, ook door een toewijzing vanuit BASIC. Programma-einde, een fout of stoppen ruimt alle BASIC-vensters op.

Grenzen: vier gelijktijdig aangemaakte vensters, 64 GUI-objecten inclusief vensters en 64 eventkoppelingen per uitvoering. Vensters en bedieningselementen moeten binnen hun ouderwerkgebied passen. GEM-teksten zijn maximaal 63 tekens. Een TextBox accepteert maximaal `Min(63, (width - 8) / 8)` tekens, met gehele deling. Deze eerste versie biedt geen venstergrootte aanpassen, timers, keuzelijsten, menu's voor eigen vensters of zelfstandig gecompileerde programma's. Code completion kent de GUI-typen, eigenschappen, methoden en events.

## 17. Debuggen (0.7)

| Actie | Bediening |
|---|---|
| Starten / doorgaan tot volgend breakpoint | F5 |
| Starten met stappen / stap in een methode | F8 |
| Methodeaanroep uitvoeren zonder erin te stappen | Shift+F8 |
| Huidige methode verlaten | Ctrl+Shift+F8 |
| Breakpoint op huidige regel wisselen | F9, of klik in de linkermarge |
| Pauze aanvragen tijdens uitvoering | Ctrl+P, of Project-menu |
| Eerste zichtbare variabele inspecteren | Ctrl+I, of klik op een variabele |
| Stoppen | Escape, of Project → Uitvoering stoppen |

**F8 is vanaf 0.7 voor debuggen. Bestand toevoegen is verplaatst naar Ctrl+F7.** Nieuw bestand blijft F7. De debugger heeft geen F11/F12 nodig en werkt dus met de oorspronkelijke Atari-toetsenbordfunctietoetsen.

Open DEBUG.TBP om een korte methodeaanroep te volgen. Zet bijvoorbeeld met F9 een breakpoint op `result = calculator.Double(startValue)`. F5 loopt tot die regel; F8 stapt de methode binnen, Shift+F8 voert de methode volledig uit, en Ctrl+Shift+F8 loopt door tot de aanroeper verdergaat. Bestaande breakpoints blijven actief tijdens stappen. De gele regel is de volgende uit te voeren regel. `Stop` in BASIC vraagt eveneens een debuggerpauze aan.

Rechts staan de aanroepstack en variabelen. Met `<` en `>` kies je een stackframe; de editor toont de bijbehorende bronregel. Het frame 0 is het hoofdprogramma. De lijst bevat parameters, lokale variabelen, Me-velden en globale variabelen. Pijl omhoog/omlaag of de pijltjes onderaan scrollen de lijst. Klik op een variabele of druk Ctrl+I om de volledige getoonde waarde en het type te lezen. GUI-objecten tonen ook hun tekst. Arrays en collecties tonen hun aantal elementen; losse elementen en velden van andere objecten zijn nog niet uitklapbaar. Inspectie voert geen BASIC-methoden uit en wijzigt geen variabelen.

Je kunt ook een breakpoint binnen een Click- of Change-handler plaatsen. Het programma opent zijn venster, wacht op de bijbehorende actie en pauzeert daarna in de handler. Bij doorgaan wordt het toepassingsvenster weer naar voren gehaald. Een pauzeaanvraag terwijl Application.Run op gebruikersinvoer wacht, gaat in bij de volgende uitvoerbare BASIC-regel; Escape stopt ook tijdens dit wachten.

Onbehandelde runtimefouten pauzeren vóór het afwikkelen van de aanroepstack, zodat je lokale waarden kunt bekijken. F5 of Escape beëindigt daarna de mislukte uitvoering; een foutieve regel kan nog niet worden aangepast en hervat. Broncode is tijdens een pauze alleen-lezen. Er is geen Immediate-venster, conditioneel breakpoint of variabelen wijzigen tijdens uitvoering.

Maximaal 64 breakpoints worden bewaard in C:\TTBASIC\STUDIO.BRK. Ze verwijzen naar fysieke bestandsregels en schuiven in deze versie niet automatisch mee wanneer je regels invoegt of verwijdert. Controleer ze na zulke wijzigingen. Plaats ze op uitvoerbare regels; commentaar, klasse-/methodedefinities en Imports worden overgeslagen.
