class ActionQueue {
  remaining { _actions.count }

  construct new(actions) {
    _time = 0 // total time
    _currentActionTime = 0 // time that has elapsed since the last action run
    _actions = actions // an array of [step_time, fn]
                 // step_time should be relative to last step, not absolute time
                 // fn should be a function to call
  }

  update(dt) {
    _time = _time + dt

    // if there are any actions, get the time since last step and compare
    if (_actions.count > 0 && _time - _currentActionTime >= _actions[0][0]) {
      _actions[0][1].call()
      _actions.removeAt(0)
      _currentActionTime = _time
    }
  }
}