using System;

namespace SeaTradeSimulator
{
    /// <summary>
    /// Порт приписки: вложенный объект корабля.
    /// На нём видна разница между поверхностной и глубокой копией.
    /// </summary>
    public class Port
    {
        public Port(string name)
        {
            Name = name;
        }

        public string Name { get; set; }

        // this: корабль передаёт сам себя порту.
        public void Report(BaseShip ship) =>
            Console.WriteLine($"Порт {Name}: принят корабль {ship.Name}.");
    }
}
