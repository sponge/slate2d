import "engine" for Trap, Scene

class Game is Scene {
   construct new() {

   }

   update(dt) {
      Trap.print("dt "+dt.toString+"\n")
   }

   draw() {
      Trap.console("echo draw echo\n")
   }
}