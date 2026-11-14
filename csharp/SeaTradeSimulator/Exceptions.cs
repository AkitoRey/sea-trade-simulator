namespace SeaTradeSimulator;

/// <summary>Базовое исключение предметной области: любое нарушение правил игры.</summary>
public class SeaTradeException : Exception
{
    public SeaTradeException(string message) : base(message) { }
}

/// <summary>Правило 1: общая масса груза не может превышать грузоподъёмность.</summary>
public class CargoCapacityExceededException : SeaTradeException
{
    public int Requested { get; }
    public int Free { get; }

    public CargoCapacityExceededException(string shipName, int requested, int free)
        : base($"«{shipName}»: груз {requested} т не помещается, свободно {free} т")
    {
        Requested = requested;
        Free = free;
    }
}

/// <summary>Правило 9: запрещённый товар нельзя легально продать в порту, где его продажа запрещена.</summary>
public class ForbiddenGoodsException : SeaTradeException
{
    public ForbiddenGoodsException(string goodName, string portName)
        : base($"товар «{goodName}» запрещён к продаже в порту {portName}") { }
}

/// <summary>Боезапас ограничен: стрелять без боезапаса нельзя.</summary>
public class OutOfAmmunitionException : SeaTradeException
{
    public OutOfAmmunitionException(string unitName)
        : base($"{unitName}: боезапас исчерпан") { }
}

/// <summary>Судно с такой осадкой не войдёт в мелкий порт.</summary>
public class PortTooShallowException : SeaTradeException
{
    public PortTooShallowException(string shipName, int draft, string portName, int depth)
        : base($"{shipName} (осадка {draft}) не войдёт в порт {portName}: глубина {depth}") { }
}

/// <summary>Правило 3: провизия расходуется в плавании; без запаса выходить нельзя.</summary>
public class NotEnoughProvisionsException : SeaTradeException
{
    public NotEnoughProvisionsException(int need, int have)
        : base($"для перехода нужно {need} ед. провизии, на борту {have}") { }
}
