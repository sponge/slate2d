
local rectUpdate = function (dt, ent, c)
    c.body.x = c.body.x + 10 * dt
    c.mov.dx = 0
    local tr = trace(ent, c.mov.dx, c.mov.dy)
end
add_system(0, 0, rectUpdate)