using System;

namespace SeaTradeSimulator
{
    /// <summary>
    /// Судовой журнал: ресурс, который нужно закрыть вовремя.
    /// Заменяет деструктор из C++ явным вызовом Dispose через using.
    /// </summary>
    public sealed class VoyageLog : IDisposable
    {
        private readonly string _title;

        public VoyageLog(string title)
        {
            _title = title;

            Console.WriteLine($"[+] журнал открыт: {_title}");
        }

        public void Write(string line) => Console.WriteLine($"    {line}");

        public void Dispose() => Console.WriteLine($"[-] журнал закрыт: {_title}");
    }
}