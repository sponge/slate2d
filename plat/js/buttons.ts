import * as SLT from 'slate2d';

class Buttons {
  static Up = 0;
  static Down = 1;
  static Left = 2;
  static Right = 3;
  static Jump = 4;
  static Shoot = 5;

  static register() {
    SLT.registerButtons(['up', 'down', 'left', 'right', 'jump', 'shoot']);
  }
}

export default Buttons;