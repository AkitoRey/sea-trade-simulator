// Курсовой проект: игровой симулятор управления торговым судном
// и морскими маршрутами.
// Выполнила: Соколова Д. С., группа ПИ-51.

#define _CRT_SECURE_NO_WARNINGS

#include "crew_member.hpp"
#include "ship.hpp"
#include "trading_company.hpp"

#include <iostream>
#include <memory>
#include <utility>
#include <windows.h>

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::cout << "=== Лабораторная работа 3, часть 2 ===\n\n";

    // ---------------------------------------------------------
    // 1. Член экипажа
    // ---------------------------------------------------------

    std::cout << "=== Член экипажа ===\n";

    // shared_ptr: за членом экипажа будут наблюдать другие объекты (weak_ptr).
    std::shared_ptr<CrewMember> sailor =
        std::make_shared<CrewMember>("Иван", 100);

    std::cout << "Имя: "
        << sailor->GetName() << '\n';

    std::cout << "Здоровье: "
        << sailor->GetHealth() << "/100\n";

    sailor->TakeDamage(30);
    sailor->Heal(10);
    sailor->ContractDisease();
    sailor->Isolate();

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
        << sailor->GetName() << '\n';

    std::cout << "Здоровье экипажа: "
        << sailor->GetHealth() << "/100\n\n";

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

    // Работа по ссылке.
    CrewMember& crewReference = staticCrewMember;

    crewReference.TakeDamage(10);

    std::cout << "Работа по ссылке: "
        << crewReference.GetHealth() << "/100\n";

    // ---------------------------------------------------------
    // 6. unique_ptr: один владелец
    // ---------------------------------------------------------

    std::cout << "\n=== unique_ptr: уничтожение при выходе из блока ===\n";

    {
        std::unique_ptr<CrewMember> dynamicCrewMember =
            std::make_unique<CrewMember>("Алексей", 100);

        std::cout << "Динамический объект создан: "
            << dynamicCrewMember->GetName() << '\n';

        // Работа по указателю.
        dynamicCrewMember->TakeDamage(20);

        std::cout << "Работа по указателю: "
            << dynamicCrewMember->GetHealth() << "/100\n";

        std::cout << "Выходим из блока...\n";
    }

    std::cout << "Блок завершён: delete не вызывался.\n";

    std::cout << "\n=== unique_ptr: передача владения ===\n";

    std::unique_ptr<CrewMember> owner;

    {
        std::unique_ptr<CrewMember> temporaryOwner =
            std::make_unique<CrewMember>("Борис", 100);

        owner = std::move(temporaryOwner);

        if (!temporaryOwner)
        {
            std::cout << "temporaryOwner пуст: владение передано.\n";
        }

        std::cout << "Выходим из внутреннего блока...\n";
    }

    std::cout << "Блок завершён, но объект жив: "
        << owner->GetName() << '\n';

    owner.reset();

    std::cout << "owner.reset(): последний владелец отказался от объекта.\n";

    // ---------------------------------------------------------
    // 7. Динамический массив объектов
    // ---------------------------------------------------------

    std::cout << "\n=== Динамический массив объектов ===\n";

    {
        // Для массива нужен конструктор по умолчанию,
        // поэтому имена задаются после создания.
        std::unique_ptr<CrewMember[]> crewArray =
            std::make_unique<CrewMember[]>(2);

        crewArray[0].Rename("Александр");
        crewArray[1].Rename("Николай");

        std::cout << "Создан массив из двух объектов CrewMember.\n";

        crewArray[0].TakeDamage(10);
        crewArray[1].TakeDamage(20);

        std::cout << "Здоровье первого: "
            << crewArray[0].GetHealth() << "/100\n";

        std::cout << "Здоровье второго: "
            << crewArray[1].GetHealth() << "/100\n";

        std::cout << "Выходим из блока...\n";
    }

    std::cout << "Динамический массив освобождён автоматически.\n";

    // ---------------------------------------------------------
    // 8. Массив динамических объектов
    // ---------------------------------------------------------

    std::cout << "\n=== Массив динамических объектов ===\n";

    {
        std::unique_ptr<CrewMember> crewPointers[2];

        crewPointers[0] = std::make_unique<CrewMember>("Сергей", 100);
        crewPointers[1] = std::make_unique<CrewMember>("Дмитрий", 100);

        crewPointers[0]->TakeDamage(15);
        crewPointers[1]->TakeDamage(25);

        std::cout << "Сергей: "
            << crewPointers[0]->GetHealth() << "/100\n";

        std::cout << "Дмитрий: "
            << crewPointers[1]->GetHealth() << "/100\n";

        std::cout << "Выходим из блока...\n";
    }

    std::cout << "Массив динамических объектов освобождён автоматически.\n";

    // ---------------------------------------------------------
    // 9. shared_ptr: несколько владельцев
    // ---------------------------------------------------------

    std::cout << "\n=== shared_ptr: счётчик владельцев ===\n";

    // Корабль хранит weak_ptr, поэтому счётчик не увеличивается.
    std::cout << "Владельцев у Ивана: "
        << sailor.use_count() << '\n';

    {
        std::shared_ptr<CrewMember> copyOfSailor = sailor;

        std::cout << "После копирования указателя: "
            << sailor.use_count() << '\n';
    }

    std::cout << "После уничтожения копии: "
        << sailor.use_count() << '\n';

    // ---------------------------------------------------------
    // 10. weak_ptr: наблюдатель
    // ---------------------------------------------------------

    std::cout << "\n=== weak_ptr: проверка через lock() ===\n";

    ship.ShowCrewMember();

    std::shared_ptr<CrewMember> secondOwner = sailor;

    std::cout << "Владельцев у Ивана: "
        << sailor.use_count() << '\n';

    sailor.reset();

    std::cout << "Первый владелец отказался. Владельцев: "
        << secondOwner.use_count() << '\n';

    ship.ShowCrewMember();

    secondOwner.reset();

    std::cout << "Последний владелец отказался.\n";

    ship.ShowCrewMember();

    std::cout << "Размер экипажа корабля: "
        << ship.GetCrewSize() << '\n';

    std::cout << "\n=== Конец программы ===\n";

    return 0;
}