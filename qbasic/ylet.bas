Rem Original code saved on 1999-05-04
Rem Successfully runs in QBasic 1.0 and compiles in Turbo Basic 1.0
Screen 9
Dim x(20), y(20), c(20)
Dim x1(20), y1(20)
Dim x2(20), y2(20)
x(1) = Int(Rnd(-1))
cx = Int(Rnd(1) * 640)
cy = Int(Rnd(1) * 350)
For i = 1 To 20
    x(i) = Int(Rnd(1) * 640)
    x1(i) = x(i)
    y(i) = Int(Rnd(1) * 350)
    y1(i) = y(i)
    c(i) = Int(Rnd(1) * 16)
Next
While InKey$ = ""
    For i = 1 To 20
        x2(i) = (x1(i) - cx) * .9 + cx
        y2(i) = (y1(i) - cy) * .9 + cy
        Line (x(i), y(i))-(x1(i), y1(i)), 0
        If Abs(y2(i) - cy) > 2 And Abs(x2(i) - cx) > 2 Then 1
        x1(i) = Int(Rnd(1) * 640)
        x2(i) = x1(i)
        y1(i) = Int(Rnd(1) * 350)
        y2(i) = y1(i)
        c(i) = Int(Rnd(1) * 16)
        1:
        Line (x1(i), y1(i))-(x2(i), y2(i)), c(i)
        cx = cx + Rnd(1) * 5 + Rnd(1) * 5 - 5
        If cx < 0 Then cx = 640
        If cx > 640 Then cx = 0
        cy = cy + Rnd(1) * 5 + Rnd(1) * 5 - 5
        If cy < 0 Then cy = 350
        If cy > 350 Then cy = 0
        x(i) = x1(i)
        y(i) = y1(i)
        x1(i) = x2(i)
        y1(i) = y2(i)
    Next
Wend
