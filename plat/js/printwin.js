import { printWin } from "slate2d";
// retained version of printwin, for use in update loops that don't necessarily
// run every frame due to frame timer accumulation. in most draw funcs, you'll
// want the regular printwin, they can be mixed
let messages = [];
function dbg(title, key, value) {
    messages.push([title, key, value]);
}
function dbgval(value) {
    let caller = new Error().stack?.split('\n')[1];
    const lastSlash = caller?.lastIndexOf('/') ?? 0;
    const lastParens = caller?.lastIndexOf(')') ?? 0;
    const loc = caller?.substr(lastSlash + 1, lastParens - lastSlash - 1) ?? 'unknown';
    messages.push(['Debug', loc, value]);
}
function drawPrintWin() {
    for (let message of messages) {
        printWin(...message);
    }
}
function clearPrintWin() {
    messages = [];
}
export { dbg, dbgval, drawPrintWin, clearPrintWin };
