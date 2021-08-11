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
                byte[] content;
                var httpCode = WinHttpUnityPluginWrapper.NativeHttpWrapper.GetContent("http://localhost/dummy/WorkArea/8ed2fce4-fc11-2db0-e053-24690b99b275", out content);
//                var httpCode = WinHttpUnityPluginWrapper.NativeHttpWrapper.GetContent("http://localhost/WorkDefinition/WorkOrder/foo", out content);

                //byte[] post_body = Encoding.ASCII.GetBytes("<somexml/>");
                //var httpCode = WinHttpUnityPluginWrapper.NativeHttpWrapper.PostContent("http://localhost/DesignProductStructure/DesignComponent/IVF/GetByUuid", post_body, out content);
                Console.Write("Managed result code is:");
                Console.WriteLine(httpCode);
                Console.WriteLine(System.Text.Encoding.Default.GetString(content));
                Console.ReadLine();
            }
        }
    }
}
