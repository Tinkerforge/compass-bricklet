Imports System
Imports Tinkerforge

Module ExampleCallback
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change XYZ to the UID of your Compass Bricklet

    ' Callback subroutine for heading callback
    Sub HeadingCB(ByVal sender As BrickletCompass, ByVal heading As Short)
        Console.WriteLine("Heading: " + (heading/10.0).ToString() + " °")
    End Sub

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim c As New BrickletCompass(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Register heading callback to subroutine HeadingCB
        AddHandler c.HeadingCallback, AddressOf HeadingCB

        ' Set period for heading callback to 0.1s (100ms) without a threshold
        c.SetHeadingCallbackConfiguration(100, False, "x"C, 0, 0)

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
