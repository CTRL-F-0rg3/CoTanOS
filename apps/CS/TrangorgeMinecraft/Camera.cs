using OpenTK.Mathematics;

namespace TrangorgeMinecraft
{
    public class Camera
    {
        public Vector3 Position;
        public Vector3 Front = new Vector3(0.0f, 0.0f, -1.0f);
        public Vector3 Up = new Vector3(0.0f, 1.0f, 0.0f);
        public Vector3 Right = new Vector3(1.0f, 0.0f, 0.0f);

        private float _pitch = 0f;
        private float _yaw = -90f;
        private float _fov = 70f;
        private float _sensitivity = 0.2f;
        private float _speed = 5f;

        public float AspectRatio = 16f / 9f;

        public Camera(Vector3 position, float aspectRatio)
        {
            Position = position;
            AspectRatio = aspectRatio;
        }

        public Matrix4 GetViewMatrix()
        {
            return Matrix4.LookAt(Position, Position + Front, Up);
        }

        public Matrix4 GetProjectionMatrix()
        {
            return Matrix4.CreatePerspectiveFieldOfView(
                MathHelper.DegreesToRadians(_fov),
                AspectRatio,
                0.1f,
                1000f
            );
        }

        public void ProcessMouseMovement(float xOffset, float yOffset)
        {
            xOffset *= _sensitivity;
            yOffset *= _sensitivity;

            _yaw += xOffset;
            _pitch += yOffset;

            // Ograniczenie pitch
            if (_pitch > 89f) _pitch = 89f;
            if (_pitch < -89f) _pitch = -89f;

            UpdateVectors();
        }

        public void ProcessKeyboard(Vector3 direction, float deltaTime)
        {
            float velocity = _speed * deltaTime;
            Position += direction * velocity;
        }

        private void UpdateVectors()
        {
            Front.X = MathF.Cos(MathHelper.DegreesToRadians(_yaw)) * MathF.Cos(MathHelper.DegreesToRadians(_pitch));
            Front.Y = MathF.Sin(MathHelper.DegreesToRadians(_pitch));
            Front.Z = MathF.Sin(MathHelper.DegreesToRadians(_yaw)) * MathF.Cos(MathHelper.DegreesToRadians(_pitch));
            Front = Vector3.Normalize(Front);

            Right = Vector3.Normalize(Vector3.Cross(Front, Up));
        }
    }
}