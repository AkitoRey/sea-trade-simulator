using System;

namespace SeaTradeSimulator
{
    /// <summary>
    /// Корабль: управляет грузом и хранит ссылку на члена экипажа.
    /// </summary>
    public class Ship
    {
        // Агрегация: объект создан снаружи, корабль только хранит ссылку на него.
        private CrewMember? _crewMember;

        public Ship(string name, int cargoCapacity)
        {
            Name = name;

            // Правило: грузоподъёмность должна быть положительной.
            if (cargoCapacity <= 0)
            {
                Console.WriteLine("Ошибка: грузоподъёмность должна быть больше нуля.");
                cargoCapacity = 1;
            }

            CargoCapacity = cargoCapacity;

            Console.WriteLine($"Корабль {Name} создан.");
        }

        public string Name { get; }

        public int CargoCapacity { get; }

        public int CurrentCargo { get; private set; }

        public int CrewSize => _crewMember is null ? 0 : 1;

        public void LoadCargo(int weight)
        {
            if (weight <= 0)
            {
                Console.WriteLine("Ошибка: вес груза должен быть больше нуля.");
                return;
            }

            // Правило: суммарный груз не должен превышать грузоподъёмность.
            if (CurrentCargo + weight > CargoCapacity)
            {
                Console.WriteLine($"Нельзя загрузить {weight} тонн. Вместимость корабля будет превышена.");
                return;
            }

            CurrentCargo += weight;

            Console.WriteLine($"На корабль загружено {weight} тонн. Всего: {CurrentCargo}/{CargoCapacity} тонн.");
        }

        public void AddCrewMember(CrewMember crewMember)
        {
            if (_crewMember is not null)
            {
                Console.WriteLine("На корабле уже есть член экипажа.");
                return;
            }

            // Сохраняется ссылка на уже существующий объект: корабль его не создаёт.
            _crewMember = crewMember;

            Console.WriteLine($"{crewMember.Name} назначен на корабль {Name}.");
        }

        public override string ToString() => $"{Name}: груз {CurrentCargo}/{CargoCapacity} тонн, экипаж {CrewSize}";
    }
}