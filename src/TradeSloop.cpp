#include "TradeSloop.h"

#include <iostream>
#include <utility>

TradeSloop::TradeSloop(std::string_view name, int cargoCapacity, std::shared_ptr<Port> homePort)
    : BaseShip{ name, cargoCapacity, std::move(homePort) }
{}

std::string TradeSloop::GetKind() const
{
    return "шлюп";
}

std::string TradeSloop::Describe() const
{
    return BaseShip::Describe() + ", шлюп (мелкие порты доступны, оружия нет)";
}

int TradeSloop::GetDraft() const
{
    return 2;
}

void TradeSloop::UseSpecialAction() const
{
    std::cout << "  " << GetName() << ": ускользание — побег из боя без потерь.\n";
}

std::unique_ptr<BaseShip> TradeSloop::Clone() const
{
    return std::make_unique<TradeSloop>(*this);
}
