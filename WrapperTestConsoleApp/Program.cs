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
                var content = NativeHttpWrapper.GetContent("http://172.16.111.144/cubetser/api/Resource/11412");
                Console.WriteLine("Managed result is:");
                Console.WriteLine(content);
                Console.ReadLine();
            }
        }
    }
}
