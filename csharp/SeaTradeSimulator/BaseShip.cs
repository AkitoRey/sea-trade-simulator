using System;

namespace SeaTradeSimulator
{
    /// <summary>
    /// Абстрактный базовый класс судов.
    /// </summary>
    public abstract class BaseShip
    {
        // Статическое поле: один счётчик на все суда.
        private static int _lastNumber;

        // Агрегация из ЛР 3: объект создан снаружи, корабль только хранит ссылку.
        private CrewMember? _crewMember;

        // Конструктор доступен только наследникам: объект BaseShip создать нельзя.
        protected BaseShip(string name, int cargoCapacity, Port homePort)
        {
            InventoryNumber = NextInventoryNumber();
            Name = name;

            // Правило: грузоподъёмность должна быть положительной.
            if (cargoCapacity <= 0)
            {
                Console.WriteLine("Ошибка: грузоподъёмность должна быть больше нуля.");
                cargoCapacity = 1;
            }

            CargoCapacity = cargoCapacity;
            HomePort = homePort;

            Console.WriteLine($"BaseShip(): {Name}");
        }

        // Конструктор копирования: глубокая копия, у копии свой порт.
        protected BaseShip(BaseShip other)
        {
            InventoryNumber = NextInventoryNumber();
            Name = other.Name;
            CargoCapacity = other.CargoCapacity;
            CurrentCargo = other.CurrentCargo;
            HomePort = new Port(other.HomePort.Name);

            Console.WriteLine($"BaseShip(BaseShip): {Name}");
        }

        public string Name { get; }

        public int CargoCapacity { get; }

        public int CurrentCargo { get; private set; }

        public int InventoryNumber { get; private set; }

        public Port HomePort { get; private set; }

        public int CrewSize => _crewMember is null ? 0 : 1;

        // Абстрактные члены: у каждого типа судна своя реализация.
        public abstract int Draft { get; }

        public abstract void UseSpecialAction();

        // Есть реализация, наследник может её дополнить или заменить.
        public virtual string Describe() =>
            $"{Header()}, осадка {Draft}, груз {CurrentCargo}/{CargoCapacity}";

        // Статические члены.
        public static int NextInventoryNumber() => ++_lastNumber;

        public static int IssuedNumbersCount => _lastNumber;

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

            _crewMember = crewMember;

            Console.WriteLine($"{crewMember.Name} назначен на корабль {Name}.");
        }

        // this: передача текущего объекта другому объекту.
        public void DockAt(Port port) => port.Report(this);

        // Поверхностная копия: MemberwiseClone копирует поля как есть,
        // ссылки на порт и экипаж остаются общими с оригиналом.
        public BaseShip ShallowCopy() => (BaseShip)MemberwiseClone();

        // Глубокая копия: клонируем поля, затем заменяем общие ссылки своими.
        public BaseShip DeepCopy()
        {
            var copy = (BaseShip)MemberwiseClone();

            copy.InventoryNumber = NextInventoryNumber();
            copy.HomePort = new Port(HomePort.Name);
            copy._crewMember = null;

            return copy;
        }

        // Сравнение по содержимому: суда равны, если совпадает название.
        public override bool Equals(object? obj) =>
            obj is BaseShip other && Name == other.Name;

        public override int GetHashCode() => Name.GetHashCode();

        public override string ToString() => Describe();

        // protected: нужен наследникам, остальному коду не нужен.
        protected string Header() => $"{Name} [порт {HomePort.Name}, №{InventoryNumber}]";
    }
}
