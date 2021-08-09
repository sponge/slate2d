/// <reference path="./decs.d.ts" />
import * as SLT from 'slate2d';
import Game from "./game.js";
class Main {
    static scene;
    static sceneName;
    static sceneTypes = {
        'game': Game,
    };
    static restartLevel() {
        if (Main.scene instanceof Game == false) {
            return;
        }
        const map = Main.scene.state.mapName;
        this.scene = new Game(''); // FIXME: can't specify map name here
        SLT.console("js_rungc");
    }
    save() {
        return [Main.sceneName, Main.scene.save()].join(',');
    }
    constructor(initialState) {
        if (initialState) {
            Main.sceneName = initialState.split(',', 1)[0];
            console.log(Main.sceneName);
            const state = initialState.substr(initialState.indexOf(',') + 1);
            Main.scene = new Main.sceneTypes[Main.sceneName](state);
        }
        else {
            Main.sceneName = 'game';
            Main.scene = new Game('');
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
