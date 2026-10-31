#pragma once

#include "base_ship.hpp"

// Тяжёлый галеон. final: подклассов у галеона не будет.
class Galleon final : public BaseShip
{
public:
    Galleon(std::string_view name, int cargoCapacity,
            std::shared_ptr<Port> homePort);
    ~Galleon() override;

    int GetDraft() const override;
    void UseSpecialAction() const override;
    std::string Describe() const override;
};
