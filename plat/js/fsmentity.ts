import CollisionType from "./collisiontype.js";
import Dir from "./dir.js";
import Entity from "./entity.js";
import { World } from './game.js';

type FSMStates = {
  [key in number | 'default']: {
    enter?: () => void
    exit?: () => void
    update?: (ticks: number) => void
    draw?: () => void
    canCollide?: (other: Entity, dir: Dir) => CollisionType
    collide?: (other: Entity, dir: Dir) => void
  }
};

class FSMEntity extends Entity {
  state = 0;
  enteringState = true;
  lastState = 0;
  startStateTime = 0;
  // for timed state transitions
  nextState = 0;
  nextStateTime = 0;

  fsmUpdate(states: FSMStates, ticks: number) {
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

  fsmDraw(states: FSMStates) {
    (states[this.state]?.draw ?? states.default?.draw)?.();
  }

  fsmCanCollide(states: FSMStates, other: Entity, dir: Dir): CollisionType {
    return (states[this.state]?.canCollide ?? states.default?.canCollide)?.(other, dir) ?? CollisionType.Enabled;
  }

  fsmCollide(states: FSMStates, other: Entity, dir: Dir) {
    (states[this.state]?.collide ?? states.default?.collide)?.(other, dir);
  }

  fsmTransitionTo(state: number) {
    this.lastState = this.state;
    this.state = state;
    this.nextState = 0;
    this.nextStateTime = 0;
    this.enteringState = true;
  }

  fsmDefaultTransitionTo(state: number) {
    if (this.state == 0) {
      this.fsmTransitionTo(state);
    }
  }

  fsmTransitionAtTime(state: number, wait: number) {
    this.nextState = state;
    this.nextStateTime = wait <= 0 ? 0 : wait + this.ticks;
  }
}

export { FSMEntity, FSMStates };