namespace SeaTradeSimulator;

/// <summary>
/// Абстрактный класс иерархии судов (ЛР 4).
/// IComparable&lt;BaseShip&gt; — стандартный интерфейс: суда можно упорядочить по названию.
/// </summary>
public abstract class BaseShip : IComparable<BaseShip>
{
    private static int _lastInventoryNumber;

    public static int NextInventoryNumber() => ++_lastInventoryNumber;

    public string Name { get; }
    public int CargoCapacity { get; }
    public int CurrentCargo { get; private set; }
    public Port HomePort { get; }
    public int InventoryNumber { get; }
    public int FreeCapacity => CargoCapacity - CurrentCargo;

    protected BaseShip(string name, int cargoCapacity, Port homePort)
    {
        // Нарушение здесь — ошибка программиста, а не игровая ситуация: ArgumentException.
        if (string.IsNullOrWhiteSpace(name))
        {
            throw new ArgumentException("у судна должно быть название", nameof(name));
        }
        if (cargoCapacity <= 0)
        {
            throw new ArgumentOutOfRangeException(nameof(cargoCapacity), "грузоподъёмность должна быть положительной");
        }
        HomePort = homePort ?? throw new ArgumentNullException(nameof(homePort), "у судна должен быть порт приписки");

        Name = name;
        CargoCapacity = cargoCapacity;
        InventoryNumber = NextInventoryNumber();
    }

    // Раннее связывание: метод НЕ виртуальный. Наследники скрывают его словом new,
    // выбор версии делается при компиляции по типу переменной.
    public string GetKind() => "судно";

    // Позднее связывание: virtual/abstract + override в наследниках.
    public virtual string Describe() =>
        $"{Header()}, осадка {Draft}, груз {CurrentCargo}/{CargoCapacity}";

    public abstract int Draft { get; }
    public abstract void UseSpecialAction();

    protected string Header() => $"{Name} [порт {HomePort.Name}, №{InventoryNumber}]";

    /// <summary>
    /// Бросает ArgumentOutOfRangeException при tons &lt;= 0 и CargoCapacityExceededException при переполнении.
    /// Проверки идут до изменения CurrentCargo, поэтому при отказе состояние судна не меняется.
    /// </summary>
    public void LoadCargo(int tons)
    {
        if (tons <= 0)
        {
            throw new ArgumentOutOfRangeException(nameof(tons), "масса груза должна быть положительной");
        }
        if (tons > FreeCapacity)
        {
            throw new CargoCapacityExceededException(Name, tons, FreeCapacity);
        }
        CurrentCargo += tons;
    }

    public void UnloadCargo(int tons)
    {
        if (tons < 0 || tons > CurrentCargo)
        {
            throw new ArgumentOutOfRangeException(nameof(tons), $"нельзя выгрузить {tons} т из {CurrentCargo} т");
        }
        CurrentCargo -= tons;
    }

    public void DockAt(Port port) => port.Report(this);

    // Сравнение по названию согласовано с Equals (ЛР 4): равные по Equals дают CompareTo == 0.
    public int CompareTo(BaseShip? other) =>
        other is null ? 1 : string.CompareOrdinal(Name, other.Name);

    public override bool Equals(object? obj) => obj is BaseShip other && Name == other.Name;

    public override int GetHashCode() => Name.GetHashCode();
}
