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

    // Порт приписки. Копируется по-разному: см. копирование в шаге 4.
    std::shared_ptr<Port> homePort_{};

public:
    BaseShip(std::string_view name, int cargoCapacity,
             std::shared_ptr<Port> homePort);
    virtual ~BaseShip();

    // Чисто виртуальные методы: у каждого типа судна своя реализация.
    virtual int GetDraft() const = 0;
    virtual void UseSpecialAction() const = 0;

    // Есть реализация, наследник может её дополнить или заменить.
    virtual std::string Describe() const;

    static int NextInventoryNumber();

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
    // Нужен наследникам, остальному коду не нужен.
    std::string Header() const;
};
