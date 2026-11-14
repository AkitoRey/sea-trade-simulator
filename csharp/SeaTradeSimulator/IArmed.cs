namespace SeaTradeSimulator;

/// <summary>
/// Интерфейс-контракт «вооружён»: умеет вести огонь.
/// Не всё вооружено (шлюп безоружен), поэтому это не свойство BaseShip.
/// </summary>
public interface IArmed
{
    string UnitName { get; }        // кто стреляет
    int Firepower { get; }          // огневая мощь залпа
    bool HasAmmunition { get; }     // есть ли чем стрелять
    int VolleysLeft { get; }        // сколько залпов осталось
    int Resupply(int volleys);      // пополнить боезапас, вернуть сколько принято
    void Fire(string target);       // дать залп; OutOfAmmunitionException, если нечем
}
