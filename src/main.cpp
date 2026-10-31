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
    // 2. Виды конструкторов и порядок вызовов
    // ---------------------------------------------------------

    std::cout << "\n=== Конструктор по умолчанию ===\n";

    {
        TradeSloop defaultSloop;
        std::cout << defaultSloop.GetName() << "\n";
        std::cout << "Выходим из блока...\n";
    }

    std::cout << "\n=== Конструктор с параметрами, копирования, перемещения ===\n";

    {
        TradeSloop sloop{ "Чайка", 30, port };

        std::cout << "-- копирование --\n";
        TradeSloop copy = sloop;

        std::cout << "-- перемещение --\n";
        TradeSloop moved = std::move(sloop);

        std::cout << "Имя оригинала после перемещения: «"
                  << sloop.GetName() << "»\n";
        std::cout << "Имя перемещённого: " << moved.GetName() << "\n";
        std::cout << "Выходим из блока...\n";
    }

    std::cout << "\n=== Виртуальный деструктор ===\n";

    {
        std::unique_ptr<BaseShip> ship =
            std::make_unique<Galleon>("Левиафан", 300, port);

        std::cout << "Уничтожаем через указатель на BaseShip...\n";
        ship.reset();
        std::cout << "Готово.\n";
    }

    // ---------------------------------------------------------
    // 3. Поверхностная и глубокая копия
    // ---------------------------------------------------------

    std::cout << "\n=== Поверхностная копия: общий порт (shared_ptr) ===\n";

    {
        const auto harbor = std::make_shared<Port>("Бийск");

        TradeSloop alpha{ "Альфа", 20, harbor };
        TradeSloop beta{ "Бета", 20, harbor };

        std::cout << "Владельцев порта: " << harbor.use_count() << "\n";

        alpha.GetHomePort()->Rename("Рубцовск");

        std::cout << "Переименовали порт через Альфу.\n";
        std::cout << alpha.Describe() << "\n";
        std::cout << beta.Describe() << "\n";
        std::cout << "Изменился и у Беты: порт общий.\n";
    }

    std::cout << "\n=== Глубокая копия: конструктор копирования ===\n";

    {
        TradeSloop original{ "Оригинал", 20,
                             std::make_shared<Port>("Бийск") };
        TradeSloop copy = original;

        std::cout << "Владельцев порта у оригинала: "
                  << original.GetHomePort().use_count() << "\n";

        copy.GetHomePort()->Rename("Рубцовск");

        std::cout << "Переименовали порт у копии.\n";
        std::cout << "оригинал: " << original.Describe() << "\n";
        std::cout << "копия:    " << copy.Describe() << "\n";
        std::cout << "Оригинал не изменился: у копии свой порт.\n";
    }

    std::cout << "\n=== Клонирование через указатель на базу: Clone() ===\n";

    {
        const std::unique_ptr<BaseShip> frigate =
            std::make_unique<NavyFrigate>("Эталон", 50,
                                          std::make_shared<Port>("Бийск"));
        const std::unique_ptr<BaseShip> twin = frigate->Clone();

        twin->GetHomePort()->Rename("Рубцовск");

        std::cout << "оригинал: " << frigate->Describe() << "\n";
        std::cout << "клон:     " << twin->Describe() << "\n";
        std::cout << "Копия того же типа (фрегат), порт у неё свой.\n";
    }

    std::cout << "\n=== Присваивание копированием ===\n";

    {
        TradeSloop first{ "Первый", 20, std::make_shared<Port>("Бийск") };
        TradeSloop second{ "Второй", 30, std::make_shared<Port>("Томск") };

        second = first;
        second.GetHomePort()->Rename("Рубцовск");

        std::cout << "first:  " << first.Describe() << "\n";
        std::cout << "second: " << second.Describe() << "\n";

        // Самоприсваивание через ссылку.
        TradeSloop& alias = first;
        first = alias;
    }

    // ---------------------------------------------------------
    // 4. Строки, this, статические члены, final
    // ---------------------------------------------------------

    std::cout << "\n=== Статический счётчик ===\n";

    std::cout << "Выдано инвентарных номеров: "
              << BaseShip::GetIssuedNumbersCount() << "\n";

    TradeSloop star{ "Морская звезда", 25, port };

    std::cout << "После создания «" << star.GetName() << "» (№"
              << star.GetInventoryNumber() << "): "
              << BaseShip::GetIssuedNumbersCount() << "\n";

    std::cout << "\n=== Строки ===\n";

    // Конкатенация.
    std::string label = star.GetName() + " / " + star.GetHomePort()->GetName();
    label += " (шлюп)";
    std::cout << "Конкатенация: " << label << "\n";

    // Поиск.
    const std::string part = "звезд";
    const std::size_t position = star.GetName().find(part);
    if (position != std::string::npos)
    {
        std::cout << "Поиск: «" << part << "» найдено с байта "
                  << position << "\n";
    }

    std::cout << "NameContains(\"Ласт\"): ";
    for (const auto& ship : fleet)
    {
        if (ship->NameContains("Ласт"))
        {
            std::cout << ship->GetName() << " ";
        }
    }
    std::cout << "\n";

    // Подстрока.
    std::cout << "Подстрока (первое слово): " << star.FirstWordOfName() << "\n";

    // Сравнение.
    std::cout << "Сравнение ==: "
              << (star.GetName() == "Морская звезда" ? "равны" : "не равны")
              << "\n";
    std::cout << "Сравнение compare: «" << fleet[0]->GetName() << "» "
              << (fleet[0]->GetName().compare(fleet[1]->GetName()) < 0
                      ? "раньше" : "позже")
              << " «" << fleet[1]->GetName() << "»\n";

    std::cout << "\n=== this: корабль передаёт себя порту ===\n";

    star.DockAt(*port);

    std::cout << "\n=== final ===\n";

    // Galleon помечен final: class Armada : public Galleon {}; не скомпилируется.
    std::cout << "Galleon final: наследоваться от галеона нельзя.\n";

    // ---------------------------------------------------------
    // 5. Композиция и агрегация из ЛР 2-3
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
