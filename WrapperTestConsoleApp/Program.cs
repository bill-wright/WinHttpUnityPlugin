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
    using System.Collections.Generic;
    using System.Net;
    using System.Text;

    class Program
    {
        private static readonly string CHARSET = "UTF-8";
        private static readonly string ACCEPT_TYPES = "application/xml, application/zip, text/plain, application/octet-stream";

        static void TestGet() {
            byte[] content;
            var headers = new Dictionary<string, string>() {
                    { "Accept-Charset", CHARSET },
                };
            //var httpCode = WinHttpUnityPluginWrapper.NativeHttpWrapper.GetContent("http://localhost/dummy/WorkArea/8ed2fce4-fc11-2db0-e053-24690b99b275", out content);
            var httpCode = WinHttpUnityPluginWrapper.NativeHttpWrapper.GetContent("http://localhost/WorkDefinition/WorkOrder/foo", out content, headers);
            Console.Write("GET Managed result code is:");
            Console.WriteLine(httpCode);
            Console.WriteLine(System.Text.Encoding.Default.GetString(content));
        }

        static void TestPost() {
            byte[] content;
            byte[] post_body = Encoding.ASCII.GetBytes("<somexml/>");
            var headers = new Dictionary<string, string>() {
                    { "Accept-Charset", CHARSET},
                    { "Accept", ACCEPT_TYPES},
                    { "Content-Type", "application/xml; charset=utf-8" },
                    { "Content-Length", post_body.Length.ToString() },
                };
            var httpCode = WinHttpUnityPluginWrapper.NativeHttpWrapper.PostContent("http://localhost/DesignProductStructure/DesignComponent/IVF/GetByUuid", post_body, out content, headers);
            Console.WriteLine($"Managed result code is: {httpCode} number of bytes: {content.Length}");
        }

        static void TestPut() {
            byte[] content;
            string json = "{\"InfoTags\": [{\"infoTagQuaternion\": {\"w\": 0.5992824, \"x\": 0.0, \"y\": 0.0, \"z\": -0.8005377}, \"itAnchor\": {\"x\": 3441.30273, \"y\": -32.98762, \"z\": 488.0143}, \"itCaption\": \"YY - DYPN\", \"itColor\": \"GREEN\", \"itCreateDate\": 1610662654114, \"itNotes\": [{\"noteContent\": \"Part identified: YY-DYPN\\nPart Class: 'A part class'\", \"noteCreateDate\": 1610662664786, \"noteCreatedBy\": \"wwright\", \"uuidPk\": \"1822f3f3-07b3-4474-a441-039d7164f4be\"}], \"itRefs\": [], \"itStyle\": \"ARROW\", \"ituuid\": \"0a128939-edb1-46eb-94b8-5b901263149d\", \"itWorkOrder\": \"BEAMER-TEST-FACES\"}]}";
            byte[] post_body = Encoding.ASCII.GetBytes(json);
            var headers = new Dictionary<string, string>() {
                    { "Accept-Charset", CHARSET},
                    { "Accept", ACCEPT_TYPES},
                    { "Content-Type", "application/json; charset=utf-8" },
                    { "Content-Length", post_body.Length.ToString() },
                };

            var httpCode = WinHttpUnityPluginWrapper.NativeHttpWrapper.PutContent("http://localhost/stuff/tag", post_body, out content, headers);
            Console.WriteLine($"Managed result code is: {httpCode} number of bytes: {content.Length}");
        }

        static void Main(string[] args)
        {
            Console.OutputEncoding = Encoding.UTF8;
            while (true)
            {
                //                TestGet();
//                TestPost();
                TestPut();
                Console.WriteLine("DONE.  -- return to go again");
                Console.ReadLine();
            }
        }
    }
}
