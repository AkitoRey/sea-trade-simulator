#pragma once

#include "BaseShip.h"

// Торговый шлюп: быстрый, мелкосидящий, БЕЗОРУЖНЫЙ (интерфейс IArmed не реализует).
class TradeSloop : public BaseShip
{
public:
    TradeSloop(std::string_view name, int cargoCapacity, std::shared_ptr<Port> homePort);
    TradeSloop(const TradeSloop&) = default;
    TradeSloop(TradeSloop&&) noexcept = default;
    TradeSloop& operator=(const TradeSloop&) = default;
    TradeSloop& operator=(TradeSloop&&) noexcept = default;
    ~TradeSloop() noexcept override = default;

    std::string GetKind() const;   // скрывает BaseShip::GetKind (раннее связывание)

    std::string Describe() const override;
    int GetDraft() const override;
    void UseSpecialAction() const override;
    std::unique_ptr<BaseShip> Clone() const override;
};
