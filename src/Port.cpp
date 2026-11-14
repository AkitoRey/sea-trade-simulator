#include "Port.h"

#include <iostream>

#include "BaseShip.h"
#include "Exceptions.h"

Port::Port(std::string_view name, bool bansContraband)
    : name_{ name }
    , bansContraband_{ bansContraband }
{}

void Port::Rename(std::string_view newName)
{
    name_ = newName;
}

const std::string& Port::GetName() const
{
    return name_;
}

bool Port::BansContraband() const
{
    return bansContraband_;
}

void Port::Report(const BaseShip& ship) const
{
    std::cout << "Порт " << name_ << ": принят корабль " << ship.GetName() << ".\n";
}

void Port::Sell(const CargoLot& lot) const
{
    CheckAcceptable(lot);
    std::cout << "  порт " << name_ << ": куплено «" << lot.goodName << "», "
              << lot.tons << " т\n";
}

void Port::CheckAcceptable(const CargoLot& lot) const
{
    if (lot.contraband && bansContraband_)
    {
        throw ForbiddenGoodsError{ lot.goodName, name_ };
    }
}
