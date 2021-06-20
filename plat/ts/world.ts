import Main from "./main";

function World() {
  return ((globalThis as any).main as Main);
}

export default World;