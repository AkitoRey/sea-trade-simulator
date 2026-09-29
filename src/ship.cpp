#include "ship.hpp"
#include "crew_member.hpp"

#include <iostream>

#pragma execution_character_set("utf-8")

// Конструктор корабля.
Ship::Ship(const std::string& name, int cargoCapacity)
    : name_(name),
    cargoCapacity_(cargoCapacity),
    currentCargo_(0),
    crewMember_(nullptr)
{
    // Грузоподъёмность должна быть положительной.
    if (cargoCapacity_ <= 0)
    {
        std::cout << "Ошибка: грузоподъёмность должна быть больше нуля.\n";
        cargoCapacity_ = 1;
    }

    std::cout << "Корабль " << name_ << " создан.\n";
}

// Деструктор корабля.
Ship::~Ship()
{
    std::cout << "Корабль " << name_ << " уничтожен.\n";

    // ВАЖНО:
    // crewMember_ не удаляется через delete.
    // Член экипажа существует независимо от корабля.
    // Это демонстрирует агрегацию.
}

// Загрузка груза.
void Ship::LoadCargo(int weight)
{
    // Вес груза должен быть положительным.
    if (weight <= 0)
    {
        std::cout << "Ошибка: вес груза должен быть больше нуля.\n";
        return;
    }

    // Нельзя превысить грузоподъёмность.
    if (currentCargo_ + weight > cargoCapacity_)
    {
        std::cout << "Нельзя загрузить " << weight
            << " тонн. Вместимость корабля будет превышена.\n";
        return;
    }

    currentCargo_ += weight;

    std::cout << "На корабль загружено " << weight
        << " тонн. Всего: "
        << currentCargo_ << "/" << cargoCapacity_
        << " тонн.\n";
}

// Добавление члена экипажа.
void Ship::AddCrewMember(CrewMember& crewMember)
{
    // На корабле может быть только один член экипажа
    // в рамках нашей демонстрационной модели.
    if (crewMember_ != nullptr)
    {
        std::cout << "На корабле уже есть член экипажа.\n";
        return;
    }

    // Сохраняем адрес существующего объекта.
    // Ship НЕ становится его владельцем.
    crewMember_ = &crewMember;

    std::cout << crewMember.GetName()
        << " назначен на корабль "
        << name_ << ".\n";
}

const std::string& Ship::GetName() const
{
    return name_;
}

int Ship::GetCargoCapacity() const
{
    return cargoCapacity_;
}

int Ship::GetCurrentCargo() const
{
    return currentCargo_;
}

int Ship::GetCrewSize() const
{
    if (crewMember_ != nullptr)
    {
        return 1;
    }

    return 0;
}