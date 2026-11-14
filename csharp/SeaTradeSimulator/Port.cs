namespace SeaTradeSimulator;

/// <summary>Партия груза: товар, масса, признак контрабанды.</summary>
public record CargoLot(string GoodName, int Tons, bool Contraband);

public class Port
{
    public string Name { get; set; }
    public bool BansContraband { get; }

    public Port(string name, bool bansContraband = false)
    {
        Name = name;
        BansContraband = bansContraband;
    }

    public void Report(BaseShip ship) =>
        Console.WriteLine($"Порт {Name}: принят корабль {ship.Name}.");

    /// <summary>Только проверка правила 9, без вывода: бросает ForbiddenGoodsException.</summary>
    public void CheckAcceptable(CargoLot lot)
    {
        if (lot.Contraband && BansContraband)
        {
            throw new ForbiddenGoodsException(lot.GoodName, Name);
        }
    }

    /// <summary>Бросает ForbiddenGoodsException, если товар запрещён в этом порту.</summary>
    public void Sell(CargoLot lot)
    {
        CheckAcceptable(lot);
        Console.WriteLine($"  порт {Name}: куплено «{lot.GoodName}», {lot.Tons} т");
    }
}
