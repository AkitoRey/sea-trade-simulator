#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include "BaseShip.h"
#include "IArmed.h"
#include "Port.h"

// Залп через интерфейс с выводом строки. Бросает OutOfAmmunition, если нечем стрелять.
void Volley(IArmed& weapon, std::string_view target);

// Работает ТОЛЬКО через интерфейс: не знает, фрегат перед ним, галеон или батарея.
bool RepelPirates(IArmed& defender, int pirateStrength);

// Проверка типа на границе: «умеет ли это судно воевать вообще».
void MeetPirates(BaseShip& ship, int pirateStrength);

// Погрузка с судовым журналом (RAII). Исключения пропускает наверх: что делать, решает вызывающий.
void LoadShipment(BaseShip& ship, int tons);

// Распределить груз по флоту. Перехватывает CargoCapacityExceeded: именно здесь
// понятно, что делать — загрузить, сколько влезает, и перейти к следующему судну.
// Возвращает массу, которую не удалось погрузить.
int LoadAcrossFleet(std::vector<std::unique_ptr<BaseShip>>& fleet, int tons);

// Продажа в порту. Перехватывает ForbiddenGoodsError: груз остаётся на борту.
bool SellAtPort(const Port& port, const CargoLot& lot);
