#pragma once

#include <string>
#include <string_view>

class BaseShip;

// Порт приписки. Вложенный объект корабля: на нём показывается
// разница между поверхностной и глубокой копией.
class Port
{
private:
    std::string name_{};

public:
    explicit Port(std::string_view name);

    const std::string& GetName() const;
    void Rename(std::string_view name);

    // Порт получает текущий корабль (демонстрация this).
    void Report(const BaseShip& ship) const;
};
