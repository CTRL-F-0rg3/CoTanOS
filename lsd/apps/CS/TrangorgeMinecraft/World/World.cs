using OpenTK.Mathematics;
using System.Collections.Generic;

namespace TrangorgeMinecraft.World
{
    public class WorldManager
    {
        private Dictionary<(int, int), Chunk> _chunks = new Dictionary<(int, int), Chunk>();

        public void GenerateFlatTerrain(int chunksX, int chunksZ)
        {
            for (int cx = 0; cx < chunksX; cx++)
            {
                for (int cz = 0; cz < chunksZ; cz++)
                {
                    var chunk = new Chunk(cx, cz);

                    // Generowanie płaskiego terenu
                    for (int x = 0; x < Chunk.SIZE; x++)
                    {
                        for (int z = 0; z < Chunk.SIZE; z++)
                        {
                            // Warstwa kamienia
                            for (int y = 0; y < 4; y++)
                            {
                                chunk.SetBlock(x, y, z, new Block(BlockType.Stone));
                            }

                            // Warstwa ziemi
                            for (int y = 4; y < 6; y++)
                            {
                                chunk.SetBlock(x, y, z, new Block(BlockType.Dirt));
                            }

                            // Warstwa trawy
                            chunk.SetBlock(x, 6, z, new Block(BlockType.Grass));
                        }
                    }

                    _chunks[(cx, cz)] = chunk;
                }
            }
        }

        public void Render(Shader shader)
        {
            foreach (var chunk in _chunks.Values)
            {
                // Przesunięcie chunka w przestrzeni świata
                Matrix4 model = Matrix4.CreateTranslation(
                    chunk.ChunkX * Chunk.SIZE,
                    0,
                    chunk.ChunkZ * Chunk.SIZE
                );
                shader.SetMatrix4("model", model);

                chunk.Render();
            }
        }

        public void Dispose()
        {
            foreach (var chunk in _chunks.Values)
            {
                chunk.Dispose();
            }
            _chunks.Clear();
        }
    }
}