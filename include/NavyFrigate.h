#pragma once

#include "BaseShip.h"
#include "IArmed.h"

// Военный фрегат. Множественное наследование: состояние и поведение судна
// берём у абстрактного класса BaseShip, контракт «вооружён» — у интерфейса IArmed.
class NavyFrigate : public BaseShip, public IArmed
{
private:
    static constexpr int CANNONS = 18;
    static constexpr int POWER_PER_CANNON = 3;

    int volleysLeft_{ 0 };
    int maxVolleys_{ 0 };

public:
    NavyFrigate(std::string_view name, int cargoCapacity, std::shared_ptr<Port> homePort,
                int volleys = 6);
    NavyFrigate(const NavyFrigate&) = default;
    NavyFrigate(NavyFrigate&&) noexcept = default;
    NavyFrigate& operator=(const NavyFrigate&) = default;
    NavyFrigate& operator=(NavyFrigate&&) noexcept = default;
    ~NavyFrigate() noexcept override = default;

    std::string GetKind() const;   // скрывает BaseShip::GetKind

    // BaseShip
    std::string Describe() const override;
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
