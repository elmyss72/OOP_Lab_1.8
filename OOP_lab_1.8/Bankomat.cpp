//////////////////////////////////////////////////////////////////////////////
// Bankomat.cpp
//                файл реалізації – реалізація методів класів Bankomat та Bankomat::Money
#include "Bankomat.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <algorithm>
using namespace std;

double ReadNumber(const string& prompt)
{
    while (true)
    {
        cout << prompt;
        string s;
        if (!getline(cin, s)) exit(0);               // кінець вводу – виходимо
        replace(s.begin(), s.end(), ',', '.');       // кома -> крапка
        istringstream in(s);
        double x;
        if (in >> x && (in >> ws).eof()) return x;   // у рядку лише одне число
        cout << "  Pomylka vvodu! Sprobuyte shche raz.\n";
    }
}

////////////////////////////////////////////////////////////
// class Bankomat::Money

// номінали в копійках: 500, 200, 100, 50, 20, 10, 5, 2, 1 грн та 50, 25, 10, 5, 2, 1 коп
static const long long NOM[Bankomat::Money::N] =
{ 50000, 20000, 10000, 5000, 2000, 1000, 500, 200, 100, 50, 25, 10, 5, 2, 1 };
static const int MAX_COUNT = 1000000;                // верхня межа кількості купюр одного номіналу

long long Bankomat::Money::Nominal(int i) { return NOM[i]; }

int Bankomat::Money::NominalIndex(double hrn)
{
    long long kop = llround(hrn * 100);
    for (int i = 0; i < N; i++)
        if (NOM[i] == kop) return i;
    return -1;
}

Bankomat::Money Bankomat::Money::FromKop(long long kop)
{
    Money m;
    for (int i = 0; i < N; i++)                      // жадібно: від найбільшого номіналу
    {
        m.cnt[i] = (int)(kop / NOM[i]);
        kop %= NOM[i];
    }
    return m;
}

Bankomat::Money::Money() { for (int i = 0; i < N; i++) cnt[i] = 0; }

int Bankomat::Money::GetCount(int i) const { return (i >= 0 && i < N) ? cnt[i] : 0; }

bool Bankomat::Money::SetCount(int i, int value)
{
    if (i < 0 || i >= N || value < 0 || value > MAX_COUNT) return false;   // перевірку не обійти
    cnt[i] = value;
    return true;
}

long long Bankomat::Money::TotalKop() const
{
    long long s = 0;
    for (int i = 0; i < N; i++) s += cnt[i] * NOM[i];
    return s;
}

bool Bankomat::Money::Init(double hrn)
{
    if (!(hrn >= 0 && hrn <= 1e7)) return false;     // від'ємні, завеликі та NaN відхиляємо
    *this = FromKop(llround(hrn * 100));
    return true;
}

void Bankomat::Money::Read()
{
    while (!Init(ReadNumber("  Vvedit sumu (hrn, napr. 150.50): ")))
        cout << "  Suma maye buty vid 0 do 10000000!\n";
}

string Bankomat::Money::toString() const
{
    long long t = TotalKop();
    ostringstream s;                                 // копійки відокремлені комою
    s << t / 100 << ',' << setw(2) << setfill('0') << t % 100 << " hrn";
    return s.str();
}

void Bankomat::Money::Display() const
{
    cout << "  Suma: " << toString() << endl;
    for (int i = 0; i < N; i++)
        if (cnt[i] > 0)
            cout << "    " << setw(4) << (NOM[i] >= 100 ? NOM[i] / 100 : NOM[i])
            << (NOM[i] >= 100 ? " hrn" : " kop") << "  x " << cnt[i] << endl;
}

Bankomat::Money Bankomat::Money::Add(const Money& o) const
{
    Money r;
    for (int i = 0; i < N; i++) r.cnt[i] = cnt[i] + o.cnt[i];
    return r;
}

Bankomat::Money Bankomat::Money::Subtract(const Money& o) const
{
    long long d = TotalKop() - o.TotalKop();
    return FromKop(d > 0 ? d : 0);                   // сума не може бути від'ємною
}

double Bankomat::Money::Divide(const Money& o) const
{
    return o.TotalKop() == 0 ? 0 : (double)TotalKop() / o.TotalKop();
}

Bankomat::Money Bankomat::Money::Divide(double k) const
{
    return k > 0 ? Multiply(1 / k) : Money();        // ділення на 0 або від'ємне -> нульова сума
}

Bankomat::Money Bankomat::Money::Multiply(double k) const
{
    double r = TotalKop() * k;                       // результат округлюємо до копійки
    return (r >= 0 && r < 1e11) ? FromKop(llround(r)) : Money();
}

bool Bankomat::Money::Take(long long kop, Money& taken)
{
    Money t;
    for (int i = 0; i < N && kop > 0; i++)           // беремо найбільші наявні купюри
    {
        long long k = min<long long>(kop / NOM[i], cnt[i]);
        t.cnt[i] = (int)k;
        kop -= k * NOM[i];
    }
    if (kop != 0) return false;                      // видати точно цю суму не можна
    for (int i = 0; i < N; i++) cnt[i] -= t.cnt[i];
    taken = t;
    return true;
}

bool Bankomat::Money::Equal(const Money& o) const { return TotalKop() == o.TotalKop(); }
bool Bankomat::Money::NotEqual(const Money& o) const { return !Equal(o); }
bool Bankomat::Money::Less(const Money& o) const { return TotalKop() < o.TotalKop(); }
bool Bankomat::Money::Greater(const Money& o) const { return TotalKop() > o.TotalKop(); }
bool Bankomat::Money::LessOrEqual(const Money& o) const { return !Greater(o); }
bool Bankomat::Money::GreaterOrEqual(const Money& o) const { return !Less(o); }

////////////////////////////////////////////////////////////
// class Bankomat

Bankomat::Bankomat() : id(0) {}

bool Bankomat::SetId(int value)
{
    if (value <= 0) return false;
    id = value;
    return true;
}

bool Bankomat::SetMinSum(const Money& value)
{
    if (value.TotalKop() <= 0) return false;
    if (maxSum.TotalKop() > 0 && value.Greater(maxSum)) return false;   // min <= max
    minSum = value;
    return true;
}

bool Bankomat::SetMaxSum(const Money& value)
{
    if (value.TotalKop() <= 0 || value.Less(minSum)) return false;      // max >= min
    maxSum = value;
    return true;
}

bool Bankomat::Init(int newId, double minHrn, double maxHrn)
{
    Money mn, mx;
    if (!mn.Init(minHrn) || !mx.Init(maxHrn)) return false;
    Bankomat t;                                      // спершу заповнюємо тимчасовий об'єкт
    if (!t.SetId(newId) || !t.SetMinSum(mn) || !t.SetMaxSum(mx)) return false;
    *this = t;                                       // повторне використання перевірок set-методів
    return true;
}

void Bankomat::Read()
{
    while (true)
    {
        int i = (int)ReadNumber("  ID bankomatu (tsile > 0): ");
        double a = ReadNumber("  Minimalna suma znyattia (hrn): ");
        double b = ReadNumber("  Maksymalna suma znyattia (hrn): ");
        if (Init(i, a, b)) return;
        cout << "  Pomylka: potribno ID > 0 ta 0 < min <= max.\n";
    }
}

string Bankomat::toString() const { return cash.toString(); }

void Bankomat::Display() const
{
    cout << "Bankomat ID = " << id << endl;
    cout << "  Znyattia vid " << minSum.toString() << " do " << maxSum.toString() << endl;
    cout << "  Zalyshok u bankomati:" << endl;
    cash.Display();
}

void Bankomat::Load(const Money& m) { cash = cash.Add(m); }

Bankomat::Result Bankomat::Withdraw(const Money& amount, Money& given)
{
    if (amount.Less(minSum))    return BELOW_MIN;    // перевірка коректності суми
    if (amount.Greater(maxSum)) return ABOVE_MAX;
    if (amount.Greater(cash))   return NOT_ENOUGH;
    return cash.Take(amount.TotalKop(), given) ? OK : CANNOT_ISSUE;
}

const char* Bankomat::ResultText(Result r)
{
    switch (r)
    {
    case OK:           return "Operatsiya uspishna";
    case BELOW_MIN:    return "Suma menshe za minimalnu";
    case ABOVE_MAX:    return "Suma perevyshchuye maksymalnu";
    case NOT_ENOUGH:   return "U bankomati nedostatno koshtiv";
    default:           return "Nemozhlyvo vydaty sumu naiavnymy kupiurmy";
    }
}