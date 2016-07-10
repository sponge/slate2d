#include <cmath>
#include <algorithm>
#include <limits>

#include "local.h"

// returns true if the boxes are colliding (velocities are not used)
bool AABBCheck(Body b1, Body b2)
{
	return !(b1.x + b1.w < b2.x || b1.x > b2.x + b2.w || b1.y + b1.h < b2.y || b1.y > b2.y + b2.h);
}

// returns true if the boxes are colliding (velocities are not used)
// moveX and moveY will return the movement the b1 must move to avoid the collision
bool AABB(Body b1, Body b2, double& moveX, double& moveY)
{
	moveX = moveY = 0.0;

	double l = b2.x - (b1.x + b1.w);
	double r = (b2.x + b2.w) - b1.x;
	double t = b2.y - (b1.y + b1.h);
	double b = (b2.y + b2.h) - b1.y;

	// check that there was a collision
	if (l > 0 || r < 0 || t > 0 || b < 0)
		return false;

	// find the offset of both sides
	moveX = abs(l) < r ? l : r;
	moveY = abs(t) < b ? t : b;

	// only use whichever offset is the smallest
	if (abs(moveX) < abs(moveY))
		moveY = 0.0;
	else
		moveX = 0.0;

	return true;
}

// returns a box the spans both a current box and the destination box
Body GetSweptBroadphaseBox(Body b, double dx, double dy)
{
	Body broadphasebox(0.0, 0.0, 0.0, 0.0);

	broadphasebox.x = dx > 0 ? b.x : b.x + dx;
	broadphasebox.y = dy > 0 ? b.y : b.y + dy;
	broadphasebox.w = dx > 0 ? dx + b.w : b.w - dx;
	broadphasebox.h = dy > 0 ? dy + b.h : b.h - dy;

	return broadphasebox;
}

// performs collision detection on moving box b1 and static box b2
// returns the time that the collision occured (where 0 is the start of the movement and 1 is the destination)
// getting the new position can be retrieved by box.x = box.x + box.vx * collisiontime
// normalx and normaly return the normal of the collided surface (this can be used to do a response)
double SweptAABB(Body b1, Body b2, double dx, double dy, double& normalx, double& normaly)
{
	double xInvEntry, yInvEntry;
	double xInvExit, yInvExit;

	// find the distance between the objects on the near and far sides for both x and y
	if (dx > 0.0)
	{
		xInvEntry = b2.x - (b1.x + b1.w);
		xInvExit = (b2.x + b2.w) - b1.x;
	}
	else
	{
		xInvEntry = (b2.x + b2.w) - b1.x;
		xInvExit = b2.x - (b1.x + b1.w);
	}

	if (dy > 0.0)
	{
		yInvEntry = b2.y - (b1.y + b1.h);
		yInvExit = (b2.y + b2.h) - b1.y;
	}
	else
	{
		yInvEntry = (b2.y + b2.h) - b1.y;
		yInvExit = b2.y - (b1.y + b1.h);
	}

	// find time of collision and time of leaving for each axis (if statement is to prevent divide by zero)
	double xEntry, yEntry;
	double xExit, yExit;

	if (dx == 0.0)
	{
		xEntry = -std::numeric_limits<double>::infinity();
		xExit = std::numeric_limits<double>::infinity();
	}
	else
	{
		xEntry = xInvEntry / dx;
		xExit = xInvExit / dx;
	}

	if (dy == 0.0)
	{
		yEntry = -std::numeric_limits<double>::infinity();
		yExit = std::numeric_limits<double>::infinity();
	}
	else
	{
		yEntry = yInvEntry / dy;
		yExit = yInvExit / dy;
	}

	if (yEntry > 1.0) {
		yEntry = -std::numeric_limits<double>::max();
	}

	if (xEntry > 1.0) {
		xEntry = -std::numeric_limits<double>::max();
	}

	// find the earliest/latest times of collision
	double entryTime = std::max(xEntry, yEntry);
	double exitTime = std::min(xExit, yExit);

	// if there was no collision
	if (entryTime > exitTime || xEntry < 0.0 && yEntry < 0.0 || xEntry > 1.0 || yEntry > 1.0)
	{
		normalx = 0.0;
		normaly = 0.0;
		return 1.0;
	}
	else if (xEntry < 0.0 && ( (b1.x+b1.w) < b2.x || b1.x > (b2.x+b2.w) )) {
		normalx = 0.0;
		normaly = 0.0;
		return 1.0;
	}
	else if (yEntry < 0.0 && ((b1.y+b1.h) < b2.y || b1.y > (b2.y+b2.h))) {
		normalx = 0.0;
		normaly = 0.0;
		return 1.0;
	}
	else // if there was a collision
	{
		// calculate normal of collided surface
		if (xEntry > yEntry)
		{
			if (xInvEntry < 0.0)
			{
				normalx = 1.0;
				normaly = 0.0;
			}
			else
			{
				normalx = -1.0;
				normaly = 0.0;
			}
		}
		else
		{
			if (yInvEntry < 0.0)
			{
				normalx = 0.0;
				normaly = 1.0;
			}
			else
			{
				normalx = 0.0;
				normaly = -1.0;
			}
		}

		// return the time of collision
		return entryTime;
	}
}

const trace_t Move(EntityManager *es, Entity ent, double dx, double dy) {
	trace_t result;

	auto body = ent.component<Body>();

	auto broad = GetSweptBroadphaseBox(*body.get(), dx, dy);

	double minTime = 1.0;
	double normX = 0, normY = 0;

	for (auto ent2 : es->entities_with_components<Body>()) {
		if (ent == ent2) {
			continue;
		}

		auto body2 = ent2.component<Body>();

		if (AABBCheck(broad, *body2.get()) == false) {
			continue;
		}

		double time, tempNormX = 0, tempNormY = 0;

		time = SweptAABB(*body.get(), *body2.get(), dx, dy, tempNormX, tempNormY);

		if (time < minTime) {
			minTime = time;
			normX = tempNormX;
			normY = tempNormY;
		}

	}

	result.time = minTime;
	result.timeRemaining = 1.0 - minTime;
	result.normalX = normX;
	result.normalY = normY;
	result.x = body->x + dx * minTime;
	result.y = body->y + dy * minTime;

	return result;
}