// Interfejs DE w C#: steruje rdzeniem (Rust + logika C++) przez P/Invoke.
//
// To jest warstwa, która ma docelowo uruchamiać się na OS-ie jako interfejs:
// host kompiluje ten program (zwykły `dotnet build` albo NativeAOT do
// natywnego ELF-a), a program woła `extern "C"` z `titanus_de/src/ffi.rs`,
// tworzy okna, ustawia motyw z C# i renderuje ramkę.

using System.Runtime.InteropServices;

namespace TitanusInterface;

public static class Program
{
    // ── C ABI z rdzenia Rust ─────────────────────────────────────────────────

    [DllImport("titanus_de", EntryPoint = "titanus_new")]
    private static extern IntPtr TitanusNew(uint w, uint h);

    [DllImport("titanus_de", EntryPoint = "titanus_free")]
    private static extern void TitanusFree(IntPtr de);

    [DllImport("titanus_de", EntryPoint = "titanus_spawn_tiled")]
    private static extern uint TitanusSpawnTiled(IntPtr de, [MarshalAs(UnmanagedType.LPUTF8Str)] string title);

    [DllImport("titanus_de", EntryPoint = "titanus_spawn_floating")]
    private static extern uint TitanusSpawnFloating(IntPtr de, [MarshalAs(UnmanagedType.LPUTF8Str)] string title, int x, int y, uint w, uint h);

    [DllImport("titanus_de", EntryPoint = "titanus_focus_next")]
    private static extern void TitanusFocusNext(IntPtr de);

    [DllImport("titanus_de", EntryPoint = "titanus_set_theme_json")]
    private static extern int TitanusSetThemeJson(IntPtr de, [MarshalAs(UnmanagedType.LPUTF8Str)] string json);

    [DllImport("titanus_de", EntryPoint = "titanus_render")]
    private static extern void TitanusRender(IntPtr de);

    [DllImport("titanus_de", EntryPoint = "titanus_framebuffer_ptr")]
    private static extern IntPtr TitanusFramebufferPtr(IntPtr de);

    [DllImport("titanus_de", EntryPoint = "titanus_width")]
    private static extern uint TitanusWidth(IntPtr de);

    [DllImport("titanus_de", EntryPoint = "titanus_height")]
    private static extern uint TitanusHeight(IntPtr de);

    // ── usługi (daemony) i autostart ─────────────────────────────────────────
    [DllImport("titanus_de", EntryPoint = "titanus_service_count")]
    private static extern uint TitanusServiceCount(IntPtr de);

    [DllImport("titanus_de", EntryPoint = "titanus_service_name")]
    private static extern uint TitanusServiceName(IntPtr de, uint index, byte[] buf, uint bufLen);

    [DllImport("titanus_de", EntryPoint = "titanus_service_state")]
    private static extern int TitanusServiceState(IntPtr de, uint index);

    [DllImport("titanus_de", EntryPoint = "titanus_service_autostart")]
    private static extern int TitanusServiceAutostart(IntPtr de, uint index);

    [DllImport("titanus_de", EntryPoint = "titanus_service_start")]
    private static extern int TitanusServiceStart(IntPtr de, uint index);

    [DllImport("titanus_de", EntryPoint = "titanus_service_set_autostart")]
    private static extern int TitanusServiceSetAutostart(IntPtr de, uint index, int enabled);

    public static int Main(string[] args)
    {
        const uint w = 960, h = 540;

        IntPtr de = TitanusNew(w, h);
        if (de == IntPtr.Zero)
        {
            Console.Error.WriteLine("[interface_cs] nie udało się utworzyć DE");
            return 1;
        }

        // C# steruje DE: okna, fokus, motyw, render.
        TitanusSpawnTiled(de, "Terminal");
        TitanusSpawnTiled(de, "Edytor");
        TitanusSpawnFloating(de, "System", 320, 180, 420, 260);
        TitanusSpawnFloating(de, "Usługi", 420, 100, 460, 300);
        TitanusFocusNext(de);

        string themePath = args.Length > 0 ? args[0] : "theme.json";
        string themeJson = File.ReadAllText(themePath);
        if (TitanusSetThemeJson(de, themeJson) != 0)
        {
            Console.Error.WriteLine($"[interface_cs] zły motyw: {themePath}");
            return 1;
        }

        TitanusRender(de);

        IntPtr fb = TitanusFramebufferPtr(de);
        uint fw = TitanusWidth(de);
        uint fh = TitanusHeight(de);

        int[] pixels = new int[fw * fh];
        Marshal.Copy(fb, pixels, 0, pixels.Length);
        WritePpm("interface_frame.ppm", pixels, fw, fh);

        Console.WriteLine($"[interface_cs] zapisano interface_frame.ppm ({fw}x{fh})");

        // ── Zarządzanie usługami (daemonami) i autostartem ───────────────────
        uint count = TitanusServiceCount(de);
        Console.WriteLine($"[interface_cs] usługi ({count}):");
        for (uint i = 0; i < count; i++)
        {
            var buf = new byte[64];
            uint len = TitanusServiceName(de, i, buf, (uint)buf.Length);
            string name = System.Text.Encoding.UTF8.GetString(buf, 0, (int)len);
            int state = TitanusServiceState(de, i);
            int autostart = TitanusServiceAutostart(de, i);
            Console.WriteLine($"  {i}: [{(autostart != 0 ? "x" : " ")}] {StateName(state)}  {name}");
        }

        // Uruchom pierwszą usługę i wyłącz jej autostart.
        if (count > 0)
        {
            TitanusServiceStart(de, 0);
            TitanusServiceSetAutostart(de, 0, 0);
            Console.WriteLine("[interface_cs] uruchomiono usługę 0 i wyłączono jej autostart");
        }

        TitanusFree(de);
        return 0;
    }

    private static string StateName(int state) => state switch
    {
        0 => "stop",
        1 => "run ",
        2 => "fail",
        _ => "?",
    };

    private static void WritePpm(string path, int[] pixels, uint w, uint h)
    {
        using var s = File.Create(path);
        byte[] header = System.Text.Encoding.ASCII.GetBytes($"P6\n{w} {h}\n255\n");
        s.Write(header, 0, header.Length);

        var bytes = new byte[pixels.Length * 3];
        for (int i = 0; i < pixels.Length; i++)
        {
            int px = pixels[i];
            bytes[i * 3] = (byte)((px >> 16) & 0xFF);
            bytes[i * 3 + 1] = (byte)((px >> 8) & 0xFF);
            bytes[i * 3 + 2] = (byte)(px & 0xFF);
        }
        s.Write(bytes, 0, bytes.Length);
    }
}
