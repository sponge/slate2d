const Phys = {
    // physics values
    // most values from https://cdn.discordapp.com/attachments/191015116655951872/332350193540268033/smw_physics.png
    pMeterCapacity: 112,
    friction: 0.03125 * 2,
    accel: 0.046875 * 2,
    skidAccel: 0.15625 * 2,
    runSpeed: 1.125 * 2,
    maxSpeed: 1.5 * 2,
    heldGravity: 0.1875 / 2 * 2,
    gravity: 0.1875 * 2,
    earlyBounceFrames: 8,
    earlyJumpFrames: 6,
    lateJumpFrames: 6,
    terminalVelocity: 2 * 2,
    enemyJumpHeld: 3.4 * 2,
    enemyJump: 1.9 * 2,
    slideSpeed: 4,
    jumpHeights: [
        [3, 2.875 * 2],
        [2.5, 2.78125 * 2],
        [2, 2.71875 * 2],
        [1.5, 2.625 * 2],
        [1, 2.5625 * 2],
        [0.5, 2.46875 * 2],
        [0, 2.40625 * 2]
    ]
};
export default Phys;
