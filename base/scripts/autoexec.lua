inspect = require 'inspect'

math.randomseed(os.time())

frame = function(dt)
    require("lovebird").update()
end