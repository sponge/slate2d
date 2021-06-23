class ObjectPool<T> {
  #template: T;
  #pool: Set<T> = new Set();
  #active: Set<T> = new Set();

  constructor(objType: T, size: number) {
    this.#template = { ...objType };
    for (let i = 0; i < size; i++) {
      this.#pool.add({ ...objType });
    }
  }

  get() {
    if (this.#pool.size == 0) {
      this.#pool.add({ ...this.#template });
    }

    const obj: T = this.#pool.keys().next().value;
    this.#active.add(obj);

    return obj;
  }

  release(obj: T) {
    if (!this.#active.has(obj)) {
      return;
    }

    this.#active.delete(obj);
    this.#pool.add(obj);
  }
}

export default ObjectPool;