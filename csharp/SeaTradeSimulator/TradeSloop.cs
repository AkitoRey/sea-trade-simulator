using System;

namespace SeaTradeSimulator
{
    /// <summary>
    /// Торговый шлюп: малая осадка, умеет ускользать из боя.
    /// </summary>
    public class TradeSloop : BaseShip
    {
        // : base(...) сначала создаёт базовую часть объекта.
        public TradeSloop(string name, int cargoCapacity, Port homePort)
            : base(name, cargoCapacity, homePort)
        {
            Console.WriteLine($"TradeSloop(): {Name}");
        }

        // Конструктор копирования.
        public TradeSloop(TradeSloop other)
            : base(other)
        {
            Console.WriteLine($"TradeSloop(TradeSloop): {Name}");
        }

        public override int Draft => 2;

        public override void UseSpecialAction() =>
            Console.WriteLine($"{Name}: ускользание — побег из боя без потерь.");

        // Дополняет метод базового класса.
        public override string Describe() =>
            $"{base.Describe()}, шлюп (мелкие порты доступны)";
    }
}
