#pragma once
#include <math.h>

static const double EPSILON = 1E-8;

template <typename T>
T clamp(const T& n, const T& lower, const T& upper) {
	return std::max(lower, std::min(n, upper));
}

template <typename T>
T sign(const T& n) {
	return n < 0 ? -1 : 1;
}

typedef struct Vec2 {
	Vec2(double x, double y) : x(x), y(y) {}
	Vec2() : x(0), y(0) {}
	double x, y;
} vec2_t;

typedef struct Hit {
	Hit() {}
	bool valid = false;
	Vec2 pos, delta, normal;
	double time = 1.0;
} hit_t;

typedef struct Sweep {
	Hit hit;
	Vec2 pos;
	double time = 1.0;
} sweep_t;

typedef struct Box {
	Box(double x, double y, double w, double h) : pos(x,y), size(w,h), half(w*0.5, h*0.5) {}
	Vec2 pos, half, size;
} box_t;

#ifdef AABB_IMPLEMENTATION

double Vec2_length(Vec2 vec) {
	double len = vec.x * vec.x + vec.y * vec.y;
	len = sqrt(len);
	return len;
}

const Vec2 Vec2_normalize(Vec2 vec) {
	Vec2 out = { 0,0 };
	double len = Vec2_length(vec);

	if (len > 0) {
		double invLen = 1.0 / len;
		out.x = vec.x * invLen;
		out.y = vec.y * invLen;
	}

	return out;
}

Hit intersectVec2(Box box, Vec2 point) {
	Hit hit;

	auto dx = point.x - box.pos.x;
	auto px = box.half.x - abs(dx);

	if (px <= 0) {
		return hit;
	}

	auto dy = point.y - box.pos.y;
	auto py = box.half.y - abs(dy);
	
	if (py <= 0) {
		return hit;
	}

	hit.valid = true;

	if (px < py) {
		auto sx = sign(dx);
		hit.delta.x = px * sx;
		hit.normal.x = sx;
		hit.pos.x = box.pos.x + (box.half.x * sx);
		hit.pos.y = point.y;
	}
	else {
		auto sy = sign(dy);
		hit.delta.y = py * sy;
		hit.normal.y = sy;
		hit.pos.x = point.x;
		hit.pos.y = box.pos.y + (box.half.y * sy);
	}

	return hit;
}

Hit intersectSegment(Box box, Vec2 pos, Vec2 delta, double paddingX = 0.0, double paddingY = 0.0) {
	Hit hit;

	auto scale = Vec2(1.0 / delta.x, 1.0 / delta.y);
	auto sgn = Vec2(sign(scale.x), sign(scale.y));

	Vec2 nearTime, farTime;
	nearTime.x = (box.pos.x - sgn.x * (box.half.x + paddingX) - pos.x) * scale.x;
	nearTime.y = (box.pos.y - sgn.y * (box.half.y + paddingY) - pos.y) * scale.y;

	farTime.x = (box.pos.x + sgn.x * (box.half.x + paddingX) - pos.x) * scale.x;
	farTime.y = (box.pos.y + sgn.y * (box.half.y + paddingY) - pos.y) * scale.y;

	if (nearTime.x > farTime.y || nearTime.y > farTime.x) {
		return hit;
	}

	auto d_nearTime = nearTime.x > nearTime.y ? nearTime.x : nearTime.y;
	auto d_farTime = farTime.x < farTime.y ? farTime.x : farTime.y;

	if (d_nearTime >= 1 || d_farTime <= 0) {
		return hit;
	}

	hit.valid = true;
	hit.time = clamp(d_nearTime, 0.0, 1.0);

	if (nearTime.x > nearTime.y) {
		hit.normal.x = -sgn.x;
		hit.normal.y = 0;
	}
	else {
		hit.normal.x = 0;
		hit.normal.y = -sgn.y;
	}

	hit.delta.x = hit.time * delta.x;
	hit.delta.y = hit.time * delta.y;
	hit.pos.x = pos.x + hit.delta.x;
	hit.pos.y = pos.y + hit.delta.y;

	return hit;
}

Hit intersectAABB(Box box1, Box box2) {
	Hit hit;

	auto dx = box2.pos.x - box1.pos.x;
	auto px = (box2.half.x + box1.half.x) - abs(dx);

	if (px <= 0) {
		return hit;
	}

	auto dy = box2.pos.y - box1.pos.y;
	auto py = (box2.half.y + box1.half.y) - abs(dy);
	
	if (py <= 0) {
		return hit;
	}

	hit.valid = true;
	hit.time = 0;

	if (px < py) {
		auto sx = sign(dx);
		hit.delta.x = px * sx;
		hit.normal.x = sx;
		hit.pos.x = box1.pos.x + (box1.half.x * sx);
		hit.pos.y = box2.pos.y;
	}
	else {
		auto sy = sign(dy);
		hit.delta.y = py * sy;
		hit.normal.y = sy;
		hit.pos.x = box2.pos.x;
		hit.pos.y = box1.pos.y + (box1.half.y * sy);
	}

	return hit;
}

Sweep sweepAABB(Box box1, Box box2, Vec2 delta) {
	Sweep sweep;

	if (delta.x == 0 && delta.y == 0) {
		sweep.pos.x = box2.pos.x;
		sweep.pos.y = box2.pos.y;
		sweep.hit = intersectAABB(box1, box2);
		if (sweep.hit.valid) {
			sweep.time = sweep.hit.time = 0;
		}
		else {
			sweep.time = 1;
		}

		return sweep;
	}
	
	sweep.hit = intersectSegment(box1, box2.pos, delta, box2.half.x, box2.half.y);
	if (sweep.hit.valid) {
		// FIXME: epsilon?
		sweep.time = clamp(sweep.hit.time, 0.0, 1.0);
		sweep.pos.x = box2.pos.x + delta.x * sweep.time;
		sweep.pos.y = box2.pos.y + delta.y * sweep.time;
		auto direction = Vec2_normalize(delta);
		sweep.hit.pos.x += direction.x * box2.half.x;
		sweep.hit.pos.y += direction.y * box2.half.y;
	}
	else {
		sweep.pos.x = box2.pos.x + delta.x;
		sweep.pos.y = box2.pos.y + delta.y;
		sweep.time = 1;
	}
	return sweep;
}

#ifdef DEBUG
#include <stdio.h>
#include <assert.h>

void testCollision() {
	printf("intersectPoint should return not valid when not colliding\n");
	{
		auto box = Box(0, 0, 16, 16);
		Vec2 points[] = {
			Vec2(0, -16),
			Vec2(16, -16),
			Vec2(16, 0),
			Vec2(16, 16),
			Vec2(0, 16),
			Vec2(-16, 16),
			Vec2(-16, 0)
		};

		for (auto point : points) {
			auto hit = intersectVec2(box, point);
			assert(hit.valid == false);
		}
	}

	printf("intersectPoint should return a hit when colliding\n");
	{
		auto box = Box(0, 0, 16, 16);
		auto point = Vec2(4, 4);
		auto hit = intersectVec2(box, point);

		assert(hit.valid == true);
	}

	printf("intersectPoint should set hit pos and normal to nearest edge of box\n");
	{
		auto box = Box(0, 0, 16, 16);
		auto hit = intersectVec2(box, Vec2(-4, -2));
		assert(hit.pos.x == -8);
		assert(hit.pos.y == -2);
		assert(hit.delta.x == -4);
		assert(hit.delta.y == 0);
		assert(hit.normal.x == -1);
		assert(hit.normal.y == 0);

		hit = intersectVec2(box, Vec2(4, -2));
		assert(hit.pos.x == 8);
		assert(hit.pos.y == -2);
		assert(hit.delta.x == 4);
		assert(hit.delta.y == 0);
		assert(hit.normal.x == 1);
		assert(hit.normal.y == 0);

		hit = intersectVec2(box, Vec2(2, -4));
		assert(hit.pos.x == 2);
		assert(hit.pos.y == -8);
		assert(hit.delta.x == 0);
		assert(hit.delta.y == -4);
		assert(hit.normal.x == 0);
		assert(hit.normal.y == -1);

		hit = intersectVec2(box, Vec2(2, 4));
		assert(hit.pos.x == 2);
		assert(hit.pos.y == 8);
		assert(hit.delta.x == 0);
		assert(hit.delta.y == 4);
		assert(hit.normal.x == 0);
		assert(hit.normal.y == 1);
	}

	printf("intersectSegment should return not valid when not colliding\n");
	{
		auto box = Box(0, 0, 16, 16);
		auto hit = intersectSegment(box, Vec2(-16, -16), Vec2(32, 0));

		assert(hit.valid == false);
	}

	printf("intersectSegment should return hit when colliding\n");
	{
		auto box = Box(0, 0, 16, 16);
		auto point = Vec2(-16, 4);
		auto delta = Vec2(32, 0);
		auto hit = intersectSegment(box, point, delta);
		auto time = 0.25;

		assert(hit.valid == true);
		// FIXME: is this actually necessary in use?
		//assert(hit.collider == aabb);
		assert(hit.time == time);
		assert(hit.pos.x == point.x + delta.x * time);
		assert(hit.pos.y == point.y + delta.y * time);
		assert(hit.delta.x == delta.x * time);
		assert(hit.delta.y == delta.y * time);
		assert(hit.normal.x == -1);
		assert(hit.normal.y == 0);
	}

	printf("intersectSegment should set hit.time to zero when segment starts inside box\n");
	{
		auto box = Box(0, 0, 16, 16);
		auto point = Vec2(-4, 4);
		auto delta = Vec2(32, 0);
		auto hit = intersectSegment(box, point, delta);

		assert(hit.time == 0);
		assert(hit.pos.x == -4);
		assert(hit.pos.y == 4);
		assert(hit.delta.x == 0);
		assert(hit.delta.y == 0);
		assert(hit.normal.x == -1);
		assert(hit.normal.y == 0);
	}

	printf("intersectSegment should add padding to half size of box\n");
	{
		auto box = Box(0, 0, 16, 16);
		auto point = Vec2(-16, 4);
		auto delta = Vec2(32, 0);
		auto padding = 4;
		auto hit = intersectSegment(box, point, delta, padding, padding);
		auto time = 0.125;

		// FIXME: is this actually necessary in use?
		//assert(hit.collider == aabb);
		assert(hit.time == time);
		assert(hit.pos.x == point.x + delta.x * time);
		assert(hit.pos.y == point.y + delta.y * time);
		assert(hit.delta.x == delta.x * time);
		assert(hit.delta.y == delta.y * time);
		assert(hit.normal.x == -1);
		assert(hit.normal.y == 0);
	}

	printf("intersectSegment should have consistent results in both directions\n");
	{
		auto box = Box(0, 0, 64, 64);
		auto farPos = Vec2(64, 0);
		auto farToNearDelta = Vec2(-32, 0);
		auto hit = intersectSegment(box, farPos, farToNearDelta);
		assert(hit.valid == false);
		
		auto nearPos = Vec2(32, 0);
		auto nearToFarDelta = Vec2(32, 0);
		hit = intersectSegment(box, nearPos, nearToFarDelta);
		assert(hit.valid == false);
	}

	printf("intersectSegment should work when segment is axis aligned\n");
	{
		auto box = Box(0, 0, 32, 32);
		auto pos = Vec2(-32, 0);
		auto delta = Vec2(64, 0);
		auto hit = intersectSegment(box, pos, delta);

		assert(hit.time == 0.25);
		assert(hit.normal.x == -1);
		assert(hit.normal.y == 0);
	}

	printf("intersectAABB should return not valid when not colliding\n");
	{
		auto box1 = Box(0, 0, 16, 16);
		auto box2 = Box(32, 0, 16, 16);

		auto hit = intersectAABB(box1, box2);
		assert(hit.valid == false);

		box2.pos = Vec2(-32, 0);
		hit = intersectAABB(box1, box2);
		assert(hit.valid == false);

		box2.pos = Vec2(0, 32);
		hit = intersectAABB(box1, box2);
		assert(hit.valid == false);

		box2.pos = Vec2(0, -32);
		hit = intersectAABB(box1, box2);
		assert(hit.valid == false);

		box2.pos = Vec2(0, -32);
		hit = intersectAABB(box1, box2);
		assert(hit.valid == false);
	}

	printf("intersectAABB should return not valid when edges are flush\n");
	{
		auto box1 = Box(0, 0, 16, 16);
		auto box2 = Box(16, 0, 16, 16);
		assert(intersectAABB(box1, box2).valid == false);

		box2.pos = Vec2(-16, 0);
		assert(intersectAABB(box1, box2).valid == false);

		box2.pos = Vec2(0, 16);
		assert(intersectAABB(box1, box2).valid == false);

		box2.pos = Vec2(0, -16);
		assert(intersectAABB(box1, box2).valid == false);

		box2.pos = Vec2(0, -16);
		assert(intersectAABB(box1, box2).valid == false);
	}

	printf("intersectAABB should return hit when colliding\n");
	{
		auto box1 = Box(0, 0, 16, 16);
		auto box2 = Box(8, 0, 16, 16);
		auto hit = intersectAABB(box1, box2);
		assert(hit.valid == true);
	}

	printf("intersectAABB should set hit.pos and hit.normal to nearest edge of box 1\n");
	{
		auto box1 = Box(0, 0, 16, 16);
		auto box2 = Box(4, 0, 16, 16);
		auto hit = intersectAABB(box1, box2);
		assert(hit.pos.x == 8);
		assert(hit.pos.y == 0);
		assert(hit.normal.x == 1);
		assert(hit.normal.y == 0);

		box2.pos = Vec2(-4, 0);
		hit = intersectAABB(box1, box2);
		assert(hit.pos.x == -8);
		assert(hit.pos.y == 0);
		assert(hit.normal.x == -1);
		assert(hit.normal.y == 0);

		box2.pos = Vec2(0, 4);
		hit = intersectAABB(box1, box2);
		assert(hit.pos.x == 0);
		assert(hit.pos.y == 8);
		assert(hit.normal.x == 0);
		assert(hit.normal.y == 1);

		box2.pos = Vec2(0, -4);
		hit = intersectAABB(box1, box2);
		assert(hit.pos.x == 0);
		assert(hit.pos.y == -8);
		assert(hit.normal.x == 0);
		assert(hit.normal.y == -1);
	}

	printf("intersectAABB should set hit.delta to move box 2 out of collision\n");
	{
		auto box1 = Box(0, 0, 16, 16);
		auto box2 = Box(4, 0, 16, 16);
		auto hit = intersectAABB(box1, box2);
		assert(hit.delta.x == 12);
		assert(hit.delta.y == 0);
		box2.pos.x += hit.delta.x;
		box2.pos.y += hit.delta.y;
		assert(intersectAABB(box1, box2).valid == false);

		box2.pos = Vec2(-4, 0);
		hit = intersectAABB(box1, box2);
		assert(hit.delta.x == -12);
		assert(hit.delta.y == 0);
		box2.pos.x += hit.delta.x;
		box2.pos.y += hit.delta.y;
		assert(intersectAABB(box1, box2).valid == false);

		box2.pos = Vec2(0, 4);
		hit = intersectAABB(box1, box2);
		assert(hit.delta.x == 0);
		assert(hit.delta.y == 12);
		box2.pos.x += hit.delta.x;
		box2.pos.y += hit.delta.y;
		assert(intersectAABB(box1, box2).valid == false);

		box2.pos = Vec2(0, -4);
		hit = intersectAABB(box1, box2);
		assert(hit.delta.x == 0);
		assert(hit.delta.y == -12);
		box2.pos.x += hit.delta.x;
		box2.pos.y += hit.delta.y;
		assert(intersectAABB(box1, box2).valid == false);
	}

	printf("sweepAABB should return sweep when not colliding\n");
	{
		auto box1 = Box(0, 0, 32, 32);
		auto box2 = Box(64, -64, 16, 16);
		auto delta = Vec2(0, 128);
		auto sweep = sweepAABB(box1, box2, delta);
		//assert(sweep instanceof intersect.Sweep);
		//assert(sweep.pos instanceof intersect.Point);
		//assert(sweep.hit == null);
		assert(sweep.pos.x == box2.pos.x + delta.x);
		assert(sweep.pos.y == box2.pos.y + delta.y);
	}

	printf("sweepAABB should return sweep with sweep.hit when colliding\n");
	{
		auto box1 = Box(0, 0, 32, 32);
		auto box2 = Box(0, -64, 16, 16);
		auto delta = Vec2(0, 128);
		auto sweep = sweepAABB(box1, box2, delta);
		//assert(sweep instanceof intersect.Sweep);
		//assert(sweep.hit instanceof intersect.Hit);
		//assert(sweep.pos instanceof intersect.Point);
	}

	printf("sweepAABB should place sweep.pos at a non-colliding point\n");
	{
		auto box1 = Box(0, 0, 32, 32);
		auto box2 = Box(0, -64, 16, 16);
		auto delta = Vec2(0, 128);
		auto sweep = sweepAABB(box1, box2, delta);
		auto time = 0.3125; //- intersect.epsilon;
		assert(sweep.time == time);
		assert(sweep.pos.x == box2.pos.x + delta.x * time);
		assert(sweep.pos.y == box2.pos.y + delta.y * time);
	}

	printf("sweepAABB should place sweep.hit.pos on the edge of the box\n");
	{
		auto box1 = Box(0, 0, 32, 32);
		auto box2 = Box(0, -64, 16, 16);
		auto delta = Vec2(0, 128);
		auto sweep = sweepAABB(box1, box2, delta);
		auto time = 0.3125;
		auto direction = Vec2_normalize(delta);
		assert(sweep.hit.time == time);
		assert(sweep.hit.pos.x == box2.pos.x + delta.x * time + direction.x * box2.half.x);
		assert(sweep.hit.pos.y == box2.pos.y + delta.y * time + direction.y * box2.half.y);
		assert(sweep.hit.delta.x == delta.x * time);
		assert(sweep.hit.delta.y == delta.y * time);
	}

	printf("sweepAABB should set sweep.hit.normal to normals of box 1\n");
	{
		auto box1 = Box(0, 0, 32, 32);
		auto box2 = Box(0, -64, 16, 16);
		auto delta = Vec2(0, 128);
		auto sweep = sweepAABB(box1, box2, delta);
		assert(sweep.hit.normal.x == 0);
		assert(sweep.hit.normal.y == -1);
	}

	printf("sweepAABB should not move when the start position is colliding\n");
	{
		auto box1 = Box(0, 0, 32, 32);
		auto box2 = Box(0, -4, 16, 16);
		auto delta = Vec2(0, 128);
		auto sweep = sweepAABB(box1, box2, delta);
		assert(sweep.pos.x == 0);
		assert(sweep.pos.y == -4);
		assert(sweep.hit.time == 0);
		assert(sweep.hit.delta.x == 0);
		assert(sweep.hit.delta.y == 0);
	}
}
#endif
#endif // AABB_IMPLEMENTATION