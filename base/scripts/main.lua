init = function() 
    -- FIXME: the rest of this hardcoded shit
    local tmap = world:getTileMap(world.master_entity.id).map
    local cam = Camera.new(1280, 720, 3, tmap.w * tmap.tile_width, tmap.h * tmap.tile_height)
    cam.active = true
    cam:Bind()
    world:addCamera(world.master_entity, cam)
end

spawn_entity = function(world, obj, props)
    if obj.type == 'player' then
        local map = world:getTileMap(world.master_entity.id).map
        local camera = world:getCamera(world.master_entity.id)

        local ent = world:new_entity()
        world:addBody(ent, Body:new(obj.x + (map.tile_width / 2), obj.y - 14.001, 16, 15))
        world:addMovable(ent, Movable:new(0, 0))
        world:addRenderable(ent, Renderable:new(200, 30, 30, 200))
        world:addPlayerInput(ent, PlayerInput:new())
        world:addSprite(ent, Sprite:new(world:new_image("player", "gfx/dog.png"), 22, 15, 0, 0))
        world:addTable(ent, {
            num_jumps = 0,
            is_wall_sliding = false,
            can_wall_jump = false,
            jump_held = false,
            will_pogo = false,
            stunTime = 0.0
        })
        world:add_entity(ent)

        camera.target = ent.id

    elseif obj.type == 'goal' then
        local map = world:getTileMap(world.master_entity).map

        local ent = world:new_entity()
        world:addBody(ent, Body:new(obj.x, obj.y, 32, 32))
        world:addSprite(ent, Sprite:new(world:new_image("bone", "gfx/bone.png"), 32, 32, 0, 0))
        world:addAnimation(ent, Animation:new(0, 0, 7, 0.1, world.time))
        world:add_entity(ent)

    else
        print("unhandled entity: " .. obj.type)
    end
end

world:add_system {
    name = "Camera Update",
    priority = 0,
    components = {COMPONENT_CAMERA},
    process = function(dt, ent, c)
        if c.camera.active == false then
            return
        end

        if c.camera.target < 0 then
            return
        end

        local body = world:getBody(c.camera.target)
        c.camera:Center(body.x, body.y)
        c.camera:Bind()
    end
}

world:add_system {
    name = "Player Update",
    priority = 0,
    components = {COMPONENT_PLAYERINPUT, COMPONENT_BODY, COMPONENT_MOVABLE, COMPONENT_LUATABLE, COMPONENT_SPRITE},
    process = function(dt, ent, c)
        if c.playerinput.right then
            c.mov.dx = 50 * dt
        elseif c.playerinput.left then
            c.mov.dx = -50 * dt
        else
            c.mov.dx = 0
        end

        if c.playerinput.up then
            c.mov.dy = -50 * dt
        elseif c.playerinput.down then
            c.mov.dy = 50 * dt
        else
            c.mov.dy = 0
        end

        if c.mov.dx ~= 0 then
            c.sprite.flipX = c.mov.dx < 0 and true or false
        end

        local xmove = world:trace(ent, c.mov.dx, 0)
        c.body.x = xmove.pos.x
        if xmove.hit.valid then
            c.mov.dx = 0
        end

        local ymove = world:trace(ent, 0, c.mov.dy)
        c.body.y = ymove.pos.y
        if ymove.hit.valid then
            c.mov.dy = 0
        end
    end
}