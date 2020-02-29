//
// Created by rodo on 2020-01-24.
//

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#pragma once

#include <cmath>
#include "core/PixFu.hpp"
#include "items/Canvas2D.hpp"
#include "input/Mouse.hpp"

class DemoEngine : public rgl::PixFu {

	float pop = 0;

public:
	bool onUserUpdate(float fElapsedTime) override;

};


inline bool DemoEngine::onUserUpdate(float fElapsedTime) {

	pop += fElapsedTime;

	canvas()->clear(rgl::Pixel(255 * sinf(pop), 255 * cosf(pop), 255 * sinf(pop), 255));

	canvas()->fillCircle(screenWidth() / 2, screenHeight() / 2, screenWidth() / 2 * sinf(pop),
						 rgl::Pixel(0, 0, 255, 255));

	canvas()->drawString(10, 250 + 50 * sinf(pop), "PixFu 1.0", rgl::Pixel(0, 255, 0, 255), 7);

	canvas()->fillCircle(rgl::Mouse::x(), rgl::Mouse::y(), 30,
						 rgl::Mouse::isHeld(0)
						      ? rgl::Pixel(255, 0, 0, 255)
						      : rgl::Pixel(255, 255,255, 255));

	float fpsec = 1 / fElapsedTime;

	canvas()->drawString(10, 10, std::to_string((int) fpsec) + " fps", rgl::Colors::MAGENTA, 2);

	return true;
}

#pragma clang diagnostic pop