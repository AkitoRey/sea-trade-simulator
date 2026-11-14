#include "Captain.h"

#include <iostream>
#include <stdexcept>
#include <utility>

Captain::Captain(std::string_view name, std::unique_ptr<IStormTactic> tactic)
    : name_{ name }
    , tactic_{ std::move(tactic) }
{
    if (!tactic_)
    {
        throw std::invalid_argument{ "у капитана должна быть тактика" };
    }
}

void Captain::SetTactic(std::unique_ptr<IStormTactic> tactic)
{
    if (!tactic)
    {
        throw std::invalid_argument{ "у капитана должна быть тактика" };
    }
    tactic_ = std::move(tactic);
}

const std::string& Captain::GetName() const
{
    return name_;
}

std::string Captain::GetTacticName() const
{
    return tactic_->GetName();
}

StormOutcome Captain::Evaluate(const BaseShip& ship, int stormPower) const
{
    // Как именно действовать — решает объект поведения, капитан не знает его класса.
    return tactic_->Resolve(stormPower, ship.GetDraft());
}

StormOutcome Captain::FaceStorm(BaseShip& ship, int stormPower) const
{
    const StormOutcome outcome = Evaluate(ship, stormPower);
    const int lostTons = ship.GetCurrentCargo() * outcome.cargoLostPercent / 100;
    ship.UnloadCargo(lostTons);

    std::cout << "  капитан " << name_ << " («" << tactic_->GetName() << "»), шторм " << stormPower
              << ": потеряно " << outcome.cargoLostPercent << "% груза (" << lostTons
              << " т), потеряно суток: " << outcome.daysLost
              << ", осталось на борту " << ship.GetCurrentCargo() << " т\n";
    return outcome;
}
