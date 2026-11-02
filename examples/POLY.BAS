Imports "DOG.BAS"

Dim actor As Animal = New Dog("Rex")
Dim printable As IShow = actor

Print actor.Speak()
Print printable.Speak()

Dim animals As New Collection
animals.Add(actor)
animals.Add(New Dog("Max"))
For Each animal As Animal In animals
    Print animal.Speak()
Next animal
End
