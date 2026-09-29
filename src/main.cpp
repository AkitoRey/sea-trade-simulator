// Курсовой проект: игровой симулятор управления торговым судном
// и морскими маршрутами.
// Выполнила: Соколова Д. С., группа ПИ-51.

#define _CRT_SECURE_NO_WARNINGS

#include "crew_member.hpp"
#include "ship.hpp"
#include "trading_company.hpp"

#include <iostream>
#include <windows.h>

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::cout << "=== Лабораторная работа 2 ===\n\n";

    // ---------------------------------------------------------
    // 1. Член экипажа
    // ---------------------------------------------------------

    std::cout << "=== Член экипажа ===\n";

    CrewMember sailor("Иван", 100);

    std::cout << "Имя: "
        << sailor.GetName() << '\n';

    std::cout << "Здоровье: "
        << sailor.GetHealth() << "/100\n";

    sailor.TakeDamage(30);
    sailor.Heal(10);
    sailor.ContractDisease();
    sailor.Isolate();

    std::cout << '\n';

    // ---------------------------------------------------------
    // 2. Корабль и агрегация
    // ---------------------------------------------------------

    std::cout << "=== Корабль ===\n";

    Ship ship("Морская звезда", 100);

    ship.AddCrewMember(sailor);

    ship.LoadCargo(60);
    ship.LoadCargo(30);

    // Попытка превысить грузоподъёмность.
    ship.LoadCargo(20);

    std::cout << '\n';

    // ---------------------------------------------------------
    // 3. Проверка агрегации
    // ---------------------------------------------------------

    std::cout << "=== Проверка агрегации ===\n";

    {
        Ship temporaryShip("Временный корабль", 50);

        temporaryShip.AddCrewMember(sailor);

        std::cout << "Временный корабль существует.\n";
    }

    std::cout << "Корабль уничтожен.\n";
    std::cout << "Но член экипажа всё ещё существует.\n";

    std::cout << "Имя экипажа: "
        << sailor.GetName() << '\n';

    std::cout << "Здоровье экипажа: "
        << sailor.GetHealth() << "/100\n\n";

    // ---------------------------------------------------------
    // 4. Проверка композиции
    // ---------------------------------------------------------

    std::cout << "=== Проверка композиции ===\n";

    {
        TradingCompany company(
            "Atlantic Trade",
            "Торговец",
            150);

        company.LoadCargo(100);
        company.ShowInfo();

        std::cout << "\nВыходим из блока компании...\n";
    }

    // ---------------------------------------------------------
    // 5. Работа с памятью
    // ---------------------------------------------------------

    std::cout << "\n=== Работа с памятью ===\n";

    // Объект со статическим временем жизни.
    static CrewMember staticCrewMember("Пётр", 100);

    std::cout << "Статический объект создан: "
        << staticCrewMember.GetName() << '\n';

    // Динамический объект.
    CrewMember* dynamicCrewMember =
        new CrewMember("Алексей", 100);

    std::cout << "Динамический объект создан: "
        << dynamicCrewMember->GetName() << '\n';

    // Работа по ссылке.
    CrewMember& crewReference = staticCrewMember;

    crewReference.TakeDamage(10);

    std::cout << "Работа по ссылке: "
        << crewReference.GetHealth() << "/100\n";

    // Работа по указателю.
    dynamicCrewMember->TakeDamage(20);

    std::cout << "Работа по указателю: "
        << dynamicCrewMember->GetHealth() << "/100\n";

    delete dynamicCrewMember;

    std::cout << "Динамический объект удалён.\n";

    // ---------------------------------------------------------
    // 6. Динамический массив объектов
    // ---------------------------------------------------------

    std::cout << "\n=== Динамический массив объектов ===\n";

    CrewMember* crewArray = new CrewMember[2]{
        {"Александр", 100},
        {"Николай", 100}
    };

    std::cout << "Создан массив из двух объектов CrewMember.\n";

    crewArray[0].TakeDamage(10);
    crewArray[1].TakeDamage(20);

    std::cout << "Здоровье первого: "
        << crewArray[0].GetHealth() << "/100\n";

    std::cout << "Здоровье второго: "
        << crewArray[1].GetHealth() << "/100\n";

    delete[] crewArray;

    std::cout << "Динамический массив удалён.\n";

    // ---------------------------------------------------------
    // 7. Массив динамических объектов
    // ---------------------------------------------------------

    std::cout << "\n=== Массив динамических объектов ===\n";

    CrewMember* crewPointers[2];

    crewPointers[0] = new CrewMember("Сергей", 100);
    crewPointers[1] = new CrewMember("Дмитрий", 100);

    crewPointers[0]->TakeDamage(15);
    crewPointers[1]->TakeDamage(25);

    std::cout << "Сергей: "
        << crewPointers[0]->GetHealth() << "/100\n";

    std::cout << "Дмитрий: "
        << crewPointers[1]->GetHealth() << "/100\n";

    delete crewPointers[0];
    delete crewPointers[1];

    std::cout << "Массив динамических объектов удалён.\n";

    std::cout << "\n=== Конец программы ===\n";

    return 0;
}