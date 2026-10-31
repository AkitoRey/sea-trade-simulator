#pragma once

#include "base_ship.hpp"

// Торговый шлюп: малая осадка, умеет ускользать из боя.
class TradeSloop : public BaseShip
{
public:
    TradeSloop(std::string_view name, int cargoCapacity,
               std::shared_ptr<Port> homePort);
    TradeSloop();
    TradeSloop(const TradeSloop& other);
    TradeSloop(TradeSloop&& other) noexcept;
    TradeSloop& operator=(const TradeSloop& other) = default;
    TradeSloop& operator=(TradeSloop&& other) noexcept = default;
    ~TradeSloop() override;

    int GetDraft() const override;
    std::unique_ptr<BaseShip> Clone() const override;
    void UseSpecialAction() const override;
    std::string Describe() const override;
};
