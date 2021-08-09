import Game from "./game.js";
import Main from "./main.js";

function World() {
  return Main.scene as Game;
}

export default World;