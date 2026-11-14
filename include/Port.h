#pragma once

#include <string>
#include <string_view>

class BaseShip;

// Партия груза: товар, масса, признак контрабанды.
struct CargoLot
{
    std::string goodName{};
    int tons{ 0 };
    bool contraband{ false };
};

// Порт. Правило нуля: все поля копируются и освобождаются сами.
class Port
{
private:
    std::string name_{};
    bool bansContraband_{ false };

public:
    explicit Port(std::string_view name, bool bansContraband = false);

    void Rename(std::string_view newName);
    const std::string& GetName() const;
    bool BansContraband() const;

    void Report(const BaseShip& ship) const;

    // Правило 9: бросает ForbiddenGoodsError, если товар запрещён в этом порту.
    // Строгая гарантия: состояние порта не меняется.
    void Sell(const CargoLot& lot) const;

    // Только проверка правила 9, без вывода: бросает ForbiddenGoodsError.
    void CheckAcceptable(const CargoLot& lot) const;
};
