//////////////////////////////////////////////////////////////////////////////
// OOP_lab_1.8.cpp  
//                головний файл проекту – функція main (меню для роботи з банкоматом)
#include "Bankomat.h"
#include <iostream>
using namespace std;
typedef Bankomat::Money Money;

// Завантаження купюр: користувач вводить номінал та кількість
void LoadMenu(Bankomat& atm)
{
    int i = Money::NominalIndex(ReadNumber("  Nominal (hrn: 500,200,...,1 abo 0.5, 0.25, ... 0.01): "));
    if (i < 0) { cout << "  Takoho nominalu nemaye!\n"; return; }
    int k = (int)ReadNumber("  Kilkist: ");
    Money m;
    if (!m.SetCount(i, k)) { cout << "  Nekorektna kilkist!\n"; return; }
    atm.Load(m);
    cout << "  Zavantazheno " << m.toString() << ". Zalyshok: " << atm.toString() << endl;
}

// Зняття грошей
void WithdrawMenu(Bankomat& atm)
{
    Money req, given;
    if (!req.Init(ReadNumber("  Suma dlia znyattia (hrn): "))) { cout << "  Nekorektna suma!\n"; return; }
    Bankomat::Result r = atm.Withdraw(req, given);
    cout << "  " << Bankomat::ResultText(r) << endl;
    if (r == Bankomat::OK)
    {
        cout << "  Vydano:" << endl;
        given.Display();
        cout << "  Zalyshok: " << atm.toString() << endl;
    }
}

// Демонстрація операцій над сумами (динамічний масив об'єктів Money)
void MoneyMenu()
{
    Money* m = new Money[2];
    for (int i = 0; i < 2; i++)
    {
        cout << " Suma " << i + 1 << ":" << endl;
        m[i].Read();
    }
    double k = ReadNumber(" Drobove chyslo k: ");
    cout << "\n A + B   = " << m[0].Add(m[1]).toString() << endl;
    cout << " A - B   = " << m[0].Subtract(m[1]).toString() << endl;
    cout << " A / B   = " << m[0].Divide(m[1]) << endl;
    cout << " A / k   = " << m[0].Divide(k).toString() << endl;
    cout << " A * k   = " << m[0].Multiply(k).toString() << endl;
    cout << boolalpha;
    cout << " A == B: " << m[0].Equal(m[1]) << "   A != B: " << m[0].NotEqual(m[1]) << endl;
    cout << " A <  B: " << m[0].Less(m[1]) << "   A >  B: " << m[0].Greater(m[1]) << endl;
    cout << " A <= B: " << m[0].LessOrEqual(m[1]) << "   A >= B: " << m[0].GreaterOrEqual(m[1]) << endl;
    delete[] m;
}

int main()
{
    cout << "=== Laboratorna robota 1.8, variant 24: Bankomat ===\n\n";

    Bankomat atms[2];                                // статичний масив об'єктів
    cout << "Bankomat 1 - vvedennia z klaviatury:\n";
    atms[0].Read();                                  // спосіб 1: введення (Read)
    atms[1].Init(2, 50, 5000);                       // спосіб 2: Init (ID=2, від 50 до 5000 грн)
    cout << "Bankomat 2 stvoreno cherez Init (ID=2, vid 50 do 5000 hrn)\n";

    int cur = 0, choice;
    do
    {
        cout << "\n----- Bankomat " << atms[cur].GetId() << " -----\n"
            << " 1 - Pokazaty stan\n 2 - Zavantazhyty kupiury\n 3 - Znyaty hroshi\n"
            << " 4 - Operatsii z sumamy (Money)\n 5 - Inshyi bankomat\n 0 - Vykhid\n";
        choice = (int)ReadNumber("Vash vybir: ");
        switch (choice)
        {
        case 1: atms[cur].Display();
            cout << "  toString(): " << atms[cur].toString() << endl; break;
        case 2: LoadMenu(atms[cur]); break;
        case 3: WithdrawMenu(atms[cur]); break;
        case 4: MoneyMenu(); break;
        case 5: cur = 1 - cur; break;
        case 0: cout << "Do pobachennia!\n"; break;
        default: cout << "Nevirnyi punkt menu!\n";
        }
    } while (choice != 0);
    return 0;
}