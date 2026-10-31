#include "navy_frigate.hpp"

#include <iostream>
#include <utility>

NavyFrigate::NavyFrigate(std::string_view name, int cargoCapacity,
                         std::shared_ptr<Port> homePort)
    : BaseShip{ name, cargoCapacity, std::move(homePort) }
{
    std::cout << "NavyFrigate(): " << GetName() << "\n";
}

NavyFrigate::~NavyFrigate()
{
    std::cout << "~NavyFrigate: " << GetName() << "\n";
}

int NavyFrigate::GetDraft() const
{
    return 4;
}

void NavyFrigate::UseSpecialAction() const
{
    std::cout << GetName() << ": эскортирование — караван под охраной.\n";
}

std::string NavyFrigate::Describe() const
{
    return BaseShip::Describe() + ", фрегат (урон от пиратов снижен)";
}
