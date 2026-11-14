#include "StormTactics.h"

#include <algorithm>

namespace
{
    int Percent(int value)
    {
        return std::clamp(value, 0, 100);
    }
}

StormOutcome PushOnTactic::Resolve(int stormPower, int shipDraft) const
{
    return { Percent(stormPower * 12 - shipDraft * 4), 0 };
}

std::string PushOnTactic::GetName() const
{
    return "идти напролом";
}

StormOutcome ChangeRouteTactic::Resolve(int stormPower, int shipDraft) const
{
    return { Percent(stormPower * 5 - shipDraft * 3), 2 };
}

std::string ChangeRouteTactic::GetName() const
{
    return "сменить маршрут";
}

StormOutcome WaitOutTactic::Resolve(int stormPower, int shipDraft) const
{
    return { Percent(stormPower - shipDraft * 2), std::max(1, stormPower / 2) };
}

std::string WaitOutTactic::GetName() const
{
    return "переждать в укрытии";
}
