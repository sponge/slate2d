
local update = function (dt, ent)
    components.body.x = 69
    --local x, y, dx, dy = body:x(), body:y(), movable:dx(), movable:dy()
    --print("x:" .. x .. " y:".. y .." dx:".. dx .. " dy:".. dy)
end
add_system(69, 0, update)