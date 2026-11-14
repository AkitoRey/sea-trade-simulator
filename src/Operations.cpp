#include "Operations.h"

#include <iostream>

#include "Exceptions.h"
#include "VoyageLog.h"

void Volley(IArmed& weapon, std::string_view target)
{
    weapon.Fire(target);   // бросит OutOfAmmunition до вывода, если нечем стрелять
    std::cout << "  " << weapon.GetUnitName() << ": залп по цели «" << target
              << "», осталось залпов: " << weapon.GetVolleysLeft() << "\n";
}

bool RepelPirates(IArmed& defender, int pirateStrength)
{
    std::cout << "  " << defender.GetUnitName() << ": пираты силой " << pirateStrength
              << ", наша мощь " << defender.GetFirepower() << "\n";
    if (!defender.HasAmmunition())
    {
        std::cout << "  нечем стрелять, отбиться не удалось\n";
        return false;
    }
    Volley(defender, "пиратский бриг");
    const bool repelled = defender.GetFirepower() >= pirateStrength;
    std::cout << (repelled ? "  атака отбита\n" : "  отбиться не удалось\n");
    return repelled;
}

void MeetPirates(BaseShip& ship, int pirateStrength)
{
    if (auto* armed = dynamic_cast<IArmed*>(&ship))   // nullptr, если судно безоружно
    {
        RepelPirates(*armed, pirateStrength);
    }
    else
    {
        std::cout << "  " << ship.GetName() << ": безоружен, уходим от боя\n";
        ship.UseSpecialAction();
    }
}

void LoadShipment(BaseShip& ship, int tons)
{
    VoyageLog log{ "погрузка: " + ship.GetName() };
    log.Write("запрошено " + std::to_string(tons) + " т");
    ship.LoadCargo(tons);   // может бросить: журнал всё равно закроется в деструкторе
    log.Write("погружено, теперь " + std::to_string(ship.GetCurrentCargo()) + "/"
              + std::to_string(ship.GetCargoCapacity()) + " т");
}

int LoadAcrossFleet(std::vector<std::unique_ptr<BaseShip>>& fleet, int tons)
{
    int remaining = tons;
    for (const auto& ship : fleet)
    {
        if (remaining == 0)
        {
            break;
        }
        try
        {
            LoadShipment(*ship, remaining);
            remaining = 0;
        }
        catch (const CargoCapacityExceeded& error)
        {
            std::cout << "  " << error.what() << " — грузим, сколько влезает\n";
            if (error.GetFree() > 0)
            {
                LoadShipment(*ship, error.GetFree());
                remaining -= error.GetFree();
            }
        }
    }
    return remaining;
}

bool SellAtPort(const Port& port, const CargoLot& lot)
{
    try
    {
        port.Sell(lot);
        return true;
    }
    catch (const ForbiddenGoodsError& error)
    {
        std::cout << "  " << error.what() << " — груз остаётся на борту, ищем другой порт\n";
        return false;
    }
}
