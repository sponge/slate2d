import { printWin } from "slate2d";

// retained version of printwin, for use in update loops that don't necessarily
// run every frame due to frame timer accumulation. in most draw funcs, you'll
// want the regular printwin, they can be mixed
let messages: [string, string, any][] = [];
let retained = true;

function setRetain(value: boolean) {
  retained = value;
}

function dbg(title: string, key: string, value: any) {
  if (retained) messages.push([title, key, value]);
  else printWin(title, key, value);
}

function dbgval(value: any) {
  let caller = new Error().stack?.split('\n')[1];
  const lastSlash = caller?.lastIndexOf('/') ?? 0;
  const lastParens = caller?.lastIndexOf(')') ?? 0;
  const loc = caller?.substr(lastSlash + 1, lastParens - lastSlash - 1) ?? 'unknown';
  if (retained) messages.push(['Debug', loc, value.toString()]);
  else printWin('Debug', loc, value.toString());
}

function drawPrintWin() {
  for (let message of messages) {
    printWin(...message);
  }
}

function clearPrintWin() {
  messages = [];
}

export { dbg, dbgval, drawPrintWin, clearPrintWin, setRetain };