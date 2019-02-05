using System;
using System.Text;

namespace WrapperTestConsoleApp
{
    using WinHttpUnityPluginWrapper;

    class Program
    {
        static void Main(string[] args)
        {
            Console.OutputEncoding = Encoding.UTF8;
            var content = NativeHttpWrapper.GetContent("http://172.16.111.144/cubetser/api/widgetdescription/115");
            Console.WriteLine("Managed result is:");
            Console.WriteLine(content);
            Console.ReadLine();
        }
    }
}
