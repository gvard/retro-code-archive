#include <vcl.h>

USEFORM("chargen_view.cpp", frmCharGen)
USEFORM("charstats_view.cpp", frmCharStats)
USEFORM("about_view.cpp", frmAbout)
USEFORM("inventory_view.cpp", frmInventory)
USEFORM("first.cpp", frmMainMenu)
USEFORM("chapt_view.cpp", frmChapt)
USEFORM("fight_view.cpp", frmFight)

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    try
    {
        Application->Initialize();
        Application->Title = "The RPG";
        Application->MainFormOnTaskBar = true;

        // Главная форма инициализируется строго первой
        Application->CreateForm(__classid(TfrmMainMenu), &frmMainMenu);
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
