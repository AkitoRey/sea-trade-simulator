#pragma once

#include <string>
#include <string_view>

// Судовой журнал. Идиома RAII: журнал закрывается в деструкторе,
// поэтому он закроется и при нормальном выходе, и при исключении (свёртка стека).
class VoyageLog
{
private:
    std::string title_{};

public:
    explicit VoyageLog(std::string_view title);
    VoyageLog(const VoyageLog&) = delete;
    VoyageLog& operator=(const VoyageLog&) = delete;
    ~VoyageLog() noexcept;   // деструктор не бросает исключений

    void Write(std::string_view record) const;
};
