using System.Text;
using static System.Math;
using Fleet = System.Collections.Generic.List<SeaTradeSimulator.BaseShip>;

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

            Console.WriteLine("=== Лабораторная работа 4, часть 2 (C#) ===");
            Console.WriteLine();

            var port = new Port("Барнаул");

            // 1. Иерархия: список базового типа, объекты разных наследников.
            Console.WriteLine("=== Создание судов (порядок конструкторов) ===");

            var fleet = new Fleet
            {
                new TradeSloop("Ласточка", 40, port),
                new NavyFrigate("Стерегущий", 80, port),
                new Galleon("Громовержец", 200, port),
            };

            Console.WriteLine();
            Console.WriteLine("=== Работа через ссылку на базовый класс ===");

            foreach (BaseShip ship in fleet)
            {
                Console.WriteLine(ship.Describe());
                ship.UseSpecialAction();
            }

            Console.WriteLine();
            Console.WriteLine("=== this(...): галеон без указания грузоподъёмности ===");

            var defaultGalleon = new Galleon("Левиафан", port);
            Console.WriteLine($"Грузоподъёмность: {defaultGalleon.CargoCapacity}");

            // 2. Поверхностная и глубокая копия.
            Console.WriteLine();
            Console.WriteLine("=== Поверхностная копия: MemberwiseClone ===");

            var original = new TradeSloop("Оригинал", 20, new Port("Бийск"));
            var shallow = (TradeSloop)original.ShallowCopy();

            shallow.HomePort.Name = "Рубцовск";

            Console.WriteLine($"Один и тот же порт: {ReferenceEquals(original.HomePort, shallow.HomePort)}");
            Console.WriteLine($"оригинал: {original.Describe()}");
            Console.WriteLine($"копия:    {shallow.Describe()}");
            Console.WriteLine("Порт изменился и у оригинала: он общий.");

            Console.WriteLine();
            Console.WriteLine("=== Глубокая копия: MemberwiseClone + замена порта ===");

            var deep = (TradeSloop)original.DeepCopy();

            deep.HomePort.Name = "Томск";

            Console.WriteLine($"Один и тот же порт: {ReferenceEquals(original.HomePort, deep.HomePort)}");
            Console.WriteLine($"оригинал: {original.Describe()}");
            Console.WriteLine($"копия:    {deep.Describe()}");

            Console.WriteLine();
            Console.WriteLine("=== Глубокая копия: конструктор копирования ===");

            var copy = new TradeSloop(original);

            copy.HomePort.Name = "Омск";

            Console.WriteLine($"оригинал: {original.Describe()}");
            Console.WriteLine($"копия:    {copy.Describe()}");

            // 3. this.
            Console.WriteLine();
            Console.WriteLine("=== this: корабль передаёт себя порту ===");

            fleet[0].DockAt(port);

            // 4. Статические члены.
            Console.WriteLine();
            Console.WriteLine("=== Статический счётчик ===");

            Console.WriteLine($"Выдано инвентарных номеров: {BaseShip.IssuedNumbersCount}");
            Console.WriteLine($"Следующий номер: {BaseShip.NextInventoryNumber()}");

            // 5. Сравнение по содержимому.
            Console.WriteLine();
            Console.WriteLine("=== Equals и GetHashCode ===");

            var twin = new TradeSloop("Ласточка", 10, new Port("Другой порт"));

            Console.WriteLine($"fleet[0] == twin (ссылки): {ReferenceEquals(fleet[0], twin)}");
            Console.WriteLine($"fleet[0].Equals(twin): {fleet[0].Equals(twin)}");
            Console.WriteLine($"Хеши равны: {fleet[0].GetHashCode() == twin.GetHashCode()}");

            var unique = new HashSet<BaseShip> { fleet[0], twin };
            Console.WriteLine($"В HashSet из двух «Ласточек»: {unique.Count}");

            // 6. Композиция и агрегация.
            Console.WriteLine();
            Console.WriteLine("=== Композиция: компания владеет кораблём ===");

            {
                var company = new TradingCompany(
                    "Atlantic Trade", new Galleon("Торговец", 150, port));

                company.LoadCargo(100);
                company.ShowInfo();
            }

            Console.WriteLine();
            Console.WriteLine("=== Агрегация: корабль хранит ссылку на члена экипажа ===");

            var sailor = new CrewMember("Иван", 100);

            fleet[0].AddCrewMember(sailor);
            Console.WriteLine($"Размер экипажа: {fleet[0].CrewSize}");

            // 7. using.
            Console.WriteLine();
            Console.WriteLine("=== using var, using static, псевдоним типа ===");

            Console.WriteLine($"Кораблей в списке Fleet: {fleet.Count}");
            Console.WriteLine($"Свободно в трюме шлюпа: {Max(0, fleet[0].CargoCapacity - fleet[0].CurrentCargo)} тонн");

            ShowUsingDeclaration();
            Console.WriteLine("Метод завершён: журнал уже закрыт.");

            Console.WriteLine();
            Console.WriteLine("=== Конец программы ===");
        }

        private static void ShowUsingDeclaration()
        {
            using var log = new VoyageLog("Рейс №2");

            log.Write("корабль вышел из порта");
            log.Write("метод заканчивается");
        }
    }
}
