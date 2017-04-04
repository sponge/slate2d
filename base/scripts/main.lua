local inspect = require 'inspect'
require 'components'

spawn_entity = function(world, ent)
	if ent.type == 'player' then
		local tmap = world:getTileMap(world.master_entity.id).map

		local ent = world:new_entity()
 --    // create the player
 --    auto ent = es.create();
 --    auto body = ent.assign<Body>(obj.x + (tmap->map->tile_width / 2), obj.y - 14.001, 16, 15);  // FIXME able to fall into ground if 14
 --    ent.assign<Movable>(0, 0);
 --    ent.assign<Renderable>(200, 30, 30, 200);
 --    ent.assign<PlayerInput>();
 --    ent.assign<Player>();
	
	-- auto playerImg = Img_Create("player", "gfx/dog.png");
	-- ent.assign<Sprite>(playerImg, Vec2(22, 15), Vec2(0, 0));

 --    // attach a camera to the world and target it at the player
 --    auto camera = worldEnt.assign<Camera>(1280, 720, 3, tmap->map->width * tmap->map->tile_width, tmap->map->height * tmap->map->tile_height);
 --    camera->target = body.get();
	else
		print("unhandled entity:")
		print(inspect(ent))
	end
end

local tmap = world:getTileMap(world.master_entity.id).map

-- FIXME: the rest of this hardcoded shit
local cam = Camera.new(1280, 720, 3, tmap.w * tmap.tile_width, tmap.h * tmap.tile_height)
cam.active = true
cam:Center(0,400)
cam:Bind()
world:addCamera(world.master_entity, cam)

local input = PlayerInput.new()
world:addPlayerInput(world.master_entity, input)