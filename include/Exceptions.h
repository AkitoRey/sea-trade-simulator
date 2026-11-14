#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

// Базовое исключение предметной области: любое нарушение правил игры.
// Наследуем от runtime_error: ошибка возникает во время работы (не хватает места, запрещённый товар).
class SeaTradeError : public std::runtime_error
{
public:
    explicit SeaTradeError(const std::string& message)
        : std::runtime_error{ message }
    {}
};

// Правило 1: общая масса груза не может превышать грузоподъёмность.
class CargoCapacityExceeded : public SeaTradeError
{
private:
    int requested_{ 0 };
    int free_{ 0 };

public:
    CargoCapacityExceeded(std::string_view shipName, int requested, int freeCapacity)
        : SeaTradeError{ "«" + std::string(shipName) + "»: груз " + std::to_string(requested)
                         + " т не помещается, свободно " + std::to_string(freeCapacity) + " т" }
        , requested_{ requested }
        , free_{ freeCapacity }
    {}

    int GetRequested() const noexcept { return requested_; }
    int GetFree() const noexcept { return free_; }
};

// Правило 9: запрещённый товар нельзя легально продать в порту, где его продажа запрещена.
class ForbiddenGoodsError : public SeaTradeError
{
public:
    ForbiddenGoodsError(std::string_view goodName, std::string_view portName)
        : SeaTradeError{ "товар «" + std::string(goodName) + "» запрещён к продаже в порту "
                         + std::string(portName) }
    {}
};

// Боезапас ограничен: стрелять без боезапаса нельзя.
class OutOfAmmunition : public SeaTradeError
{
public:
    explicit OutOfAmmunition(std::string_view unitName)
        : SeaTradeError{ std::string(unitName) + ": боезапас исчерпан" }
    {}
};

// Судно с такой осадкой не войдёт в мелкий порт.
class PortTooShallow : public SeaTradeError
{
public:
    PortTooShallow(std::string_view shipName, int draft, std::string_view portName, int depth)
        : SeaTradeError{ std::string(shipName) + " (осадка " + std::to_string(draft)
                         + ") не войдёт в порт " + std::string(portName) + ": глубина "
                         + std::to_string(depth) }
    {}
};

// Правило 3: провизия расходуется в плавании; без запаса выходить нельзя.
class NotEnoughProvisions : public SeaTradeError
{
public:
    NotEnoughProvisions(int need, int have)
        : SeaTradeError{ "для перехода нужно " + std::to_string(need) + " ед. провизии, на борту "
                         + std::to_string(have) }
    {}
};
