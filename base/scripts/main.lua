local loaded_objects = {}

init = function() 
    -- FIXME: the rest of this hardcoded shit
    local tmap = world:getTileMap(world.master_entity.id).map
    local cam = Camera.new(1280, 720, 3, tmap.w * tmap.tile_width, tmap.h * tmap.tile_height)
    cam.active = true
    cam:Bind()
    world:addCamera(world.master_entity, cam)
    play_music("/music/frantic_-_dog_doesnt_care.it")
end

spawn_entity = function(obj, props)
    local modstr = string.gsub(obj.type, '%.', '')

    -- hack to reload object scripts on map load
    if loaded_objects[modstr] ~= true then
        package.loaded['objects/'.. modstr] = nil
    end
    
    local res, mod = pcall(require, 'objects/'.. modstr)
    if res == true then
        mod.spawner(obj, props)
        if loaded_objects[modstr] ~= true and type(mod.world_loaded) == 'function' then
            mod.world_loaded()
        end
        loaded_objects[modstr] = true
    else
        print("unhandled entity type: " .. modstr)
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