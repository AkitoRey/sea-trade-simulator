#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "port.hpp"

class CrewMember;

// Абстрактный базовый класс судов.
class BaseShip
{
private:
    // Один счётчик на все суда (статическое поле).
    inline static int lastInventoryNumber_{ 0 };

    int inventoryNumber_{};
    std::string name_{};
    int cargoCapacity_{};
    int currentCargo_{ 0 };

    // Агрегация из ЛР 3: корабль не владеет членом экипажа.
    std::weak_ptr<CrewMember> crewMember_{};

    // Порт приписки. Корабли могут делить один порт (поверхностно),
    // а конструктор копирования даёт копии собственный порт (глубоко).
    std::shared_ptr<Port> homePort_{};

public:
    // Конструктор с параметрами.
    BaseShip(std::string_view name, int cargoCapacity,
             std::shared_ptr<Port> homePort);
    virtual ~BaseShip();

    // Чисто виртуальные методы: у каждого типа судна своя реализация.
    virtual int GetDraft() const = 0;
    virtual void UseSpecialAction() const = 0;

    // Копия нужного типа через указатель на базовый класс.
    virtual std::unique_ptr<BaseShip> Clone() const = 0;

    // Есть реализация, наследник может её дополнить или заменить.
    virtual std::string Describe() const;

    // Статические члены: номер выдаётся один на все суда.
    static int NextInventoryNumber();
    static int GetIssuedNumbersCount();

    // Строки: поиск и выделение подстроки.
    bool NameContains(std::string_view part) const;
    std::string FirstWordOfName() const;

    // this: корабль передаёт себя порту.
    void DockAt(const Port& port) const;

    void LoadCargo(int weight);
    void AddCrewMember(const std::shared_ptr<CrewMember>& crewMember);
    void ShowCrewMember() const;

    const std::string& GetName() const;
    int GetCargoCapacity() const;
    int GetCurrentCargo() const;
    int GetCrewSize() const;
    int GetInventoryNumber() const;
    const std::shared_ptr<Port>& GetHomePort() const;

protected:
    // По умолчанию, копирования и перемещения доступны только наследникам:
    // так объект наследника нельзя случайно «срезать» до BaseShip.
    BaseShip();
    BaseShip(const BaseShip& other);
    BaseShip(BaseShip&& other) noexcept;
    BaseShip& operator=(const BaseShip& other);
    BaseShip& operator=(BaseShip&& other) noexcept = default;

    // Нужен наследникам, остальному коду не нужен.
    std::string Header() const;
};
