class Timer {
  static init() {
    __fibers = []
  }

  static clear() {
    __fibers.clear()
  }

  static runLater(time, func) {
    var f = Fiber.new {
      while (time > 0) {
        var t = Fiber.yield()
        time = time - t
      }

      func.call()
    }

    __fibers.add(f)
  }

  static tick(t) {
    for (f in __fibers) {
      f.call(t)
    }

    if (__fibers.isEmpty) {
      return
    }

    for (i in __fibers.count-1..0) {
      if (__fibers[i].isDone) {
        __fibers.removeAt(i)
      }
    }
  }
}
