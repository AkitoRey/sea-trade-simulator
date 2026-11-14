namespace SeaTradeSimulator;

/// <summary>Результат встречи со штормом.</summary>
public readonly record struct StormOutcome(int CargoLostPercent, int DaysLost);

/// <summary>Интерфейс объекта поведения: как капитан действует в шторм (сценарий 5).</summary>
public interface IStormTactic
{
    string Name { get; }
    StormOutcome Resolve(int stormPower, int shipDraft);
}

/// <summary>Продолжить движение: быстро, но груз страдает сильнее всего.</summary>
public class PushOnTactic : IStormTactic
{
    public string Name => "идти напролом";

    public StormOutcome Resolve(int stormPower, int shipDraft) =>
        new(Math.Clamp(stormPower * 12 - shipDraft * 4, 0, 100), 0);
}

/// <summary>Изменить маршрут: умеренные потери, теряем двое суток.</summary>
public class ChangeRouteTactic : IStormTactic
{
    public string Name => "сменить маршрут";

    public StormOutcome Resolve(int stormPower, int shipDraft) =>
        new(Math.Clamp(stormPower * 5 - shipDraft * 3, 0, 100), 2);
}

/// <summary>Переждать шторм в укрытии: груз почти цел, но уходят сутки.</summary>
public class WaitOutTactic : IStormTactic
{
    public string Name => "переждать в укрытии";

    public StormOutcome Resolve(int stormPower, int shipDraft) =>
        new(Math.Clamp(stormPower - shipDraft * 2, 0, 100), Math.Max(1, stormPower / 2));
}

/// <summary>
/// Капитан поручает решение объекту поведения. Свойство Tactic можно менять во время работы:
/// динамическое конфигурирование.
/// </summary>
public class Captain
{
    private IStormTactic _tactic;

    public string Name { get; }

    public IStormTactic Tactic
    {
        get => _tactic;
        set => _tactic = value ?? throw new ArgumentNullException(nameof(value), "у капитана должна быть тактика");
    }

    public Captain(string name, IStormTactic tactic)
    {
        Name = name;
        _tactic = tactic ?? throw new ArgumentNullException(nameof(tactic), "у капитана должна быть тактика");
    }

    /// <summary>Только расчёт: что даст шторм при текущей тактике (судно не меняется).</summary>
    public StormOutcome Evaluate(BaseShip ship, int stormPower) =>
        _tactic.Resolve(stormPower, ship.Draft);

    public StormOutcome FaceStorm(BaseShip ship, int stormPower)
    {
        // Как именно действовать — решает объект поведения.
        StormOutcome outcome = Evaluate(ship, stormPower);
        int lostTons = ship.CurrentCargo * outcome.CargoLostPercent / 100;
        ship.UnloadCargo(lostTons);

        Console.WriteLine($"  капитан {Name} («{_tactic.Name}»), шторм {stormPower}: " +
                          $"потеряно {outcome.CargoLostPercent}% груза ({lostTons} т), " +
                          $"потеряно суток: {outcome.DaysLost}, осталось на борту {ship.CurrentCargo} т");
        return outcome;
    }
}
