Rem Native GEM windows, controls and events (TT BASIC 0.7)
Dim form As New Window("TT BASIC - GEM demo", 360, 220)
Dim caption As New Label(form, "Enter your name:", 16, 14, 320, 16)
Dim nameBox As New TextBox(form, "Marco", 16, 42, 240, 24)
Dim upperBox As New CheckBox(form, "Uppercase greeting", 16, 78, 240, 24)
Dim greetButton As New Button(form, "Greet", 16, 120, 144, 28)
Dim closeButton As New Button(form, "Close", 184, 120, 144, 28)
Dim message As New Label(form, "Ready", 16, 168, 320, 24)
Dim clicks As Integer = 0
AddHandler form.Load, AddressOf FormLoaded
AddHandler form.Closed, AddressOf FormClosed
AddHandler nameBox.Change, AddressOf NameChanged
AddHandler upperBox.Click, AddressOf UpperChanged
AddHandler greetButton.Click, AddressOf Greet
AddHandler closeButton.Click, AddressOf CloseForm
form.Show()
Application.Run()
Print "GEM demo finished."
End

Sub FormLoaded()
    Print "Window loaded."
End Sub

Sub NameChanged(sender As TextBox)
    message.Text = "Name: " & sender.Text
End Sub

Sub UpperChanged(sender As CheckBox)
    If sender.Checked Then
        message.Text = "Uppercase enabled"
    Else
        message.Text = "Uppercase disabled"
    End If
End Sub

Sub Greet(sender As Button)
    Dim greeting As String = "Hello " & nameBox.Text
    clicks = clicks + 1
    If upperBox.Checked Then
        greeting = UCase(greeting)
    End If
    message.Text = greeting
    Print "Click "; clicks; ": "; greeting
End Sub

Sub CloseForm()
    form.Close()
End Sub

Sub FormClosed()
    Print "Window closed."
End Sub
