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

    TEnum? CurrentState = default;
    protected TEnum? LastState { get; private set; } = default;
    TEnum? NextState = default;
    FSMState CurrentStateHandlers;
    protected uint StartStateTime { get; private set; } = 0;
    (TEnum State, uint Time)? TimedChange; // for timed state transitions
    Dictionary<TEnum, FSMState> Handlers = new();

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

        CurrentStateHandlers = Handlers[CurrentState];
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
        NextState = state;
        TimedChange = null;
    }

    public void FSMTimer(TEnum state, uint wait) => TimedChange = (state, wait + Ticks);

    public override void Update(uint ticks, float dt)
    {
        // if time based transition has passed
        if (ticks >= TimedChange?.Time)
            FSMTransitionTo(TimedChange.Value.State);

        // if we changed the fsm state
        if (!Equals(NextState, default(TEnum)))
        {
            CurrentStateHandlers?.Exit?.Invoke();

            LastState = CurrentState;
            CurrentState = NextState;
            CurrentStateHandlers = Handlers[CurrentState];
            NextState = default;
            StartStateTime = ticks;

            CurrentStateHandlers?.Enter?.Invoke();
        }

        CurrentStateHandlers?.Update?.Invoke(ticks, dt);
    }

    public override void Draw()
    {
        var handler = CurrentStateHandlers?.Draw;
        handler?.Invoke();
        if (handler == null)
            base.Draw();
    }

    public override CollisionType CanCollide(Entity other, Dir dir)
    {
        return CurrentStateHandlers?.CanCollide?.Invoke(other, dir) ?? CollisionType.Enabled;
    }

    public override void Collide(Entity other, Dir dir)
    {
        CurrentStateHandlers?.Collide?.Invoke(other, dir);
    }

    public uint Animate<TFramesEnum>(TFramesEnum[] animation) where TFramesEnum : Enum
    {
        if (TimedChange == null) return Convert.ToUInt32(animation[0]);
        var v = Util.InvLerp<float>(StartStateTime, TimedChange.Value.Time, Ticks);
        return Convert.ToUInt32(animation[(int)(v * animation.Length)]);
    }
}