#include "trading_company.hpp"

#include <iostream>
#include <utility>

TradingCompany::TradingCompany(std::string_view name,
                               std::unique_ptr<BaseShip> ship)
    : name_{ name }
    , ship_{ std::move(ship) }
{
    std::cout << "Торговая компания " << name_ << " создана.\n";
}

TradingCompany::~TradingCompany()
{
    std::cout << "Торговая компания " << name_ << " уничтожена.\n";
}

void TradingCompany::LoadCargo(int weight)
{
    ship_->LoadCargo(weight);
}

void TradingCompany::ShowInfo() const
{
    std::cout << "\nИнформация о торговой компании:\n"
              << "Компания: " << name_ << "\n"
              << "Корабль: " << ship_->Describe() << "\n";
}

const std::string& TradingCompany::GetName() const
{
    return name_;
}
