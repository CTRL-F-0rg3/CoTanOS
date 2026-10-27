using OpenTK.Graphics.OpenGL4;
using System;

namespace TrangorgeMinecraft.World
{
    public class Chunk : IDisposable
    {
        public const int SIZE = 16;
        public const int HEIGHT = 16;

        public int ChunkX { get; private set; }
        public int ChunkZ { get; private set; }

        private Block[,,] _blocks;
        private int _vao, _vbo, _ebo;
        private int _indexCount;
        private bool _meshGenerated = false;

        public Chunk(int chunkX, int chunkZ)
        {
            ChunkX = chunkX;
            ChunkZ = chunkZ;
            _blocks = new Block[SIZE, HEIGHT, SIZE];
        }

        public void SetBlock(int x, int y, int z, Block block)
        {
            if (x >= 0 && x < SIZE && y >= 0 && y < HEIGHT && z >= 0 && z < SIZE)
            {
                _blocks[x, y, z] = block;
                _meshGenerated = false; // Wymagana regeneracja mesha
            }
        }

        public Block GetBlock(int x, int y, int z)
        {
            if (x >= 0 && x < SIZE && y >= 0 && y < HEIGHT && z >= 0 && z < SIZE)
            {
                return _blocks[x, y, z];
            }
            return Block.Air;
        }

        public void GenerateMesh()
        {
            if (_meshGenerated) return;

            var vertices = new System.Collections.Generic.List<float>();
            var indices = new System.Collections.Generic.List<uint>();

            uint indexOffset = 0;

            for (int x = 0; x < SIZE; x++)
            {
                for (int y = 0; y < HEIGHT; y++)
                {
                    for (int z = 0; z < SIZE; z++)
                    {
                        if (_blocks[x, y, z].IsSolid)
                        {
                            // Dodaj ściany tylko jeśli sąsiadują z powietrzem
                            if (y == 0 || !_blocks[x, y - 1, z].IsSolid)
                                AddFace(vertices, indices, x, y, z, FaceDirection.Down, ref indexOffset);
                            if (y == HEIGHT - 1 || !_blocks[x, y + 1, z].IsSolid)
                                AddFace(vertices, indices, x, y, z, FaceDirection.Up, ref indexOffset);
                            if (x == 0 || !_blocks[x - 1, y, z].IsSolid)
                                AddFace(vertices, indices, x, y, z, FaceDirection.Left, ref indexOffset);
                            if (x == SIZE - 1 || !_blocks[x + 1, y, z].IsSolid)
                                AddFace(vertices, indices, x, y, z, FaceDirection.Right, ref indexOffset);
                            if (z == 0 || !_blocks[x, y, z - 1].IsSolid)
                                AddFace(vertices, indices, x, y, z, FaceDirection.Front, ref indexOffset);
                            if (z == SIZE - 1 || !_blocks[x, y, z + 1].IsSolid)
                                AddFace(vertices, indices, x, y, z, FaceDirection.Back, ref indexOffset);
                        }
                    }
                }
            }

            _indexCount = indices.Count;

            if (_indexCount == 0) return;

            // Tworzenie VAO, VBO, EBO
            GL.GenVertexArrays(1, out _vao);
            GL.BindVertexArray(_vao);

            GL.GenBuffers(1, out _vbo);
            GL.BindBuffer(BufferTarget.ArrayBuffer, _vbo);
            GL.BufferData(BufferTarget.ArrayBuffer, vertices.Count * sizeof(float), vertices.ToArray(), BufferUsageHint.StaticDraw);

            GL.GenBuffers(1, out _ebo);
            GL.BindBuffer(BufferTarget.ElementArrayBuffer, _ebo);
            GL.BufferData(BufferTarget.ElementArrayBuffer, indices.Count * sizeof(uint), indices.ToArray(), BufferUsageHint.StaticDraw);

            // Pozycja (3 floaty)
            GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, 7 * sizeof(float), 0);
            GL.EnableVertexAttribArray(0);

            // Normalna (3 floaty)
            GL.VertexAttribPointer(1, 3, VertexAttribPointerType.Float, false, 7 * sizeof(float), 3 * sizeof(float));
            GL.EnableVertexAttribArray(1);

            // Typ bloku (1 float)
            GL.VertexAttribPointer(2, 1, VertexAttribPointerType.Float, false, 7 * sizeof(float), 6 * sizeof(float));
            GL.EnableVertexAttribArray(2);

            _meshGenerated = true;
        }

        private void AddFace(System.Collections.Generic.List<float> vertices, System.Collections.Generic.List<uint> indices,
            int x, int y, int z, FaceDirection dir, ref uint indexOffset)
        {
            float[] faceVertices;
            float[] normal;

            switch (dir)
            {
                case FaceDirection.Up:
                    faceVertices = new float[] {
                        x, y + 1, z,     x + 1, y + 1, z,     x + 1, y + 1, z + 1,
                        x, y + 1, z,     x + 1, y + 1, z + 1, x, y + 1, z + 1
                    };
                    normal = new float[] { 0, 1, 0 };
                    break;
                case FaceDirection.Down:
                    faceVertices = new float[] {
                        x, y, z + 1,     x + 1, y, z + 1,     x + 1, y, z,
                        x, y, z + 1,     x + 1, y, z,         x, y, z
                    };
                    normal = new float[] { 0, -1, 0 };
                    break;
                case FaceDirection.Left:
                    faceVertices = new float[] {
                        x, y, z,         x, y, z + 1,         x, y + 1, z + 1,
                        x, y, z,         x, y + 1, z + 1,     x, y + 1, z
                    };
                    normal = new float[] { -1, 0, 0 };
                    break;
                case FaceDirection.Right:
                    faceVertices = new float[] {
                        x + 1, y, z + 1, x + 1, y, z,         x + 1, y + 1, z,
                        x + 1, y, z + 1, x + 1, y + 1, z,     x + 1, y + 1, z + 1
                    };
                    normal = new float[] { 1, 0, 0 };
                    break;
                case FaceDirection.Front:
                    faceVertices = new float[] {
                        x, y, z,         x + 1, y, z,         x + 1, y + 1, z,
                        x, y, z,         x + 1, y + 1, z,     x, y + 1, z
                    };
                    normal = new float[] { 0, 0, -1 };
                    break;
                case FaceDirection.Back:
                    faceVertices = new float[] {
                        x + 1, y, z + 1, x, y, z + 1,         x, y + 1, z + 1,
                        x + 1, y, z + 1, x, y + 1, z + 1,     x + 1, y + 1, z + 1
                    };
                    normal = new float[] { 0, 0, 1 };
                    break;
                default:
                    return;
            }

            // Dodaj wierzchołki z normalną i typem bloku
            float blockType = (float)_blocks[x, y, z].Type;
            for (int i = 0; i < 6; i++)
            {
                vertices.Add(faceVertices[i * 3]);
                vertices.Add(faceVertices[i * 3 + 1]);
                vertices.Add(faceVertices[i * 3 + 2]);
                vertices.Add(normal[0]);
                vertices.Add(normal[1]);
                vertices.Add(normal[2]);
                vertices.Add(blockType);
            }

            // Dodaj indeksy
            indices.Add(indexOffset);
            indices.Add(indexOffset + 1);
            indices.Add(indexOffset + 2);
            indices.Add(indexOffset);
            indices.Add(indexOffset + 2);
            indices.Add(indexOffset + 3);
            indexOffset += 4;
        }

        public void Render()
        {
            if (!_meshGenerated) GenerateMesh();
            if (_indexCount == 0) return;

            GL.BindVertexArray(_vao);
            GL.DrawElements(PrimitiveType.Triangles, _indexCount, DrawElementsType.UnsignedInt, 0);
        }

        public void Dispose()
        {
            if (_meshGenerated)
            {
                GL.DeleteBuffer(_vbo);
                GL.DeleteBuffer(_ebo);
                GL.DeleteVertexArray(_vao);
            }
        }
    }

    public enum FaceDirection
    {
        Up,
        Down,
        Left,
        Right,
        Front,
        Back
    }
}