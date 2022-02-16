import * as SLT from 'slate2d';
import * as Draw from 'draw';
import * as Assets from 'assets';
import { Layer } from '../entity.js';
import { FSMEntity } from '../fsmentity.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import { World } from '../game.js';
import Buttons from '../buttons.js';
import { clamp, lerp } from '../util.js';
var States;
(function (States) {
    States[States["None"] = 0] = "None";
    States[States["Idle"] = 1] = "Idle";
    States[States["Expand"] = 2] = "Expand";
    States[States["Read"] = 3] = "Read";
    States[States["Shrink"] = 4] = "Shrink";
})(States || (States = {}));
class Sign extends FSMEntity {
    type = 'Sign';
    collidable = CollisionType.Trigger;
    sprite = Assets.find('sign');
    runWhilePaused = true;
    layer = Layer.Background;
    signX = 0;
    signY = 130;
    signW = 0;
    signH = 0;
    signTime = 20;
    signText = '';
    #states = {
        default: {
            enter: () => this.fsmDefaultTransitionTo(States.Idle),
        },
        [States.Idle]: {
            enter: () => World().state.paused = false,
            collide: (other, dir) => {
                if (other instanceof Player == false)
                    return;
                if (!SLT.buttonPressed(Buttons.Up))
                    return;
                this.fsmTransitionTo(States.Expand);
            }
        },
        [States.Expand]: {
            enter: () => {
                this.fsmTransitionAtTime(States.Read, this.signTime);
                const size = Assets.textSize(0, this.signText, 0);
                this.signW = size.w + 20;
                this.signH = size.h + 20;
                this.signX = World().res.w / 2 - this.signW / 2;
                World().state.paused = true;
            },
            draw: () => {
                const t = (this.ticks - this.startStateTime) / this.signTime;
                const w = lerp(0, this.signW, t);
                const h = lerp(0, this.signH, t);
                const x = this.signX + (this.signW - w) / 2;
                const y = this.signY + (this.signH - h) / 2;
                Draw.setColor(0, 0, 0, 255);
                Draw.rect(x, y, w, h, false);
            },
        },
        [States.Read]: {
            update: (ticks) => {
                if (SLT.buttonPressed(Buttons.Jump)) {
                    this.fsmTransitionTo(States.Shrink);
                }
            },
            draw: () => {
                Draw.setColor(0, 0, 0, 255);
                Draw.rect(this.signX, this.signY, this.signW, this.signH, false);
                Draw.setTextStyle(World().blueFont, 1, 1, 2);
                Draw.setColor(255, 255, 255, 255);
                Draw.text(this.signX + 10, this.signY + 10, this.signW - 20, this.signText, 0);
            },
        },
        [States.Shrink]: {
            enter: () => this.fsmTransitionAtTime(States.Idle, this.signTime),
            draw: () => {
                const t = clamp((this.ticks - this.startStateTime) / this.signTime, 0, 1);
                const w = lerp(this.signW, 0, t);
                const h = lerp(this.signH, 0, t);
                const x = this.signX + (this.signW - w) / 2;
                const y = this.signY + (this.signH - h) / 2;
                Draw.setColor(0, 0, 0, 255);
                Draw.rect(x, y, w, h, false);
            },
        },
    };
    constructor(args) {
        super(args);
        this.signText = args.properties?.Text ?? '';
    }
    update(ticks, dt) {
        this.fsmUpdate(this.#states, ticks);
    }
    draw() {
        super.draw();
        this.fsmDraw(this.#states);
    }
    collide(other, dir) {
        this.fsmCollide(this.#states, other, dir);
    }
}
export { Sign };
//# sourceMappingURL=sign.js.map