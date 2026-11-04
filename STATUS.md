# TT BASIC 0.10

Ontwikkelversie voor testen en verspreiding als begrensd, op Visual Basic geïnspireerd dialect. Zie RELEASE.md voor installatie, migratie en ontbrekende functies.

Nieuw: Single/Double, decimale en wetenschappelijke notatie, 32-bit Integer/Long-aliases, gewone en gehele deling, machten, numerieke conversies en wiskundefuncties. Vaste en dynamische getypeerde arrayvelden werken rechtstreeks via objecten en Me, inclusief ReDim Preserve. Completion, debugger en handleidingen NL/EN/DE zijn bijgewerkt. NUMBERS.TBP demonstreert de taaluitbreiding.

Inclusief MIDI-uitvoer, droogtest, notenopruimen bij stoppen/fouten/pauzeren, een Yamaha DX7-klasse en formulierdemo. Arturia-modus bedient Algorithm via een vooraf ingestelde CC18 MIDI Learn-koppeling; andere Arturia-parameters zijn nog niet gekoppeld. Die algoritmekoppeling is door de gebruiker bevestigd.

Validatie: alle hostregressies slagen met ASan/UBSan, inclusief 100 numerieke/arraycontroles, 41 MIDI/DX7-controles en zeven consolevoorbeelden. Pure C op de geëmuleerde TT: 100 numerieke/arraycontroles, 41 MIDI/DX7-controles en zeven voorbeelden geslaagd. De native IDE voert NUMBERS uit; de DX7-demo is in TT Medium getest met droogtest CC18. Eerdere UART-traces bevestigen Note Off bij pauzeren en stoppen. Fysieke TT/DX7-validatie staat open.

Voor verspreiding: tools/package.py maakt een schoon pakket met PRG, voorbeelden, drie handleidingen, GPLv3-licentie en bijbehorende broncode. Geen persoonlijke projecten, TOS-ROM of Pure C-compiler/bibliotheekbestanden in het pakket. BASIC-bestandstoegang, exception handling, meerdimensionale arrays, generieke collecties, overloads, garbage collection en zelfstandige BASIC-compilatie ontbreken nog.

Beeldcorrectie 4 november: typen tekent alleen betrokken regels/status, verticaal scrollen gebruikt VDI-kopie waar het editorgebied volledig zichtbaar is. Native Pure C-build geslaagd; zeven PNG-paren (typen, splitsen, samenvoegen, op/neer scrollen, horizontaal scrollen, completion sluiten) identiek aan volledige hertekening. Gebruikersbronnen en configuratie ongewijzigd bij installatie van deze build.

Formulier/code-wisseling: Ctrl+D wisselt ontwerper en bewerkbare broncode, met behoud van wijzigingen en eventinhoud. F4 blijft eventcode. Tijdens uitvoeren wisselt Ctrl+Tab formulier en alleen-lezen code; de Formulier-knop brengt het formulier terug. Bladeren en projectbestanden bekijken werkt tijdens uitvoeren. Native tests bevestigen behoud van veldwaarden/focus, werkende MIDI-bediening na terugkeren, muis-/knopwisseling, stoppen en exact behouden eventcode na ontwerpwijziging en opslaan.

Ctrl+D/VSync-correctie: gedeelde toetsvergrendeling voorkomt herhaald openen/sluiten bij vasthouden. Dubbele refreshes en herhaald bovenaan zetten van hetzelfde venster zijn verwijderd. Native tests met de Cocoa-driver, VSync aan en fast-forward uit: één tekenbeurt per wisseling, nul tekenbeurten tijdens wachten; TT Medium en uitgebreide VDI (896×672, 16 kleuren), inclusief vasthouden en snelle toetsdruk. Gewone tekstinvoer, ontwerpwijzigingen/opslaan, handlerinhoud en draaiend-formulier/DX7-bediening blijven werken.
