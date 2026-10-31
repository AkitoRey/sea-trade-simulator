#pragma once

#include "base_ship.hpp"

// Торговый шлюп: малая осадка, умеет ускользать из боя.
class TradeSloop : public BaseShip
{
public:
    TradeSloop(std::string_view name, int cargoCapacity,
               std::shared_ptr<Port> homePort);
    ~TradeSloop() override;

    int GetDraft() const override;
    void UseSpecialAction() const override;
    std::string Describe() const override;
};
