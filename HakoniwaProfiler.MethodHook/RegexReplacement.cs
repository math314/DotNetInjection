using System;
using System.Collections.Generic;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;

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

        static public string Replace0(string input, string pattern, string replacement)
        {
            Console.WriteLine("[!] HakoniwaProfiler.MethodHook.RegexReplacement.Replace0");
            return input;
        }

        static public string Replace1(Regex regex, string input, string replacement)
        {
            var pattern_info = typeof(Regex).GetField("pattern", BindingFlags.NonPublic | BindingFlags.Instance);
            string pattern = (string)pattern_info.GetValue(regex);
            System.Diagnostics.Debug.Write(string.Format("input = {0},pattern = {1}", input, pattern));

            if (input == null) {
                throw new ArgumentNullException("input");
            }

            var use_opetion_r = typeof(Regex).GetMethod("UseOptionR", BindingFlags.NonPublic | BindingFlags.Instance);
            bool useOptionR = (bool)use_opetion_r.Invoke(regex,new object[0]);

            return regex.Replace(input, replacement, -1, useOptionR ? input.Length : 0);
        }

    }
}
