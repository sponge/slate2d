/// <reference path="./decs.d.ts" />
import * as SLT from 'slate2d';
import { Game } from "./game.js";
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
        Main.scene = new Game('', map);
        Main.sceneName = 'game';
        SLT.console("js_rungc");
    }
    static switchLevel(num) {
        const mapName = `maps/${String(num).padStart(4, '0')}-Level_${num}.ldtkl`;
        Main.scene = new Game('', mapName);
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
            Main.switchLevel(0);
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
