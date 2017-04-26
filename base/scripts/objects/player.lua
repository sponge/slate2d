local clamp = function(val, min, max)
    return math.max(min, math.min(max, val))
end

local module = {
    spawner = function(obj, props)
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
    end,

    world_loaded = function()
        world:add_system {
            name = "Player Update",
            priority = 0,
            components = {COMPONENT_PLAYERINPUT, COMPONENT_BODY, COMPONENT_MOVABLE, COMPONENT_LUATABLE, COMPONENT_SPRITE, COMPONENT_ANIMATION},
            process = function(dt, ent, c)
                local player = c.table
                local mov = c.mov
                local input = c.input
                local spr = c.sprite
                local anim = c.animation
                local body = c.body

                local right_touch = world:trace(ent, 1, 0).time < 1e-7;
                local left_touch = world:trace(ent, -1, 0).time < 1e-7;
                local down_touch = world:trace(ent, 0, 1).time < 1e-7;
                local up_touch = world:trace(ent, 0, -1).time < 1e-7;

                world:debug_text(inspect(player))
                world:debug_text("l:" .. tostring(left_touch) .. " r:".. tostring(right_touch) .. " d:".. tostring(down_touch) .. " u:" .. tostring(up_touch))

                -- if they're on the ground, they can always jump
                if down_touch then
                    player.num_jumps = 0
                end

                player.is_wall_sliding = false

                -- if they're midair and are holding down toward the wall, they can wall jump
                player.can_wall_jump = (not down_touch) and (input.left and left_touch) or (input.right and right_touch)

                -- if they're moving down and touching a wall the direction they're holding down, they are wall sliding
                if mov.dy > 0 and player.can_wall_jump then
                    player.is_wall_sliding = true
                end

                -- FIXME: is this right?
                if down_touch and (mov.dy < 0 or not player.will_pogo) then
                    player.will_pogo = input.down
                end

                -- finished updating player, now start figuring out speeds

                mov.dy = mov.dy + cvars["p_gravity"].value * dt
                
                -- apply wallslide speed after gravity
                if player.is_wall_sliding then
                    mov.dy = cvars["p_wallSlideSpeed"].value
                end

                -- reset jump and slow upward velocity
                if not input.jump and player.jump_held then
                    player.jump_held = false
                    if mov.dy < 0 then
                        mov.dy = mov.dy * cvars["p_earlyJumpEndModifier"].value
                    end
                end

                -- if touching ground and are about to pogo, pogo is first prio
                if down_touch and player.will_pogo then
                    mov.dy = 0 - cvars["p_pogoJumpHeight"].value
                    player.num_jumps = 1
                    player.will_pogo = false
                -- check for various types of jumps
                elseif input.jump and not player.jump_held then
                    -- wall jump
                    if player.can_wall_jump then
                        mov.dy = 0 - cvars["p_doubleJumpHeight"].value
                        mov.dx = cvars["p_wallJumpX"].value * (input.right and -1 or 1)
                        player.stun_time = world.time + 0.2
                        player.jump_held = true
                        player.num_jumps = 1

                    -- regular on ground jump
                    elseif down_touch then
                        -- FIXME: mov.dy = -(p_jumpHeight->value + (fabs(mov.dx) >= p_maxSpeed->value * 0.25f ? p_speedJumpBonus->value : 0));
                        mov.dy = -cvars["p_jumpHeight"].value
                        player.jump_held = true
                        player.num_jumps = 1

                    -- midair double jump
                    elseif player.num_jumps < 2 then
                        mov.dy = -cvars["p_doubleJumpHeight"].value
                        player.num_jumps = 2
                        player.jump_held = true
                    end

                end

                -- moving in a direction
                if input.right or input.left then
                    local accel = 0;
                    local isSkidding = (input.left and mov.dx > 0) or (input.right and mov.dx < 0)
                    -- player can't move in any direction
                    if world.time < player.stun_time then
                        accel = 0
                    -- player has ground acceleration
                    elseif down_touch then
                        accel = isSkidding and cvars["p_skidAccel"].value or cvars["p_accel"].value
                    -- player has air acceleration
                    else
                        accel = isSkidding and cvars["p_turnAirAccel"].value or cvars["p_airAccel"].value
                    end
                    
                    if input.left then
                        accel = -accel
                    end

                    mov.dx = mov.dx + accel * dt
                    spr.flipX = not input.right

                elseif mov.dx ~= 0 then
                    -- apply ground friction to bring to a stop
                    local friction = cvars["p_groundFriction"].value * dt;
                    if friction > math.abs(mov.dx) then
                        mov.dx = 0;
                    else
                        mov.dx = mov.dx + friction * (mov.dx > 0 and -1 or 1);
                    end
                end

                mov.dx = clamp(mov.dx, -cvars["p_maxSpeed"].value, cvars["p_maxSpeed"].value);
                local uncappedY = mov.dy;
                mov.dy = clamp(mov.dy, -cvars["p_terminalVelocity"].value, cvars["p_terminalVelocity"].value);

                -- sprite and animation
                if mov.dx ~= 0 then
                    if anim.endFrame == 0 then
                        anim.startTime = world.time
                        anim.endFrame = 6
                    end
                else
                    anim.endFrame = 0
                end
                

                local xmove = world:trace(ent, mov.dx * dt, 0)
                body.x = xmove.pos.x
                if xmove.hit.valid then
                    mov.dx = 0
                end

                local ymove = world:trace(ent, 0, mov.dy * dt)
                body.y = ymove.pos.y
                if ymove.hit.valid then
                    mov.dy = 0
                end

                local trigger_ent = world:check_trigger(ent)
                if trigger_ent ~= nil then
                    local trig = world:getTrigger(trigger_ent.id)
                    trig.enabled = false
                    world:kill_entity(trigger_ent)
                    play_speech("great job! you are a good dog!")
                end

                if math.abs(mov.dy) < 0.2 then
                    mov.dy = 0;
                end

                -- keep going upward as long as we don't have an upward collision
                if mov.dy < 0 and not ymove.hit.valid then
                    mov.dy = uncappedY;
                end
            end
        }
    end
}



return module