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

        static string getStr2()
        {
            return "getStr2";
        }

        static string fff()
        {
            return getStr1();
            // return get_Now_dummy();
        }

        static string sa()
        {
            return HakoniwaProfiler.MethodHook.RegexReplacement.getStr1();
        }

        static void hoge() {
            Console.WriteLine(DateTime.Now);
            Console.WriteLine(fff());
            string a = Regex.Replace("poyohugapoyopiyo", "poyo", "xxxx");
            Console.WriteLine(a);
        }

        static void Main(string[] args) {
            hoge();
            string fuga = Console.ReadLine();
            Console.WriteLine(fuga);
        }
    }
}
