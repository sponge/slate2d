import { printWin } from "slate2d";

// retained version of printwin, for use in update loops that don't necessarily
// run every frame due to frame timer accumulation. in most draw funcs, you'll
// want the regular printwin, they can be mixed
let messages: [string, string, any][] = [];

function dbg(title: string, key: string, value: any) {
  messages.push([title, key, value]);
}

function drawPrintWin() {
  for (let message of messages) {
    printWin(...message);
  }
}

function clearPrintWin() {
  messages = [];
}

export { dbg, drawPrintWin, clearPrintWin };