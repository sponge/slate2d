/// <reference path="./decs.d.ts" />
import * as SLT from 'slate2d';
import { Game } from "./game.js";
class Main {
    static scene;
    static sceneName;
    static sceneTypes = {
        'game': Game,
    };
    static switchLevel(num, startPos) {
        const mapName = `maps/Map${num}.ldtkl`;
        Main.scene = new Game('', mapName, startPos);
        Main.sceneName = 'game';
        SLT.console("js_rungc");
    }
    save() {
        return [Main.sceneName, Main.scene.save()].join(',');
    }
    constructor(initialState) {
        if (initialState) {
            Main.sceneName = initialState.split(',', 1)[0];
            const state = initialState.substr(initialState.indexOf(',') + 1);
            Main.scene = new Main.sceneTypes[Main.sceneName](state);
        }
        else {
            Main.switchLevel(1);
        }
    }
    update(dt) {
        Main.scene.update(dt);
    }
    draw() {
        Main.scene.draw();
    }
}
export default Main;
