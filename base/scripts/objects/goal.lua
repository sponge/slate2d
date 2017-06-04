local Event = require 'event'

local module = {
    spawner = function(obj, props)
        local ent = world:new_entity()
        world:addBody(ent, Body:new(obj.x, obj.y, 32, 32))
        world:addSprite(ent, Sprite:new(new_image("bone", "gfx/bone.png"), 32, 32, 0, 0))
        world:addAnimation(ent, Animation:new(0, 0, 7, 0.1, world.time))
        world:addTrigger(ent, Trigger:new(1))
        world:add_entity(ent)
    end
}

Event.on('trigger 1', function(activator, ent)
    world:kill_entity(ent)
    if world:entity_has(activator, COMPONENT_LUATABLE) then
        local t = world:getTable(activator.id)
        t.goal_time = world.time
    end
    play_speech("great job! you are a good dog!")
end)

return module