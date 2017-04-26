local module = {
    spawner = function(obj, props)
        local ent = world:new_entity()
        world:addBody(ent, Body:new(obj.x, obj.y, 32, 32))
        world:addSprite(ent, Sprite:new(world:new_image("bone", "gfx/bone.png"), 32, 32, 0, 0))
        world:addAnimation(ent, Animation:new(0, 0, 7, 0.1, world.time))
        world:addTrigger(ent, Trigger:new(69))
        world:add_entity(ent)
    end
}

return module