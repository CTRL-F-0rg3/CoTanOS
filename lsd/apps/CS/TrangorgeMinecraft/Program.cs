using OpenTK.Windowing.Common;
using OpenTK.Windowing.Desktop;
using OpenTK.Mathematics;

namespace TrangorgeMinecraft
{
    class Program
    {
        static void Main(string[] args)
        {
            // Konfiguracja okna
            var nativeWindowSettings = new NativeWindowSettings()
            {
                ClientSize = new Vector2i(1280, 720),
                Title = "TrangorgeOS - Minecraft Clone",
                Flags = ContextFlags.ForwardCompatible
            };

            // Uruchomienie gry (zmieniono GameWindow na Game)
            using (var game = new Game(GameWindowSettings.Default, nativeWindowSettings))
            {
                game.Run();
            }
        }
    }
}