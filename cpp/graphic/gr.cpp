#include <vcl.h>
#pragma hdrstop
#include "gr.h"
#include <math.h>
#pragma package(smart_init)
#pragma resource "*.dfm"

TForm1 *Form1;

__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	n = 0;
	er = 0;
	a = 0;
	b = 0;
	Mas = nullptr;
	Res = nullptr;
	p = nullptr;
}

double* TForm1::initMas(double a_val, double b_val, int n_val)
{
	double h = (b_val - a_val) / n_val;
	double *p_mas = new double[n_val + 1];

	if ((a_val == 0 && b_val == 0) || b_val <= a_val)
	{
		ShowMessage("Проверьте правильность границ вывода");
		this->er = 1;
		return p_mas;
	}

	*p_mas = a_val;
	for (int i = 1; i <= n_val; i++)
	{
		*(p_mas + i) = *(p_mas + i - 1) + h;
	}

	return p_mas;
}

void TForm1::f(AnsiString &str, double *values, uzel *&node)
{
	int bracketCount = 0;
	int functionType = 0;
	int isOperatorFound = 0;
	int sign = 1;

	if (this->er == 1)
	{
		return;
	}

	node = new uzel;
	node->m = new double[this->n + 1];
	node->l = nullptr;
	node->r = nullptr;

	while (str == '(' && str[str.Length()] == ')')
	{
		bracketCount = 1;
		int i = 1;
		while (bracketCount != 0)
		{
			if (str[++i] == '(')
			{
				bracketCount++;
			}
			if (str[i] == ')')
			{
				bracketCount--;
			}
		}

		if (i == str.Length())
		{
			str.Delete(1, 1);
			str.Delete(str.Length(), 1);
		}
		else
		{
			break;
		}
	}

	for (int i = str.Length(); i >= 1; i--)
	{
		if (str[i] == '(')
		{
			bracketCount++;
		}
		if (str[i] == ')')
		{
			bracketCount--;
		}

		if (bracketCount == 0 && isOperatorFound == 0)
		{
			if (str[i] == '+' || str[i] == '-')
			{
				sign = (str[i] == '+' ? 1 : -1);
				isOperatorFound = 1;

				if (sign == -1 && i == 1)
				{
					for (int j = 0; j <= this->n; j++)
					{
						values[j] = 0;
					}
				}
				else
				{
					AnsiString sub1 = str.SubString(1, i - 1);
					f(sub1, values, node->l);
				}

				AnsiString sub2 = str.SubString(i + 1, str.Length() - i);
				f(sub2, node->m, node->r);

				for (int j = 0; j <= this->n; j++)
				{
					values[j] = values[j] + sign * (*((node->m) + j));
				}

				delete[] node->m;
				break;
			}
		}
	}

	if (isOperatorFound == 1)
	{
		return;
	}

	for (int i = str.Length(); i >= 1; i--)
	{
		if (str[i] == '(')
		{
			bracketCount++;
		}
		if (str[i] == ')')
		{
			bracketCount--;
		}

		if (bracketCount == 0 && isOperatorFound == 0)
		{
			if (str[i] == '*')
			{
				isOperatorFound = 1;
				AnsiString sub1 = str.SubString(1, i - 1);
				f(sub1, values, node->l);

				AnsiString sub2 = str.SubString(i + 1, str.Length() - i);
				f(sub2, node->m, node->r);

				for (int j = 0; j <= this->n; j++)
				{
					values[j] = values[j] * (*((node->m) + j));
				}

				delete[] node->m;
				break;
			}
		}
	}

	if (isOperatorFound == 1)
	{
		return;
	}

	for (int i = str.Length(); i >= 1; i--)
	{
		if (str[i] == '(')
		{
			bracketCount++;
		}
		if (str[i] == ')')
		{
			bracketCount--;
		}

		if (bracketCount == 0 && isOperatorFound == 0)
		{
			if (str[i] == '^')
			{
				isOperatorFound = 1;
				AnsiString sub1 = str.SubString(1, i - 1);
				f(sub1, values, node->l);

				AnsiString sub2 = str.SubString(i + 1, str.Length() - i);
				f(sub2, node->m, node->r);

				for (int j = 0; j <= this->n; j++)
				{
					values[j] = pow(values[j], *(node->m + j));
				}

				delete[] node->m;
				break;
			}
		}
	}

	if (isOperatorFound == 1)
	{
		return;
	}

	for (int i = 1; i <= str.Length(); i++)
	{
		if (str[i] == '(')
		{
			bracketCount++;
		}
		if (str[i] == ')')
		{
			bracketCount--;
		}

		if (bracketCount == 0)
		{
			if (str.SubString(i, 4) == "sin(")       functionType = 1;
			else if (str.SubString(i, 4) == "cos(")  functionType = 2;
			else if (str.SubString(i, 4) == "ctg(")  functionType = 3;
			else if (str.SubString(i, 3) == "tg(")   functionType = 4;
			else if (str.SubString(i, 3) == "In(")   functionType = 5;

			if (functionType >= 1 && functionType <= 5)
			{
				isOperatorFound = 1;
				if (functionType == 1 || functionType == 2 || functionType == 3)
				{
					AnsiString sub = str.SubString(i + 4, str.Length() - i - 4);
					f(sub, node->m, node->l);
				}
				else
				{
					AnsiString sub = str.SubString(i + 3, str.Length() - i - 3);
					f(sub, node->m, node->l);
				}

				for (int j = 0; j <= this->n; j++)
				{
					switch (functionType)
					{
						case 1: values[j] = sin(*(node->m + j)); break;
						case 2: values[j] = cos(*(node->m + j)); break;
						case 3: values[j] = (fabs(tan(*(node->m + j))) <= 1e-10 ? 1e10 : 1 / tan(*(node->m + j))); break;
						case 4: values[j] = tan(*(node->m + j)); break;
						case 5: values[j] = log(*(node->m + j)); break;
					}
				}

				delete[] node->m;
				break;
			}
		}
	}

	if (isOperatorFound == 1)
	{
		return;
	}

	while (str == '(' && str[str.Length()] == ')')
	{
		bracketCount = 1;
		int i = 1;
		while (bracketCount != 0)
		{
			if (str[++i] == '(')
			{
				bracketCount++;
			}
			if (str[i] == ')')
			{
				bracketCount--;
			}
		}
		if (i == str.Length())
		{
			str.Delete(1, 1);
			str.Delete(str.Length(), 1);
		}
	}

	if (str.AnsiPos(")") + str.AnsiPos("("))
	{
		ShowMessage("Проверьте скобки в вашем выражении!");
		this->er = 1;
		return;
	}

	if (str == "x")
	{
		for (int j = 0; j <= this->n; j++)
		{
			values[j] = this->Mas[j];
		}
	}
	else if (str != "")
	{
		for (int j = 0; j <= this->n; j++)
		{
			values[j] = str.ToDouble();
		}
	}
	else //(либо были пустые скобки
	{
		ShowMessage("Проверьте скобки в вашем выражении!");
		this->er = 1;
	}

	return;
}

void __fastcall TForm1::Button1Click(TObject *Sender)
{
	int corr = -1;
	int bracketCount = 0;
	double mi, ma;

	if (this->er == 1)
	{
		this->er = 0;
	}

	PaintBox1->Repaint();
	this->n = PaintBox1->Width - 1;

	TPoint *v = new TPoint[this->n + 1];
	TPoint *o1 = new TPoint[2];
    TPoint *o2 = new TPoint[2];

	this->s = AnsiString(ComboBox1->Text);
	for (int i = 1; i <= this->s.Length(); i++)
	{
		if (this->s[i] == '(')
		{
			bracketCount++;
		}
		if (this->s[i] == ')')
		{
			bracketCount--;
		}
	}

	if (bracketCount != 0)
	{
		ShowMessage("Проверьте скобки в вашем выражении!");
		delete[] v;
		delete[] o1;
		delete[] o2;
		return;
	}

	try
	{
		if (Edit2->Text != "")
		{
			this->a = Edit2->Text.ToDouble();
		}
		if (Edit3->Text != "")
		{
			this->b = Edit3->Text.ToDouble();
		}
	}
	catch (EConvertError&)
	{
		ShowMessage("Введите границы вывода в нормальном виде!");
		delete[] v;
		delete[] o1;
		delete[] o2;
		return;
	}

	this->Mas = initMas(this->a, this->b, this->n);
	this->Res = new double[this->n + 1];

	f(this->s, this->Res, this->p);
	mi = (ma = *(this->Res));

	for (int i = 1; i <= this->n; i++)
	{
		if (mi > *(this->Res + i))
		{
			mi = *(this->Res + i);
		}
		if (ma < *(this->Res + i))
		{
			ma = *(this->Res + i);
		}
	}

	mi = (mi > 0 ? 0 : mi);
	ma = (ma < 0 ? 0 : ma);

	if (ma == 0)
	{
		corr = 1;
	}

	int padTop = 25;
	int padBottom = 30;
	int padLeft = 20;
	int padRight = 25;

	int workHeight = PaintBox1->Height - padTop - padBottom;
	int workWidth = PaintBox1->Width - padLeft - padRight;

	for (int i = 0; i <= this->n; i++)
	{
		int screenX = padLeft + floor((double)i * workWidth / this->n);
		int screenY = padTop + floor(workHeight * (ma - this->Res[i]) / (ma - mi)) + (mi * ma == 0 ? corr : 0);
		v[i] = Point(screenX, screenY);
	}

	PaintBox1->Canvas->Pen->Color = clBlack;
	PaintBox1->Canvas->Polyline(v, this->n);

	PaintBox1->Canvas->Pen->Color = clRed;
	PaintBox1->Canvas->Brush->Color = clRed;

	// Отрисовка оси X
	int posY = padTop + floor(workHeight * ma / (ma - mi) + (mi * ma == 0 ? corr : 0));

	if (posY < padTop) posY = padTop;
	if (posY > PaintBox1->Height - padBottom) posY = PaintBox1->Height - padBottom;

	o1[0] = Point(padLeft, posY);
	o1[1] = Point(PaintBox1->Width - 5, posY);
	PaintBox1->Canvas->Polyline(o1, 1);

	// Стрелка оси X
	TPoint arrowX[3]; // Массив из 3 точек для треугольника
	arrowX[0] = Point(PaintBox1->Width - 5, posY);
	arrowX[1] = Point(PaintBox1->Width - 13, posY - 4);
	arrowX[2] = Point(PaintBox1->Width - 13, posY + 4);
	PaintBox1->Canvas->Polygon(arrowX, 2); // Рисуем замкнутый полигон стрелки

	// Отрисовка оси Y
	int posX = 0;
	if (this->a <= 0 && this->b >= 0)
	{
		posX = padLeft + floor(workWidth * (0 - this->a) / (this->b - this->a));
	}
	else if (this->a > 0)
	{
		posX = padLeft;
	}
	else
	{
		posX = PaintBox1->Width - padRight;
	}

	o2[0] = Point(posX, PaintBox1->Height - padBottom);
	o2[1] = Point(posX, 5);
	PaintBox1->Canvas->Polyline(o2, 1);

	// Стрелка оси Y
	TPoint arrowY[3]; // Массив из 3 точек для треугольника
	arrowY[0] = Point(posX, 5);
	arrowY[1] = Point(posX - 4, 13);
	arrowY[2] = Point(posX + 4, 13);
	PaintBox1->Canvas->Polygon(arrowY, 2); // Рисуем замкнутый полигон стрелки

	// Отрисовка отметок на осях с шагом 0.5
	PaintBox1->Canvas->Pen->Color = clRed;

	// Вертикальные засечки на оси X
	// Округление границ до ближайшего кратного 0.5 шага
	double startX = ceil(this->a * 2.0) / 2.0;
	double endX = floor(this->b * 2.0) / 2.0;

	for (double valX = startX; valX <= endX; valX += 0.5)
	{
		if (fabs(valX) < 1e-9) continue; // Пропускаем сам ноль (пересечение осей)

		int markX = padLeft + floor(workWidth * (valX - this->a) / (this->b - this->a));
		// Ограничиваем риску, чтобы она не уходила за стрелку оси X
		if (markX >= padLeft && markX <= PaintBox1->Width - 15)
		{
			PaintBox1->Canvas->MoveTo(markX, posY - 3);
			PaintBox1->Canvas->LineTo(markX, posY + 3);
		}
	}

	// Вертикальные засечки на оси Y
	double startY = ceil(mi * 2.0) / 2.0;
	double endY = floor(ma * 2.0) / 2.0;

	for (double valY = startY; valY <= endY; valY += 0.5)
	{
		if (fabs(valY) < 1e-9) continue; // Пропускаем сам ноль

		int markY = padTop + floor(workHeight * (ma - valY) / (ma - mi)) + (mi * ma == 0 ? corr : 0);
		// Ограничиваем риску, чтобы она не уходила за стрелку оси Y
		if (markY >= 15 && markY <= PaintBox1->Height - padBottom)
		{
			PaintBox1->Canvas->MoveTo(posX - 3, markY);
			PaintBox1->Canvas->LineTo(posX + 3, markY);
		}
	}

	// Добавление подписей
	PaintBox1->Canvas->Font->Name = "Arial";
	PaintBox1->Canvas->Font->Size = 10;
	PaintBox1->Canvas->Font->Color = clBlack;
	PaintBox1->Canvas->Font->Style = TFontStyles() << fsBold;
	PaintBox1->Canvas->Brush->Style = bsClear;

	int textY = posY + 6;
	if (textY + 15 > PaintBox1->Height)
	{
		textY = posY - 18;
	}
	PaintBox1->Canvas->TextOut(PaintBox1->Width - 18, textY, "X");

	PaintBox1->Canvas->TextOut(posX + 8, 4, "Y");

	delete[] this->Res;
	delete[] v;
	delete[] o1;
	delete[] o2;
}

void __fastcall TForm1::FormCreate(TObject *Sender)
{
	ComboBox1->Focused();
}

void __fastcall TForm1::ComboKeyPress(TObject *Sender, char &Key)
{
	if (Key == VK_RETURN)
	{
		Edit2->SetFocus();
	}
}

void __fastcall TForm1::E2KeyPress(TObject *Sender, char &Key)
{
	if (Key == VK_RETURN)
	{
		Edit3->SetFocus();
	}
}

void __fastcall TForm1::E3KeyPress(TObject *Sender, char &Key)
{
	if (Key == VK_RETURN)
	{
		Button1->SetFocus();
	}
}
