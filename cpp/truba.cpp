/* Original code saved on 1999-11-27.
   Successfully compiles in Borland C++ 3.1 */

#include <conio.h>
#include <dos.h>
#include <stdio.h>

void main() {
    int a = 0;
    int x = 2;
    int y = 2;
    int c = 300;

    textmode(C4350);
    _setcursortype(_NOCURSOR);

    while (a < 10) {
        clrscr();
        gotoxy(x, y);
        putch(219);
        gotoxy(x, y + 2);
        putch(219);
        gotoxy(x + 2, y);
        putch(219);
        gotoxy(x + 2, y + 2);
        putch(219);
        delay(c);

        clrscr();
        gotoxy(x + 1, y);
        putch(219);
        gotoxy(x, y + 1);
        putch(219);
        gotoxy(x + 1, y + 2);
        putch(219);
        gotoxy(x + 2, y + 1);
        putch(219);
        delay(c);

        a++;
    }
}
