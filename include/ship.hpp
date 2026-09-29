#pragma once

#include <string>

// Предварительное объявление класса члена экипажа.
// Ship будет хранить указатель на уже существующий CrewMember.
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

    // Добавляет существующего члена экипажа на корабль.
    // CrewMember создаётся отдельно — это агрегация.
    void AddCrewMember(CrewMember& crewMember);

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

    // Указатель на существующего члена экипажа.
    // Ship не создаёт и не уничтожает этот объект.
    CrewMember* crewMember_;
};