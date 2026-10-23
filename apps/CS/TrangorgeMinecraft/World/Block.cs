namespace TrangorgeMinecraft.World
{
    public enum BlockType : byte
    {
        Air = 0,
        Grass = 1,
        Dirt = 2,
        Stone = 3,
        Wood = 4,
        Leaves = 5
    }

    public struct Block
    {
        public BlockType Type;

        public Block(BlockType type)
        {
            Type = type;
        }

        public bool IsSolid => Type != BlockType.Air;

        public static Block Air => new Block(BlockType.Air);
    }
}