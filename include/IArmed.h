#pragma once

#include <string>
#include <string_view>

// Интерфейс-контракт «вооружён»: умеет вести огонь.
// Только чисто виртуальные методы и виртуальный деструктор, полей нет.
// Не всё в проекте вооружено: шлюп безоружен, поэтому это не свойство BaseShip.
class IArmed
{
public:
    virtual ~IArmed() = default;

    virtual std::string GetUnitName() const = 0;   // кто стреляет
    virtual int GetFirepower() const = 0;          // огневая мощь залпа
    virtual bool HasAmmunition() const = 0;        // есть ли чем стрелять
    virtual int GetVolleysLeft() const = 0;        // сколько залпов осталось
    virtual int Resupply(int volleys) = 0;         // пополнить боезапас, вернуть сколько принято
    virtual void Fire(std::string_view target) = 0; // дать залп; OutOfAmmunition, если нечем
};
