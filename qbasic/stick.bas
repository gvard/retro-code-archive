Rem Program for plotting moving paddle in Arkanoid game
Rem Original code saved on 1999-05-04
Rem Successfully runs in QBasic 1.0 and compiles in QB64 2.1

Dim A(350)
Screen 9

x = 200
y = 250

' Paddle size: 120x10
Line (x, y)-(x + 120, y + 10), 4, BF
Get (x, y)-(x + 120, y + 10), A()
Cls

Put (x, y), A(), Xor
Sleep 1

Do
    Wait &H3DA, 8

    z$ = InKey$

    ' Keyboard Controls
    If z$ = "o" Or z$ = "p" Or z$ = "a" Or z$ = "q" Then

        Put (x, y), A(), Xor

        If z$ = "o" Then
            If x > 10 Then x = x - 10
        End If

        If z$ = "p" Then
            If x < 510 Then x = x + 10
        End If

        If z$ = "a" Then
            If y < 330 Then y = y + 10
        End If

        If z$ = "q" Then
            If y > 40 Then y = y - 10
        End If

        Put (x, y), A(), Xor

    End If

    Rem Press ESC to exit
Loop Until z$ = Chr$(27)
