#include <vcl.h>
#pragma hdrstop
#include <tchar.h>

USEFORM("about.cpp", AboutBox);
USEFORM("chapt.cpp", frmChapt);
USEFORM("fight.cpp", frmFight);
USEFORM("first.cpp", frmFirst);
USEFORM("invent.cpp", DualListDlg);
USEFORM("uinfo.cpp", frmUInfo);
USEFORM("utest.cpp", frmUTest);
USEFORM("utype.cpp", frmUType);

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
        Application->CreateForm(__classid(TDualListDlg), &DualListDlg);
        Application->CreateForm(__classid(TfrmUInfo), &frmUInfo);
        Application->CreateForm(__classid(TfrmUTest), &frmUTest);
        Application->CreateForm(__classid(TfrmUType), &frmUType);
        Application->CreateForm(__classid(TAboutBox), &AboutBox);
        Application->CreateForm(__classid(TfrmFight), &frmFight);

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
            throw Exception(L"Unknown Error");
        }
        catch (Exception &exception)
        {
            Application->ShowException(&exception);
        }
    }
    return 0;
}
