/// <reference path="./decs.d.ts" />
import * as SLT from 'slate2d';
import Game from "./game.js";

interface IScene {
  save(): string,
  update(dt: number): void,
  draw(): void,
}

class Main {
  static scene: IScene;
  static sceneName: string;

  static sceneTypes: { [key: string]: any } = {
    'game': Game,
  };

  static restartLevel() {
    if (Main.scene instanceof Game == false) {
      return;
    }

    const map = (Main.scene as Game).state.mapName;
    this.scene = new Game('', map);
    SLT.console("js_rungc");
  }

  static switchLevel(num: number) {
    const mapName = `maps/${String(num).padStart(4, '0')}-Level_${num}.ldtkl`;
    this.scene = new Game('', mapName);
    SLT.console("js_rungc");
  }

  save() {
    return [Main.sceneName, Main.scene.save()].join(',');
  }

  constructor(initialState: string) {
    if (initialState) {
      Main.sceneName = initialState.split(',', 1)[0];
      const state = initialState.substr(initialState.indexOf(',') + 1);
      Main.scene = new Main.sceneTypes[Main.sceneName](state);
    }
    else {
      Main.switchLevel(0);
    }
  }

  update(dt: number) {
    Main.scene.update(dt);
  }

  draw() {
    Main.scene.draw();
  }
}

export default Main;
