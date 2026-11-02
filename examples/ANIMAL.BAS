Imports "ISHOW.BAS"

Class Animal
    Implements IShow
    Protected label As String

    Public Sub New(name As String)
        Me.label = name
    End Sub

    Public Overridable Function Speak() As String
        Return Me.label
    End Function
End Class
