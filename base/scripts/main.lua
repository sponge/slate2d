
local update = function (dt, ent)
    -- print('in update')
    print("dx: ".. mov:dx())
end
add_system(69, 0, update)