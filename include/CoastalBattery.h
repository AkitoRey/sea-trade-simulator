#pragma once

#include <string>
#include <string_view>

#include "IArmed.h"

// Береговая батарея порта: не судно, но тоже «вооружена».
// Показывает, что интерфейс не привязан к иерархии BaseShip.
class CoastalBattery : public IArmed
{
private:
    static constexpr int POWER_PER_CANNON = 5;

    std::string name_{};
    int cannons_{ 0 };
    int volleysLeft_{ 0 };
    int maxVolleys_{ 0 };

public:
    CoastalBattery(std::string_view name, int cannons, int volleys);
    ~CoastalBattery() noexcept override = default;

    std::string GetUnitName() const override;
    int GetFirepower() const override;
    bool HasAmmunition() const override;
    int GetVolleysLeft() const override;
    int Resupply(int volleys) override;
    void Fire(std::string_view target) override;
};
