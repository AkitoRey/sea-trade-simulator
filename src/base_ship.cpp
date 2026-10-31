#include "base_ship.hpp"

#include "crew_member.hpp"

#include <iostream>
#include <utility>

BaseShip::BaseShip(std::string_view name, int cargoCapacity,
                   std::shared_ptr<Port> homePort)
    : inventoryNumber_{ NextInventoryNumber() }
    , name_{ name }
    , cargoCapacity_{ cargoCapacity }
    , homePort_{ std::move(homePort) }
{
    if (cargoCapacity_ <= 0)
    {
        std::cout << "Ошибка: грузоподъёмность должна быть больше нуля.\n";
        cargoCapacity_ = 1;
    }

    std::cout << "BaseShip(): " << name_ << "\n";
}

BaseShip::~BaseShip()
{
    std::cout << "~BaseShip: " << name_ << "\n";
}

int BaseShip::NextInventoryNumber()
{
    return ++lastInventoryNumber_;
}

std::string BaseShip::Header() const
{
    return name_ + " [порт " + homePort_->GetName()
        + ", №" + std::to_string(inventoryNumber_) + "]";
}

std::string BaseShip::Describe() const
{
    return Header() + ", осадка " + std::to_string(GetDraft())
        + ", груз " + std::to_string(currentCargo_) + "/"
        + std::to_string(cargoCapacity_);
}

void BaseShip::LoadCargo(int weight)
{
    if (weight <= 0)
    {
        std::cout << "Ошибка: вес груза должен быть больше нуля.\n";
        return;
    }

    if (currentCargo_ + weight > cargoCapacity_)
    {
        std::cout << "Нельзя загрузить " << weight
                  << " тонн. Вместимость корабля будет превышена.\n";
        return;
    }

    currentCargo_ += weight;

    std::cout << "На корабль загружено " << weight << " тонн. Всего: "
              << currentCargo_ << "/" << cargoCapacity_ << " тонн.\n";
}

void BaseShip::AddCrewMember(const std::shared_ptr<CrewMember>& crewMember)
{
    if (!crewMember)
    {
        std::cout << "Ошибка: член экипажа не задан.\n";
        return;
    }

    if (!crewMember_.expired())
    {
        std::cout << "На корабле уже есть член экипажа.\n";
        return;
    }

    crewMember_ = crewMember;

    std::cout << crewMember->GetName() << " назначен на корабль "
              << name_ << ".\n";
}

void BaseShip::ShowCrewMember() const
{
    if (const std::shared_ptr<CrewMember> crewMember = crewMember_.lock())
    {
        std::cout << "На корабле " << name_ << " служит: "
                  << crewMember->GetName() << ".\n";
    }
    else
    {
        std::cout << "На корабле " << name_
                  << " нет члена экипажа: объект уже уничтожен или не назначен.\n";
    }
}

const std::string& BaseShip::GetName() const
{
    return name_;
}

int BaseShip::GetCargoCapacity() const
{
    return cargoCapacity_;
}

int BaseShip::GetCurrentCargo() const
{
    return currentCargo_;
}

int BaseShip::GetCrewSize() const
{
    return crewMember_.expired() ? 0 : 1;
}

int BaseShip::GetInventoryNumber() const
{
    return inventoryNumber_;
}

const std::shared_ptr<Port>& BaseShip::GetHomePort() const
{
    return homePort_;
}
