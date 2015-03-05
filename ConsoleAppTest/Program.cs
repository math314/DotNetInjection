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

        static void hoge() {
            Console.WriteLine(fff());
            Console.WriteLine(getDate());
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
