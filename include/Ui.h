#pragma once

#include <string>
#include <string_view>
#include <vector>

// Консольный интерфейс: цвета ANSI, рамки, индикаторы, ввод с проверкой.
namespace ui
{
    inline constexpr const char* RESET = "\x1b[0m";
    inline constexpr const char* BOLD = "\x1b[1m";
    inline constexpr const char* AMBER = "\x1b[38;5;214m";
    inline constexpr const char* SEA = "\x1b[38;5;38m";
    inline constexpr const char* GREEN = "\x1b[38;5;78m";
    inline constexpr const char* RED = "\x1b[38;5;203m";
    inline constexpr const char* GRAY = "\x1b[38;5;245m";
    inline constexpr const char* WHITE = "\x1b[97m";

    // Включает UTF-8 и цвета (в Windows это нужно делать явно).
    void Init();
    void Clear();
    void Sleep(int milliseconds);
    void SetFast(bool fast);   // без пауз и анимации (для проверок)

    std::string Paint(std::string_view text, const char* color);
    // Ширина текста на экране: без цветовых кодов, символ UTF-8 считается за один.
    int VisibleWidth(std::string_view text);
    std::string Pad(std::string_view text, int width, bool alignRight = false);
    std::string Bar(int value, int max, int width, const char* color);

    // Рамка с заголовком. Строки дополняются пробелами до одной ширины.
    void Box(const std::vector<std::string>& lines, int width, const char* color,
             std::string_view title = "");

    void Pause(std::string_view text = "Нажмите Enter, чтобы продолжить");
    std::string ReadLine(std::string_view prompt, std::string_view fallback);
    // Спрашивает число в диапазоне [min; max], пока не получит корректное.
    int ReadInt(std::string_view prompt, int min, int max);
}
