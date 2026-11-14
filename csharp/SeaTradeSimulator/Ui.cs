using System.Runtime.InteropServices;
using System.Text;

namespace SeaTradeSimulator;

/// <summary>Консольный интерфейс: цвета ANSI, рамки, индикаторы, ввод с проверкой.</summary>
public static class Ui
{
    public const string Reset = "\u001b[0m";
    public const string Bold = "\u001b[1m";
    public const string Amber = "\u001b[38;5;214m";
    public const string Sea = "\u001b[38;5;38m";
    public const string Green = "\u001b[38;5;78m";
    public const string Red = "\u001b[38;5;203m";
    public const string Gray = "\u001b[38;5;245m";

    /// <summary>Без пауз и анимации (для проверок).</summary>
    public static bool Fast { get; set; }

    [DllImport("kernel32.dll")]
    private static extern IntPtr GetStdHandle(int handle);

    [DllImport("kernel32.dll")]
    private static extern bool GetConsoleMode(IntPtr handle, out uint mode);

    [DllImport("kernel32.dll")]
    private static extern bool SetConsoleMode(IntPtr handle, uint mode);

    /// <summary>Включает UTF-8 и цвета (в Windows это нужно делать явно).</summary>
    public static void Init()
    {
        try
        {
            Console.OutputEncoding = Encoding.UTF8;
            Console.InputEncoding = Encoding.UTF8;
        }
        catch (IOException)
        {
            // вывод перенаправлен в файл: кодировку менять не нужно
        }
        if (OperatingSystem.IsWindows())
        {
            IntPtr handle = GetStdHandle(-11);   // STD_OUTPUT_HANDLE
            if (GetConsoleMode(handle, out uint mode))
            {
                SetConsoleMode(handle, mode | 0x0004);   // ENABLE_VIRTUAL_TERMINAL_PROCESSING
            }
        }
        if (Environment.GetEnvironmentVariable("SEATRADE_FAST") != null)
        {
            Fast = true;
        }
    }

    public static void Clear() => Console.Write("\u001b[2J\u001b[H");

    public static void Sleep(int milliseconds)
    {
        if (!Fast)
        {
            Thread.Sleep(milliseconds);
        }
    }

    public static string Paint(string text, string color) => color + text + Reset;

    /// <summary>Ширина текста на экране: без цветовых кодов.</summary>
    public static int VisibleWidth(string text)
    {
        int width = 0;
        for (int i = 0; i < text.Length; i++)
        {
            if (text[i] == '\u001b')
            {
                while (i < text.Length && text[i] != 'm')
                {
                    i++;
                }
                continue;
            }
            width++;
        }
        return width;
    }

    public static string Pad(string text, int width, bool alignRight = false)
    {
        int gap = width - VisibleWidth(text);
        if (gap <= 0)
        {
            return text;
        }
        string spaces = new(' ', gap);
        return alignRight ? spaces + text : text + spaces;
    }

    public static string Bar(int value, int max, int width, string color)
    {
        if (max <= 0)
        {
            max = 1;
        }
        int clamped = Math.Clamp(value, 0, max);
        int filled = clamped * width / max;
        return Paint(new string('█', filled), color) + Paint(new string('░', width - filled), Gray);
    }

    /// <summary>Рамка с заголовком. Строки дополняются пробелами до одной ширины.</summary>
    public static void Box(IEnumerable<string> lines, int width, string color, string title = "")
    {
        string top = color + "╔";
        if (title.Length == 0)
        {
            top += new string('═', width + 2);
        }
        else
        {
            int used = VisibleWidth(title) + 2;
            top += "═ " + Reset + Bold + title + Reset + color + " " + new string('═', Math.Max(0, width + 2 - used - 1));
        }
        Console.WriteLine(top + "╗" + Reset);
        foreach (string line in lines)
        {
            Console.WriteLine(color + "║" + Reset + " " + Pad(line, width) + " " + color + "║" + Reset);
        }
        Console.WriteLine(color + "╚" + new string('═', width + 2) + "╝" + Reset);
    }

    public static void Pause(string text = "Нажмите Enter, чтобы продолжить")
    {
        Console.Write($"\n{Gray}  {text}{Reset} ");
        if (Console.ReadLine() is null)
        {
            Environment.Exit(0);
        }
    }

    public static string ReadLine(string prompt, string fallback)
    {
        Console.Write($"{Amber}  {prompt}{Reset} ");
        string? line = Console.ReadLine();
        if (line is null)
        {
            Environment.Exit(0);
        }
        line = line.Trim();
        return line.Length == 0 ? fallback : line;
    }

    /// <summary>Спрашивает число в диапазоне [min; max], пока не получит корректное.</summary>
    public static int ReadInt(string prompt, int min, int max)
    {
        while (true)
        {
            Console.Write($"{Amber}  {prompt}{Reset} ");
            string? line = Console.ReadLine();
            if (line is null)
            {
                Environment.Exit(0);
            }
            if (int.TryParse(line.Trim(), out int value) && value >= min && value <= max)
            {
                return value;
            }
            Console.WriteLine($"{Red}  Введите число от {min} до {max}.{Reset}");
        }
    }
}
