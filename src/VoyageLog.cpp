#include "VoyageLog.h"

#include <iostream>

VoyageLog::VoyageLog(std::string_view title)
    : title_{ title }
{
    std::cout << "  [журнал «" << title_ << "» открыт]\n";
}

VoyageLog::~VoyageLog() noexcept
{
    std::cout << "  [журнал «" << title_ << "» закрыт]\n";
}

void VoyageLog::Write(std::string_view record) const
{
    std::cout << "  [журнал] " << record << "\n";
}
