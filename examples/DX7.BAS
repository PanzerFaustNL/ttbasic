Rem Original Yamaha DX7: MIDI Data Format, pages 30-32.
Rem Reuse one seven-byte SysEx buffer; no per-click allocations.
Class DX7
    Private channel As Integer
    Private message As Variant

    Public Sub New(channelNumber As Integer)
        Me.channel = Midi.CheckRange(channelNumber, 1, 16)
        Me.message = Array(240, 67, 16, 0, 0, 0, 247)
    End Sub

    Public Sub SetChannel(channelNumber As Integer)
        Dim nextChannel As Integer = Midi.CheckRange(channelNumber, 1, 16)
        If nextChannel <> Me.channel Then
            Midi.StopNotes(Me.channel)
        End If
        Me.channel = nextChannel
    End Sub

    Private Sub Parameter(index As Integer, value As Integer)
        Dim bytes As Variant = Me.message
        bytes(2) = 16 + Me.channel - 1
        bytes(3) = index \ 128
        bytes(4) = index Mod 128
        bytes(5) = value
        Midi.SysEx(bytes)
    End Sub

    Public Sub Algorithm(number As Integer)
        Dim value As Integer = Midi.CheckRange(number, 1, 32)
        Me.Parameter(134, value - 1)
    End Sub

    Public Sub Feedback(value As Integer)
        Me.Parameter(135, Midi.CheckRange(value, 0, 7))
    End Sub

    Public Sub OperatorLevel(operatorNumber As Integer, level As Integer)
        Dim op As Integer = Midi.CheckRange(operatorNumber, 1, 6)
        Dim value As Integer = Midi.CheckRange(level, 0, 99)
        Rem Yamaha stores OP6 first and OP1 last, 21 bytes per operator.
        Me.Parameter((6 - op) * 21 + 16, value)
    End Sub

    Public Sub Program(number As Integer)
        Dim program As Integer = Midi.CheckRange(number, 1, 32)
        Midi.StopNotes(Me.channel)
        Midi.ProgramChange(Me.channel, program - 1)
    End Sub

    Public Sub NoteOn(note As Integer, velocity As Integer)
        Midi.NoteOn(Me.channel, note, velocity)
    End Sub

    Public Sub NoteOff(note As Integer)
        Midi.NoteOff(Me.channel, note)
    End Sub

    Public Sub Panic()
        Midi.Panic(Me.channel)
    End Sub
End Class
