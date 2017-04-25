local clamp = function(val, min, max)
    return math.max(min, math.min(max, val))
end

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
        world:addBody(ent, Body:new(obj.x + (map.tile_width / 2), obj.y - 14.001, 15, 15))
        world:addMovable(ent, Movable:new(0, 0))
        world:addRenderable(ent, Renderable:new(200, 30, 30, 200))
        world:addPlayerInput(ent, PlayerInput:new())
        world:addSprite(ent, Sprite:new(world:new_image("player", "gfx/dog.png"), 22, 16, 0, 0))
        world:addAnimation(ent, Animation:new(0, 0, 6, 0.1, world.time))
        world:addTable(ent, {
            num_jumps = 0,
            is_wall_sliding = false,
            can_wall_jump = false,
            jump_held = false,
            will_pogo = false,
            stun_time = 0.0
        })
        world:add_entity(ent)

        camera.target = ent.id

    elseif obj.type == 'goal' then
        local ent = world:new_entity()
        world:addBody(ent, Body:new(obj.x, obj.y, 32, 32))
        world:addSprite(ent, Sprite:new(world:new_image("bone", "gfx/bone.png"), 32, 32, 0, 0))
        world:addAnimation(ent, Animation:new(0, 0, 7, 0.1, world.time))
        world:addTrigger(ent, Trigger:new(69))
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
    components = {COMPONENT_PLAYERINPUT, COMPONENT_BODY, COMPONENT_MOVABLE, COMPONENT_LUATABLE, COMPONENT_SPRITE, COMPONENT_ANIMATION},
    process = function(dt, ent, c)
        local player = c.table

        local right_touch = world:trace(ent, 1, 0).time < 1e-7;
        local left_touch = world:trace(ent, -1, 0).time < 1e-7;
        local down_touch = world:trace(ent, 0, 1).time < 1e-7;
        local up_touch = world:trace(ent, 0, -1).time < 1e-7;

        world:debug_text(inspect(c.table))
        world:debug_text("l:" .. tostring(left_touch) .. " r:".. tostring(right_touch) .. " d:".. tostring(down_touch) .. " u:" .. tostring(up_touch))

        -- if they're on the ground, they can always jump
        if down_touch then
            player.num_jumps = 0
        end

        player.is_wall_sliding = false

        -- if they're midair and are holding down toward the wall, they can wall jump
        player.can_wall_jump = (not down_touch) and (c.input.left and left_touch) or (c.input.right and right_touch)

        -- if they're moving down and touching a wall the direction they're holding down, they are wall sliding
        if c.mov.dy > 0 and player.can_wall_jump then
            player.is_wall_sliding = true
        end

        -- FIXME: is this right?
        if down_touch and (c.mov.dy < 0 or not player.will_pogo) then
            player.will_pogo = c.input.down
        end

        -- finished updating player, now start figuring out speeds

        c.mov.dy = c.mov.dy + cvars["p_gravity"].value * dt
        
        -- apply wallslide speed after gravity
        if player.is_wall_sliding then
            c.mov.dy = cvars["p_wallSlideSpeed"].value
        end

        -- reset jump and slow upward velocity
        if not c.input.jump and player.jump_held then
            player.jump_held = false
            if c.mov.dy < 0 then
                c.mov.dy = c.mov.dy * cvars["p_earlyJumpEndModifier"].value
            end
        end

        -- if touching ground and are about to pogo, pogo is first prio
        if down_touch and player.will_pogo then
            c.mov.dy = 0 - cvars["p_pogoJumpHeight"].value
            player.num_jumps = 1
            player.will_pogo = false
        -- check for various types of jumps
        elseif c.input.jump and not player.jump_held then
            -- wall jump
            if player.can_wall_jump then
                c.mov.dy = 0 - cvars["p_doubleJumpHeight"].value -- TODO: separate wall jump height?
                c.mov.dx = cvars["p_wallJumpX"].value * (c.input.right and -1 or 1)
                player.stun_time = world.time + 0.1
                player.jump_held = true
                player.num_jumps = 1

            -- regular on ground jump
            elseif down_touch then
                -- FIXME: mov.dy = -(p_jumpHeight->value + (fabs(mov.dx) >= p_maxSpeed->value * 0.25f ? p_speedJumpBonus->value : 0));
                c.mov.dy = -cvars["p_jumpHeight"].value
                player.jump_held = true
                player.num_jumps = 1

            -- midair double jump
            elseif player.num_jumps < 2 then
                c.mov.dy = -cvars["p_doubleJumpHeight"].value
                player.num_jumps = 2
                player.jump_held = true
            end

        end

        if c.input.right or c.input.left then
            -- float accel = 0;
            -- auto isSkidding = (input.left && mov.dx > 0) || (input.right && mov.dx < 0);
            -- // FIXME: check stun time here
            -- if (mov.downTouch) {
            --     accel = isSkidding ? p_skidAccel->value : p_accel->value;
            -- }
            -- else {
            --     accel = isSkidding ? p_turnAirAccel->value : p_airAccel->value;
            -- }

            -- mov.dx += (input.right ? accel : input.left ? -accel : 0) * dt;
            -- spr.flipX = !input.right;
        elseif c.mov.dx ~= 0 then
            -- auto friction = p_groundFriction->value * dt;
            -- if (friction > fabs(mov.dx)) {
            --     mov.dx = 0;
            -- }
            -- else {
            --     mov.dx += friction * (mov.dx > 0 ? -1 : 1);
            -- }
        end

        -- mov.dx = clamp(mov.dx, -p_maxSpeed->value, p_maxSpeed->value);
        -- auto uncappedY = mov.dy;
        -- mov.dy = clamp(mov.dy, -p_terminalVelocity->value, p_terminalVelocity->value);

        -- sprite and animation
        if c.mov.dx ~= 0 then
            c.sprite.flipX = c.mov.dx < 0 and true or false
            if c.animation.endFrame == 0 then
                c.animation.startTime = world.time
                c.animation.endFrame = 6
            end
        else
            c.animation.endFrame = 0
        end

        local xmove = world:trace(ent, c.mov.dx * dt, 0)
        c.body.x = xmove.pos.x
        if xmove.hit.valid then
            c.mov.dx = 0
        end

        local ymove = world:trace(ent, 0, c.mov.dy * dt)
        c.body.y = ymove.pos.y
        if ymove.hit.valid then
            c.mov.dy = 0
        end

        local trigger_ent = world:check_trigger(ent)
        if trigger_ent ~= nil then
            local trig = world:getTrigger(trigger_ent.id)
            world:debug_text(inspect(trig))
            trig.enabled = false
            print("triggered! ".. tostring(trig.type))
        end

        -- if (fabs(mov.dx) < 0.2) {
        --     mov.dx = 0;
        -- }

        -- if (fabs(mov.dy) < 0.2) {
        --     mov.dy = 0;
        -- }

        -- // keep going upward as long as we don't have an upward collision
        -- if (mov.dy < 0 && !ymove.hit.valid) {
        --     mov.dy = uncappedY;
        -- }
    end
}