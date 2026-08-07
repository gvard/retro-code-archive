Rem Program for plotting abstract figures
Rem Original code saved on 1999-05-04
Rem Successfully runs in QBasic 1.0 and compiles in Turbo Basic 1.0
'                 Автор неизвестен. Редакция <MUAD.dib>, 1999
Cls
Print Tab(20); "АБСТРАКТНЫЕ КАРТИНКИ"
Print Tab(23); "1 - из точек"
Print Tab(25); "2 - из отрезков"
Print Tab(27); "3 - из рамок"
Print Tab(29); "4 - из окошек"
Print Tab(31); "5 - из кружков"
Input " ваш выбор"; v
Input "  А сколько будет элементов"; n
Screen 12
If v = 1 Then GoSub 2
If v = 2 Then GoSub 4
If v = 3 Then GoSub 6
If v = 4 Then GoSub 8
If v = 5 Then GoSub 10
'COLOR 7: PRINT "Типа пока, баклан!"
End

2 For k = 1 To n
    x = 650 * Rnd(1)
    y = 600 * Rnd(1)
    c = 15 * Rnd(1)
    PSet (x, y), c
Next
Return

4 For k = 1 To n
    x1 = 650 * Rnd(1)
    y1 = 600 * Rnd(1)
    x2 = 650 * Rnd(1)
    y2 = 600 * Rnd(1)
    c = 15 * Rnd(1)
    Line (x1, y1)-(x2, y2), c
Next
Return

6 For k = 1 To n
    x1 = 650 * Rnd(1)
    y1 = 600 * Rnd(1)
    x2 = 650 * Rnd(1)
    y2 = 600 * Rnd(1)
    c = 15 * Rnd(1)
    Line (x1, y1)-(x2, y2), c, B
Next
Return

8 For k = 1 To n
    x1 = 650 * Rnd(1)
    y1 = 600 * Rnd(1)
    x2 = 650 * Rnd(1)
    y2 = 600 * Rnd(1)
    c = 15 * Rnd(1)
    Line (x1, y1)-(x2, y2), c, BF
Next
Return

10 For k = 1 To n
    x = 650 * Rnd(1)
    y = 600 * Rnd(1)
    c = 15 * Rnd(1)
    r = 300 * Rnd(1)
    Circle (x, y), r, c
Next
Return
