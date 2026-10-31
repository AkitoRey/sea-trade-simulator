using System;

namespace SeaTradeSimulator
{
    /// <summary>
    /// Тяжёлый галеон. sealed: подклассов у галеона не будет.
    /// </summary>
    public sealed class Galleon : BaseShip
    {
        public Galleon(string name, int cargoCapacity, Port homePort)
            : base(name, cargoCapacity, homePort)
        {
            Console.WriteLine($"Galleon(): {Name}");
        }

        // this(...) вызывает другой конструктор того же класса.
        public Galleon(string name, Port homePort)
            : this(name, 200, homePort)
        {
        }

        public Galleon(Galleon other)
            : base(other)
        {
            Console.WriteLine($"Galleon(Galleon): {Name}");
        }

        public override int Draft => 7;

        public override void UseSpecialAction() =>
            Console.WriteLine($"{Name}: залповый огонь и абордажный захват.");

        // Заменяет метод полностью, без вызова базового.
        public override string Describe() =>
            $"Галеон «{Name}», осадка {Draft} (только глубокие порты)";
    }
}
