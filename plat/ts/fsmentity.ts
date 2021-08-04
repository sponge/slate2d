import Entity from "./entity.js";

class FSMEntity extends Entity {
  state = 0;
  lastState = 0;
  nextState = 0;
  nextStateTime = 0;
  startStateTime = 0;

  update(ticks: number, dt: number) {
    if (this.nextStateTime > 0 && ticks >= this.nextStateTime) {
      this.lastState = this.state;
      this.state = this.nextState;
      this.nextState = 0;
      this.startStateTime = ticks;
    }
  }

  transitionAtTime(state: number, time: number) {
    this.nextState = state;
    this.nextStateTime = time;
  }
}

export default FSMEntity;