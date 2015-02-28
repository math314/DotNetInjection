using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Injector {
    class Program {

        const string PROFILER_UUID = "{941EC77E-F7C0-42C8-84E1-15FEFA3CE96F}";
        const string PROFILER_NAME = "ILRewriteProfiler.dll";

        void StartTarget(string targetPath)
        {
            string current_dir = Directory.GetCurrentDirectory();
            string profiler_path = Path.Combine(current_dir, PROFILER_NAME);
            string profiler_output_path = Path.Combine(current_dir, "target_output.txt");
            string profiler_debug_path = Path.Combine(current_dir, "target_debug.txt");

            ProcessStartInfo psi = new ProcessStartInfo();
            psi.FileName = targetPath;
            psi.UseShellExecute = false;

            psi.EnvironmentVariables.Add("ILREWRITE_PROFILER_OUTPUT", profiler_output_path);
            psi.EnvironmentVariables.Add("ILREWRITE_PROFILER_DEBUG", profiler_debug_path);

            psi.EnvironmentVariables.Add("COR_ENABLE_PROFILING", "1");
            psi.EnvironmentVariables.Add("COR_PROFILER", PROFILER_UUID);
            psi.EnvironmentVariables.Add("COR_PROFILER_PATH", profiler_path);
            psi.EnvironmentVariables.Add("COMPLUS_Version", "v4.0.30319"); //.netのバージョンを強制的に4.0に固定する
            using (var proc = Process.Start(psi)) {
                Console.WriteLine("{0} started.",proc.ProcessName);
                proc.WaitForExit();
            }
        }

        static void Main(string[] args)
        {
            if (args.Length != 1) {
                Console.WriteLine("usage : Injector.exe <target app>");
                return;
            }
            new Program().StartTarget(args[0]);
        }
    }
}
