using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace ConsoleAppTest {
    class Program {

        static void hoge() {
            Console.WriteLine(DateTime.Now);
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
