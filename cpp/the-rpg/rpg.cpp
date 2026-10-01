#include <vcl.h>

USEFORM("chargen_view.cpp", frmUInfo)
USEFORM("character_view.cpp", frmUType)
USEFORM("about.cpp", AboutBox)
USEFORM("chapt.cpp", frmChapt)
USEFORM("fight.cpp", frmFight)
USEFORM("first.cpp", frmFirst)
USEFORM("invent.cpp", DualListDlg)
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    try
    {
        Application->Initialize();
        Application->Title = "The RPG";
        Application->MainFormOnTaskBar = true;

        // Главная форма инициализируется строго первой
        Application->CreateForm(__classid(TfrmFirst), &frmFirst);
        Application->CreateForm(__classid(TfrmChapt), &frmChapt);
        Application->Run();
    }
    catch (Exception& exception)
    {
        Application->ShowException(&exception);
    }
    catch (...)
    {
        try
        {
            throw Exception(L"Unknown Error");
        }
        catch (Exception& exception)
        {
            Application->ShowException(&exception);
        }
    }
    return 0;
}
