using System;

namespace SeaTradeSimulator
{
    /// <summary>
    /// Торговая компания: владеет своим кораблём (композиция).
    /// </summary>
    public class TradingCompany
    {
        // BaseShip абстрактный, поэтому корабль передаётся готовым.
        // Компания единственный владелец и наружу его не отдаёт.
        private readonly BaseShip _ship;

        public TradingCompany(string name, BaseShip ship)
        {
            Name = name;
            _ship = ship;

            Console.WriteLine($"Торговая компания {Name} создана.");
        }

        public string Name { get; }

        public void LoadCargo(int weight) => _ship.LoadCargo(weight);

        public void ShowInfo()
        {
            Console.WriteLine();
            Console.WriteLine("Информация о торговой компании:");
            Console.WriteLine($"Компания: {Name}");
            Console.WriteLine($"Корабль: {_ship.Describe()}");
            Console.WriteLine($"Груз: {_ship.CurrentCargo}/{_ship.CargoCapacity} тонн");
        }

        public override string ToString() => $"{Name} (корабль {_ship.Name})";
    }
}
