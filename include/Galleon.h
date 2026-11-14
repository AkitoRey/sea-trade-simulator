#pragma once

#include "BaseShip.h"
#include "IArmed.h"

// Тяжёлый галеон: у него не будет подклассов (final), но он вооружён.
class Galleon final : public BaseShip, public IArmed
{
private:
    static constexpr int CANNONS = 32;
    static constexpr int POWER_PER_CANNON = 4;

    int volleysLeft_{ 0 };
    int maxVolleys_{ 0 };

public:
    Galleon(std::string_view name, int cargoCapacity, std::shared_ptr<Port> homePort,
            int volleys = 10);
    Galleon(const Galleon&) = default;
    Galleon(Galleon&&) noexcept = default;
    Galleon& operator=(const Galleon&) = default;
    Galleon& operator=(Galleon&&) noexcept = default;
    ~Galleon() noexcept override = default;

    std::string GetKind() const;   // скрывает BaseShip::GetKind

    // BaseShip
    std::string Describe() const override;   // заменяет описание базы целиком
    int GetDraft() const override;
    void UseSpecialAction() const override;
    std::unique_ptr<BaseShip> Clone() const override;

    // IArmed
    std::string GetUnitName() const override;
    int GetFirepower() const override;
    bool HasAmmunition() const override;
    int GetVolleysLeft() const override;
    int Resupply(int volleys) override;
    void Fire(std::string_view target) override;
};
