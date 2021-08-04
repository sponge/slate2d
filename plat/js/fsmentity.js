import Entity from "./entity.js";
class FSMEntity extends Entity {
    state = 0;
    lastState = 0;
    nextState = 0;
    nextStateTime = 0;
    startStateTime = 0;
    update(ticks, dt) {
        if (this.nextStateTime > 0 && ticks >= this.nextStateTime) {
            this.lastState = this.state;
            this.state = this.nextState;
            this.nextState = 0;
            this.startStateTime = ticks;
        }
    }
    transitionAtTime(state, time) {
        this.nextState = state;
        this.nextStateTime = time;
    }
}
export default FSMEntity;
