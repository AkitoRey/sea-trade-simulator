#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "base_ship.hpp"

// Композиция: компания единственный владелец своего корабля.
// BaseShip абстрактный, поэтому корабль хранится через unique_ptr.
class TradingCompany
{
private:
    std::string name_{};
    std::unique_ptr<BaseShip> ship_{};

public:
    TradingCompany(std::string_view name, std::unique_ptr<BaseShip> ship);
    ~TradingCompany();

    void LoadCargo(int weight);
    void ShowInfo() const;
    const std::string& GetName() const;
};
