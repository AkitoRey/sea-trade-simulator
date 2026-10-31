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

// По умолчанию: делегирует конструктору с параметрами.
BaseShip::BaseShip()
    : BaseShip{ "Без имени", 1, std::make_shared<Port>("Без порта") }
{
    std::cout << "(конструктор по умолчанию)\n";
}

// Копирования (глубокая копия): новый корабль получает свой
// инвентарный номер и СВОЙ порт, экипаж не копируется.
BaseShip::BaseShip(const BaseShip& other)
    : inventoryNumber_{ NextInventoryNumber() }
    , name_{ other.name_ }
    , cargoCapacity_{ other.cargoCapacity_ }
    , currentCargo_{ other.currentCargo_ }
    , homePort_{ std::make_shared<Port>(*other.homePort_) }
{
    std::cout << "BaseShip(const BaseShip&): " << name_ << "\n";
}

// Перемещения: данные забираются, у источника остаётся пустая оболочка.
BaseShip::BaseShip(BaseShip&& other) noexcept
    : inventoryNumber_{ other.inventoryNumber_ }
    , name_{ std::move(other.name_) }
    , cargoCapacity_{ other.cargoCapacity_ }
    , currentCargo_{ other.currentCargo_ }
    , crewMember_{ std::move(other.crewMember_) }
    , homePort_{ std::move(other.homePort_) }
{
    std::cout << "BaseShip(BaseShip&&): " << name_ << "\n";
}

// Присваивание копированием согласовано с конструктором копирования:
// порт тоже копируется глубоко. Номер и экипаж остаются своими.
BaseShip& BaseShip::operator=(const BaseShip& other)
{
    if (this == &other)
    {
        std::cout << "Самоприсваивание: ничего не делаем.\n";
        return *this;
    }

    name_ = other.name_;
    cargoCapacity_ = other.cargoCapacity_;
    currentCargo_ = other.currentCargo_;
    homePort_ = std::make_shared<Port>(*other.homePort_);

    std::cout << "BaseShip::operator=(const BaseShip&): " << name_ << "\n";
    return *this;
}

BaseShip::~BaseShip()
{
    std::cout << "~BaseShip: " << name_ << "\n";
}

int BaseShip::NextInventoryNumber()
{
    return ++lastInventoryNumber_;
}

int BaseShip::GetIssuedNumbersCount()
{
    return lastInventoryNumber_;
}

// Поиск подстроки: find возвращает позицию или npos.
bool BaseShip::NameContains(std::string_view part) const
{
    return name_.find(part) != std::string::npos;
}

// Первое слово названия: substr до первого пробела.
// Пробел занимает один байт, поэтому буква в UTF-8 не разрезается.
std::string BaseShip::FirstWordOfName() const
{
    const std::size_t space = name_.find(' ');
    if (space == std::string::npos)
    {
        return name_;
    }

    return name_.substr(0, space);
}

// Передача текущего объекта другому объекту.
void BaseShip::DockAt(const Port& port) const
{
    port.Report(*this);
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
