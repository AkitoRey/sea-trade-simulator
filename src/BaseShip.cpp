#include "BaseShip.h"

#include <iostream>
#include <stdexcept>
#include <utility>

#include "Exceptions.h"

namespace
{
    // Проверки аргументов конструктора. Нарушение здесь — ошибка программиста,
    // поэтому std::invalid_argument (наследник std::logic_error).
    int CheckedCapacity(int capacity)
    {
        if (capacity <= 0)
        {
            throw std::invalid_argument{ "грузоподъёмность должна быть положительной" };
        }
        return capacity;
    }

    std::string CheckedName(std::string_view name)
    {
        if (name.empty())
        {
            throw std::invalid_argument{ "у судна должно быть название" };
        }
        return std::string{ name };
    }

    std::shared_ptr<Port> CheckedPort(std::shared_ptr<Port> port)
    {
        if (!port)
        {
            throw std::invalid_argument{ "у судна должен быть порт приписки" };
        }
        return port;
    }
}

BaseShip::BaseShip(std::string_view name, int cargoCapacity, std::shared_ptr<Port> homePort)
    : name_{ CheckedName(name) }
    , cargoCapacity_{ CheckedCapacity(cargoCapacity) }
    , currentCargo_{ 0 }
    , homePort_{ CheckedPort(std::move(homePort)) }
    , inventoryNumber_{ NextInventoryNumber() }
{}

BaseShip::BaseShip(const BaseShip& other)
    : name_{ other.name_ }
    , cargoCapacity_{ other.cargoCapacity_ }
    , currentCargo_{ other.currentCargo_ }
    , homePort_{ std::make_shared<Port>(*other.homePort_) }   // свой порт: глубокая копия
    , inventoryNumber_{ NextInventoryNumber() }               // копия — новое судно
{}

BaseShip::BaseShip(BaseShip&& other) noexcept
    : name_{ std::move(other.name_) }
    , cargoCapacity_{ other.cargoCapacity_ }
    , currentCargo_{ other.currentCargo_ }
    , homePort_{ std::move(other.homePort_) }
    , inventoryNumber_{ other.inventoryNumber_ }              // то же судно
{}

BaseShip& BaseShip::operator=(const BaseShip& other)
{
    if (this == &other)
    {
        return *this;
    }
    name_ = other.name_;
    cargoCapacity_ = other.cargoCapacity_;
    currentCargo_ = other.currentCargo_;
    homePort_ = std::make_shared<Port>(*other.homePort_);
    return *this;
}

int BaseShip::NextInventoryNumber()
{
    return ++lastInventoryNumber_;
}

std::string BaseShip::Header() const
{
    return name_ + " [порт " + homePort_->GetName() + ", №" + std::to_string(inventoryNumber_) + "]";
}

std::string BaseShip::GetKind() const
{
    return "судно";
}

std::string BaseShip::Describe() const
{
    return Header() + ", осадка " + std::to_string(GetDraft()) + ", груз "
           + std::to_string(currentCargo_) + "/" + std::to_string(cargoCapacity_);
}

void BaseShip::LoadCargo(int tons)
{
    if (tons <= 0)
    {
        throw std::invalid_argument{ "масса груза должна быть положительной" };
    }
    if (tons > GetFreeCapacity())
    {
        throw CargoCapacityExceeded{ name_, tons, GetFreeCapacity() };
    }
    currentCargo_ += tons;   // все проверки позади: состояние меняется последним
}

void BaseShip::UnloadCargo(int tons)
{
    if (tons < 0 || tons > currentCargo_)
    {
        throw std::invalid_argument{ "нельзя выгрузить " + std::to_string(tons) + " т из "
                                     + std::to_string(currentCargo_) + " т" };
    }
    currentCargo_ -= tons;
}

void BaseShip::DockAt(const Port& port) const
{
    port.Report(*this);
}

const std::string& BaseShip::GetName() const { return name_; }
int BaseShip::GetCargoCapacity() const { return cargoCapacity_; }
int BaseShip::GetCurrentCargo() const { return currentCargo_; }
int BaseShip::GetFreeCapacity() const { return cargoCapacity_ - currentCargo_; }
int BaseShip::GetInventoryNumber() const { return inventoryNumber_; }
std::shared_ptr<Port> BaseShip::GetHomePort() const { return homePort_; }
