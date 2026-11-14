#include "NavyFrigate.h"

#include <algorithm>
#include <iostream>
#include <utility>

#include "Exceptions.h"

NavyFrigate::NavyFrigate(std::string_view name, int cargoCapacity, std::shared_ptr<Port> homePort,
                         int volleys)
    : BaseShip{ name, cargoCapacity, std::move(homePort) }
    , volleysLeft_{ volleys }
    , maxVolleys_{ volleys }
{}

std::string NavyFrigate::GetKind() const
{
    return "фрегат";
}

std::string NavyFrigate::Describe() const
{
    return BaseShip::Describe() + ", фрегат (урон от пиратов снижен), залпов: "
           + std::to_string(volleysLeft_);
}

int NavyFrigate::GetDraft() const
{
    return 4;
}

void NavyFrigate::UseSpecialAction() const
{
    std::cout << "  " << GetName() << ": эскортирование — караван под охраной.\n";
}

std::unique_ptr<BaseShip> NavyFrigate::Clone() const
{
    return std::make_unique<NavyFrigate>(*this);
}

std::string NavyFrigate::GetUnitName() const
{
    return "Фрегат «" + GetName() + "»";
}

int NavyFrigate::GetFirepower() const
{
    return CANNONS * POWER_PER_CANNON;
}

bool NavyFrigate::HasAmmunition() const
{
    return volleysLeft_ > 0;
}

int NavyFrigate::GetVolleysLeft() const
{
    return volleysLeft_;
}

int NavyFrigate::Resupply(int volleys)
{
    const int added = std::clamp(volleys, 0, maxVolleys_ - volleysLeft_);
    volleysLeft_ += added;
    return added;
}

void NavyFrigate::Fire(std::string_view /*target*/)
{
    if (volleysLeft_ == 0)
    {
        throw OutOfAmmunition{ GetUnitName() };   // состояние не менялось: строгая гарантия
    }
    --volleysLeft_;
}
