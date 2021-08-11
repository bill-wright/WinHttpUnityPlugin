
namespace WinHttpUnityPluginWrapper {
    using System;
    using System.Collections.Generic;
    using System.Runtime.InteropServices;
    using System.Text;

    /// <summary>
    /// Wrap native http access dll written on C++ for using in Unity apps for Windows desktop (editor and standonle)
    /// </summary>
    public static class NativeHttpWrapper {

        private static string FormatHeaders(Dictionary<string, string> headers) {
            string ret = "";
            if (headers != null) {
                foreach (var h in headers) {
                    ret += "\n" + h.Key + ": " + h.Value;
                }
            }
            return ret;
        }

        public static int GetContent(string url, out byte[] content, Dictionary<string, string> headers = null) {
            var parsedUrl = new UrlParts(url);
            WebResults wr = GetContent(parsedUrl.Host, parsedUrl.Port, parsedUrl.ApiMethod, FormatHeaders(headers));
            try {
                // Copy the array from unmanaged memory.
                content = new byte[wr.contentOutSize];
                Marshal.Copy(wr.contentOut, content, 0, wr.contentOutSize);
            } finally {
                // Free the unmanaged memory.
                Marshal.FreeHGlobal(wr.contentOut);
            }

            return wr.httpResultCode;
        }

        public static int PostContent(string url, byte[] data, out byte[] content, Dictionary<string, string> headers = null) {
            var parsedUrl = new UrlParts(url);
            var size = Marshal.SizeOf(data[0]) * data.Length;
            var unmanagementDataPointer = Marshal.AllocHGlobal(size);

            WebResults wr = new WebResults() { contentOut = IntPtr.Zero };
            try {
                // Copy the array to unmanaged memory.
                Marshal.Copy(data, 0, unmanagementDataPointer, data.Length);
                wr = PostContent(parsedUrl.Host, parsedUrl.Port, parsedUrl.ApiMethod, FormatHeaders(headers), unmanagementDataPointer, data.Length);
                // Copy the array from unmanaged memory.
                content = new byte[wr.contentOutSize];
                Marshal.Copy(wr.contentOut, content, 0, wr.contentOutSize);
            } finally {
                // Free the unmanaged memory.
                Marshal.FreeHGlobal(unmanagementDataPointer);
                Marshal.FreeHGlobal(wr.contentOut);
            }
            return wr.httpResultCode;
        }

        struct WebResults {
            public int httpResultCode;
            public int contentOutSize;
            public IntPtr contentOut;
        };

        [DllImport("WinHttpUnityPlugin", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        private static extern WebResults GetContent(
            [MarshalAs(UnmanagedType.LPWStr)] string server,
            [MarshalAs(UnmanagedType.U2)]ushort port,
            [MarshalAs(UnmanagedType.LPWStr)] string apiMethod,
            [MarshalAs(UnmanagedType.LPWStr)] string headers
            );

        [DllImport("WinHttpUnityPlugin", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        private static extern WebResults PostContent(
            [MarshalAs(UnmanagedType.LPWStr)] string server,
            [MarshalAs(UnmanagedType.U2)]ushort port,
            [MarshalAs(UnmanagedType.LPWStr)] string apiMethod,
            [MarshalAs(UnmanagedType.LPWStr)] string headers,
            IntPtr contentAddr,
            int contentSize);

        /// <summary>
        ///  Представляет части URL-запроса, необходимые для его выполнения через нативную реализацию
        /// Helper structure to exctract necessary parts from full url string
        /// </summary>
        private struct UrlParts {
            /// <summary>
            /// Initializes a new instance of the <see cref="UrlParts"/> struct.
            /// </summary>
            /// <param name="url">
            /// The url.
            /// </param>
            public UrlParts(string url)
                : this() {
                var parsedUrl = new Uri(url);

                this.Host = parsedUrl.Host;
                this.Port = (ushort)parsedUrl.Port;
                this.ApiMethod = parsedUrl.PathAndQuery;
            }

            /// <summary>
            /// Gets the host.
            /// </summary>
            public string Host { get; }

            /// <summary>
            /// Gets the port.
            /// </summary>
            public ushort Port { get; }

            /// <summary>
            /// Gets the api method.
            /// </summary>
            public string ApiMethod { get; }
        }
    }
}
