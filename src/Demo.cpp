#include <iostream>
#include <memory>
#include <stdexcept>
#include <typeinfo>
#include <vector>

#include "Captain.h"
#include "CoastalBattery.h"
#include "Exceptions.h"
#include "Galleon.h"
#include "NavyFrigate.h"
#include "Operations.h"
#include "StormTactics.h"
#include "TradeSloop.h"

namespace
{
    void Section(const char* title)
    {
        std::cout << "\n=== " << title << " ===\n";
    }
}

int RunDemo()
{
    const auto port = std::make_shared<Port>("Барнаул");
    const auto strictPort = std::make_shared<Port>("Бийск", true);   // контрабанда запрещена

    std::vector<std::unique_ptr<BaseShip>> fleet;
    fleet.push_back(std::make_unique<TradeSloop>("Ласточка", 40, port));
    fleet.push_back(std::make_unique<NavyFrigate>("Стерегущий", 80, port, 2));
    fleet.push_back(std::make_unique<Galleon>("Громовержец", 200, port));

    // ---------------------------------------------------------------
    Section("1. Интерфейс IArmed: функция не знает конкретных классов");
    {
        // Одна и та же функция работает с фрегатом, галеоном и береговой батареей.
        auto& frigate = dynamic_cast<NavyFrigate&>(*fleet[1]);
        auto& galleon = dynamic_cast<Galleon&>(*fleet[2]);
        CoastalBattery battery{ "Старый форт", 6, 3 };

        RepelPirates(frigate, 40);
        RepelPirates(galleon, 100);
        RepelPirates(battery, 25);
    }

    // ---------------------------------------------------------------
    Section("2. Множественное наследование: BaseShip + IArmed");
    {
        Galleon galleon{ "Левиафан", 300, port };
        const BaseShip& asShip = galleon;   // как судно
        const IArmed& asArmed = galleon;    // как вооружённая единица
        std::cout << "  как BaseShip: " << asShip.Describe() << "\n";
        std::cout << "  как IArmed:   " << asArmed.GetUnitName() << ", мощность "
                  << asArmed.GetFirepower() << "\n";
        std::cout << "  адреса баз различаются (два подобъекта в одном объекте): "
                  << (static_cast<const void*>(&asShip) != static_cast<const void*>(&asArmed)
                          ? "да" : "нет")
                  << "\n";
    }

    // ---------------------------------------------------------------
    Section("3. Раннее и позднее связывание");
    for (const auto& ship : fleet)
    {
        BaseShip& base = *ship;   // статический тип — BaseShip, динамический — наследник
        std::cout << "  GetKind()  (не виртуальный, раннее): " << base.GetKind() << "\n";
        std::cout << "  Describe() (виртуальный, позднее):   " << base.Describe() << "\n";
    }
    {
        const auto& sloop = dynamic_cast<const TradeSloop&>(*fleet[0]);
        std::cout << "  Та же «Ласточка» через ссылку на TradeSloop: GetKind() = "
                  << sloop.GetKind() << "\n";
    }

    // ---------------------------------------------------------------
    Section("4. dynamic_cast: воевать может только то, что реализует IArmed");
    for (const auto& ship : fleet)
    {
        MeetPirates(*ship, 40);
    }
    try
    {
        // Приведение ссылки к интерфейсу, который судно не реализует, бросает bad_cast.
        auto& armed = dynamic_cast<IArmed&>(*fleet[0]);
        armed.Fire("цель");
    }
    catch (const std::bad_cast&)
    {
        std::cout << "  «" << fleet[0]->GetName() << "» не реализует IArmed: bad_cast\n";
    }

    // ---------------------------------------------------------------
    Section("5. Конфигурирование: тактика капитана в шторм");
    {
        BaseShip& galleon = *fleet[2];
        BaseShip& sloop = *fleet[0];
        galleon.LoadCargo(100);
        sloop.LoadCargo(30);

        // Один и тот же шторм, один и тот же груз — меняется только объект поведения.
        const auto reload = [](BaseShip& ship, int tons)
        {
            ship.UnloadCargo(ship.GetCurrentCargo());
            ship.LoadCargo(tons);
        };

        // Статическое конфигурирование: тактика задана при создании капитана.
        Captain captain{ "Беринг", std::make_unique<PushOnTactic>() };
        std::cout << "  Шлюп «" << sloop.GetName() << "», груз " << sloop.GetCurrentCargo() << " т\n";
        captain.FaceStorm(sloop, 7);

        // Динамическое конфигурирование: тот же капитан получает другой объект поведения.
        reload(sloop, 30);
        captain.SetTactic(std::make_unique<ChangeRouteTactic>());
        captain.FaceStorm(sloop, 7);

        reload(sloop, 30);
        captain.SetTactic(std::make_unique<WaitOutTactic>());
        captain.FaceStorm(sloop, 7);

        std::cout << "  Галеон «" << galleon.GetName() << "» (осадка " << galleon.GetDraft()
                  << ") в том же шторме, тактика «напролом»:\n";
        captain.SetTactic(std::make_unique<PushOnTactic>());
        captain.FaceStorm(galleon, 7);

        galleon.UnloadCargo(galleon.GetCurrentCargo());
        sloop.UnloadCargo(sloop.GetCurrentCargo());
    }

    // ---------------------------------------------------------------
    Section("6. Исключения");

    std::cout << "-- 6.1. Переполнение трюма: создаёт BaseShip::LoadCargo, ловит LoadAcrossFleet --\n";
    {
        const int left = LoadAcrossFleet(fleet, 300);
        std::cout << "  не поместилось: " << left << " т\n";
        for (const auto& ship : fleet)
        {
            std::cout << "  " << ship->Describe() << "\n";
        }
        for (const auto& ship : fleet)
        {
            ship->UnloadCargo(ship->GetCurrentCargo());
        }
    }

    std::cout << "-- 6.2. Строгая гарантия: после отказа состояние судна не изменилось --\n";
    {
        BaseShip& sloop = *fleet[0];
        sloop.LoadCargo(35);
        try
        {
            sloop.LoadCargo(10);   // свободно только 5 т
        }
        catch (const CargoCapacityExceeded& error)   // частный тип — первым
        {
            std::cout << "  " << error.what() << "\n";
        }
        catch (const SeaTradeError& error)           // затем общий для предметной области
        {
            std::cout << "  ошибка правил: " << error.what() << "\n";
        }
        catch (const std::exception& error)          // самый общий — последним
        {
            std::cout << "  ошибка: " << error.what() << "\n";
        }
        std::cout << "  груз после отказа: " << sloop.GetCurrentCargo() << " т (было 35)\n";
        sloop.UnloadCargo(35);
    }

    std::cout << "-- 6.3. Запрещённый товар (правило 9) --\n";
    {
        const CargoLot rum{ "ром", 5, false };
        const CargoLot smuggled{ "контрабандный шёлк", 3, true };
        SellAtPort(*strictPort, rum);        // успешное действие
        SellAtPort(*strictPort, smuggled);   // нарушение правила: исключение перехвачено внутри
        SellAtPort(*port, smuggled);         // в этом порту запрета нет
    }

    std::cout << "-- 6.4. Боезапас и свёртка стека (иерархия исключений) --\n";
    try
    {
        NavyFrigate frigate{ "Дозорный", 50, port, 1 };   // боезапас: один залп
        Volley(frigate, "пират №1");                      // успешное действие
        Volley(frigate, "пират №2");                      // боезапас кончился: OutOfAmmunition
    }
    catch (const OutOfAmmunition& error)
    {
        std::cout << "  " << error.what() << "\n";
    }

    std::cout << "-- 6.5. Ошибки программиста: std::logic_error --\n";
    try
    {
        fleet[0]->LoadCargo(-5);
    }
    catch (const std::invalid_argument& error)
    {
        std::cout << "  invalid_argument: " << error.what() << "\n";
    }
    try
    {
        TradeSloop broken{ "", 10, port };
    }
    catch (const std::logic_error& error)   // invalid_argument — наследник logic_error
    {
        std::cout << "  logic_error при создании судна: " << error.what() << "\n";
    }

    std::cout << "-- 6.6. RAII: журнал закрывается и при исключении --\n";
    try
    {
        LoadShipment(*fleet[0], 20);   // успешно: журнал закрылся при выходе из функции
        LoadShipment(*fleet[0], 99);   // исключение: журнал закроется при свёртке стека
    }
    catch (const SeaTradeError& error)
    {
        std::cout << "  перехвачено выше: " << error.what() << "\n";
    }

    // ---------------------------------------------------------------
    Section("Конец программы");
    return 0;
}
