local inspect = require 'inspect'
require 'components'

local WORLD_WIDTH = 1280
local WORLD_HEIGHT = 720

for i=0,15 do
    local x = (i % 4) * (WORLD_WIDTH / 4) + 100
    local y = math.floor(i / 4) * (WORLD_HEIGHT / 4) + 100
    local w = math.random(40,90)
    local h = math.random(40,90)
    local dx = math.random(50, 250) * (x < WORLD_WIDTH / 2 and 1 or -1);
    local dy = math.random(50, 250) * (y < WORLD_HEIGHT / 2 and 1 or -1);
    local ent = world:get_entity()
    world:addBody(ent, Body.new(x, y, w, h))
    world:addMovable(ent, Movable.new(dx, dy))
    world:addRenderable(ent, Renderable.new(math.random(0,255), math.random(0,255), math.random(0,255), math.random(55,255)))
    world:add_entity(ent)
end

local rectUpdate = function (dt, ent, c)
    local dx = c.mov.dx * dt
    local dy = c.mov.dy * dt

    local move = world:trace(ent, dx, dy)

    c.body.x = move.pos.x
    c.body.y = move.pos.y

    if move.hit.valid then
        local dotprod = (dx * move.hit.normal.y + dy * move.hit.normal.x) * (1.0 - move.time)
        dx = dotprod * move.hit.normal.y
        dy = dotprod * move.hit.normal.x

        move = world:trace(ent, dx, dy)
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
world:add_system("Rect Mover", 0, COMPONENT_BODY|COMPONENT_MOVABLE, rectUpdate)