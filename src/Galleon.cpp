#include "Galleon.h"

#include <algorithm>
#include <iostream>
#include <utility>

#include "Exceptions.h"

Galleon::Galleon(std::string_view name, int cargoCapacity, std::shared_ptr<Port> homePort,
                 int volleys)
    : BaseShip{ name, cargoCapacity, std::move(homePort) }
    , volleysLeft_{ volleys }
    , maxVolleys_{ volleys }
{}

std::string Galleon::GetKind() const
{
    return "галеон";
}

std::string Galleon::Describe() const
{
    // Базовое описание не вызываем: галеон описывает себя целиком.
    return "Галеон «" + GetName() + "», осадка " + std::to_string(GetDraft())
           + " (только глубокие порты), залпов: " + std::to_string(volleysLeft_);
}

int Galleon::GetDraft() const
{
    return 7;
}

void Galleon::UseSpecialAction() const
{
    std::cout << "  " << GetName() << ": залповый огонь и абордажный захват.\n";
}

std::unique_ptr<BaseShip> Galleon::Clone() const
{
    return std::make_unique<Galleon>(*this);
}

std::string Galleon::GetUnitName() const
{
    return "Галеон «" + GetName() + "»";
}

int Galleon::GetFirepower() const
{
    return CANNONS * POWER_PER_CANNON;
}

bool Galleon::HasAmmunition() const
{
    return volleysLeft_ > 0;
}

int Galleon::GetVolleysLeft() const
{
    return volleysLeft_;
}

int Galleon::Resupply(int volleys)
{
    const int added = std::clamp(volleys, 0, maxVolleys_ - volleysLeft_);
    volleysLeft_ += added;
    return added;
}

void Galleon::Fire(std::string_view /*target*/)
{
    if (volleysLeft_ == 0)
    {
        throw OutOfAmmunition{ GetUnitName() };
    }
    --volleysLeft_;
}
