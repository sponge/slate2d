class ObjectPool {
    #template;
    #pool = new Set();
    #active = new Set();
    constructor(objType, size) {
        this.#template = { ...objType };
        for (let i = 0; i < size; i++) {
            this.#pool.add({ ...objType });
        }
    }
    get() {
        if (this.#pool.size == 0) {
            this.#pool.add({ ...this.#template });
        }
        const obj = this.#pool.keys().next().value;
        this.#active.add(obj);
        return obj;
    }
    release(obj) {
        if (!this.#active.has(obj)) {
            return;
        }
        this.#active.delete(obj);
        this.#pool.add(obj);
    }
}
export default ObjectPool;
