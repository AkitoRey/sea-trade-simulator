namespace SeaTradeSimulator;

/// <summary>Торговый шлюп: БЕЗОРУЖНЫЙ, интерфейс IArmed не реализует.</summary>
public class TradeSloop : BaseShip
{
    public TradeSloop(string name, int cargoCapacity, Port homePort)
        : base(name, cargoCapacity, homePort) { }

    public new string GetKind() => "шлюп";   // скрывает BaseShip.GetKind (раннее связывание)

    public override int Draft => 2;

    public override string Describe() =>
        $"{base.Describe()}, шлюп (мелкие порты доступны, оружия нет)";

    public override void UseSpecialAction() =>
        Console.WriteLine($"  {Name}: ускользание — побег из боя без потерь.");
}

/// <summary>
/// Военный фрегат. Один базовый класс (BaseShip) плюс интерфейс IArmed:
/// в C# множественное наследование классов запрещено, интерфейсов — сколько угодно.
/// </summary>
public class NavyFrigate : BaseShip, IArmed
{
    private const int Cannons = 18;
    private const int PowerPerCannon = 3;

    private int _volleysLeft;
    private readonly int _maxVolleys;

    public NavyFrigate(string name, int cargoCapacity, Port homePort, int volleys = 6)
        : base(name, cargoCapacity, homePort)
    {
        _volleysLeft = volleys;
        _maxVolleys = volleys;
    }

    public new string GetKind() => "фрегат";

    // BaseShip
    public override int Draft => 4;

    public override string Describe() =>
        $"{base.Describe()}, фрегат (урон от пиратов снижен), залпов: {_volleysLeft}";

    public override void UseSpecialAction() =>
        Console.WriteLine($"  {Name}: эскортирование — караван под охраной.");

    // IArmed
    public string UnitName => $"Фрегат «{Name}»";
    public int Firepower => Cannons * PowerPerCannon;
    public bool HasAmmunition => _volleysLeft > 0;
    public int VolleysLeft => _volleysLeft;

    public int Resupply(int volleys)
    {
        int added = Math.Clamp(volleys, 0, _maxVolleys - _volleysLeft);
        _volleysLeft += added;
        return added;
    }

    public void Fire(string target)
    {
        if (_volleysLeft == 0)
        {
            throw new OutOfAmmunitionException(UnitName);
        }
        _volleysLeft--;
        Console.WriteLine($"  {UnitName}: залп по цели «{target}», осталось залпов: {_volleysLeft}");
    }
}

/// <summary>Тяжёлый галеон: наследников нет (sealed), но он вооружён.</summary>
public sealed class Galleon : BaseShip, IArmed
{
    private const int Cannons = 32;
    private const int PowerPerCannon = 4;

    private int _volleysLeft;
    private readonly int _maxVolleys;

    public Galleon(string name, int cargoCapacity, Port homePort, int volleys = 10)
        : base(name, cargoCapacity, homePort)
    {
        _volleysLeft = volleys;
        _maxVolleys = volleys;
    }

    public new string GetKind() => "галеон";

    // BaseShip
    public override int Draft => 7;

    // Базовое описание не вызываем: галеон описывает себя целиком.
    public override string Describe() =>
        $"Галеон «{Name}», осадка {Draft} (только глубокие порты), залпов: {_volleysLeft}";

    public override void UseSpecialAction() =>
        Console.WriteLine($"  {Name}: залповый огонь и абордажный захват.");

    // IArmed
    public string UnitName => $"Галеон «{Name}»";
    public int Firepower => Cannons * PowerPerCannon;
    public bool HasAmmunition => _volleysLeft > 0;
    public int VolleysLeft => _volleysLeft;

    public int Resupply(int volleys)
    {
        int added = Math.Clamp(volleys, 0, _maxVolleys - _volleysLeft);
        _volleysLeft += added;
        return added;
    }

    public void Fire(string target)
    {
        if (_volleysLeft == 0)
        {
            throw new OutOfAmmunitionException(UnitName);
        }
        _volleysLeft--;
        Console.WriteLine($"  {UnitName}: залп по цели «{target}», осталось залпов: {_volleysLeft}");
    }
}

/// <summary>Береговая батарея: не судно, но тоже «вооружена».</summary>
public class CoastalBattery : IArmed
{
    private const int PowerPerCannon = 5;

    private readonly string _name;
    private readonly int _cannons;
    private int _volleysLeft;
    private readonly int _maxVolleys;

    public CoastalBattery(string name, int cannons, int volleys)
    {
        _name = name;
        _cannons = cannons;
        _volleysLeft = volleys;
        _maxVolleys = volleys;
    }

    public string UnitName => $"Батарея «{_name}»";
    public int Firepower => _cannons * PowerPerCannon;
    public bool HasAmmunition => _volleysLeft > 0;
    public int VolleysLeft => _volleysLeft;

    public int Resupply(int volleys)
    {
        int added = Math.Clamp(volleys, 0, _maxVolleys - _volleysLeft);
        _volleysLeft += added;
        return added;
    }

    public void Fire(string target)
    {
        if (_volleysLeft == 0)
        {
            throw new OutOfAmmunitionException(UnitName);
        }
        _volleysLeft--;
        Console.WriteLine($"  {UnitName}: залп по цели «{target}», осталось залпов: {_volleysLeft}");
    }
}
