#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "Port.h"

// Абстрактный класс иерархии судов (ЛР 4).
class BaseShip
{
private:
    inline static int lastInventoryNumber_{ 0 };

    std::string name_{};
    int cargoCapacity_{ 0 };
    int currentCargo_{ 0 };
    std::shared_ptr<Port> homePort_{};
    int inventoryNumber_{ 0 };   // последним: номер выдаётся, только если проверки прошли

protected:
    BaseShip(std::string_view name, int cargoCapacity, std::shared_ptr<Port> homePort);
    BaseShip(const BaseShip& other);                      // глубокая копия порта
    BaseShip(BaseShip&& other) noexcept;
    BaseShip& operator=(const BaseShip& other);
    BaseShip& operator=(BaseShip&& other) noexcept = default;

    // Нужен наследникам, остальному коду не нужен.
    std::string Header() const;

public:
    virtual ~BaseShip() noexcept = default;

    static int NextInventoryNumber();

    // Раннее связывание: метод НЕ виртуальный, выбирается по типу указателя/ссылки.
    // Наследники его скрывают, а не переопределяют.
    std::string GetKind() const;

    // Позднее связывание: виртуальные методы, выбираются по настоящему типу объекта.
    virtual std::string Describe() const;
    virtual int GetDraft() const = 0;
    virtual void UseSpecialAction() const = 0;
    virtual std::unique_ptr<BaseShip> Clone() const = 0;

    // Бросает std::invalid_argument при tons <= 0 и CargoCapacityExceeded при переполнении.
    // Строгая гарантия: проверки идут до изменения currentCargo_.
    void LoadCargo(int tons);
    // Бросает std::invalid_argument, если tons вне диапазона [1; текущий груз].
    void UnloadCargo(int tons);

    void DockAt(const Port& port) const;

    const std::string& GetName() const;
    int GetCargoCapacity() const;
    int GetCurrentCargo() const;
    int GetFreeCapacity() const;
    int GetInventoryNumber() const;
    std::shared_ptr<Port> GetHomePort() const;
};
