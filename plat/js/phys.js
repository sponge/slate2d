const Phys = {
    // physics values
    // most values from https://cdn.discordapp.com/attachments/191015116655951872/332350193540268033/smw_physics.png
    pMeterCapacity: 112,
    friction: 0.0625,
    accel: 0.09375,
    skidAccel: 0.3125,
    runSpeed: 2.25,
    maxSpeed: 3,
    heldGravity: 0.1875,
    gravity: 0.375,
    earlyBounceFrames: 8,
    earlyJumpFrames: 6,
    lateJumpFrames: 6,
    terminalVelocity: 4,
    enemyJumpHeld: 5.5,
    enemyJump: 3,
    slideSpeed: 4,
    springJumpHeld: 8.5,
    springJump: 5.5,
    jumpHeights: [
        [3, 5.75],
        [2.5, 5.5625],
        [2, 5.4375],
        [1.5, 5.25],
        [1, 5.125],
        [0.5, 4.9375],
        [0, 4.8125]
    ]
};
export default Phys;
