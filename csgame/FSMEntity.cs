using Slate2D;
using System.Reflection;

public class FSMState
{
    public Action? Enter;
    public Action? Exit;
    public Action<uint>? Update;
    public Action? Draw;
    public Func<Entity, Dir, CollisionType>? CanCollide;
    public Action<Entity, Dir>? Collide;
}

public abstract class FSMEntity<TEnum> : Entity where TEnum : Enum
{
    TEnum State = default;
    public FSMState DefaultStateHandlers;
    FSMState StateHandlers;
    bool EnteringState = true;
    TEnum LastState = default;
    uint StartStateTime = 0;
    // for timed state transitions
    TEnum NextState = default;
    uint NextStateTime = 0;
    public virtual Dictionary<TEnum, FSMState> Handlers { get; protected set; }

    public FSMEntity(LDTKEntity ent) : base(ent)
    {
        Handlers.TryGetValue(State, out StateHandlers);
    }

    public void FSMUpdate(uint ticks)
    {
        if (NextStateTime > 0 && ticks >= NextStateTime)
        {
            FSMTransitionTo(NextState);
        }

        if (!Enum.Equals(LastState, default(TEnum)))
        {
            if (Handlers.TryGetValue(LastState, out var LastStateHandlers))
                (LastStateHandlers?.Exit ?? DefaultStateHandlers?.Exit)?.Invoke();
            LastState = default;
        }

        if (EnteringState)
        {
            // set false first because enter could immediately transfer to another state
            EnteringState = false;
            StartStateTime = ticks;
            (StateHandlers?.Enter ?? DefaultStateHandlers?.Enter)?.Invoke();
        }

        (StateHandlers?.Update ?? DefaultStateHandlers?.Update)?.Invoke(ticks);

        // if state changes here, update time now so if draw is called the time will be right
        // FIXME: maybe need to be calling exit/enter here too?
        if (EnteringState)
        {
            StartStateTime = ticks;
        }

    }

    public void FSMDraw()
    {
        (StateHandlers?.Draw ?? DefaultStateHandlers?.Draw)?.Invoke();
    }

    public CollisionType FSMCanCollide(Entity other, Dir dir)
    {
        return (StateHandlers?.CanCollide ?? DefaultStateHandlers?.CanCollide)?.Invoke(other, dir) ?? CollisionType.Enabled;
    }

    public void FSMCollide(Entity other, Dir dir)
    {
        (StateHandlers?.Collide ?? DefaultStateHandlers?.Collide)?.Invoke(other, dir);
    }

    public void FSMTransitionTo(TEnum state)
    {
        LastState = State;
        State = state;
        Handlers.TryGetValue(state, out StateHandlers);
        NextState = default;
        NextStateTime = 0;
        EnteringState = true;
    }

    public void FSMDefaultTransitionTo(TEnum state)
    {
        if (Enum.Equals(State, default(TEnum)))
        {
            FSMTransitionTo(state);
        }
    }

    public void FSMTransitionAtTime(TEnum state, uint wait)
    {
        NextState = state;
        NextStateTime = wait + Ticks;
    }
}