#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "BaseShip.h"
#include "IStormTactic.h"

// Капитан. Тактику поведения в шторм он не решает сам, а поручает объекту поведения.
// Подставили другой объект — изменилось поведение (динамическое конфигурирование).
class Captain
{
private:
    std::string name_{};
    std::unique_ptr<IStormTactic> tactic_{};   // капитан владеет своей тактикой

public:
    // Бросает std::invalid_argument, если тактика не задана.
    Captain(std::string_view name, std::unique_ptr<IStormTactic> tactic);
    Captain(Captain&&) noexcept = default;
    Captain& operator=(Captain&&) noexcept = default;
    ~Captain() noexcept = default;

    // Динамическое конфигурирование: заменить объект поведения во время работы.
    void SetTactic(std::unique_ptr<IStormTactic> tactic);

    const std::string& GetName() const;
    std::string GetTacticName() const;

    // Только расчёт: что даст шторм при текущей тактике (судно не меняется).
    StormOutcome Evaluate(const BaseShip& ship, int stormPower) const;

    // Шторм: тактика считает последствия, капитан применяет их к судну.
    StormOutcome FaceStorm(BaseShip& ship, int stormPower) const;
};
