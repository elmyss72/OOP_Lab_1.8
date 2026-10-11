//////////////////////////////////////////////////////////////////////////////
// Bankomat.h
//                заголовний файл – визначення класу Bankomat з вкладеним класом Money
#pragma once
#include <string>

// Допоміжна функція: безпечне введення дробового числа з консолі
// (приймає і крапку, і кому; повторює запит при помилці)
double ReadNumber(const std::string& prompt);

class Bankomat
{
public:
    // ---------- Вкладений клас Money: сума = кількість купюр/монет кожного номіналу ----------
    class Money
    {
    public:
        enum { N = 15 };                          // кількість номіналів
        static long long Nominal(int i);          // номінал №i у копійках: 0 -> 500 грн ... 14 -> 1 коп
        static int NominalIndex(double hrn);      // номер номіналу за значенням у грн (-1, якщо такого немає)
        static Money FromKop(long long kop);      // сума в копійках -> мінімальна кількість купюр

        Money();                                  // нульова сума

        int  GetCount(int i) const;               // методи доступу до полів-номіналів
        bool SetCount(int i, int value);          // з перевіркою коректності
        long long TotalKop() const;               // вся сума в копійках

        bool Init(double hrn);                    // ініціалізація з дробового числа (грн.коп)
        void Read();
        void Display() const;
        std::string toString() const;

        Money  Add(const Money& o) const;         // додавання сум (купюри додаються по номіналах)
        Money  Subtract(const Money& o) const;    // віднімання (результат не менший за 0)
        double Divide(const Money& o) const;      // ділення суми на суму (0, якщо дільник = 0)
        Money  Divide(double k) const;            // ділення суми на дробове число
        Money  Multiply(double k) const;          // множення на дробове число
        bool Take(long long kop, Money& taken);   // видати суму наявними купюрами

        bool Equal(const Money& o) const;         // операції порівняння
        bool NotEqual(const Money& o) const;
        bool Less(const Money& o) const;
        bool Greater(const Money& o) const;
        bool LessOrEqual(const Money& o) const;
        bool GreaterOrEqual(const Money& o) const;

    private:
        int cnt[N];                               // cnt[i] – кількість купюр номіналу Nominal(i)
    };

    // Результат операції зняття грошей
    enum Result { OK, BELOW_MIN, ABOVE_MAX, NOT_ENOUGH, CANNOT_ISSUE };
    static const char* ResultText(Result r);

    Bankomat();

    int   GetId() const { return id; }
    Money GetCash() const { return cash; }
    Money GetMinSum() const { return minSum; }
    Money GetMaxSum() const { return maxSum; }
    bool  SetId(int value);
    void  SetCash(const Money& value) { cash = value; }
    bool  SetMinSum(const Money& value);
    bool  SetMaxSum(const Money& value);

    bool   Init(int id, double minHrn, double maxHrn);
    void   Read();
    void   Display() const;
    std::string toString() const;                 // сума, що залишилась у банкоматі

    void   Load(const Money& m);                  // завантаження купюр
    Result Withdraw(const Money& amount, Money& given);  // зняття суми

private:
    int   id;
    Money cash;                                   // поточна сума в банкоматі
    Money minSum, maxSum;                         // межі суми, яку можна зняти
};