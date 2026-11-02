Dim name As String
Input "Your name?"; name

Dim age As Integer
Input "Your age?"; age

If age >= 18 Then
    Print "Hello " & name & ", adult."
Else
    Print "Hello " & name & ", junior."
End If
Print "Next year: "; age + 1
End
