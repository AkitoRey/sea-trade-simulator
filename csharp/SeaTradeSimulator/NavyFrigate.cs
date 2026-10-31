using System;

namespace SeaTradeSimulator
{
    /// <summary>
    /// Военный фрегат: эскортирует караваны.
    /// </summary>
    public class NavyFrigate : BaseShip
    {
        public NavyFrigate(string name, int cargoCapacity, Port homePort)
            : base(name, cargoCapacity, homePort)
        {
            Console.WriteLine($"NavyFrigate(): {Name}");
        }

        public NavyFrigate(NavyFrigate other)
            : base(other)
        {
            Console.WriteLine($"NavyFrigate(NavyFrigate): {Name}");
        }

        public override int Draft => 4;

        // sealed override: потомки фрегата не смогут изменить это действие.
        public sealed override void UseSpecialAction() =>
            Console.WriteLine($"{Name}: эскортирование — караван под охраной.");

        public override string Describe() =>
            $"{base.Describe()}, фрегат (урон от пиратов снижен)";
    }
}
