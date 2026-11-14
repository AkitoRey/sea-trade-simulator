#pragma once

#include "IStormTactic.h"

// Продолжить движение: быстро, но груз страдает сильнее всего.
class PushOnTactic : public IStormTactic
{
public:
    StormOutcome Resolve(int stormPower, int shipDraft) const override;
    std::string GetName() const override;
};

// Изменить маршрут: умеренные потери, теряем двое суток.
class ChangeRouteTactic : public IStormTactic
{
public:
    StormOutcome Resolve(int stormPower, int shipDraft) const override;
    std::string GetName() const override;
};

// Переждать шторм в укрытии: груз почти цел, но уходят сутки.
class WaitOutTactic : public IStormTactic
{
public:
    StormOutcome Resolve(int stormPower, int shipDraft) const override;
    std::string GetName() const override;
};
