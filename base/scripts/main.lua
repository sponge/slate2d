local inspect = require 'inspect'

local WORLD_WIDTH = 1280
local WORLD_HEIGHT = 720

local rectUpdate = function (dt, ent, c)
    local dx = c.mov.dx * dt
    local dy = c.mov.dy * dt

    local move = trace(ent, dx, dy)

    c.body.x = move.pos.x
    c.body.y = move.pos.y

    if move.hit.valid then
        local dotprod = (dx * move.hit.normal.y + dy * move.hit.normal.x) * (1.0 - move.time)
        dx = dotprod * move.hit.normal.y
        dy = dotprod * move.hit.normal.x

        move = trace(ent, dx, dy)
        c.body.x = move.pos.x
        c.body.y = move.pos.y

        if dx ~= 0 and move.hit.normal.x ~= 0 then
            c.mov.dx = c.mov.dx * -1
        end

        if dy ~=0 and move.hit.normal.y ~= 0 then
            c.mov.dy = c.mov.dy * -1
        end 
    end

    if c.body.x + c.body.hw > WORLD_WIDTH or c.body.x - c.body.hw < 0 then
        if c.body.x - c.body.hw <= 0 then
            c.body.x = c.body.hw
        else
            c.body.x = WORLD_WIDTH - c.body.hw
        end
        c.mov.dx = c.mov.dx * -1
    end

    if c.body.y + c.body.hh > WORLD_HEIGHT or c.body.y - c.body.hh < 0 then
        if c.body.y - c.body.hh <= 0 then
            c.body.y = c.body.hh
        else
            c.body.y = WORLD_HEIGHT - c.body.hh
        end
        c.mov.dy = c.mov.dy * -1
    end
end
add_system("Rect Mover", 0, 1 + 2, rectUpdate)