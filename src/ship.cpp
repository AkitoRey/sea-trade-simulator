#include "ship.hpp"
#include "crew_member.hpp"

#include <iostream>

#pragma execution_character_set("utf-8")

// Конструктор корабля.
Ship::Ship(const std::string& name, int cargoCapacity)
    : name_(name),
    cargoCapacity_(cargoCapacity),
    currentCargo_(0)
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
    // crewMember_ - это weak_ptr, он не владеет членом экипажа.
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
void Ship::AddCrewMember(const std::shared_ptr<CrewMember>& crewMember)
{
    if (!crewMember)
    {
        std::cout << "Ошибка: член экипажа не задан.\n";
        return;
    }

    // На корабле может быть только один член экипажа
    // в рамках нашей демонстрационной модели.
    if (!crewMember_.expired())
    {
        std::cout << "На корабле уже есть член экипажа.\n";
        return;
    }

    // weak_ptr запоминает объект, но владельцем не становится.
    crewMember_ = crewMember;

    std::cout << crewMember->GetName()
        << " назначен на корабль "
        << name_ << ".\n";
}

// Вывод информации о члене экипажа.
void Ship::ShowCrewMember() const
{
    // lock() возвращает пустой указатель, если объект уже уничтожен.
    if (std::shared_ptr<CrewMember> crewMember = crewMember_.lock())
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
    if (crewMember_.expired())
    {
        return 0;
    }

    return 1;
}