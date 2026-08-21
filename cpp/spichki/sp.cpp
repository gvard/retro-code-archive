#include <vcl.h>
#pragma hdrstop
#include <tchar.h>

USEFORM("spichki.cpp", Form1);

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    try
    {
        Application->Initialize();

        // Check if the compiler is newer than Borland C++ Builder 6
        #if defined(__BORLANDC__) && (__BORLANDC__ >= 0x0590)
        Application->MainFormOnTaskBar = true;
        #else
        // Set application title for legacy compilers like Borland C++ Builder 5/6
        Application->Title = "Игра в спички";
        #endif

        Application->CreateForm(__classid(TForm1), &Form1);
        Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    catch (...)
    {
        try
        {
            throw Exception("");
        }
        catch (Exception &exception)
        {
            Application->ShowException(&exception);
        }
    }

    return 0;
}
