#pragma once

#include <string>

#include "ship.hpp"

// Торговая компания управляет своим кораблём.
// Ship хранится внутри TradingCompany, поэтому это композиция.
class TradingCompany
{
public:
    // Создаёт торговую компанию с названием
    // и заданными параметрами собственного корабля.
    TradingCompany(
        const std::string& name,
        const std::string& shipName,
        int cargoCapacity);

    // Деструктор торговой компании.
    ~TradingCompany();

    // Загружает груз на корабль компании.
    void LoadCargo(int weight);

    // Выводит информацию о компании и её корабле.
    void ShowInfo() const;

    // Возвращает название компании.
    const std::string& GetName() const;

private:
    // Название торговой компании.
    std::string name_;

    // Корабль является частью торговой компании.
    // Он создаётся и уничтожается вместе с ней.
    Ship ship_;
};