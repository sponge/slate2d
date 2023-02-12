using System;
using System.Reflection;
using System.Security.Cryptography.X509Certificates;

public abstract class FSMEntity<TEnum> : Entity where TEnum : Enum
{
    public class FSMState
    {
        public Action? Enter;
        public Action? Exit;
        public Action<uint, float>? Update;
        public Action? Draw;
        public Func<Entity, Dir, CollisionType>? CanCollide;
        public Action<Entity, Dir>? Collide;
    }

    TEnum? State = default;
    FSMState? StateHandlers;
    bool EnteringState = true;
    TEnum? LastState = default;
    uint StartStateTime = 0;
    (TEnum State, uint Time)? TimedChange; // for timed state transitions
    public Dictionary<TEnum, FSMState> Handlers = new();

    public FSMEntity(LDTKEntity ent) : base(ent)
    {
        var values = (TEnum[])Enum.GetValues(typeof(TEnum));
        foreach (var val in values)
        {
            Handlers[val] = new FSMState
            {
                Enter = BindMethod<Action>(val, "Enter"),
                Exit = BindMethod<Action>(val, "Exit"),
                Update = BindMethod<Action<uint, float>>(val, "Update"),
                Draw = BindMethod<Action>(val, "Draw"),
                CanCollide = BindMethod<Func<Entity, Dir, CollisionType>>(val, "CanCollide"),
                Collide = BindMethod<Action<Entity, Dir>>(val, "Collide"),

            };
        }

        Handlers.TryGetValue(State, out StateHandlers);
    }

    T? BindMethod<T>(TEnum state, string action)
    {
        string[] functions = {
            $"{state}_{action}",
            $"{default(TEnum)}_{action}"
        };

        foreach (var name in functions)
        {
            var methodInfo = this.GetType().GetMethod(name, BindingFlags.Instance | BindingFlags.NonPublic | BindingFlags.Public);
            if (methodInfo == null)
                continue;

            var methodDelegate = (T)(object)Delegate.CreateDelegate(typeof(T), this, methodInfo);
            return methodDelegate;
        }

        return default;
    }

    public void FSMTransitionTo(TEnum state)
    {
        LastState = State;
        State = state;
        Handlers.TryGetValue(state, out StateHandlers);
        TimedChange = null;
        EnteringState = true;
    }

    // since default state is a fallback, but you don't want to have to define
    // an Enter action for each state, use this to conditionally change the state
    public void FSMDefaultTransitionTo(TEnum state) { if (Equals(State, default(TEnum))) FSMTransitionTo(state); }
    public void FSMTimer(TEnum state, uint wait) => TimedChange = (state, wait + Ticks);

    public override void Update(uint ticks, float dt)
    {
        // if time based transition has passed
        if (ticks >= TimedChange?.Time)
        {
            FSMTransitionTo(TimedChange.Value.State);
        }

        // if we've exited the state in the last tick
        if (!Equals(LastState, default(TEnum)))
        {
            if (Handlers.TryGetValue(LastState, out var LastStateHandlers))
                LastStateHandlers?.Exit?.Invoke();
            LastState = default;
        }

        // if we're going into a new state
        if (EnteringState)
        {
            // set false first because enter could immediately transfer to another state
            EnteringState = false;
            StartStateTime = ticks;
            StateHandlers?.Enter?.Invoke();
        }

        StateHandlers?.Update?.Invoke(ticks, dt);

        // if state changes here, update time now so if draw is called the time will be right
        // FIXME: maybe need to be calling exit/enter here too?
        if (EnteringState)
        {
            StartStateTime = ticks;
        }
    }

    public override void Draw()
    {
        var handler = StateHandlers?.Draw;
        handler?.Invoke();
        if (handler == null)
        {
            base.Draw();
        }
    }

    public override CollisionType CanCollide(Entity other, Dir dir)
    {
        return StateHandlers?.CanCollide?.Invoke(other, dir) ?? CollisionType.Enabled;
    }

    public override void Collide(Entity other, Dir dir)
    {
        StateHandlers?.Collide?.Invoke(other, dir);
    }
}