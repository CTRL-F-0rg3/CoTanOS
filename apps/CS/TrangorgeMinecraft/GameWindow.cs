using OpenTK.Graphics.OpenGL4;
using OpenTK.Windowing.Common;
using OpenTK.Windowing.Desktop;
using OpenTK.Mathematics;
using TrangorgeMinecraft.World;
using TrangorgeMinecraft.Input;

namespace TrangorgeMinecraft
{
    public class Game : OpenTK.Windowing.Desktop.GameWindow
    {
        private Shader _shader;
        private Camera _camera;
        private WorldManager _world;
        private InputHandler _input;

        private float _lastFrame = 0f;

        public Game(GameWindowSettings gameWindowSettings, NativeWindowSettings nativeWindowSettings)
            : base(gameWindowSettings, nativeWindowSettings)
        {
        }

        protected override void OnLoad()
        {
            base.OnLoad();

            // Tło (niebo)
            GL.ClearColor(0.5f, 0.7f, 1.0f, 1.0f);
            GL.Enable(EnableCap.DepthTest);
            GL.Enable(EnableCap.CullFace);
            GL.CullFace(CullFaceMode.Back);

            // Inicjalizacja komponentów
            _shader = new Shader("shaders/shader.vert", "shaders/shader.frag");
            
            // Kamera wyżej (y=10) i na środku świata (24, 24)
            _camera = new Camera(new Vector3(24, 10, 24), Size.X / (float)Size.Y);
            _world = new WorldManager();
            _input = new InputHandler(this, _camera);

            // Generowanie świata
            _world.GenerateFlatTerrain(3, 3); // 3x3 chunki = 48x48 bloków

            Console.WriteLine("Świat wygenerowany, kamera na pozycji: " + _camera.Position);

            CursorState = CursorState.Grabbed;
        }

        protected override void OnRenderFrame(FrameEventArgs e)
        {
            base.OnRenderFrame(e);

            // Obliczanie deltaTime
            float currentFrame = (float)OpenTK.Windowing.GraphicsLibraryFramework.GLFW.GetTime();
            float deltaTime = currentFrame - _lastFrame;
            _lastFrame = currentFrame;

            // Czyszczenie bufora
            GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);

            // Aktywacja shadera
            _shader.Use();

            // Ustawianie macierzy widoku i projekcji
            _shader.SetMatrix4("view", _camera.GetViewMatrix());
            _shader.SetMatrix4("projection", _camera.GetProjectionMatrix());

            // Renderowanie świata
            _world.Render(_shader);

            SwapBuffers();
        }

        protected override void OnUpdateFrame(FrameEventArgs e)
        {
            base.OnUpdateFrame(e);

            float deltaTime = (float)e.Time;

            // Obsługa inputu
            _input.ProcessInput(deltaTime);

            // Aktualizacja tytułu okna z FPS
            Title = $"TrangorgeOS - Minecraft Clone | FPS: {1.0f / deltaTime:F1}";
        }

        protected override void OnResize(ResizeEventArgs e)
        {
            base.OnResize(e);
            GL.Viewport(0, 0, e.Width, e.Height);
            _camera.AspectRatio = e.Width / (float)e.Height;
        }

        protected override void OnUnload()
        {
            base.OnUnload();
            _shader.Dispose();
            _world.Dispose();
        }
    }
}