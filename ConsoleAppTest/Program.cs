using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace ConsoleAppTest {
    class Program {

        static string getStr1()
        {
            Console.WriteLine("ConsoleAppTest.Program.getStr1");
            return "getStr1";
        }

        static DateTime getDate()
        {
            Console.WriteLine("ConsoleAppTest.Program.getDate");
            return DateTime.Now;
        }

        static string getStr2()
        {
            return "getStr2";
        }

        static string fff()
        {
            return getStr1();
            // return get_Now_dummy();
        }

        static string haveArguments(string arg1, string arg2)
        {
            return string.Format("{0} + {1}", arg1, arg2);
        }

        static string haveManyArguments(string arg1, string arg2, string arg3, double arg4, int arg5, int arg6)
        {
            return string.Format("{0} + {1} + {2} + {3} + {4} + {5}", arg1, arg2, arg3, arg4, arg5, arg6);
        }

        static string haveManyArguments_(string arg1, string arg2, string arg3, double arg4, int arg5, int arg6)
        {
            return haveManyArguments(arg1, arg2, arg3, arg4, arg5, arg6);
        }

        static int intarg2(int x,int b)
        {
            return x + b;
        }

        static void hoge() {
            Console.WriteLine(fff());
            Console.WriteLine(getDate());
            Console.WriteLine(haveArguments("aa", "bb"));
            Console.WriteLine(intarg2(1,2));
            Console.WriteLine(haveManyArguments_("a", "b", "c", 2.0, 3, 4));
            string a = Regex.Replace("poyohugapoyopiyo", "piyo", "xxxx");
            Console.WriteLine(a);
            string b = new Regex("poyo").Replace("poyohugapoyopiyo", "xxxx");
            Console.WriteLine(b);

        }

        static void Main(string[] args) {
            hoge();
            var x = TestClass.test1(1, "aaa");
            Console.WriteLine(x);
            var y = new TestClass(1).test2("aaa");
            Console.WriteLine(y);
        }
    }

    public class TestClass {

        int _a;

        public TestClass(int a)
        {
            _a = a;
        }

        public string test2(string b)
        {
            Console.WriteLine(b);
            return b;
        }

    public static string test1(int a,string b)
        {
            Console.WriteLine(b);
            return b;
        }
    }
}
