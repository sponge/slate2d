var CollisionType;
(function (CollisionType) {
    CollisionType[CollisionType["Disabled"] = 0] = "Disabled";
    CollisionType[CollisionType["Enabled"] = 1] = "Enabled";
    CollisionType[CollisionType["Platform"] = 2] = "Platform";
    CollisionType[CollisionType["Trigger"] = 3] = "Trigger";
})(CollisionType || (CollisionType = {}));
export default CollisionType;
