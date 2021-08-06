// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Program.cs" company="Open Source">
//   2019
// </copyright>
// <summary>
//   Defines the Program type.
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace WrapperTestConsoleApp
{
    using System;
    using System.Text;

    class Program
    {
        static void Main(string[] args)
        {
            Console.OutputEncoding = Encoding.UTF8;
            while (true)
            {
                var content = WinHttpUnityPluginWrapper.NativeHttpWrapper.GetContent("http://localhost/WorkDefinition/WorkOrder/foo");
                Console.WriteLine("Managed result is:");
                Console.WriteLine(content);
                Console.ReadLine();
            }
        }
    }
}
