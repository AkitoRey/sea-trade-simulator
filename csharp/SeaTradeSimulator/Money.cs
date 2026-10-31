namespace SeaTradeSimulator
{
    /// <summary>
    /// Небольшое значение без собственной «личности» - структура (тип-значение).
    /// </summary>
    public struct Money
    {
        public int Coins { get; set; }

        public override string ToString() => $"{Coins} монет";
    }
}