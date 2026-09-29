#pragma once

#include <string>

class CrewMember
{
public:
    CrewMember();
    CrewMember(const std::string& name, int maxHealth);
    ~CrewMember();

    void TakeDamage(int damage);
    void Heal(int amount);
    void ContractDisease();
    void Isolate();

    const std::string& GetName() const;
    int GetHealth() const;
    bool IsSick() const;
    bool IsIsolated() const;

private:
    std::string name_;
    int health_;
    int maxHealth_;
    bool isSick_;
    bool isIsolated_;
};