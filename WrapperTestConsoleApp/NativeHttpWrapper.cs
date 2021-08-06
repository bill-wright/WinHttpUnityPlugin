// --------------------------------------------------------------------------------------------------------------------
// <copyright file="NativeHttpWrapper.cs" company="Open source">
//   2019 
// </copyright>
// --------------------------------------------------------------------------------------------------------------------

namespace WinHttpUnityPluginWrapper {
    using System;
    using System.Runtime.InteropServices;
    using System.Text;

    /// <summary>
    /// Wrap native http access dll written on C++ for using in Unity apps for Windows desktop (editor and standonle)
    /// </summary>
    public static class NativeHttpWrapper {
        /// <summary>
        /// The get content.
        /// </summary>
        /// <param name="url">
        /// The url.
        /// </param>
        /// <returns>
        /// The string result of request.
        /// </returns>
        public static string GetContent(string url) {
            var parsedUrl = new UrlParts(url);
            // return StringFromNativeUtf8(GetContent(parsedUrl.Host, parsedUrl.Port, parsedUrl.ApiMethod));
            return GetContent(parsedUrl.Host, parsedUrl.Port, parsedUrl.ApiMethod);
        }

        /// <summary>
        /// The string from native utf 8.
        /// </summary>
        /// <param name="nativeUtf8">
        /// The native utf 8.
        /// </param>
        /// <returns>
        /// The <see cref="string"/>.
        /// </returns>
        public static string StringFromNativeUtf8(IntPtr nativeUtf8) {
            var len = 0;
            while (Marshal.ReadByte(nativeUtf8, len) != 0) {
                len += 2;
            }

            var buffer = new byte[len];
            Marshal.Copy(nativeUtf8, buffer, 0, buffer.Length);
            Marshal.Release(nativeUtf8);
            return Encoding.UTF8.GetString(buffer);
        }

        /// <summary>
        /// The post content.
        /// </summary>
        /// <param name="url">
        /// The url.
        /// </param>
        /// <param name="data">
        /// The data.
        /// </param>
        public static void PostContent(string url, byte[] data) {
            var parsedUrl = new UrlParts(url);

            var size = Marshal.SizeOf(data[0]) * data.Length;

            var unmanagementDataPointer = Marshal.AllocHGlobal(size);

            try {
                // Copy the array to unmanaged memory.
                Marshal.Copy(data, 0, unmanagementDataPointer, data.Length);
                PostContent(parsedUrl.Host, parsedUrl.Port, parsedUrl.ApiMethod, unmanagementDataPointer, data.Length);
            } finally {
                // Free the unmanaged memory.
                Marshal.FreeHGlobal(unmanagementDataPointer);
            }
        }

        [DllImport("WinHttpUnityPlugin", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.BStr)]
        private static extern string GetContent(
            [MarshalAs(UnmanagedType.LPWStr)] string server,
            [MarshalAs(UnmanagedType.U2)]ushort port,
            [MarshalAs(UnmanagedType.LPWStr)] string apiMethod);

        [DllImport("WinHttpUnityPlugin", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        private static extern void PostContent(
            [MarshalAs(UnmanagedType.LPWStr)] string server,
            [MarshalAs(UnmanagedType.U2)]ushort port,
            [MarshalAs(UnmanagedType.LPWStr)] string apiMethod,
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
