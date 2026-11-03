# TT BASIC 0.8 — status

De grafische formulierontwerper is geïmplementeerd.

- Ctrl+Shift+D: nieuw formulierproject; Ctrl+D: formulier opnieuw openen.
- Gereedschapskist, canvas en eigenschappenpaneel voor Button, Label, TextBox en CheckBox.
- Slepen, formaat wijzigen, toetsenbordbediening, verschuiven van grote formulieren en ongedaan maken.
- F4: eventcode openen/aanmaken. F2 of Ctrl+S: toepassen en opslaan. F5: toepassen en testen.
- Layout en metadata in een herkenbaar BASIC-blok; bestaande eventcode daarbuiten blijft behouden.
- DESIGN.BAS/.TBP als bewerkbaar voorbeeld; Nederlandse, Engelse en Duitse handleidingen bijgewerkt.

Alle bestaande hosttests plus 43 formulier-/generatiechecks slagen met geheugencontroles. De 40 kernchecks voor formulieren zijn ook met Pure C op de geëmuleerde TT uitgevoerd. Native GEM-interactie in Hatari bevestigt nieuw project, plaatsen, verplaatsen, formaat wijzigen, eigenschappen, eventcode, uitvoeren, opslaan, opnieuw openen en verwerpen. Bestaande gebruikersbestanden worden bij installatie behouden.

Grenzen: één ontwerperformulier in het startbestand, maximaal 16 elementen, 256 regels inclusief handlers. Handgeschreven GUI-layouts worden niet automatisch geïmporteerd. Naamwijzigingen passen verwijzingen in eigen BASIC-code niet automatisch aan. Geen meervoudige selectie of zelfstandig compileren. Breakpoints gebruiken nog fysieke regelnummers. Fysieke TT-hardware is nog niet gevalideerd.
