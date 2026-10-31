// Курсовой проект: игровой симулятор управления торговым судном
// и морскими маршрутами.
// Выполнила: Соколова Д. С., группа ПИ-51.

#include "crew_member.hpp"
#include "galleon.hpp"
#include "navy_frigate.hpp"
#include "port.hpp"
#include "trade_sloop.hpp"
#include "trading_company.hpp"

#include <iostream>
#include <memory>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#endif

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    std::cout << "=== Лабораторная работа 4, часть 1 ===\n\n";

    // ---------------------------------------------------------
    // 1. Массив unique_ptr<BaseShip> из разных наследников
    // ---------------------------------------------------------

    std::cout << "=== Создание судов ===\n";

    const auto port = std::make_shared<Port>("Барнаул");

    std::unique_ptr<BaseShip> fleet[3];
    fleet[0] = std::make_unique<TradeSloop>("Ласточка", 40, port);
    fleet[1] = std::make_unique<NavyFrigate>("Стерегущий", 80, port);
    fleet[2] = std::make_unique<Galleon>("Громовержец", 200, port);

    std::cout << "\n=== Работа через указатель на базовый класс ===\n";

    for (const auto& ship : fleet)
    {
        std::cout << ship->Describe() << "\n";
        ship->UseSpecialAction();
    }

    // ---------------------------------------------------------
    // 2. Композиция и агрегация из ЛР 2-3
    // ---------------------------------------------------------

    std::cout << "\n=== Композиция: компания владеет кораблём ===\n";

    {
        TradingCompany company{
            "Atlantic Trade",
            std::make_unique<Galleon>("Торговец", 150, port) };

        company.LoadCargo(100);
        company.ShowInfo();

        std::cout << "\nВыходим из блока компании...\n";
    }

    std::cout << "\n=== Агрегация: корабль наблюдает за членом экипажа ===\n";

    {
        const auto sailor = std::make_shared<CrewMember>("Иван", 100);

        fleet[0]->AddCrewMember(sailor);
        fleet[0]->ShowCrewMember();
    }

    fleet[0]->ShowCrewMember();

    std::cout << "\n=== Конец программы ===\n";

    return 0;
}
