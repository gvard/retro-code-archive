#include <vcl.h>
#pragma hdrstop
#include "main_window.h"
#include <exception>

USEFORM("main_window.cpp", MainWindow);

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    try
    {
        Application->Initialize();
        Application->MainFormOnTaskBar = true;
        Application->CreateForm(__classid(TMainWindow), &MainWindow);
        Application->Run();
    }
    // Ловим специфичные для VCL/Embarcadero ошибки (ошибки UI, конвертации типов)
    catch (const Exception& exception)
    {
        Application->ShowException(&exception);
    }
    catch (const std::exception& std_exception)
    {
        UnicodeString msg = "Критическая системная ошибка: " + UnicodeString(std_exception.what());
        Application->MessageBox(msg.w_str(), L"Сбой приложения", MB_OK | MB_ICONERROR);
    }
    // Перехват неизвестных аппаратных исключений ОС
    catch (...)
    {
        Application->MessageBox(L"Произошло непредвиденное аппаратное исключение.", L"Критический сбой", MB_OK | MB_ICONERROR);
    }
    return 0;
}
