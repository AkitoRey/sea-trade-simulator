using System;
using System.Runtime.CompilerServices;

namespace SeaTradeSimulator
{
    /// <summary>
    /// Демонстрации работы C# с памятью: ссылки, структуры, освобождение ресурса, сборщик мусора.
    /// </summary>
    public static class MemoryDemo
    {
        public static void ShowReferences()
        {
            Console.WriteLine("=== Ссылки и копии ===");

            var first = new CrewMember("Пётр", 100);
            var second = first; // копируется ссылка, а не объект

            second.TakeDamage(10);

            Console.WriteLine($"first:  {first}");
            Console.WriteLine($"second: {second}");
            Console.WriteLine($"Один и тот же объект: {ReferenceEquals(first, second)}");
            Console.WriteLine();
        }

        public static void ShowStruct()
        {
            Console.WriteLine("=== Структура (тип-значение) ===");

            var a = new Money { Coins = 150 };
            var b = a; // копируется значение целиком

            b.Coins = 300;

            Console.WriteLine($"a: {a}");
            Console.WriteLine($"b: {b}");
            Console.WriteLine("Исходная структура не изменилась: b - независимая копия.");
            Console.WriteLine();
        }

        public static void ShowResourceRelease()
        {
            Console.WriteLine("=== Освобождение ресурса (IDisposable + using) ===");

            using (var log = new VoyageLog("Рейс №1"))
            {
                log.Write("корабль вышел из порта");
                log.Write("груз: 90 тонн");
            }

            Console.WriteLine("Блок using завершён - журнал уже закрыт.");
            Console.WriteLine();
        }

        public static void ShowGarbageCollection()
        {
            Console.WriteLine("=== Сборщик мусора ===");

            // Объект создаётся и проверяется в отдельных методах, чтобы в отладочной
            // сборке локальные переменные не удерживали его до конца текущего метода.
            WeakReference<CrewMember> watcher = CreateTemporary();

            Console.WriteLine("Последняя ссылка исчезла (метод создания завершён).");
            Console.WriteLine($"Объект ещё жив до сборки мусора: {IsAlive(watcher)}");

            GC.Collect();
            GC.WaitForPendingFinalizers();

            Console.WriteLine($"Объект жив после GC.Collect(): {IsAlive(watcher)}");
            Console.WriteLine();
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        private static WeakReference<CrewMember> CreateTemporary()
        {
            var temporary = new CrewMember("Списанный матрос", 100);

            return new WeakReference<CrewMember>(temporary);
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        private static bool IsAlive(WeakReference<CrewMember> reference)
        {
            return reference.TryGetTarget(out _);
        }
    }
}