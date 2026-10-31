#include "trade_sloop.hpp"

#include <iostream>
#include <utility>

TradeSloop::TradeSloop(std::string_view name, int cargoCapacity,
                       std::shared_ptr<Port> homePort)
    : BaseShip{ name, cargoCapacity, std::move(homePort) }
{
    std::cout << "TradeSloop(): " << GetName() << "\n";
}

TradeSloop::TradeSloop()
    : BaseShip{}
{
    std::cout << "TradeSloop(): по умолчанию\n";
}

TradeSloop::TradeSloop(const TradeSloop& other)
    : BaseShip{ other }
{
    std::cout << "TradeSloop(const TradeSloop&): " << GetName() << "\n";
}

TradeSloop::TradeSloop(TradeSloop&& other) noexcept
    : BaseShip{ std::move(other) }
{
    std::cout << "TradeSloop(TradeSloop&&): " << GetName() << "\n";
}

TradeSloop::~TradeSloop()
{
    std::cout << "~TradeSloop: " << GetName() << "\n";
}

std::unique_ptr<BaseShip> TradeSloop::Clone() const
{
    return std::make_unique<TradeSloop>(*this);
}

int TradeSloop::GetDraft() const
{
    return 2;
}

void TradeSloop::UseSpecialAction() const
{
    std::cout << GetName() << ": ускользание — побег из боя без потерь.\n";
}

// Дополняет метод базового класса.
std::string TradeSloop::Describe() const
{
    return BaseShip::Describe() + ", шлюп (мелкие порты доступны)";
}
