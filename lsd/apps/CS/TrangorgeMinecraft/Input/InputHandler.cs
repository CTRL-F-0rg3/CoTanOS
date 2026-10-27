using OpenTK.Windowing.GraphicsLibraryFramework;
using OpenTK.Mathematics;

namespace TrangorgeMinecraft.Input
{
    public class InputHandler
    {
        private Game _window;
        private Camera _camera;

        public InputHandler(Game window, Camera camera)
        {
            _window = window;
            _camera = camera;
        }

        public void ProcessInput(float deltaTime)
        {
            // Ruch WASD
            Vector3 direction = Vector3.Zero;

            if (_window.IsKeyDown(Keys.W))
                direction += _camera.Front;
            if (_window.IsKeyDown(Keys.S))
                direction -= _camera.Front;
            if (_window.IsKeyDown(Keys.A))
                direction -= Vector3.Normalize(Vector3.Cross(_camera.Front, _camera.Up));
            if (_window.IsKeyDown(Keys.D))
                direction += Vector3.Normalize(Vector3.Cross(_camera.Front, _camera.Up));
            if (_window.IsKeyDown(Keys.Space))
                direction += Vector3.UnitY;
            if (_window.IsKeyDown(Keys.LeftShift))
                direction -= Vector3.UnitY;

            if (direction.Length > 0)
            {
                direction = Vector3.Normalize(direction);
                _camera.ProcessKeyboard(direction, deltaTime);
            }

            // Obracanie myszką
            var mouseDelta = _window.MouseState.Delta;
            if (mouseDelta.X != 0 || mouseDelta.Y != 0)
            {
                _camera.ProcessMouseMovement(mouseDelta.X, mouseDelta.Y);
            }

            // Wyjście ESC
            if (_window.IsKeyDown(Keys.Escape))
            {
                _window.Close();
            }
        }
    }
}