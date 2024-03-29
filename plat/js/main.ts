/// <reference path="./decs.d.ts" />
import * as SLT from 'slate2d';
import { Game } from "./game.js";

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

  static switchLevel(num: number, startPos?: [number, number]) {
    const mapName = `maps/Map${num}.ldtkl`;
    Main.scene = new Game('', mapName, startPos);
    Main.sceneName = 'game';
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
      Main.switchLevel(1);
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
