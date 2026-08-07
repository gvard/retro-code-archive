Rem Program for plotting Lissajous curve
Rem Original code saved on 1999-05-03
Rem Successfully runs in QBasic 1.0 and compiles in Turbo Basic 1.0
Cls
Print Tab(65); "<MUAD.dib>, 1998"
Print Tab(25); "Построение графика Лиссажу"
Print
Print "Уравнения кривой Лиссажу имеют вид:"
Print " X=sin(M*t)"
Print " Y=sin(M*(t+ALFA)"
Print
Input "Частота колебаний по оси OX ", M
Input "Частота колебаний по оси OY ", N
Input "Начальная фаза колебаний по оси OY ", ALFA
Screen 9
WX = 500: WY = 350: XC = WX / 1.75: YC = WY / 2: C = 3: MAX = WY / 2 - 35
X1 = XC: Y1 = YC - Sin(N * ALFA) * MAX
For T = .01 To 6.28318 Step .01
    X2 = XC + Sin(M * T) * MAX
    Y2 = YC - Sin(N * (T + ALFA)) * MAX
    Line (X1, Y1)-(X2, Y2), C: X1 = X2: Y1 = Y2
Next
120 If InKey$ = "" Then 120
