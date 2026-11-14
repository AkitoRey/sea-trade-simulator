using System;

namespace SeaTradeSimulator
{
    /// <summary>
    /// Член экипажа: хранит состояние здоровья, болезни и изоляции.
    /// </summary>
    public class CrewMember
    {
        private readonly int _maxHealth;
        private int _health;

        public CrewMember(string name, int maxHealth)
        {
            Name = name;

            if (maxHealth <= 0)
            {
                Console.WriteLine("Ошибка: максимальное здоровье должно быть больше нуля.");
                maxHealth = 1;
            }

            _maxHealth = maxHealth;
            _health = maxHealth;
        }

        public string Name { get; }

        public int Health => _health;

        public int MaxHealth => _maxHealth;

        public bool IsSick { get; private set; }

        public bool IsIsolated { get; private set; }

        public void TakeDamage(int damage)
        {
            if (damage <= 0)
            {
                Console.WriteLine("Ошибка: урон должен быть больше нуля.");
                return;
            }

            // Правило: здоровье не может стать меньше нуля.
            _health = Math.Max(0, _health - damage);

            Console.WriteLine($"{Name} получил урон. Здоровье: {_health}/{_maxHealth}.");
        }

        public void Heal(int amount)
        {
            if (amount <= 0)
            {
                Console.WriteLine("Ошибка: лечение должно быть больше нуля.");
                return;
            }

            // Правило: здоровье не может превысить максимум.
            _health = Math.Min(_maxHealth, _health + amount);

            Console.WriteLine($"{Name} восстановил здоровье. Здоровье: {_health}/{_maxHealth}.");
        }

        public void ContractDisease()
        {
            IsSick = true;

            Console.WriteLine($"{Name} заболел.");
        }

        public void Isolate()
        {
            if (!IsSick)
            {
                Console.WriteLine($"{Name} не болен и не нуждается в изоляции.");
                return;
            }

            IsIsolated = true;

            Console.WriteLine($"{Name} изолирован.");
        }

        public override string ToString() => $"{Name}: {_health}/{_maxHealth}";
    }
}