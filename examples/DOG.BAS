Imports "ANIMAL.BAS"

Class Dog
    Inherits Animal

    Public Sub New(name As String)
        MyBase.New(name)
    End Sub

    Public Overrides Function Speak() As String
        Return MyBase.Speak() & " says woof"
    End Function
End Class
