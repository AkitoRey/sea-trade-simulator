using System;
using System.Text;

namespace SeaTradeSimulator
{
    /// <summary>
    /// Курсовой проект: игровой симулятор управления торговым судном
    /// и морскими маршрутами. Выполнила: Соколова Д. С., группа ПИ-51.
    /// </summary>
    public static class Program
    {
        public static void Main()
        {
            Console.OutputEncoding = Encoding.UTF8;

            Console.WriteLine("=== Лабораторная работа 3 (C#) ===");
            Console.WriteLine();

            // 1. Член экипажа
            Console.WriteLine("=== Член экипажа ===");

            var sailor = new CrewMember("Иван", 100);

            Console.WriteLine($"Имя: {sailor.Name}");
            Console.WriteLine($"Здоровье: {sailor.Health}/{sailor.MaxHealth}");

            sailor.TakeDamage(30);
            sailor.Heal(10);
            sailor.ContractDisease();
            sailor.Isolate();
            Console.WriteLine();

            // 2. Корабль и агрегация
            Console.WriteLine("=== Корабль ===");

            var ship = new Ship("Морская звезда", 100);

            ship.AddCrewMember(sailor);
            ship.LoadCargo(60);
            ship.LoadCargo(30);

            // Попытка превысить грузоподъёмность.
            ship.LoadCargo(20);
            Console.WriteLine();

            // 3. Проверка агрегации
            Console.WriteLine("=== Проверка агрегации ===");

            {
                var temporaryShip = new Ship("Временный корабль", 50);

                temporaryShip.AddCrewMember(sailor);

                Console.WriteLine("Временный корабль существует.");
            }

            Console.WriteLine("Блок завершён: ссылка на корабль исчезла.");
            Console.WriteLine("Но член экипажа всё ещё доступен.");
            Console.WriteLine($"Член экипажа: {sailor}");
            Console.WriteLine();

            // 4. Проверка композиции
            Console.WriteLine("=== Проверка композиции ===");

            {
                var company = new TradingCompany("Atlantic Trade", "Торговец", 150);

                company.LoadCargo(100);
                company.ShowInfo();

                Console.WriteLine();
                Console.WriteLine("Выходим из блока компании...");
            }

            Console.WriteLine("Сообщений об уничтожении нет: момент уборки объектов решает сборщик мусора.");
            Console.WriteLine();

            // 5. Работа с памятью
            MemoryDemo.ShowReferences();
            MemoryDemo.ShowStruct();
            MemoryDemo.ShowResourceRelease();
            MemoryDemo.ShowGarbageCollection();

            Console.WriteLine("=== Конец программы ===");
        }
    }
}