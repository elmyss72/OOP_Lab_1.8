//////////////////////////////////////////////////////////////////////////////
// UnitTest.cpp
//                Native Unit Test Project (Visual Studio) – тести класів Bankomat та Bankomat::Money
#include "pch.h"
#include "CppUnitTest.h"
#include "../OOP_lab_1.8/Bankomat.cpp"          // шлях залежить від структури вашого рішення
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using std::string;
typedef Bankomat::Money Money;

namespace Lab18Tests
{
    TEST_CLASS(MoneyTests)
    {
    public:
        TEST_METHOD(Init_NormalizesToBanknotes)
        {
            Money m;
            Assert::IsTrue(m.Init(1234.56));
            Assert::AreEqual(2, m.GetCount(0));                   // 2 банкноти по 500 грн
            Assert::AreEqual(123456LL, m.TotalKop());
            Assert::AreEqual(string("1234,56 hrn"), m.toString());
        }
        TEST_METHOD(Init_RejectsNegative)
        {
            Money m;
            Assert::IsFalse(m.Init(-1));
        }
        TEST_METHOD(SetCount_ValidatesArguments)
        {
            Money m;
            Assert::IsFalse(m.SetCount(0, -5));
            Assert::IsFalse(m.SetCount(99, 1));
            Assert::IsTrue(m.SetCount(8, 3));              // 3 банкноти по 1 грн
            Assert::AreEqual(300LL, m.TotalKop());
        }
        TEST_METHOD(AddAndSubtract)
        {
            Money a, b;
            a.Init(100.50); b.Init(40.25);
            Assert::AreEqual(14075LL, a.Add(b).TotalKop());
            Assert::AreEqual(6025LL, a.Subtract(b).TotalKop());
            Assert::AreEqual(0LL, b.Subtract(a).TotalKop()); // менше не буває
        }
        TEST_METHOD(MultiplyAndDivide)
        {
            Money a, b;
            a.Init(100); b.Init(40);
            Assert::AreEqual(15000LL, a.Multiply(1.5).TotalKop());
            Assert::AreEqual(4000LL, a.Divide(2.5).TotalKop());
            Assert::AreEqual(0LL, a.Divide(0.0).TotalKop());
            Assert::AreEqual(2.5, a.Divide(b), 1e-9);
        }
        TEST_METHOD(Comparison)
        {
            Money a, b, c;
            a.Init(10); b.Init(20); c.Init(10);
            Assert::IsTrue(a.Less(b) && b.Greater(a) && a.Equal(c));
            Assert::IsTrue(a.NotEqual(b) && a.LessOrEqual(c) && b.GreaterOrEqual(a));
        }
        TEST_METHOD(Take_FailsWhenNoSuitableBanknotes)
        {
            Money cash, out;
            cash.SetCount(0, 1);                           // лише 500 грн
            Assert::IsFalse(cash.Take(15000, out));
            Assert::AreEqual(50000LL, cash.TotalKop());    // стан не змінився
        }
    };

    TEST_CLASS(BankomatTests)
    {
        // допоміжний банкомат: ID=1, ліміт 100..1000, у ньому 2x500 + 5x100
        static Bankomat Make()
        {
            Bankomat b; b.Init(1, 100, 1000);
            Money m; m.SetCount(0, 2); m.SetCount(2, 5);
            b.Load(m);
            return b;
        }
    public:
        TEST_METHOD(Init_ValidatesLimits)
        {
            Bankomat b;
            Assert::IsFalse(b.Init(0, 100, 1000));         // ID <= 0
            Assert::IsFalse(b.Init(1, 0, 1000));           // min = 0
            Assert::IsFalse(b.Init(1, 500, 100));          // min > max
            Assert::IsTrue(b.Init(1, 100, 1000));
            Assert::AreEqual(1, b.GetId());
        }
        TEST_METHOD(Load_AddsMoney)
        {
            Bankomat b = Make();
            Assert::AreEqual(150000LL, b.GetCash().TotalKop());
            Assert::AreEqual(string("1500,00 hrn"), b.toString());
        }
        TEST_METHOD(Withdraw_Ok)
        {
            Bankomat b = Make(); Money req, given;
            req.Init(600);
            Assert::IsTrue(b.Withdraw(req, given) == Bankomat::OK);
            Assert::AreEqual(60000LL, given.TotalKop());
            Assert::AreEqual(90000LL, b.GetCash().TotalKop());
        }
        TEST_METHOD(Withdraw_BelowMin)
        {
            Bankomat b = Make(); Money req, given;
            req.Init(50);
            Assert::IsTrue(b.Withdraw(req, given) == Bankomat::BELOW_MIN);
        }
        TEST_METHOD(Withdraw_AboveMax)
        {
            Bankomat b = Make(); Money req, given;
            req.Init(1500);
            Assert::IsTrue(b.Withdraw(req, given) == Bankomat::ABOVE_MAX);
        }
        TEST_METHOD(Withdraw_NotEnoughMoney)
        {
            Bankomat b; b.Init(1, 100, 1000);              // порожній банкомат
            Money req, given; req.Init(500);
            Assert::IsTrue(b.Withdraw(req, given) == Bankomat::NOT_ENOUGH);
        }
        TEST_METHOD(Withdraw_CannotIssue)
        {
            Bankomat b = Make(); Money req, given;
            req.Init(150);                                 // немає купюр, щоб зібрати 150
            Assert::IsTrue(b.Withdraw(req, given) == Bankomat::CANNOT_ISSUE);
            Assert::AreEqual(150000LL, b.GetCash().TotalKop());
        }
    };
}