import Entity from "./entity.js";
import World from "./world.js";
class FSMEntity extends Entity {
    state = 0;
    lastState = 0;
    nextState = 0;
    nextStateTime = 0;
    startStateTime = 0;
    fsmUpdate(states, ticks) {
        if (this.nextStateTime > 0 && ticks >= this.nextStateTime) {
            this.lastState = this.state;
            this.state = this.nextState;
            this.nextState = 0;
            this.startStateTime = ticks;
        }
        if (this.lastState != 0) {
            (states[this.lastState]?.exit ?? states.default?.exit)?.();
            this.lastState = 0;
        }
        if (this.nextState == 0) {
            (states[this.state]?.enter ?? states.default?.enter)?.();
        }
        (states[this.state]?.update ?? states.default?.update)?.(ticks);
    }
    fsmCanCollide(states, other, dir) {
        return (states[this.state]?.canCollide ?? states.default?.canCollide)?.(other, dir);
    }
    fsmCollide(states, other, dir) {
        (states[this.state]?.collide ?? states.default?.collide)?.(other, dir);
    }
    fsmTransitionTo(state) {
        this.state = state;
        this.nextState = 0;
        this.nextStateTime = 0;
    }
    fsmTransitionAtTime(state, wait) {
        this.nextState = state;
        this.nextStateTime = wait <= 0 ? 0 : wait + World().state.ticks;
    }
}
export default FSMEntity;
