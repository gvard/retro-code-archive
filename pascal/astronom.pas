{ Program for calculating the hour angle of a celestial body }
{ Original code by Petr Kalmykov, saved on 2000-06-20 }
{ Successfully compiles in Turbo Pascal 7.0 }
program astronomy;

uses crt;

label
  replay;

type
  dates = record
    month, day: integer;
  end;

  times = record
    hour, min: integer;
    sec: real;
  end;

const
  { startime = 6.664167; }
  delta = 0.0656944;
  dol   = 2.5166666;

var
  dni: array[1..12] of byte;
  tmp, st, ds, t, s, a, s0, th: real;
  d, i: integer;
  date: dates;
  timeangle, startime, time, alpha: times;
  ch, c: char;

begin
  clrscr;

  {
    writeln('Введите дату (для 2000 года):');
    write('       месяц (номер) ');
    read(date.month);
    write('            день');
    read(date.day);
  }

  textcolor(3);
  gotoxy(15, 5);
  writeln('Программа для расчета часового угла небесного светила');
  gotoxy(35, 7);
  writeln('Версия 1.1');

  textcolor(yellow);
  gotoxy(30, 15);
  writeln('Автор - Калмыков Петр ');

  repeat until keypressed;
  clrscr;

  textcolor(white);
  writeln('Введите звездное время');
  write('       часы ');
  read(startime.hour);
  write('            минуты ');
  read(startime.min);
  write('                  секунды ');
  readln(startime.sec);

replay:
  textcolor(11);
  writeln('Введите местное летнее время (долгота Москвы)');
  write('       часы ');
  read(time.hour);
  write('            минуты ');
  read(time.min);
  write('                  секунды ');
  readln(time.sec);

  textcolor(10);
  writeln('Введите прямое восхождение светила');
  write('       часы ');
  read(alpha.hour);
  write('            минуты ');
  read(alpha.min);
  write('                  секунды ');
  readln(alpha.sec);

  (*
    dni[1]:=31; dni[2]:=29; dni[3]:=31; dni[4]:=30; dni[5]:=31; dni[6]:=3;
    dni[7]:=31; dni[8]:=31; dni[9]:=30; dni[10]:=31; dni[11]:=30; dni[12]:=31;
    for i := 1 to date.month - 1 do
      d := d + dni[i];
    d := d + date.day;   {end: Вычисление дня от 01.01.2000}
  *)

  t  := time.hour + time.min * (1 / 60) + time.sec * (1 / 3600); {end: Вычисление времени в десятых долях}
  st := startime.hour + startime.min * (1 / 60) + startime.sec * (1 / 3600);
  a  := alpha.hour + alpha.min * (1 / 60) + alpha.sec * (1 / 3600);

  {
    ds := d * delta;
    s0 := startime + ds;
  }

  s  := s0 + (t - 4) + dol + 0.986 * (t - 4);
  th := a - s;

  if th < 0 then
    th := -th;

  repeat
    if th > 24 then
      th := th - 24;
  until th < 24;

  textcolor(white);
  writeln(' ');
  writeln(' ');
  writeln('Часовой угол равен: ', th:0:5);

  tmp := th - trunc(th);
  timeangle.hour := trunc(th);
  timeangle.min  := trunc(tmp * 60);
  timeangle.sec  := (tmp * 60 - trunc(tmp * 60)) * 60;

  textcolor(9);
  writeln('            Или ', timeangle.hour, ' часов ', timeangle.min, ' минут ', timeangle.sec:0:2, ' секунд.');

  repeat until keypressed;
  c := readkey;
  clrscr;

  textcolor(red);
  gotoxy(8, 12);
  writeln('Повторить с сохранением указанного звездного времени ? (y/n)');
  readln(ch);

  if ord(ch) = 121 then
  begin
    clrscr;
    goto replay;
  end
  else
  begin
    textcolor(white);
    clrscr;
    gotoxy(35, 12);
    writeln('До встречи !!!');
    delay(5000);
    clrscr;
  end;
end.
