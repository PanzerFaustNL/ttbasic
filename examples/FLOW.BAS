Dim total As Integer
For i As Integer = 1 To 6
    If i Mod 2 = 0 Then
        total = total + i
    Else
        total = total + 0
    End If
Next i

Dim counter As Integer
While counter < 3
    counter = counter + 1
Wend

Do
    counter = counter - 1
Loop Until counter = 0

Select Case total
Case 1 To 10
    Print "Small"
Case Is >= 12
    Print "Even sum: "; total
Case Else
    Print "Other"
End Select

Dim names(1) As String
names(0) = "Atari"
names(1) = "TT"
Dim label As String
For Each name As String In names
    label = label & name & " "
Next name
Print label

Dim values As New Collection
values.Add(20)
values.Add(22)
Dim answer As Integer
For Each item In values
    answer = answer + item
Next
Print "Collection sum: "; answer
Print "Count: "; values.Count
End
