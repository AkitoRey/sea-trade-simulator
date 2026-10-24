#pragma once

#include <memory>
#include <string>

// Предварительное объявление класса члена экипажа.
// Ship наблюдает за уже существующим CrewMember через weak_ptr.
class CrewMember;

class Ship
{
public:
    // Конструктор: создаёт корабль с названием
    // и заданной грузоподъёмностью.
    Ship(const std::string& name, int cargoCapacity);

    // Деструктор: вызывается при уничтожении корабля.
    ~Ship();

    // Загружает груз на корабль.
    // Не позволяет превысить грузоподъёмность.
    void LoadCargo(int weight);

    // Назначает на корабль существующего члена экипажа.
    // CrewMember создаётся отдельно — это агрегация.
    void AddCrewMember(const std::shared_ptr<CrewMember>& crewMember);

    // Выводит информацию о члене экипажа, если он ещё существует.
    void ShowCrewMember() const;

    // Возвращает название корабля.
    const std::string& GetName() const;

    // Возвращает максимальную грузоподъёмность корабля.
    int GetCargoCapacity() const;

    // Возвращает текущий вес груза.
    int GetCurrentCargo() const;

    // Возвращает количество членов экипажа.
    int GetCrewSize() const;

private:
    // Название корабля.
    std::string name_;

    // Максимальная грузоподъёмность в условных тоннах.
    int cargoCapacity_;

    // Текущий вес груза.
    int currentCargo_;

    // Наблюдатель за членом экипажа.
    // Ship не владеет этим объектом и не продлевает ему жизнь.
    std::weak_ptr<CrewMember> crewMember_;
};