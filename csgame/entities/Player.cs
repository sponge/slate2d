using Slate2D;

class Player : Entity
{
    public int Health = 3;
    public int MaxHealth = 3;

    public Player(LDTKEntity ent) : base(ent)
    {
        Sprite = Assets.Find("dogspr");
    }

    public void StompEnemy()
    {
        throw new NotImplementedException();
        // this.vel[1] = SLT.buttonPressed(Buttons.Jump) ? -Phys.enemyJumpHeld : -Phys.enemyJump;
        // this.jumpHeld = true;
    }
}