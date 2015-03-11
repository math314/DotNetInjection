using System;
using System.Collections.Generic;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using System.Linq;

namespace HakoniwaProfiler.MethodHook {
    public class MethodHook {

        static public string getStr1()
        {
            Console.WriteLine("[!] HakoniwaProfiler.MethodHook.MethodHook.getStr1");
            return "HHHH";
        }

        static public DateTime get_Now()
        {
            Console.WriteLine("[!] HakoniwaProfiler.MethodHook.MethodHook.get_Now");
            return new DateTime(2000,1,1);
        }

        static public string haveArguments(string arg1, string arg2)
        {
            Console.WriteLine("[!] HakoniwaProfiler.MethodHook.MethodHook.haveArguments");
            return string.Format("{0} + {1}", arg1, arg2);
        }

        static public string haveManyArguments(string arg1, string arg2, string arg3, double arg4, int arg5, int arg6)
        {
            Console.WriteLine("[!] HakoniwaProfiler.MethodHook.MethodHook.haveManyArguments");
            return string.Format("{0} + {1} + {2} + {3} + {4} + {5}", arg1, arg2, arg3, arg4, arg5, arg6);
        }
        
        static public int intarg2(int x, int b)
        {
            Console.WriteLine("[!] HakoniwaProfiler.MethodHook.MethodHook.intarg2");
            return x + b;
        }


        static public string Replace(string input, string pattern, string replacement)
        {
            return Replace(new Regex(pattern), input, replacement);
        }

        static public string Replace(Regex regex, string input, string replacement)
        {
            Console.WriteLine("[!] HakoniwaProfiler.MethodHook.MethodHook.Replace");

            var pattern_info = typeof(Regex).GetField("pattern", BindingFlags.NonPublic | BindingFlags.Instance);
            string pattern = (string)pattern_info.GetValue(regex);
            Console.WriteLine("------------------------------");
            Console.WriteLine(string.Format("input = {0}\npattern = {1},pattern_length = {2}", input, pattern, pattern.Length));
            Console.WriteLine("------------------------------");

            return input;
        }

        static public string test1(int a, string b)
        {
            Console.WriteLine("[!] HakoniwaProfiler.MethodHook.MethodHook.test1");
            return "";
        }

        static public string test2(ConsoleAppTest.TestClass x, string b)
        {
            Console.WriteLine("[!] HakoniwaProfiler.MethodHook.MethodHook.test2");
            return "";
        }
    }
}
