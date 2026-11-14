#include "Ui.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace ui
{
    namespace
    {
        bool fast = false;

        std::string Repeat(std::string_view piece, int count)
        {
            std::string result;
            for (int i = 0; i < count; ++i)
            {
                result += piece;
            }
            return result;
        }
    }

    void Init()
    {
#ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        const HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        if (GetConsoleMode(out, &mode))
        {
            SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
#endif
        if (std::getenv("SEATRADE_FAST") != nullptr)
        {
            fast = true;
        }
    }

    void Clear()
    {
        std::cout << "\x1b[2J\x1b[H";
    }

    void Sleep(int milliseconds)
    {
        if (!fast)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds{ milliseconds });
        }
    }

    void SetFast(bool value)
    {
        fast = value;
    }

    std::string Paint(std::string_view text, const char* color)
    {
        return std::string{ color } + std::string{ text } + RESET;
    }

    int VisibleWidth(std::string_view text)
    {
        int width = 0;
        for (std::size_t i = 0; i < text.size(); ++i)
        {
            const unsigned char c = static_cast<unsigned char>(text[i]);
            if (c == 0x1b)   // цветовой код: ESC [ ... m
            {
                while (i < text.size() && text[i] != 'm')
                {
                    ++i;
                }
                continue;
            }
            if ((c & 0xC0) != 0x80)   // не продолжающий байт UTF-8: новый символ
            {
                ++width;
            }
        }
        return width;
    }

    std::string Pad(std::string_view text, int width, bool alignRight)
    {
        const int gap = width - VisibleWidth(text);
        if (gap <= 0)
        {
            return std::string{ text };
        }
        const std::string spaces(static_cast<std::size_t>(gap), ' ');
        return alignRight ? spaces + std::string{ text } : std::string{ text } + spaces;
    }

    std::string Bar(int value, int max, int width, const char* color)
    {
        if (max <= 0)
        {
            max = 1;
        }
        const int clamped = value < 0 ? 0 : (value > max ? max : value);
        const int filled = clamped * width / max;
        return Paint(Repeat("█", filled), color) + Paint(Repeat("░", width - filled), GRAY);
    }

    void Box(const std::vector<std::string>& lines, int width, const char* color,
             std::string_view title)
    {
        const std::string edge{ color };
        std::string top = edge + "╔";
        if (title.empty())
        {
            top += Repeat("═", width + 2);
        }
        else
        {
            const int used = VisibleWidth(title) + 2;
            top += "═ " + std::string{ RESET } + std::string{ BOLD } + std::string{ title }
                   + RESET + edge + " " + Repeat("═", width + 2 - used - 1);
        }
        std::cout << top << "╗" << RESET << "\n";
        for (const std::string& line : lines)
        {
            std::cout << edge << "║" << RESET << " " << Pad(line, width) << " " << edge << "║"
                      << RESET << "\n";
        }
        std::cout << edge << "╚" << Repeat("═", width + 2) << "╝" << RESET << "\n";
    }

    void Pause(std::string_view text)
    {
        std::cout << "\n" << GRAY << "  " << text << RESET << " ";
        std::string ignored;
        if (!std::getline(std::cin, ignored))
        {
            std::exit(0);
        }
    }

    std::string ReadLine(std::string_view prompt, std::string_view fallback)
    {
        std::cout << AMBER << "  " << prompt << RESET << " ";
        std::string line;
        if (!std::getline(std::cin, line))
        {
            std::exit(0);
        }
        const auto first = line.find_first_not_of(" \t\r");
        if (first == std::string::npos)
        {
            return std::string{ fallback };
        }
        const auto last = line.find_last_not_of(" \t\r");
        return line.substr(first, last - first + 1);
    }

    int ReadInt(std::string_view prompt, int min, int max)
    {
        while (true)
        {
            std::cout << AMBER << "  " << prompt << RESET << " ";
            std::string line;
            if (!std::getline(std::cin, line))
            {
                std::exit(0);
            }
            try
            {
                std::size_t used = 0;
                const int value = std::stoi(line, &used);
                if (line.find_first_not_of(" \t\r", used) == std::string::npos && value >= min
                    && value <= max)
                {
                    return value;
                }
            }
            catch (const std::exception&)
            {
                // не число или слишком большое: спросим ещё раз
            }
            std::cout << RED << "  Введите число от " << min << " до " << max << "." << RESET
                      << "\n";
        }
    }
}
