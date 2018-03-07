class Game is Scene {
   construct new() {

   }

   update(dt) {
      Trap.print("dt "+dt.toString)
   }

   draw() {
      Trap.console("echo draw echo\n")
   }
}