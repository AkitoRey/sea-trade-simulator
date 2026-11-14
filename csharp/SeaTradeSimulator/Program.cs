using SeaTradeSimulator;

Ui.Init();
if (args.Contains("--fast"))
{
    Ui.Fast = true;
}
// Без аргументов запускается игра; ключ --demo показывает по пунктам все задания ЛР.
if (!args.Contains("--demo"))
{
    new Game().Run();
    return;
}

static void Section(string title) => Console.WriteLine($"\n=== {title} ===");

var port = new Port("Барнаул");
var strictPort = new Port("Бийск", bansContraband: true);   // контрабанда запрещена

var fleet = new List<BaseShip>
{
    new TradeSloop("Ласточка", 40, port),
    new NavyFrigate("Стерегущий", 80, port, 2),
    new Galleon("Громовержец", 200, port),
};

// ---------------------------------------------------------------
Section("1. Интерфейс IArmed: метод не знает конкретных классов");
{
    // Одна и та же функция работает с фрегатом, галеоном и береговой батареей.
    var frigate = (NavyFrigate)fleet[1];
    var galleon = (Galleon)fleet[2];
    var battery = new CoastalBattery("Старый форт", 6, 3);

    Operations.RepelPirates(frigate, 40);
    Operations.RepelPirates(galleon, 100);
    Operations.RepelPirates(battery, 25);
}

// ---------------------------------------------------------------
Section("2. Один класс + интерфейс: BaseShip + IArmed");
{
    var galleon = new Galleon("Левиафан", 300, port);
    BaseShip asShip = galleon;   // как судно
    IArmed asArmed = galleon;    // как вооружённая единица
    Console.WriteLine($"  как BaseShip: {asShip.Describe()}");
    Console.WriteLine($"  как IArmed:   {asArmed.UnitName}, мощность {asArmed.Firepower}");
    Console.WriteLine($"  это один и тот же объект: {object.ReferenceEquals(asShip, asArmed)}");
}

// ---------------------------------------------------------------
Section("3. Раннее (new) и позднее (override) связывание");
foreach (BaseShip ship in fleet)   // статический тип переменной — BaseShip
{
    Console.WriteLine($"  GetKind()  (new, раннее):          {ship.GetKind()}");
    Console.WriteLine($"  Describe() (override, позднее):    {ship.Describe()}");
}
{
    var sloop = (TradeSloop)fleet[0];
    Console.WriteLine($"  Та же «Ласточка» через переменную TradeSloop: GetKind() = {sloop.GetKind()}");
}

// ---------------------------------------------------------------
Section("4. is и as: воевать может только то, что реализует IArmed");
foreach (BaseShip ship in fleet)
{
    Operations.MeetPirates(ship, 40);
}
{
    IArmed? maybe = fleet[0] as IArmed;   // null, если не реализует
    Console.WriteLine($"  «{fleet[0].Name}» as IArmed = {(maybe is null ? "null" : "объект")}");
    try
    {
        var armed = (IArmed)fleet[0];     // явное приведение бросает InvalidCastException
        armed.Fire("цель");
    }
    catch (InvalidCastException)
    {
        Console.WriteLine($"  «{fleet[0].Name}» не реализует IArmed: InvalidCastException");
    }
}

// ---------------------------------------------------------------
Section("5. Конфигурирование: тактика капитана в шторм");
{
    BaseShip galleon = fleet[2];
    BaseShip sloop = fleet[0];

    static void Reload(BaseShip ship, int tons)
    {
        ship.UnloadCargo(ship.CurrentCargo);
        ship.LoadCargo(tons);
    }

    Reload(galleon, 100);
    Reload(sloop, 30);

    // Статическое конфигурирование: тактика задана при создании капитана.
    var captain = new Captain("Беринг", new PushOnTactic());
    Console.WriteLine($"  Шлюп «{sloop.Name}», груз {sloop.CurrentCargo} т");
    captain.FaceStorm(sloop, 7);

    // Динамическое конфигурирование: тот же капитан получает другой объект поведения.
    Reload(sloop, 30);
    captain.Tactic = new ChangeRouteTactic();
    captain.FaceStorm(sloop, 7);

    Reload(sloop, 30);
    captain.Tactic = new WaitOutTactic();
    captain.FaceStorm(sloop, 7);

    Console.WriteLine($"  Галеон «{galleon.Name}» (осадка {galleon.Draft}) в том же шторме, тактика «напролом»:");
    captain.Tactic = new PushOnTactic();
    captain.FaceStorm(galleon, 7);

    galleon.UnloadCargo(galleon.CurrentCargo);
    sloop.UnloadCargo(sloop.CurrentCargo);
}

// ---------------------------------------------------------------
Section("6. Исключения");

Console.WriteLine("-- 6.1. Переполнение трюма: создаёт BaseShip.LoadCargo, ловит LoadAcrossFleet --");
{
    int left = Operations.LoadAcrossFleet(fleet, 300);
    Console.WriteLine($"  не поместилось: {left} т");
    foreach (BaseShip ship in fleet)
    {
        Console.WriteLine($"  {ship.Describe()}");
    }
    foreach (BaseShip ship in fleet)
    {
        ship.UnloadCargo(ship.CurrentCargo);
    }
}

Console.WriteLine("-- 6.2. После отказа состояние судна не изменилось --");
{
    BaseShip sloop = fleet[0];
    sloop.LoadCargo(35);
    try
    {
        sloop.LoadCargo(10);   // свободно только 5 т
    }
    catch (CargoCapacityExceededException error)   // частный тип — первым
    {
        Console.WriteLine($"  {error.Message}");
    }
    catch (SeaTradeException error)                // затем общий для предметной области
    {
        Console.WriteLine($"  ошибка правил: {error.Message}");
    }
    Console.WriteLine($"  груз после отказа: {sloop.CurrentCargo} т (было 35)");
    sloop.UnloadCargo(35);
}

Console.WriteLine("-- 6.3. Запрещённый товар (правило 9), блок finally --");
{
    var rum = new CargoLot("ром", 5, false);
    var smuggled = new CargoLot("контрабандный шёлк", 3, true);
    Operations.SellAtPort(strictPort, rum);        // успешное действие
    Operations.SellAtPort(strictPort, smuggled);   // нарушение правила: исключение перехвачено внутри
    Operations.SellAtPort(port, smuggled);         // в этом порту запрета нет
}

Console.WriteLine("-- 6.4. Боезапас --");
try
{
    var frigate = new NavyFrigate("Дозорный", 50, port, 1);   // боезапас: один залп
    frigate.Fire("пират №1");                                 // успешное действие
    frigate.Fire("пират №2");                                 // боезапас кончился
}
catch (OutOfAmmunitionException error)
{
    Console.WriteLine($"  {error.Message}");
}

Console.WriteLine("-- 6.5. Ошибки программиста: ArgumentException --");
try
{
    fleet[0].LoadCargo(-5);
}
catch (ArgumentOutOfRangeException error)   // частный тип — первым
{
    Console.WriteLine($"  ArgumentOutOfRangeException: {error.ParamName}");
}
try
{
    _ = new TradeSloop("", 10, port);
}
catch (ArgumentException error)             // общий тип — после частного
{
    Console.WriteLine($"  ArgumentException при создании судна: {error.ParamName}");
}

Console.WriteLine("-- 6.6. using: журнал закрывается и при исключении --");
try
{
    Operations.LoadShipment(fleet[0], 20);   // успешно: журнал закрылся при выходе из метода
    Operations.LoadShipment(fleet[0], 99);   // исключение: журнал закроется при выходе из блока using
}
catch (SeaTradeException error)
{
    Console.WriteLine($"  перехвачено выше: {error.Message}");
}

// ---------------------------------------------------------------
Section("7. Стандартный интерфейс IComparable<BaseShip>");
{
    BaseShip[] ships = fleet.ToArray();
    Console.WriteLine("  до сортировки:   " + string.Join(", ", ships.Select(s => s.Name)));
    Array.Sort(ships);   // Array.Sort знает только о IComparable
    Console.WriteLine("  после сортировки: " + string.Join(", ", ships.Select(s => s.Name)));
}

Section("Конец программы");
