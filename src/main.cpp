#include <cstring>

#include "Game.h"
#include "Ui.h"

int RunDemo();   // демонстрация заданий ЛР 5 (Demo.cpp)

int main(int argc, char* argv[])
{
    ui::Init();
    // Без аргументов запускается игра; ключ --demo показывает по пунктам все задания ЛР.
    for (int i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "--demo") == 0)
        {
            return RunDemo();
        }
        if (std::strcmp(argv[i], "--fast") == 0)
        {
            ui::SetFast(true);
        }
    }
    RunGame();
    return 0;
}
