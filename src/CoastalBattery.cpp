#include "CoastalBattery.h"

#include <algorithm>
#include <iostream>

#include "Exceptions.h"

CoastalBattery::CoastalBattery(std::string_view name, int cannons, int volleys)
    : name_{ name }
    , cannons_{ cannons }
    , volleysLeft_{ volleys }
    , maxVolleys_{ volleys }
{}

std::string CoastalBattery::GetUnitName() const
{
    return "Батарея «" + name_ + "»";
}

int CoastalBattery::GetFirepower() const
{
    return cannons_ * POWER_PER_CANNON;
}

bool CoastalBattery::HasAmmunition() const
{
    return volleysLeft_ > 0;
}

int CoastalBattery::GetVolleysLeft() const
{
    return volleysLeft_;
}

int CoastalBattery::Resupply(int volleys)
{
    const int added = std::clamp(volleys, 0, maxVolleys_ - volleysLeft_);
    volleysLeft_ += added;
    return added;
}

void CoastalBattery::Fire(std::string_view /*target*/)
{
    if (volleysLeft_ == 0)
    {
        throw OutOfAmmunition{ GetUnitName() };
    }
    --volleysLeft_;
}
