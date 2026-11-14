#include "Game.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "Captain.h"
#include "Exceptions.h"
#include "Galleon.h"
#include "IArmed.h"
#include "NavyFrigate.h"
#include "Port.h"
#include "StormTactics.h"
#include "TradeSloop.h"
#include "Ui.h"

namespace
{
    constexpr int GOAL = 3000;
    constexpr int MAX_DAY = 45;
    constexpr int PROV_PRICE = 3;
    constexpr int REPAIR_PRICE = 4;
    constexpr int AMMO_PRICE = 20;
    constexpr int WIDTH = 76;
    constexpr int GOODS_COUNT = 4;

    struct GoodInfo
    {
        const char* name;
        int basePrice;
        bool contraband;
    };

    constexpr GoodInfo GOODS[GOODS_COUNT] = {
        { "Ром", 12, false }, { "Зерно", 6, false }, { "Пушнина", 22, false }, { "Шёлк", 45, true },
    };

    struct PortInfo
    {
        std::shared_ptr<Port> port;
        int depth;
        std::array<double, GOODS_COUNT> mod;
    };

    struct Link
    {
        int a;
        int b;
        int days;
    };

    constexpr Link LINKS[] = {
        { 0, 1, 2 }, { 0, 2, 3 }, { 0, 5, 2 }, { 0, 4, 3 },
        { 2, 3, 2 }, { 3, 1, 3 }, { 5, 2, 3 }, { 1, 4, 2 },
    };

    std::string Num(int value)
    {
        return std::to_string(value);
    }
}

class Game
{
private:
    std::vector<PortInfo> ports_;
    std::unique_ptr<BaseShip> ship_;
    std::unique_ptr<Captain> captain_;
    std::array<int, GOODS_COUNT> cargo_{};
    std::array<double, GOODS_COUNT> noise_{};
    std::vector<std::string> notes_;
    std::mt19937 rng_{ std::random_device{}() };
    int hull_{ 0 };
    int hullMax_{ 0 };
    int ammoMax_{ 0 };
    int gold_{ 0 };
    int day_{ 1 };
    int provisions_{ 0 };
    int port_{ 0 };
    bool over_{ false };

public:
    Game()
    {
        auto add = [this](const char* name, bool strict, int depth, std::array<double, GOODS_COUNT> mod)
        {
            ports_.push_back({ std::make_shared<Port>(name, strict), depth, mod });
        };
        add("Барнаул", false, 9, { 1.0, 0.7, 1.2, 1.3 });
        add("Бийск", true, 9, { 1.2, 1.0, 0.8, 2.0 });
        add("Томск", false, 9, { 0.8, 1.1, 1.0, 1.5 });
        add("Новосибирск", false, 9, { 1.1, 0.9, 1.3, 1.2 });
        add("Рубцовск", false, 5, { 0.9, 1.3, 0.7, 1.6 });
        add("Камень-на-Оби", true, 5, { 1.3, 1.2, 0.9, 2.2 });
    }

    void Run()
    {
        while (true)
        {
            ui::Clear();
            DrawBanner();
            ui::Box({ ui::Paint("[1]", ui::AMBER) + "  Новая игра",
                      ui::Paint("[2]", ui::AMBER) + "  Правила",
                      ui::Paint("[0]", ui::AMBER) + "  Выход" },
                    WIDTH, ui::SEA, "Главное меню");
            const int choice = ui::ReadInt("Ваш выбор:", 0, 2);
            if (choice == 0)
            {
                std::cout << "\n  Попутного ветра!\n";
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

private:
    // ---------- вспомогательное ----------
    int Random(int low, int high)
    {
        return std::uniform_int_distribution<int>{ low, high }(rng_);
    }

    double Chance()
    {
        return std::uniform_real_distribution<double>{ 0.0, 1.0 }(rng_);
    }

    void NewNoise()
    {
        for (double& n : noise_)
        {
            n = 0.88 + 0.24 * Chance();
        }
    }

    int Price(int good) const
    {
        const double raw = GOODS[good].basePrice * ports_[static_cast<std::size_t>(port_)].mod[static_cast<std::size_t>(good)]
                           * noise_[static_cast<std::size_t>(good)];
        return std::max(1, static_cast<int>(std::lround(raw)));
    }

    int SellPrice(int good) const
    {
        return std::max(1, static_cast<int>(std::lround(Price(good) * 0.95)));
    }

    int DaysBetween(int from, int to) const
    {
        for (const Link& link : LINKS)
        {
            if ((link.a == from && link.b == to) || (link.a == to && link.b == from))
            {
                return link.days;
            }
        }
        return 0;
    }

    void Note(const std::string& text)
    {
        notes_.push_back("день " + Num(day_) + ": " + text);
        if (notes_.size() > 6)
        {
            notes_.erase(notes_.begin());
        }
    }

    void Error(const std::string& className, const std::string& message)
    {
        Note(ui::Paint("✗ " + className + ": ", ui::RED) + message);
    }

    // Убрать лишние тонны из учёта по товарам, если судно потеряло груз.
    void SyncCargoToShip()
    {
        while (std::accumulate(cargo_.begin(), cargo_.end(), 0) > ship_->GetCurrentCargo())
        {
            *std::max_element(cargo_.begin(), cargo_.end()) -= 1;
        }
    }

    int LoseCargoPercent(int percent)
    {
        int lost = 0;
        for (int& tons : cargo_)
        {
            const int part = tons * percent / 100;
            tons -= part;
            lost += part;
        }
        ship_->UnloadCargo(lost);
        return lost;
    }

    IArmed* Armed() const
    {
        return dynamic_cast<IArmed*>(ship_.get());   // nullptr, если судно безоружно
    }

    // ---------- экраны ----------
    static void DrawBanner()
    {
        const char* art[] = {
            "              ╱|        ",
            "             ╱ |    ╱|  ",
            "            ╱__|___╱ |  ",
            "        ____╲_______╱___",
            "   ~~~~~~~~~╲_______╱~~~~~~~~~",
        };
        for (const char* line : art)
        {
            std::cout << "    " << ui::SEA << line << ui::RESET << "\n";
        }
        std::cout << "\n    " << ui::BOLD << ui::AMBER << "М О Р С К О Й   Т О Р Г О В Е Ц" << ui::RESET
                  << "\n    " << ui::GRAY << "лабораторная работа 5 · полиморфизм" << ui::RESET << "\n\n";
    }

    void ShowRules()
    {
        ui::Clear();
        ui::Box({ "Накопите " + ui::Paint(Num(GOAL), ui::AMBER) + " золота за " + Num(MAX_DAY) + " суток.",
                  "",
                  "• Покупайте товар там, где он дёшев, и продавайте там, где дорог.",
                  "• Выходите в плавание по маршрутам между портами.",
                  "• В пути бывают шторма (выбираете тактику капитана) и пираты.",
                  "• Орудия есть только у фрегата и галеона (интерфейс IArmed).",
                  "• Шёлк — контрабанда: в Бийске и Камне-на-Оби его нельзя продать,",
                  "  а на досмотре его конфискуют.",
                  "• Галеон слишком глубоко сидит и не войдёт в мелкие порты.",
                  "• Провизия тратится каждые сутки в пути, корпус чинят в порту.",
                  "• Стартовый капитал: 400 золота." },
                WIDTH, ui::SEA, "Правила");
        ui::Pause();
    }

    bool NewGame()
    {
        ui::Clear();
        ui::Box({ ui::Paint("[1] Торговый шлюп", ui::BOLD),
                  "    трюм 40 т · осадка 2 · корпус 70 · орудий нет",
                  "    быстрый и мелкосидящий, от пиратов только уходит",
                  "",
                  ui::Paint("[2] Военный фрегат", ui::BOLD),
                  "    трюм 80 т · осадка 4 · корпус 110 · мощь залпа 54",
                  "    отобьётся от большинства пиратов, боезапас 6 залпов",
                  "",
                  ui::Paint("[3] Тяжёлый галеон", ui::BOLD),
                  "    трюм 200 т · осадка 7 · корпус 160 · мощь залпа 128",
                  "    огромный трюм, но в мелкие порты не войдёт",
                  "",
                  ui::Paint("[0] Назад", ui::GRAY) },
                WIDTH, ui::AMBER, "Выберите судно");
        const int kind = ui::ReadInt("Судно:", 0, 3);
        if (kind == 0)
        {
            return false;
        }
        const std::string shipName = ui::ReadLine("Название судна (Enter — «Ласточка»):", "Ласточка");
        const std::string captainName = ui::ReadLine("Имя капитана (Enter — «Беринг»):", "Беринг");

        const auto& home = ports_[0].port;
        if (kind == 1)
        {
            ship_ = std::make_unique<TradeSloop>(shipName, 40, home);
            hullMax_ = 70;
            ammoMax_ = 0;
        }
        else if (kind == 2)
        {
            ship_ = std::make_unique<NavyFrigate>(shipName, 80, home, 6);
            hullMax_ = 110;
            ammoMax_ = 6;
        }
        else
        {
            ship_ = std::make_unique<Galleon>(shipName, 200, home, 10);
            hullMax_ = 160;
            ammoMax_ = 10;
        }
        captain_ = std::make_unique<Captain>(captainName, std::make_unique<PushOnTactic>());
        hull_ = hullMax_;
        cargo_.fill(0);
        notes_.clear();
        gold_ = 400;
        day_ = 1;
        provisions_ = 12;
        port_ = 0;
        over_ = false;
        NewNoise();
        Note("судно «" + shipName + "» отходит от причала в Барнауле");
        return true;
    }

    void DrawPort()
    {
        ui::Clear();
        const PortInfo& here = ports_[static_cast<std::size_t>(port_)];
        const IArmed* armed = Armed();

        std::vector<std::string> status;
        status.push_back(ui::Paint(ship_->GetName(), ui::BOLD) + ui::Paint("   капитан " + captain_->GetName(), ui::GRAY)
                         + (armed != nullptr ? ui::Paint("   [вооружён]", ui::AMBER) : ui::Paint("   [без орудий]", ui::GRAY)));
        const auto row = [](const std::string& label, const std::string& bar, const std::string& tail)
        {
            return ui::Pad(label, 10) + bar + "  " + tail;
        };
        status.push_back(row("Трюм", ui::Bar(ship_->GetCurrentCargo(), ship_->GetCargoCapacity(), 26, ui::SEA),
                             Num(ship_->GetCurrentCargo()) + "/" + Num(ship_->GetCargoCapacity()) + " т"));
        status.push_back(row("Корпус", ui::Bar(hull_, hullMax_, 26, hull_ * 4 < hullMax_ ? ui::RED : ui::GREEN),
                             Num(hull_) + "/" + Num(hullMax_)));
        status.push_back(row("Провизия", ui::Bar(provisions_, 30, 26, provisions_ < 6 ? ui::RED : ui::AMBER),
                             Num(provisions_) + " ед."));
        if (armed != nullptr)
        {
            status.push_back(row("Боезапас", ui::Bar(armed->GetVolleysLeft(), ammoMax_, 26, ui::AMBER),
                                 Num(armed->GetVolleysLeft()) + "/" + Num(ammoMax_) + " залпов"));
        }
        status.push_back(row("Цель", ui::Bar(gold_, GOAL, 26, ui::GREEN), Num(gold_) + "/" + Num(GOAL) + " золота"));
        ui::Box(status, WIDTH, ui::AMBER,
                here.port->GetName() + " · день " + Num(day_) + "/" + Num(MAX_DAY) + " · золото " + Num(gold_));

        std::vector<std::string> market;
        market.push_back(ui::Paint(ui::Pad("№ Товар", 22) + ui::Pad("Цена", 8, true) + ui::Pad("Продажа", 10, true)
                                       + ui::Pad("На борту", 12, true), ui::GRAY));
        for (int g = 0; g < GOODS_COUNT; ++g)
        {
            std::string name = Num(g + 1) + " " + GOODS[g].name;
            if (GOODS[g].contraband)
            {
                name += here.port->BansContraband() ? ui::Paint(" (запрет)", ui::RED) : ui::Paint(" (контрабанда)", ui::GRAY);
            }
            market.push_back(ui::Pad(name, 22) + ui::Pad(Num(Price(g)), 8, true)
                             + ui::Pad(ui::Paint(Num(SellPrice(g)), ui::GREEN), 10, true)
                             + ui::Pad(Num(cargo_[static_cast<std::size_t>(g)]) + " т", 12, true));
        }
        ui::Box(market, WIDTH, ui::SEA, "Рынок");

        std::vector<std::string> journal;
        for (const std::string& note : notes_)
        {
            journal.push_back(note);
        }
        if (journal.empty())
        {
            journal.push_back(ui::Paint("записей пока нет", ui::GRAY));
        }
        ui::Box(journal, WIDTH, ui::GRAY, "Судовой журнал");

        std::cout << "  " << ui::Paint("[1]", ui::AMBER) << " Купить   " << ui::Paint("[2]", ui::AMBER)
                  << " Продать   " << ui::Paint("[3]", ui::AMBER) << " Услуги   " << ui::Paint("[4]", ui::AMBER)
                  << " Плыть   " << ui::Paint("[0]", ui::AMBER) << " В меню\n";
    }

    // ---------- порт ----------
    void PortLoop()
    {
        while (!over_)
        {
            DrawPort();
            const int choice = ui::ReadInt("Действие:", 0, 4);
            if (choice == 0)
            {
                return;
            }
            if (choice == 1)
            {
                Buy();
            }
            else if (choice == 2)
            {
                Sell();
            }
            else if (choice == 3)
            {
                Services();
            }
            else
            {
                Sail();
            }
        }
    }

    void Buy()
    {
        const int good = ui::ReadInt("Что купить? (1-4, 0 — отмена):", 0, GOODS_COUNT) - 1;
        if (good < 0)
        {
            return;
        }
        const int tons = ui::ReadInt("Сколько тонн? (0 — отмена):", 0, 999);
        if (tons == 0)
        {
            return;
        }
        const int cost = Price(good) * tons;
        if (gold_ < cost)
        {
            Note(ui::Paint("Не хватает золота: нужно " + Num(cost), ui::RED));
            return;
        }
        try
        {
            ship_->LoadCargo(tons);   // может бросить CargoCapacityExceeded
        }
        catch (const CargoCapacityExceeded& error)
        {
            Error("CargoCapacityExceeded", error.what());
            return;
        }
        gold_ -= cost;
        cargo_[static_cast<std::size_t>(good)] += tons;
        Note("куплено " + Num(tons) + " т «" + GOODS[good].name + "» за " + Num(cost));
    }

    void Sell()
    {
        const int good = ui::ReadInt("Что продать? (1-4, 0 — отмена):", 0, GOODS_COUNT) - 1;
        if (good < 0)
        {
            return;
        }
        const int onBoard = cargo_[static_cast<std::size_t>(good)];
        if (onBoard == 0)
        {
            Note(ui::Paint("На борту нет товара «" + std::string{ GOODS[good].name } + "»", ui::RED));
            return;
        }
        const int tons = std::min(onBoard, ui::ReadInt("Сколько тонн? (0 — отмена, 999 — всё):", 0, 999));
        if (tons == 0)
        {
            return;
        }
        const CargoLot lot{ GOODS[good].name, tons, GOODS[good].contraband };
        try
        {
            ports_[static_cast<std::size_t>(port_)].port->CheckAcceptable(lot);   // правило 9
        }
        catch (const ForbiddenGoodsError& error)
        {
            Error("ForbiddenGoodsError", error.what());
            return;
        }
        ship_->UnloadCargo(tons);
        cargo_[static_cast<std::size_t>(good)] -= tons;
        const int income = SellPrice(good) * tons;
        gold_ += income;
        Note(ui::Paint("продано " + Num(tons) + " т «" + GOODS[good].name + "» за " + Num(income), ui::GREEN));
    }

    void Services()
    {
        IArmed* armed = Armed();
        std::vector<std::string> lines{
            ui::Paint("[1]", ui::AMBER) + " Провизия +5      " + Num(PROV_PRICE * 5) + " золота",
            ui::Paint("[2]", ui::AMBER) + " Ремонт корпуса +10   " + Num(REPAIR_PRICE * 10) + " золота",
        };
        if (armed != nullptr)
        {
            lines.push_back(ui::Paint("[3]", ui::AMBER) + " Боезапас +2 залпа   " + Num(AMMO_PRICE * 2) + " золота");
        }
        lines.push_back(ui::Paint("[0]", ui::AMBER) + " Назад");
        ui::Box(lines, WIDTH, ui::SEA, "Услуги порта");
        const int choice = ui::ReadInt("Услуга:", 0, armed != nullptr ? 3 : 2);
        if (choice == 1)
        {
            if (gold_ < PROV_PRICE * 5)
            {
                Note(ui::Paint("Не хватает золота", ui::RED));
                return;
            }
            gold_ -= PROV_PRICE * 5;
            provisions_ += 5;
            Note("закуплено 5 ед. провизии");
        }
        else if (choice == 2)
        {
            const int amount = std::min(10, hullMax_ - hull_);
            if (amount <= 0)
            {
                Note("корпус цел");
            }
            else if (gold_ < amount * REPAIR_PRICE)
            {
                Note(ui::Paint("Не хватает золота", ui::RED));
            }
            else
            {
                gold_ -= amount * REPAIR_PRICE;
                hull_ += amount;
                Note("ремонт: корпус +" + Num(amount));
            }
        }
        else if (choice == 3 && armed != nullptr)
        {
            if (gold_ < AMMO_PRICE * 2)
            {
                Note(ui::Paint("Не хватает золота", ui::RED));
                return;
            }
            const int added = armed->Resupply(2);   // через интерфейс
            if (added == 0)
            {
                Note("погреба полны");
                return;
            }
            gold_ -= AMMO_PRICE * added;
            Note("боезапас +" + Num(added) + " залпа");
        }
    }

    // ---------- плавание ----------
    void Sail()
    {
        std::vector<int> targets;
        std::vector<std::string> lines;
        for (const Link& link : LINKS)
        {
            if (link.a != port_ && link.b != port_)
            {
                continue;
            }
            const int to = link.a == port_ ? link.b : link.a;
            const PortInfo& info = ports_[static_cast<std::size_t>(to)];
            targets.push_back(to);
            std::string flags;
            if (info.port->BansContraband())
            {
                flags += ui::Paint("  контрабанда запрещена", ui::RED);
            }
            if (info.depth < 7)
            {
                flags += ui::Paint("  мелко (глубина " + Num(info.depth) + ")", ui::GRAY);
            }
            lines.push_back(ui::Paint("[" + Num(static_cast<int>(targets.size())) + "]", ui::AMBER) + " "
                            + ui::Pad(info.port->GetName(), 15) + Num(link.days) + " сут." + flags);
        }
        lines.push_back(ui::Paint("[0]", ui::AMBER) + " Остаться");
        ui::Box(lines, WIDTH, ui::SEA, "Куда плывём?  (провизии: " + Num(provisions_) + ")");
        const int choice = ui::ReadInt("Порт назначения:", 0, static_cast<int>(targets.size()));
        if (choice == 0)
        {
            return;
        }
        const int to = targets[static_cast<std::size_t>(choice - 1)];
        const PortInfo& target = ports_[static_cast<std::size_t>(to)];
        const int days = DaysBetween(port_, to);

        try
        {
            if (ship_->GetDraft() > target.depth)
            {
                throw PortTooShallow{ ship_->GetName(), ship_->GetDraft(), target.port->GetName(), target.depth };
            }
            if (provisions_ < days)
            {
                throw NotEnoughProvisions{ days, provisions_ };
            }
        }
        catch (const PortTooShallow& error)
        {
            Error("PortTooShallow", error.what());
            return;
        }
        catch (const NotEnoughProvisions& error)
        {
            Error("NotEnoughProvisions", error.what());
            return;
        }

        const std::string from = ports_[static_cast<std::size_t>(port_)].port->GetName();
        AnimateVoyage(from, target.port->GetName(), days);
        day_ += days;
        provisions_ -= days;
        port_ = to;
        NewNoise();
        Note("прибытие в " + target.port->GetName() + " (" + Num(days) + " сут. в пути)");

        const double roll = Chance();
        if (roll < 0.30)
        {
            Storm();
        }
        else if (roll < 0.55)
        {
            Pirates();
        }
        if (!over_)
        {
            Customs();
        }
        CheckEnd();
    }

    void AnimateVoyage(const std::string& from, const std::string& to, int days) const
    {
        constexpr int TRACK = 34;
        for (int i = 0; i <= TRACK; ++i)
        {
            std::string track;
            for (int k = 0; k <= TRACK; ++k)
            {
                track += k == i ? ui::Paint("▲", ui::AMBER) : ui::Paint("~", ui::SEA);
            }
            std::cout << "\r  " << from << " " << track << " " << to << ui::Paint("  (" + Num(days) + " сут.)", ui::GRAY) << std::flush;
            ui::Sleep(35);
        }
        std::cout << "\n";
    }

    static std::unique_ptr<IStormTactic> MakeTactic(int index)
    {
        if (index == 0)
        {
            return std::make_unique<PushOnTactic>();
        }
        if (index == 1)
        {
            return std::make_unique<ChangeRouteTactic>();
        }
        return std::make_unique<WaitOutTactic>();
    }

    void Storm()
    {
        const int power = Random(4, 9);
        std::vector<std::string> lines{
            ui::Paint("Сила шторма: " + Num(power), ui::BOLD) + ui::Paint("   осадка судна: " + Num(ship_->GetDraft()), ui::GRAY),
            "Чем больше осадка, тем меньше потери. Что прикажет капитан " + captain_->GetName() + "?",
            "",
        };
        // Динамическое конфигурирование: капитан по очереди примеряет тактики.
        for (int i = 0; i < 3; ++i)
        {
            captain_->SetTactic(MakeTactic(i));
            const StormOutcome preview = captain_->Evaluate(*ship_, power);
            lines.push_back(ui::Paint("[" + Num(i + 1) + "]", ui::AMBER) + " " + ui::Pad(captain_->GetTacticName(), 22)
                            + "потеря груза " + Num(preview.cargoLostPercent) + "%, суток " + Num(preview.daysLost));
        }
        ui::Box(lines, WIDTH, ui::RED, "Шторм!");
        const int choice = ui::ReadInt("Тактика:", 1, 3);
        captain_->SetTactic(MakeTactic(choice - 1));
        const StormOutcome outcome = captain_->Evaluate(*ship_, power);
        const int lost = LoseCargoPercent(outcome.cargoLostPercent);
        hull_ = std::max(0, hull_ - (outcome.cargoLostPercent / 6 + 1));
        day_ += outcome.daysLost;
        Note(ui::Paint("шторм " + Num(power) + ", «" + captain_->GetTacticName() + "»: −" + Num(lost)
                           + " т груза, корпус " + Num(hull_) + "/" + Num(hullMax_) + ", +" + Num(outcome.daysLost) + " сут.",
                       lost > 0 ? ui::AMBER : ui::GREEN));
        CheckEnd();
    }

    void Pirates()
    {
        const int strength = Random(25, 110);
        IArmed* armed = Armed();
        const double escape = 0.9 - 0.1 * ship_->GetDraft();
        const int ransom = 60 + strength;

        while (true)
        {
            std::vector<std::string> ids;
            std::vector<std::string> lines{
                "Пиратский бриг, сила " + ui::Paint(Num(strength), ui::BOLD) + ". "
                    + (armed != nullptr ? "Орудия заряжены." : "Орудий на борту нет."),
                "",
            };
            if (armed != nullptr)
            {
                ids.push_back("fire");
                lines.push_back(ui::Paint("[" + Num(static_cast<int>(ids.size())) + "]", ui::AMBER) + " Открыть огонь   мощь "
                                + Num(armed->GetFirepower()) + ", залпов " + Num(armed->GetVolleysLeft()));
            }
            ids.push_back("flee");
            lines.push_back(ui::Paint("[" + Num(static_cast<int>(ids.size())) + "]", ui::AMBER) + " Уйти от боя   шанс "
                            + Num(static_cast<int>(escape * 100)) + "%");
            ids.push_back("pay");
            lines.push_back(ui::Paint("[" + Num(static_cast<int>(ids.size())) + "]", ui::AMBER) + " Откупиться   " + Num(ransom) + " золота");
            ui::Box(lines, WIDTH, ui::RED, "Пираты по курсу");
            const std::string action = ids[static_cast<std::size_t>(ui::ReadInt("Решение:", 1, static_cast<int>(ids.size())) - 1)];

            if (action == "fire")
            {
                try
                {
                    armed->Fire("пиратский бриг");
                }
                catch (const OutOfAmmunition& error)
                {
                    Error("OutOfAmmunition", error.what());
                    ui::Pause("Выберите другое решение (Enter)");
                    continue;
                }
                if (armed->GetFirepower() >= strength)
                {
                    const int prize = 40 + strength;
                    gold_ += prize;
                    Note(ui::Paint("залп! пираты отбиты, трофеи " + Num(prize) + " золота", ui::GREEN));
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
                    Note(ui::Paint("ускользнули от пиратов без потерь", ui::GREEN));
                }
                else
                {
                    Hurt("уйти не удалось");
                }
            }
            else if (gold_ >= ransom)
            {
                gold_ -= ransom;
                Note(ui::Paint("откупились от пиратов за " + Num(ransom), ui::AMBER));
            }
            else
            {
                Note(ui::Paint("не хватает золота на выкуп", ui::RED));
                Hurt("пираты взяли своё силой");
            }
            break;
        }
        CheckEnd();
    }

    void Hurt(const std::string& reason)
    {
        const int lost = LoseCargoPercent(30);
        hull_ = std::max(0, hull_ - 15);
        Note(ui::Paint(reason + ": потеряно " + Num(lost) + " т груза, корпус " + Num(hull_) + "/" + Num(hullMax_), ui::RED));
    }

    void Customs()
    {
        const PortInfo& here = ports_[static_cast<std::size_t>(port_)];
        int& silk = cargo_[3];
        if (!here.port->BansContraband() || silk == 0)
        {
            return;
        }
        if (Chance() < 0.6)
        {
            const int confiscated = silk;
            ship_->UnloadCargo(silk);
            silk = 0;
            gold_ = std::max(0, gold_ - 100);
            Note(ui::Paint("таможня в " + here.port->GetName() + ": конфисковано " + Num(confiscated) + " т шёлка, штраф 100", ui::RED));
        }
        else
        {
            Note(ui::Paint("таможня в " + here.port->GetName() + ": контрабанду не нашли", ui::AMBER));
        }
    }

    void CheckEnd()
    {
        if (over_)
        {
            return;
        }
        SyncCargoToShip();
        if (hull_ <= 0)
        {
            Finish(false, "Корабль затонул.");
        }
        else if (gold_ >= GOAL)
        {
            Finish(true, "Вы накопили " + Num(gold_) + " золота за " + Num(day_) + " суток.");
        }
        else if (day_ > MAX_DAY)
        {
            Finish(false, "Сезон закончился: на счёте " + Num(gold_) + " из " + Num(GOAL) + ".");
        }
    }

    void Finish(bool win, const std::string& text)
    {
        over_ = true;
        DrawPort();
        ui::Box({ ui::Paint(text, ui::BOLD) },
                WIDTH, win ? ui::GREEN : ui::RED, win ? "Компания процветает" : "Плавание окончено");
        ui::Pause("Нажмите Enter, чтобы вернуться в меню");
    }
};

void RunGame()
{
    Game game;
    game.Run();
}
