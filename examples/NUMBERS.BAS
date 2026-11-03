Rem Numeric and object-array example for TT BASIC 0.10.
Dim price As Double = 12.5
Dim total As Double = price * 1.21
Print "Total: "; total
Print "Real division: "; 7 / 2
Print "Integer division: "; 7 \ 2
Print "Power: "; 2 ^ 3 ^ 2
Print "Ties to even: "; CInt(2.5); ", "; CInt(3.5)
Dim sample As New Measurements
sample.values(2) = sample.values(1) / 2
Print "Array field: "; sample.values(2)
Dim sum As Double
For i As Double = .25 To 1 Step .25
    sum = sum + i
Next
Print "Fractional loop: "; sum
End

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
