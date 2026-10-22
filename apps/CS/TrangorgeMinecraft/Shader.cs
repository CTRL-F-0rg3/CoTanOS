using OpenTK.Graphics.OpenGL4;
using OpenTK.Mathematics;
using System;
using System.IO;

namespace TrangorgeMinecraft
{
    public class Shader : IDisposable
    {
        private int _handle;
        private bool _disposed = false;

        public Shader(string vertexPath, string fragmentPath)
        {
            string vertexSource = File.ReadAllText(vertexPath);
            string fragmentSource = File.ReadAllText(fragmentPath);

            int vertexShader = GL.CreateShader(ShaderType.VertexShader);
            GL.ShaderSource(vertexShader, vertexSource);
            CompileShader(vertexShader);

            int fragmentShader = GL.CreateShader(ShaderType.FragmentShader);
            GL.ShaderSource(fragmentShader, fragmentSource);
            CompileShader(fragmentShader);

            _handle = GL.CreateProgram();
            GL.AttachShader(_handle, vertexShader);
            GL.AttachShader(_handle, fragmentShader);
            LinkProgram(_handle);

            GL.DetachShader(_handle, vertexShader);
            GL.DetachShader(_handle, fragmentShader);
            GL.DeleteShader(vertexShader);
            GL.DeleteShader(fragmentShader);
        }

        private void CompileShader(int shader)
        {
            GL.CompileShader(shader);
            GL.GetShader(shader, ShaderParameter.CompileStatus, out int code);
            if (code != (int)All.True)
            {
                string infoLog = GL.GetShaderInfoLog(shader);
                throw new Exception($"Błąd kompilacji shadera: {infoLog}");
            }
        }

        private void LinkProgram(int program)
        {
            GL.LinkProgram(program);
            GL.GetProgram(program, GetProgramParameterName.LinkStatus, out int code);
            if (code != (int)All.True)
            {
                string infoLog = GL.GetProgramInfoLog(program);
                throw new Exception($"Błąd linkowania programu: {infoLog}");
            }
        }

        public void Use()
        {
            GL.UseProgram(_handle);
        }

        public void SetMatrix4(string name, Matrix4 value)
        {
            Use();
            int location = GL.GetUniformLocation(_handle, name);
            GL.UniformMatrix4(location, true, ref value);
        }

        public void SetVector3(string name, Vector3 value)
        {
            Use();
            int location = GL.GetUniformLocation(_handle, name);
            GL.Uniform3(location, value);
        }

        public void Dispose()
        {
            if (!_disposed)
            {
                GL.DeleteProgram(_handle);
                _disposed = true;
            }
        }
    }
}