Rem F9 toggles a breakpoint. F8 starts or steps into code.
Rem Shift+F8 steps over; Ctrl+Shift+F8 steps out.
Dim startValue As Integer = 6
Dim calculator As New Calculator
Dim result As Integer
result = calculator.Double(startValue)
Print "Result: "; result
End

Class Calculator
    Private calls As Integer = 0
    Public Function Double(value As Integer) As Integer
        Dim doubled As Integer
        Me.calls = Me.calls + 1
        doubled = value * 2
        Return doubled
    End Function
End Class
