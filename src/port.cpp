#include "port.hpp"

#include "base_ship.hpp"

#include <iostream>

Port::Port(std::string_view name)
    : name_{ name }
{
}

const std::string& Port::GetName() const
{
    return name_;
}

void Port::Rename(std::string_view name)
{
    name_ = name;
}

void Port::Report(const BaseShip& ship) const
{
    std::cout << "Порт " << name_ << ": принят корабль "
              << ship.GetName() << ".\n";
}
