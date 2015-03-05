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
        
        static int intarg2(int x, int b)
        {
            Console.WriteLine("[!] HakoniwaProfiler.MethodHook.RegexReplacement.intarg2");
            return x + b;
        }

        static string a(MatchEvaluator evaluator, Regex regex, string input, int count, int startat)
        {
            return Replace(evaluator, regex, input, count, startat);
        }

        static string Replace(MatchEvaluator evaluator, Regex regex, string input, int count, int startat) {
            var pattern_info = typeof(Regex).GetField("pattern", BindingFlags.NonPublic | BindingFlags.Instance);
            string pattern = (string)pattern_info.GetValue(regex);
            System.Diagnostics.Debug.Write(string.Format("input = {0},pattern = {1}", input, pattern));

            if (evaluator == null) {
                throw new ArgumentNullException("evaluator");
            }
            if (count < -1) {
                throw new ArgumentOutOfRangeException("count", ("CountTooSmall"));
            }
            if (startat < 0 || startat > input.Length) {
                throw new ArgumentOutOfRangeException("startat", ("BeginIndexNotNegative"));
            }
            if (count == 0) {
                return input;
            }
            Match match = regex.Match(input, startat);
            if (!match.Success) {
                return input;
            }
            StringBuilder stringBuilder;
            if (!regex.RightToLeft) {
                stringBuilder = new StringBuilder();
                int num = 0;
                do {
                    if (match.Index != num) {
                        stringBuilder.Append(input, num, match.Index - num);
                    }
                    num = match.Index + match.Length;
                    stringBuilder.Append(evaluator(match));
                    if (--count == 0) {
                        break;
                    }
                    match = match.NextMatch();
                }
                while (match.Success);
                if (num < input.Length) {
                    stringBuilder.Append(input, num, input.Length - num);
                }
            } else {
                List<string> list = new List<string>();
                int num2 = input.Length;
                do {
                    if (match.Index + match.Length != num2) {
                        list.Add(input.Substring(match.Index + match.Length, num2 - match.Index - match.Length));
                    }
                    num2 = match.Index;
                    list.Add(evaluator(match));
                    if (--count == 0) {
                        break;
                    }
                    match = match.NextMatch();
                }
                while (match.Success);
                stringBuilder = new StringBuilder();
                if (num2 > 0) {
                    stringBuilder.Append(input, 0, num2);
                }
                for (int i = list.Count - 1; i >= 0; i--) {
                    stringBuilder.Append(list[i]);
                }
            }
            return stringBuilder.ToString();
        }

        static void hoge(int a, double b, string c) {
            int x = 1, y = 2;
            RefTest(ref x, out y);
        }

        static void RefTest(ref int a,out int b){
            a = 1;
            b = 2;
        }

        static void Main(string[] args) {

            get_Now();
            hoge(1, 3.0, "aaa");

            System.Collections.ArrayList list = new System.Collections.ArrayList();
            var x = 1;
            var y = 2;
            list.Add(x);
            list.Add(y);
            list.Add("asdf");

            object[] a = new object[10];
            a[0] = x;
            a[1] = y;
            a[2] = "asdf";
        }
    }
}
