using System;
using System.Collections.Generic;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using System.Linq;

namespace HakoniwaProfiler.MethodHook {
    public class RegexReplacement {

        static public string getStr1()
        {
            Console.WriteLine("[!] HakoniwaProfiler.MethodHook.RegexReplacement.getStr1");
            return "HHHH";
        }

        static public DateTime get_Now()
        {
            Console.WriteLine("[!] HakoniwaProfiler.MethodHook.RegexReplacement.get_Now");
            return new DateTime(2000,1,1);
        }

        static public string haveArguments(string arg1, string arg2)
        {
            Console.WriteLine("[!] HakoniwaProfiler.MethodHook.RegexReplacement.haveArguments");
            return string.Format("{0} + {1}", arg1, arg2);
        }

        static public string haveManyArguments(string arg1, string arg2, string arg3, double arg4, int arg5, int arg6)
        {
            Console.WriteLine("[!] HakoniwaProfiler.MethodHook.RegexReplacement.haveManyArguments");
            return string.Format("{0} + {1} + {2} + {3} + {4} + {5}", arg1, arg2, arg3, arg4, arg5, arg6);
        }
        
        static public int intarg2(int x, int b)
        {
            Console.WriteLine("[!] HakoniwaProfiler.MethodHook.RegexReplacement.intarg2");
            return x + b;
        }

        static public bool IsMatch(Regex regex, string input, int startat) {
            System.Diagnostics.Debug.WriteLine("[!] HakoniwaProfiler.MethodHook.RegexReplacement.IsMatch");
            if (input == null) {
                throw new ArgumentNullException("input");
            }
            var mi = typeof(Regex).GetMethod("Run", BindingFlags.NonPublic | BindingFlags.Instance);
            return null == mi.Invoke(regex, new object[] { true, -1, input, 0, input.Length, startat });
            // return null == regex.Run(true, -1, input, 0, input.Length, startat);
        }

        static public string Replace(string input, string pattern, string replacement)
        {
            // Console.WriteLine("[!] HakoniwaProfiler.MethodHook.RegexReplacement.Replace");
            return Replace(new Regex(pattern), input, replacement);
        }

        static public string Replace(Regex regex, string input, string replacement)
        {
            Console.WriteLine("[!] HakoniwaProfiler.MethodHook.RegexReplacement.Replace");

            var pattern_info = typeof(Regex).GetField("pattern", BindingFlags.NonPublic | BindingFlags.Instance);
            string pattern = (string)pattern_info.GetValue(regex);
            System.Diagnostics.Debug.WriteLine("------------------------------");
            System.Diagnostics.Debug.WriteLine(string.Format("input = {0}\npattern = {1},pattern_length = {2}", input, pattern, pattern.Length));
            System.Diagnostics.Debug.WriteLine("------------------------------");

            if (input == null) {
                throw new ArgumentNullException("input");
            }

            if (pattern.Length == 40) {

            }

            var use_opetion_r = typeof(Regex).GetMethod("UseOptionR", BindingFlags.NonPublic | BindingFlags.Instance);
            bool useOptionR = (bool)use_opetion_r.Invoke(regex,new object[0]);

            return input;
            // return regex.Replace(input, replacement, -1, useOptionR ? input.Length : 0);
        }

        //public static int IndexOf(String obj, string value) {
        //    return obj.IndexOf(value, StringComparison.Ordinal);
        //}

        static public string test1(int a, string b)
        {
            Console.WriteLine("[!] HakoniwaProfiler.MethodHook.RegexReplacement.test1");
            return "";
        }

        static public string test2(ConsoleAppTest.TestClass x, string b)
        {
            Console.WriteLine("[!] HakoniwaProfiler.MethodHook.RegexReplacement.test2");
            return "";
        }
    }
}
