namespace SeaTradeSimulator;

/// <summary>
/// Судовой журнал. IDisposable: оператор using вызывает Dispose при любом выходе из блока,
/// в том числе при исключении (аналог RAII из C++).
/// </summary>
public class VoyageLog : IDisposable
{
    private readonly string _title;

    public VoyageLog(string title)
    {
        _title = title;
        Console.WriteLine($"  [журнал «{_title}» открыт]");
    }

    public void Write(string record) => Console.WriteLine($"  [журнал] {record}");

    public void Dispose() => Console.WriteLine($"  [журнал «{_title}» закрыт]");
}

public static class Operations
{
    /// <summary>Работает ТОЛЬКО через интерфейс: не знает, фрегат перед ним, галеон или батарея.</summary>
    public static bool RepelPirates(IArmed defender, int pirateStrength)
    {
        Console.WriteLine($"  {defender.UnitName}: пираты силой {pirateStrength}, наша мощь {defender.Firepower}");
        if (!defender.HasAmmunition)
        {
            Console.WriteLine("  нечем стрелять, отбиться не удалось");
            return false;
        }
        defender.Fire("пиратский бриг");
        bool repelled = defender.Firepower >= pirateStrength;
        Console.WriteLine(repelled ? "  атака отбита" : "  отбиться не удалось");
        return repelled;
    }

    /// <summary>Проверка типа на границе: «умеет ли это судно воевать вообще».</summary>
    public static void MeetPirates(BaseShip ship, int pirateStrength)
    {
        if (ship is IArmed armed)   // is + сразу переменная нужного типа
        {
            RepelPirates(armed, pirateStrength);
        }
        else
        {
            Console.WriteLine($"  {ship.Name}: безоружен, уходим от боя");
            ship.UseSpecialAction();
        }
    }

    /// <summary>Погрузка с журналом. Исключения пропускает наверх: что делать, решает вызывающий.</summary>
    public static void LoadShipment(BaseShip ship, int tons)
    {
        using var log = new VoyageLog($"погрузка: {ship.Name}");
        log.Write($"запрошено {tons} т");
        ship.LoadCargo(tons);   // может бросить: журнал всё равно закроется
        log.Write($"погружено, теперь {ship.CurrentCargo}/{ship.CargoCapacity} т");
    }

    /// <summary>
    /// Распределить груз по флоту. Перехватывает CargoCapacityExceededException: именно здесь
    /// понятно, что делать — загрузить, сколько влезает, и перейти к следующему судну.
    /// Возвращает массу, которую не удалось погрузить.
    /// </summary>
    public static int LoadAcrossFleet(List<BaseShip> fleet, int tons)
    {
        int remaining = tons;
        foreach (BaseShip ship in fleet)
        {
            if (remaining == 0)
            {
                break;
            }
            try
            {
                LoadShipment(ship, remaining);
                remaining = 0;
            }
            catch (CargoCapacityExceededException error)
            {
                Console.WriteLine($"  {error.Message} — грузим, сколько влезает");
                if (error.Free > 0)
                {
                    LoadShipment(ship, error.Free);
                    remaining -= error.Free;
                }
            }
        }
        return remaining;
    }

    /// <summary>Продажа в порту. Перехватывает ForbiddenGoodsException: груз остаётся на борту.</summary>
    public static bool SellAtPort(Port port, CargoLot lot)
    {
        try
        {
            port.Sell(lot);
            return true;
        }
        catch (ForbiddenGoodsException error)
        {
            Console.WriteLine($"  {error.Message} — груз остаётся на борту, ищем другой порт");
            return false;
        }
        finally
        {
            // Выполняется при любом исходе: и после успеха, и после исключения.
            Console.WriteLine($"  [касса: операция «{lot.GoodName}» в порту {port.Name} закрыта]");
        }
    }
}
