#pragma once
#include "console/console.h"

void RegisterGameCvars( void );

extern cvar_t *dbg_drawBbox;
extern cvar_t *p_gravity;
extern cvar_t *p_jumpHeight;
extern cvar_t *p_wallSlideSpeed;
extern cvar_t *p_earlyJumpEndModifier;
extern cvar_t *p_pogoJumpHeight;
extern cvar_t *p_doubleJumpHeight;
extern cvar_t *p_wallJumpX;
extern cvar_t *p_maxSpeed;
extern cvar_t *p_terminalVelocity;
extern cvar_t *p_speedJumpBonus;
extern cvar_t *p_airAccel;
extern cvar_t *p_turnAirAccel;
extern cvar_t *p_accel;
extern cvar_t *p_skidAccel;
extern cvar_t *p_groundFriction;
extern cvar_t *p_airFriction;