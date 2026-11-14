namespace SeaTradeSimulator;

/// <summary>
/// Интерактивная игра в консоли: меню, рынок, плавание, штормы, пираты, досмотр.
/// Построена на классах ЛР 5 (BaseShip, IArmed, Captain, исключения).
/// </summary>
public class Game
{
    private const int Goal = 3000;
    private const int MaxDay = 45;
    private const int ProvPrice = 3;
    private const int RepairPrice = 4;
    private const int AmmoPrice = 20;
    private const int Width = 76;
    private const int GoodsCount = 4;

    private record GoodInfo(string Name, int BasePrice, bool Contraband);
    private record PortInfo(Port Port, int Depth, double[] Mod);

    private static readonly GoodInfo[] Goods =
    {
        new("Ром", 12, false), new("Зерно", 6, false), new("Пушнина", 22, false), new("Шёлк", 45, true),
    };

    private static readonly (int A, int B, int Days)[] Links =
    {
        (0, 1, 2), (0, 2, 3), (0, 5, 2), (0, 4, 3), (2, 3, 2), (3, 1, 3), (5, 2, 3), (1, 4, 2),
    };

    private readonly List<PortInfo> _ports = new()
    {
        new(new Port("Барнаул"), 9, new[] { 1.0, 0.7, 1.2, 1.3 }),
        new(new Port("Бийск", true), 9, new[] { 1.2, 1.0, 0.8, 2.0 }),
        new(new Port("Томск"), 9, new[] { 0.8, 1.1, 1.0, 1.5 }),
        new(new Port("Новосибирск"), 9, new[] { 1.1, 0.9, 1.3, 1.2 }),
        new(new Port("Рубцовск"), 5, new[] { 0.9, 1.3, 0.7, 1.6 }),
        new(new Port("Камень-на-Оби", true), 5, new[] { 1.3, 1.2, 0.9, 2.2 }),
    };

    private readonly Random _rng = new();
    private readonly int[] _cargo = new int[GoodsCount];
    private readonly double[] _noise = new double[GoodsCount];
    private readonly List<string> _notes = new();

    private BaseShip _ship = null!;
    private Captain _captain = null!;
    private int _hull;
    private int _hullMax;
    private int _ammoMax;
    private int _gold;
    private int _day = 1;
    private int _provisions;
    private int _port;
    private bool _over;

    public void Run()
    {
        while (true)
        {
            Ui.Clear();
            DrawBanner();
            Ui.Box(new[]
            {
                Ui.Paint("[1]", Ui.Amber) + "  Новая игра",
                Ui.Paint("[2]", Ui.Amber) + "  Правила",
                Ui.Paint("[0]", Ui.Amber) + "  Выход",
            }, Width, Ui.Sea, "Главное меню");
            int choice = Ui.ReadInt("Ваш выбор:", 0, 2);
            if (choice == 0)
            {
                Console.WriteLine("\n  Попутного ветра!");
                return;
            }
            if (choice == 2)
            {
                ShowRules();
            }
            else if (NewGame())
            {
                PortLoop();
            }
        }
    }

    // ---------- вспомогательное ----------
    private double Chance() => _rng.NextDouble();

    private void NewNoise()
    {
        for (int i = 0; i < _noise.Length; i++)
        {
            _noise[i] = 0.88 + 0.24 * Chance();
        }
    }

    private int Price(int good)
    {
        double raw = Goods[good].BasePrice * _ports[_port].Mod[good] * _noise[good];
        return Math.Max(1, (int)Math.Round(raw, MidpointRounding.AwayFromZero));
    }

    private int SellPrice(int good) =>
        Math.Max(1, (int)Math.Round(Price(good) * 0.95, MidpointRounding.AwayFromZero));

    private static int DaysBetween(int from, int to)
    {
        foreach (var link in Links)
        {
            if ((link.A == from && link.B == to) || (link.A == to && link.B == from))
            {
                return link.Days;
            }
        }
        return 0;
    }

    private void Note(string text)
    {
        _notes.Add($"день {_day}: {text}");
        if (_notes.Count > 6)
        {
            _notes.RemoveAt(0);
        }
    }

    private void Error(string className, string message) =>
        Note(Ui.Paint($"✗ {className}: ", Ui.Red) + message);

    // Убрать лишние тонны из учёта по товарам, если судно потеряло груз.
    private void SyncCargoToShip()
    {
        while (_cargo.Sum() > _ship.CurrentCargo)
        {
            _cargo[Array.IndexOf(_cargo, _cargo.Max())]--;
        }
    }

    private int LoseCargoPercent(int percent)
    {
        int lost = 0;
        for (int i = 0; i < _cargo.Length; i++)
        {
            int part = _cargo[i] * percent / 100;
            _cargo[i] -= part;
            lost += part;
        }
        _ship.UnloadCargo(lost);
        return lost;
    }

    // as возвращает null, если судно безоружно
    private IArmed? Armed() => _ship as IArmed;

    // ---------- экраны ----------
    private static void DrawBanner()
    {
        string[] art =
        {
            "              ╱|        ",
            "             ╱ |    ╱|  ",
            "            ╱__|___╱ |  ",
            "        ____╲_______╱___",
            "   ~~~~~~~~~╲_______╱~~~~~~~~~",
        };
        foreach (string line in art)
        {
            Console.WriteLine("    " + Ui.Sea + line + Ui.Reset);
        }
        Console.WriteLine($"\n    {Ui.Bold}{Ui.Amber}М О Р С К О Й   Т О Р Г О В Е Ц{Ui.Reset}");
        Console.WriteLine($"    {Ui.Gray}лабораторная работа 5 · полиморфизм{Ui.Reset}\n");
    }

    private static void ShowRules()
    {
        Ui.Clear();
        Ui.Box(new[]
        {
            $"Накопите {Ui.Paint(Goal.ToString(), Ui.Amber)} золота за {MaxDay} суток.",
            "",
            "• Покупайте товар там, где он дёшев, и продавайте там, где дорог.",
            "• Выходите в плавание по маршрутам между портами.",
            "• В пути бывают шторма (выбираете тактику капитана) и пираты.",
            "• Орудия есть только у фрегата и галеона (интерфейс IArmed).",
            "• Шёлк — контрабанда: в Бийске и Камне-на-Оби его нельзя продать,",
            "  а на досмотре его конфискуют.",
            "• Галеон слишком глубоко сидит и не войдёт в мелкие порты.",
            "• Провизия тратится каждые сутки в пути, корпус чинят в порту.",
            "• Стартовый капитал: 400 золота.",
        }, Width, Ui.Sea, "Правила");
        Ui.Pause();
    }

    private bool NewGame()
    {
        Ui.Clear();
        Ui.Box(new[]
        {
            Ui.Paint("[1] Торговый шлюп", Ui.Bold),
            "    трюм 40 т · осадка 2 · корпус 70 · орудий нет",
            "    быстрый и мелкосидящий, от пиратов только уходит",
            "",
            Ui.Paint("[2] Военный фрегат", Ui.Bold),
            "    трюм 80 т · осадка 4 · корпус 110 · мощь залпа 54",
            "    отобьётся от большинства пиратов, боезапас 6 залпов",
            "",
            Ui.Paint("[3] Тяжёлый галеон", Ui.Bold),
            "    трюм 200 т · осадка 7 · корпус 160 · мощь залпа 128",
            "    огромный трюм, но в мелкие порты не войдёт",
            "",
            Ui.Paint("[0] Назад", Ui.Gray),
        }, Width, Ui.Amber, "Выберите судно");
        int kind = Ui.ReadInt("Судно:", 0, 3);
        if (kind == 0)
        {
            return false;
        }
        string shipName = Ui.ReadLine("Название судна (Enter — «Ласточка»):", "Ласточка");
        string captainName = Ui.ReadLine("Имя капитана (Enter — «Беринг»):", "Беринг");

        Port home = _ports[0].Port;
        switch (kind)
        {
            case 1:
                _ship = new TradeSloop(shipName, 40, home);
                _hullMax = 70;
                _ammoMax = 0;
                break;
            case 2:
                _ship = new NavyFrigate(shipName, 80, home, 6);
                _hullMax = 110;
                _ammoMax = 6;
                break;
            default:
                _ship = new Galleon(shipName, 200, home, 10);
                _hullMax = 160;
                _ammoMax = 10;
                break;
        }
        _captain = new Captain(captainName, new PushOnTactic());
        _hull = _hullMax;
        Array.Clear(_cargo);
        _notes.Clear();
        _gold = 400;
        _day = 1;
        _provisions = 12;
        _port = 0;
        _over = false;
        NewNoise();
        Note($"судно «{shipName}» отходит от причала в Барнауле");
        return true;
    }

    private void DrawPort()
    {
        Ui.Clear();
        PortInfo here = _ports[_port];
        IArmed? armed = Armed();

        string Row(string label, string bar, string tail) => Ui.Pad(label, 10) + bar + "  " + tail;

        var status = new List<string>
        {
            Ui.Paint(_ship.Name, Ui.Bold) + Ui.Paint("   капитан " + _captain.Name, Ui.Gray)
                + (armed != null ? Ui.Paint("   [вооружён]", Ui.Amber) : Ui.Paint("   [без орудий]", Ui.Gray)),
            Row("Трюм", Ui.Bar(_ship.CurrentCargo, _ship.CargoCapacity, 26, Ui.Sea), $"{_ship.CurrentCargo}/{_ship.CargoCapacity} т"),
            Row("Корпус", Ui.Bar(_hull, _hullMax, 26, _hull * 4 < _hullMax ? Ui.Red : Ui.Green), $"{_hull}/{_hullMax}"),
            Row("Провизия", Ui.Bar(_provisions, 30, 26, _provisions < 6 ? Ui.Red : Ui.Amber), $"{_provisions} ед."),
        };
        if (armed != null)
        {
            status.Add(Row("Боезапас", Ui.Bar(armed.VolleysLeft, _ammoMax, 26, Ui.Amber), $"{armed.VolleysLeft}/{_ammoMax} залпов"));
        }
        status.Add(Row("Цель", Ui.Bar(_gold, Goal, 26, Ui.Green), $"{_gold}/{Goal} золота"));
        Ui.Box(status, Width, Ui.Amber, $"{here.Port.Name} · день {_day}/{MaxDay} · золото {_gold}");

        var market = new List<string>
        {
            Ui.Paint(Ui.Pad("№ Товар", 22) + Ui.Pad("Цена", 8, true) + Ui.Pad("Продажа", 10, true) + Ui.Pad("На борту", 12, true), Ui.Gray),
        };
        for (int g = 0; g < GoodsCount; g++)
        {
            string name = $"{g + 1} {Goods[g].Name}";
            if (Goods[g].Contraband)
            {
                name += here.Port.BansContraband ? Ui.Paint(" (запрет)", Ui.Red) : Ui.Paint(" (контрабанда)", Ui.Gray);
            }
            market.Add(Ui.Pad(name, 22) + Ui.Pad(Price(g).ToString(), 8, true)
                       + Ui.Pad(Ui.Paint(SellPrice(g).ToString(), Ui.Green), 10, true)
                       + Ui.Pad($"{_cargo[g]} т", 12, true));
        }
        Ui.Box(market, Width, Ui.Sea, "Рынок");

        var journal = new List<string>(_notes);
        if (journal.Count == 0)
        {
            journal.Add(Ui.Paint("записей пока нет", Ui.Gray));
        }
        Ui.Box(journal, Width, Ui.Gray, "Судовой журнал");

        Console.WriteLine($"  {Ui.Paint("[1]", Ui.Amber)} Купить   {Ui.Paint("[2]", Ui.Amber)} Продать   " +
                          $"{Ui.Paint("[3]", Ui.Amber)} Услуги   {Ui.Paint("[4]", Ui.Amber)} Плыть   {Ui.Paint("[0]", Ui.Amber)} В меню");
    }

    // ---------- порт ----------
    private void PortLoop()
    {
        while (!_over)
        {
            DrawPort();
            int choice = Ui.ReadInt("Действие:", 0, 4);
            switch (choice)
            {
                case 0:
                    return;
                case 1:
                    Buy();
                    break;
                case 2:
                    Sell();
                    break;
                case 3:
                    Services();
                    break;
                default:
                    Sail();
                    break;
            }
        }
    }

    private void Buy()
    {
        int good = Ui.ReadInt("Что купить? (1-4, 0 — отмена):", 0, GoodsCount) - 1;
        if (good < 0)
        {
            return;
        }
        int tons = Ui.ReadInt("Сколько тонн? (0 — отмена):", 0, 999);
        if (tons == 0)
        {
            return;
        }
        int cost = Price(good) * tons;
        if (_gold < cost)
        {
            Note(Ui.Paint($"Не хватает золота: нужно {cost}", Ui.Red));
            return;
        }
        try
        {
            _ship.LoadCargo(tons);   // может бросить CargoCapacityExceededException
        }
        catch (CargoCapacityExceededException error)
        {
            Error("CargoCapacityExceeded", error.Message);
            return;
        }
        _gold -= cost;
        _cargo[good] += tons;
        Note($"куплено {tons} т «{Goods[good].Name}» за {cost}");
    }

    private void Sell()
    {
        int good = Ui.ReadInt("Что продать? (1-4, 0 — отмена):", 0, GoodsCount) - 1;
        if (good < 0)
        {
            return;
        }
        if (_cargo[good] == 0)
        {
            Note(Ui.Paint($"На борту нет товара «{Goods[good].Name}»", Ui.Red));
            return;
        }
        int tons = Math.Min(_cargo[good], Ui.ReadInt("Сколько тонн? (0 — отмена, 999 — всё):", 0, 999));
        if (tons == 0)
        {
            return;
        }
        var lot = new CargoLot(Goods[good].Name, tons, Goods[good].Contraband);
        try
        {
            _ports[_port].Port.CheckAcceptable(lot);   // правило 9
        }
        catch (ForbiddenGoodsException error)
        {
            Error("ForbiddenGoodsError", error.Message);
            return;
        }
        _ship.UnloadCargo(tons);
        _cargo[good] -= tons;
        int income = SellPrice(good) * tons;
        _gold += income;
        Note(Ui.Paint($"продано {tons} т «{Goods[good].Name}» за {income}", Ui.Green));
    }

    private void Services()
    {
        IArmed? armed = Armed();
        var lines = new List<string>
        {
            $"{Ui.Paint("[1]", Ui.Amber)} Провизия +5      {ProvPrice * 5} золота",
            $"{Ui.Paint("[2]", Ui.Amber)} Ремонт корпуса +10   {RepairPrice * 10} золота",
        };
        if (armed != null)
        {
            lines.Add($"{Ui.Paint("[3]", Ui.Amber)} Боезапас +2 залпа   {AmmoPrice * 2} золота");
        }
        lines.Add($"{Ui.Paint("[0]", Ui.Amber)} Назад");
        Ui.Box(lines, Width, Ui.Sea, "Услуги порта");
        int choice = Ui.ReadInt("Услуга:", 0, armed != null ? 3 : 2);

        if (choice == 1)
        {
            if (_gold < ProvPrice * 5)
            {
                Note(Ui.Paint("Не хватает золота", Ui.Red));
                return;
            }
            _gold -= ProvPrice * 5;
            _provisions += 5;
            Note("закуплено 5 ед. провизии");
        }
        else if (choice == 2)
        {
            int amount = Math.Min(10, _hullMax - _hull);
            if (amount <= 0)
            {
                Note("корпус цел");
            }
            else if (_gold < amount * RepairPrice)
            {
                Note(Ui.Paint("Не хватает золота", Ui.Red));
            }
            else
            {
                _gold -= amount * RepairPrice;
                _hull += amount;
                Note($"ремонт: корпус +{amount}");
            }
        }
        else if (choice == 3 && armed != null)
        {
            if (_gold < AmmoPrice * 2)
            {
                Note(Ui.Paint("Не хватает золота", Ui.Red));
                return;
            }
            int added = armed.Resupply(2);   // через интерфейс
            if (added == 0)
            {
                Note("погреба полны");
                return;
            }
            _gold -= AmmoPrice * added;
            Note($"боезапас +{added} залпа");
        }
    }

    // ---------- плавание ----------
    private void Sail()
    {
        var targets = new List<int>();
        var lines = new List<string>();
        foreach (var link in Links)
        {
            if (link.A != _port && link.B != _port)
            {
                continue;
            }
            int to = link.A == _port ? link.B : link.A;
            PortInfo info = _ports[to];
            targets.Add(to);
            string flags = "";
            if (info.Port.BansContraband)
            {
                flags += Ui.Paint("  контрабанда запрещена", Ui.Red);
            }
            if (info.Depth < 7)
            {
                flags += Ui.Paint($"  мелко (глубина {info.Depth})", Ui.Gray);
            }
            lines.Add($"{Ui.Paint($"[{targets.Count}]", Ui.Amber)} {Ui.Pad(info.Port.Name, 15)}{link.Days} сут.{flags}");
        }
        lines.Add($"{Ui.Paint("[0]", Ui.Amber)} Остаться");
        Ui.Box(lines, Width, Ui.Sea, $"Куда плывём?  (провизии: {_provisions})");
        int choice = Ui.ReadInt("Порт назначения:", 0, targets.Count);
        if (choice == 0)
        {
            return;
        }
        int destination = targets[choice - 1];
        PortInfo target = _ports[destination];
        int days = DaysBetween(_port, destination);

        try
        {
            if (_ship.Draft > target.Depth)
            {
                throw new PortTooShallowException(_ship.Name, _ship.Draft, target.Port.Name, target.Depth);
            }
            if (_provisions < days)
            {
                throw new NotEnoughProvisionsException(days, _provisions);
            }
        }
        catch (PortTooShallowException error)
        {
            Error("PortTooShallow", error.Message);
            return;
        }
        catch (NotEnoughProvisionsException error)
        {
            Error("NotEnoughProvisions", error.Message);
            return;
        }

        AnimateVoyage(_ports[_port].Port.Name, target.Port.Name, days);
        _day += days;
        _provisions -= days;
        _port = destination;
        NewNoise();
        Note($"прибытие в {target.Port.Name} ({days} сут. в пути)");

        double roll = Chance();
        if (roll < 0.30)
        {
            Storm();
        }
        else if (roll < 0.55)
        {
            Pirates();
        }
        if (!_over)
        {
            Customs();
        }
        CheckEnd();
    }

    private static void AnimateVoyage(string from, string to, int days)
    {
        const int Track = 34;
        for (int i = 0; i <= Track; i++)
        {
            var track = new System.Text.StringBuilder();
            for (int k = 0; k <= Track; k++)
            {
                track.Append(k == i ? Ui.Paint("▲", Ui.Amber) : Ui.Paint("~", Ui.Sea));
            }
            Console.Write($"\r  {from} {track} {to}{Ui.Paint($"  ({days} сут.)", Ui.Gray)}");
            Ui.Sleep(35);
        }
        Console.WriteLine();
    }

    private static IStormTactic MakeTactic(int index) => index switch
    {
        0 => new PushOnTactic(),
        1 => new ChangeRouteTactic(),
        _ => new WaitOutTactic(),
    };

    private void Storm()
    {
        int power = _rng.Next(4, 10);
        var lines = new List<string>
        {
            Ui.Paint($"Сила шторма: {power}", Ui.Bold) + Ui.Paint($"   осадка судна: {_ship.Draft}", Ui.Gray),
            $"Чем больше осадка, тем меньше потери. Что прикажет капитан {_captain.Name}?",
            "",
        };
        // Динамическое конфигурирование: капитан по очереди примеряет тактики.
        for (int i = 0; i < 3; i++)
        {
            _captain.Tactic = MakeTactic(i);
            StormOutcome preview = _captain.Evaluate(_ship, power);
            lines.Add($"{Ui.Paint($"[{i + 1}]", Ui.Amber)} {Ui.Pad(_captain.Tactic.Name, 22)}потеря груза {preview.CargoLostPercent}%, суток {preview.DaysLost}");
        }
        Ui.Box(lines, Width, Ui.Red, "Шторм!");
        int choice = Ui.ReadInt("Тактика:", 1, 3);
        _captain.Tactic = MakeTactic(choice - 1);
        StormOutcome outcome = _captain.Evaluate(_ship, power);
        int lost = LoseCargoPercent(outcome.CargoLostPercent);
        _hull = Math.Max(0, _hull - (outcome.CargoLostPercent / 6 + 1));
        _day += outcome.DaysLost;
        Note(Ui.Paint($"шторм {power}, «{_captain.Tactic.Name}»: −{lost} т груза, корпус {_hull}/{_hullMax}, +{outcome.DaysLost} сут.",
            lost > 0 ? Ui.Amber : Ui.Green));
        CheckEnd();
    }

    private void Pirates()
    {
        int strength = _rng.Next(25, 111);
        IArmed? armed = Armed();
        double escape = 0.9 - 0.1 * _ship.Draft;
        int ransom = 60 + strength;

        while (true)
        {
            var ids = new List<string>();
            var lines = new List<string>
            {
                $"Пиратский бриг, сила {Ui.Paint(strength.ToString(), Ui.Bold)}. " + (armed != null ? "Орудия заряжены." : "Орудий на борту нет."),
                "",
            };
            if (armed != null)
            {
                ids.Add("fire");
                lines.Add($"{Ui.Paint($"[{ids.Count}]", Ui.Amber)} Открыть огонь   мощь {armed.Firepower}, залпов {armed.VolleysLeft}");
            }
            ids.Add("flee");
            lines.Add($"{Ui.Paint($"[{ids.Count}]", Ui.Amber)} Уйти от боя   шанс {(int)(escape * 100)}%");
            ids.Add("pay");
            lines.Add($"{Ui.Paint($"[{ids.Count}]", Ui.Amber)} Откупиться   {ransom} золота");
            Ui.Box(lines, Width, Ui.Red, "Пираты по курсу");
            string action = ids[Ui.ReadInt("Решение:", 1, ids.Count) - 1];

            if (action == "fire" && armed != null)
            {
                try
                {
                    armed.Fire("пиратский бриг");
                }
                catch (OutOfAmmunitionException error)
                {
                    Error("OutOfAmmunition", error.Message);
                    Ui.Pause("Выберите другое решение (Enter)");
                    continue;
                }
                if (armed.Firepower >= strength)
                {
                    int prize = 40 + strength;
                    _gold += prize;
                    Note(Ui.Paint($"залп! пираты отбиты, трофеи {prize} золота", Ui.Green));
                }
                else
                {
                    Hurt("залп не остановил пиратов");
                }
            }
            else if (action == "flee")
            {
                if (Chance() < escape)
                {
                    Note(Ui.Paint("ускользнули от пиратов без потерь", Ui.Green));
                }
                else
                {
                    Hurt("уйти не удалось");
                }
            }
            else if (_gold >= ransom)
            {
                _gold -= ransom;
                Note(Ui.Paint($"откупились от пиратов за {ransom}", Ui.Amber));
            }
            else
            {
                Note(Ui.Paint("не хватает золота на выкуп", Ui.Red));
                Hurt("пираты взяли своё силой");
            }
            break;
        }
        CheckEnd();
    }

    private void Hurt(string reason)
    {
        int lost = LoseCargoPercent(30);
        _hull = Math.Max(0, _hull - 15);
        Note(Ui.Paint($"{reason}: потеряно {lost} т груза, корпус {_hull}/{_hullMax}", Ui.Red));
    }

    private void Customs()
    {
        PortInfo here = _ports[_port];
        int silk = _cargo[3];
        if (!here.Port.BansContraband || silk == 0)
        {
            return;
        }
        if (Chance() < 0.6)
        {
            _ship.UnloadCargo(silk);
            _cargo[3] = 0;
            _gold = Math.Max(0, _gold - 100);
            Note(Ui.Paint($"таможня в {here.Port.Name}: конфисковано {silk} т шёлка, штраф 100", Ui.Red));
        }
        else
        {
            Note(Ui.Paint($"таможня в {here.Port.Name}: контрабанду не нашли", Ui.Amber));
        }
    }

    private void CheckEnd()
    {
        if (_over)
        {
            return;
        }
        SyncCargoToShip();
        if (_hull <= 0)
        {
            Finish(false, "Корабль затонул.");
        }
        else if (_gold >= Goal)
        {
            Finish(true, $"Вы накопили {_gold} золота за {_day} суток.");
        }
        else if (_day > MaxDay)
        {
            Finish(false, $"Сезон закончился: на счёте {_gold} из {Goal}.");
        }
    }

    private void Finish(bool win, string text)
    {
        _over = true;
        DrawPort();
        Ui.Box(new[] { Ui.Paint(text, Ui.Bold) }, Width, win ? Ui.Green : Ui.Red,
            win ? "Компания процветает" : "Плавание окончено");
        Ui.Pause("Нажмите Enter, чтобы вернуться в меню");
    }
}
