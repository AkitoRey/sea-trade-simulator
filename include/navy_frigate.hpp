#pragma once

#include "base_ship.hpp"

// Военный фрегат: эскортирует караваны.
class NavyFrigate : public BaseShip
{
public:
    NavyFrigate(std::string_view name, int cargoCapacity,
                std::shared_ptr<Port> homePort);
    ~NavyFrigate() override;

    int GetDraft() const override;
    void UseSpecialAction() const override;
    std::string Describe() const override;
};
