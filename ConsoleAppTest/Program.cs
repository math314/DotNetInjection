using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ConsoleAppTest {
    class Program {

        static void hoge() {
            Console.WriteLine(DateTime.Now);
        }

        static void Main(string[] args) {
            hoge();
            string fuga = Console.ReadLine();
            Console.WriteLine(fuga);
        }
    }
}
