import CollisionType from "./collisiontype.js";
import Entity from "./entity.js";
import { World } from './game.js';
class FSMEntity extends Entity {
    state = 0;
    enteringState = true;
    lastState = 0;
    startStateTime = 0;
    // for timed state transitions
    nextState = 0;
    nextStateTime = 0;
    fsmUpdate(states, ticks) {
        if (this.nextStateTime > 0 && ticks >= this.nextStateTime) {
            this.fsmTransitionTo(this.nextState);
        }
        if (this.lastState != 0) {
            (states[this.lastState]?.exit ?? states.default?.exit)?.();
            this.lastState = 0;
        }
        if (this.enteringState) {
            // set false first because enter could immediately transfer to another state
            this.enteringState = false;
            this.startStateTime = ticks;
            (states[this.state]?.enter ?? states.default?.enter)?.();
        }
        (states[this.state]?.update ?? states.default?.update)?.(ticks);
        // if state changes here, update time now so if draw is called the time will be right
        // FIXME: maybe need to be calling exit/enter here too?
        if (this.enteringState) {
            this.startStateTime = ticks;
        }
    }
    fsmDraw(states) {
        (states[this.state]?.draw ?? states.default?.draw)?.();
    }
    fsmCanCollide(states, other, dir) {
        return (states[this.state]?.canCollide ?? states.default?.canCollide)?.(other, dir) ?? CollisionType.Enabled;
    }
    fsmCollide(states, other, dir) {
        (states[this.state]?.collide ?? states.default?.collide)?.(other, dir);
    }
    fsmTransitionTo(state) {
        this.lastState = this.state;
        this.state = state;
        this.nextState = 0;
        this.nextStateTime = 0;
        this.enteringState = true;
    }
    fsmDefaultTransitionTo(state) {
        if (this.state == 0) {
            this.fsmTransitionTo(state);
        }
    }
    fsmTransitionAtTime(state, wait) {
        this.nextState = state;
        this.nextStateTime = wait <= 0 ? 0 : wait + World().state.ticks;
    }
}
export { FSMEntity };
