# TT BASIC 0.7 — status

Eigen GEM-vensters en een eerste debugger zijn geïmplementeerd.

- Window, Button, Label, TextBox en CheckBox; eigenschappen, events en eventhandlers van eigen objecten.
- Application.Run/Exit, AddHandler/RemoveHandler en opruimen na afloop of stoppen.
- Breakpoints, stap-in/stap-over/stap-uit, aanroepstack en variabeleninspectie.
- Inspectie bij runtimefouten, met behoud van de lopende stack.
- Code completion voor GUI-objecten; GEMDEMO en DEBUG als voorbeeldprojecten.
- Handleidingen in Nederlands, Engels en Duits.

F5: uitvoeren/doorgaan. F8: stap-in. Shift+F8: stap-over. Ctrl+Shift+F8: stap-uit. F9: breakpoint. Ctrl+P: pauze aanvragen. Ctrl+I: inspectie. Escape: stoppen. Bestand toevoegen is vanaf 0.7 Ctrl+F7.

Gecontroleerd met hosttests inclusief geheugencontroles, Pure C-interpretertests op een geëmuleerde TT en GEM-interactietests in Hatari. Bestaande gebruikersbestanden zijn bij installatie behouden.

Dit is nog geen volledige Visual Basic-implementatie. Een visuele formulierontwerper, zelfstandig compileren, Immediate-venster en conditionele breakpoints ontbreken nog. Code en variabelen zijn tijdens een pauze alleen-lezen. Breakpoints schuiven nog niet automatisch mee bij regelwijzigingen. Fysieke TT-hardware is nog niet gevalideerd.
