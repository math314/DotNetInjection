using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Injector {
    class Program {

        const string PROFILER_UUID = "{9992F2A6-DF35-472B-AD3E-317F85D958D7}";
        const string PROFILER_NAME = "HakoniwaProfiler.dll";

        void StartTarget(string targetPath)
        {
            string current_dir = Directory.GetCurrentDirectory();
            string profiler_path = Path.Combine(current_dir, PROFILER_NAME);

            ProcessStartInfo psi = new ProcessStartInfo();
            psi.FileName = targetPath;
            psi.UseShellExecute = false;

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
