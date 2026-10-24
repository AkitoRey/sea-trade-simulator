using System;

namespace SeaTradeSimulator
{
    /// <summary>
    /// Торговая компания: владеет своим кораблём (композиция).
    /// </summary>
    public class TradingCompany
    {
        // Композиция: корабль создаётся самой компанией и наружу не отдаётся.
        private readonly Ship _ship;

        public TradingCompany(string name, string shipName, int cargoCapacity)
        {
            Name = name;
            _ship = new Ship(shipName, cargoCapacity);

            Console.WriteLine($"Торговая компания {Name} создана.");
        }

        public string Name { get; }

        public void LoadCargo(int weight) => _ship.LoadCargo(weight);

        public void ShowInfo()
        {
            Console.WriteLine();
            Console.WriteLine("Информация о торговой компании:");
            Console.WriteLine($"Компания: {Name}");
            Console.WriteLine($"Корабль: {_ship.Name}");
            Console.WriteLine($"Груз: {_ship.CurrentCargo}/{_ship.CargoCapacity} тонн");
        }

        public override string ToString() => $"{Name} (корабль {_ship.Name})";
    }
}