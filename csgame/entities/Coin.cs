using Slate2D;

class Coin : Entity
{
    public Coin(LDTKEntity ent) : base(ent)
    {
        Sprite = Assets.Find("coin");
        Console.WriteLine("hello");
    }
}