Rem Program for plotting a linear function
Rem Original code saved on 1999-05-03
Rem Successfully runs in QBasic 1.0 and compiles in Turbo Basic 1.0
Cls
Print Tab(65); "<MUAD.dib>, 1998"
Print Tab(20); "Построение графика функции y=K*x+B"
Print
Input "k=? ", k
Input "b=? ", b
Screen 9
Line (315, 15)-(315, 330), 2
Line (23, 165)-(610, 165), 2
Locate 12, 78
Print "x"
Locate 1, 39
Print "y"
For x = 25 To 610 Step 10
    PSet (x, 165)
Next
For y = 20 To 329 Step 8
    PSet (315, y)
Next
Line (610, (165 - 275 * k - (55 / 6) * b))-(20, (275 * k + 165 - (55 / 6) * b)), 4
120 If InKey$ = "" Then 120
