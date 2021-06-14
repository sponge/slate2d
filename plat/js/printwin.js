import { printWin } from "slate2d";
// retained version of printwin, for use in update loops that don't necessarily
// run every frame due to frame timer accumulation. in most draw funcs, you'll
// want the regular printwin, they can be mixed
let messages = [];
function dbg(title, key, value) {
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
