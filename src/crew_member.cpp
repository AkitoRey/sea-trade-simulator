#include "crew_member.hpp"

#include <iostream>

CrewMember::CrewMember()
    : name_("Без имени"),
    health_(100),
    maxHealth_(100),
    isSick_(false),
    isIsolated_(false)
{
}
CrewMember::CrewMember(const std::string& name, int maxHealth)
    : name_(name),
      health_(maxHealth),
      maxHealth_(maxHealth),
      isSick_(false),
      isIsolated_(false)
{
    if (maxHealth_ <= 0)
    {
        std::cout << "Ошибка: максимальное здоровье должно быть больше нуля.\n";
        maxHealth_ = 1;
        health_ = 1;
    }
}

CrewMember::~CrewMember()
{
    std::cout << "Член экипажа " << name_ << " уничтожен.\n";
}

void CrewMember::TakeDamage(int damage)
{
    if (damage <= 0)
    {
        std::cout << "Ошибка: урон должен быть больше нуля.\n";
        return;
    }

    health_ -= damage;

    if (health_ < 0)
    {
        health_ = 0;
    }

    std::cout << name_ << " получил урон. Здоровье: "
              << health_ << "/" << maxHealth_ << ".\n";
}

void CrewMember::Heal(int amount)
{
    if (amount <= 0)
    {
        std::cout << "Ошибка: лечение должно быть больше нуля.\n";
        return;
    }

    health_ += amount;

    if (health_ > maxHealth_)
    {
        health_ = maxHealth_;
    }

    std::cout << name_ << " восстановил здоровье. Здоровье: "
              << health_ << "/" << maxHealth_ << ".\n";
}

void CrewMember::ContractDisease()
{
    isSick_ = true;

    std::cout << name_ << " заболел.\n";
}

void CrewMember::Isolate()
{
    if (!isSick_)
    {
        std::cout << name_
                  << " не болен и не нуждается в изоляции.\n";
        return;
    }

    isIsolated_ = true;

    std::cout << name_ << " изолирован.\n";
}

const std::string& CrewMember::GetName() const
{
    return name_;
}

int CrewMember::GetHealth() const
{
    return health_;
}

bool CrewMember::IsSick() const
{
    return isSick_;
}

bool CrewMember::IsIsolated() const
{
    return isIsolated_;
}