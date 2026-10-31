#include "galleon.hpp"

#include <iostream>
#include <utility>

Galleon::Galleon(std::string_view name, int cargoCapacity,
                 std::shared_ptr<Port> homePort)
    : BaseShip{ name, cargoCapacity, std::move(homePort) }
{
    std::cout << "Galleon(): " << GetName() << "\n";
}

Galleon::Galleon()
    : BaseShip{}
{
    std::cout << "Galleon(): по умолчанию\n";
}

Galleon::Galleon(const Galleon& other)
    : BaseShip{ other }
{
    std::cout << "Galleon(const Galleon&): " << GetName() << "\n";
}

Galleon::Galleon(Galleon&& other) noexcept
    : BaseShip{ std::move(other) }
{
    std::cout << "Galleon(Galleon&&): " << GetName() << "\n";
}

Galleon::~Galleon()
{
    std::cout << "~Galleon: " << GetName() << "\n";
}

std::unique_ptr<BaseShip> Galleon::Clone() const
{
    return std::make_unique<Galleon>(*this);
}

int Galleon::GetDraft() const
{
    return 7;
}

void Galleon::UseSpecialAction() const
{
    std::cout << GetName() << ": залповый огонь и абордажный захват.\n";
}

// Заменяет метод полностью, без вызова базового.
std::string Galleon::Describe() const
{
    return "Галеон «" + GetName() + "», осадка "
        + std::to_string(GetDraft()) + " (только глубокие порты)";
}
