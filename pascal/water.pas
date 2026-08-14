{ Original code by Petr Kalmykov, saved on 1999-02-28 }
{ Successfully compiles in Turbo Pascal 7.0 }
program water;

uses crt;

var
  b, a, x, y, z, i, m : integer;

begin
  clrscr;
  textbackground(7);
  textcolor(7);

  for m := 1 to 3500 do
  begin
    write(chr(219));
  end;

  textcolor(6);
  gotoxy(1, 1);
  write('Падение капли воды из крана');

  a := 25;
  repeat
    gotoxy(a, 3);
    write(chr(219));
    a := a + 1;
  until a = 36;

  b := 29;
  repeat
    gotoxy(b, 4);
    write(chr(219));
    b := b + 1;
  until b = 32;

  gotoxy(1, 1);
  x := 1;
  i := 1;

  repeat
    gotoxy(x, 5);
    write(chr(219));
    x := x + 1;
  until x = 40;

  y := 5;
  repeat
    gotoxy(40, y);
    write(chr(219));
    y := y + 1;
  until y = 10;

  y := 5;
  repeat
    gotoxy(39, y);
    write(chr(219));
    y := y + 1;
  until y = 10;

  z := 11;
  textcolor(1);

  repeat
    textcolor(1);
    gotoxy(40, z);
    write('O');
    delay(500);

    begin
      gotoxy(40, z);
      textcolor(7);
      write(chr(219));
    end;

    z := z + 1;
  until z = 26;

  sound(1000);
  delay(400);
  nosound;
  delay(4000);
  textbackground(8);
  clrscr;
end.
