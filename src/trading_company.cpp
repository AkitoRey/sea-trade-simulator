#include "trading_company.hpp"

#include <iostream>

#pragma execution_character_set("utf-8")

// Конструктор торговой компании.
TradingCompany::TradingCompany(
    const std::string& name,
    const std::string& shipName,
    int cargoCapacity)
    : name_(name),
    ship_(shipName, cargoCapacity)
{
    std::cout << "Торговая компания "
        << name_
        << " создана.\n";
}

// Деструктор торговой компании.
TradingCompany::~TradingCompany()
{
    std::cout << "Торговая компания "
        << name_
        << " уничтожена.\n";

    // Ship является частью TradingCompany.
    // Поэтому после завершения деструктора компании
    // ship_ уничтожится автоматически.
}

// Загрузка груза на корабль компании.
void TradingCompany::LoadCargo(int weight)
{
    ship_.LoadCargo(weight);
}

// Вывод информации о компании.
void TradingCompany::ShowInfo() const
{
    std::cout << "\nИнформация о торговой компании:\n";

    std::cout << "Компания: "
        << name_ << '\n';

    std::cout << "Корабль: "
        << ship_.GetName() << '\n';

    std::cout << "Груз: "
        << ship_.GetCurrentCargo()
        << "/"
        << ship_.GetCargoCapacity()
        << " тонн\n";
}

const std::string& TradingCompany::GetName() const
{
    return name_;
}