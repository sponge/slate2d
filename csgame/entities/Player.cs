using Slate2D;

class Player : Entity
{
    public Player(LDTKEntity ent) : base(ent)
    {
    
    }

    public void StompEnemy()
    {
        throw new NotImplementedException();
        // this.vel[1] = SLT.buttonPressed(Buttons.Jump) ? -Phys.enemyJumpHeld : -Phys.enemyJump;
        // this.jumpHeld = true;
    }
}