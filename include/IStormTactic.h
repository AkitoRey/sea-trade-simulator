#pragma once

#include <string>

// Результат встречи со штормом.
struct StormOutcome
{
    int cargoLostPercent{ 0 };   // какая часть груза потеряна
    int daysLost{ 0 };           // сколько суток потрачено
};

// Интерфейс объекта поведения: как капитан действует в шторм (сценарий 5).
class IStormTactic
{
public:
    virtual ~IStormTactic() = default;

    virtual StormOutcome Resolve(int stormPower, int shipDraft) const = 0;
    virtual std::string GetName() const = 0;
};
