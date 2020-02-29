//
// Created by rodo on 2020-01-24.
//

#pragma once

#include "PixFu.hpp"
#include "arch/android/plugins/lonescreenkey.h"
#include "input/Keyboard.hpp"

class DemoControls : public rgl::PixFu {

	int nX, nY, lastClickX, lastClickY;

public:
	bool onUserCreate(bool restarted) override;

	bool onUserUpdate(float fElapsedTime) override;
};


/**
 * HELLO CONTROLS
 */


inline bool DemoControls::onUserCreate(bool restarted) {

	rgl::LoneScreenKey::currentInstance->clear();
	rgl::LoneScreenKey::currentInstance->reset();
	rgl::LoneScreenKey::currentInstance->addCursors(0, 120);
	rgl::LoneScreenKey::currentInstance->addMouse(screenWidth() - 100, 0);

	nX = 160;
	nY = 200;

	return true;
}

inline bool DemoControls::onUserUpdate(float fElapsedTime) {

	canvas()->clear(rgl::Colors::BLUE);

	rgl::LoneScreenKey::currentInstance->DrawSelf(canvas(), rgl::Colors::WHITE, true);

	canvas()->fillCircle(nX, nY, 14, rgl::Colors::CYAN);

	if (rgl::Mouse::isPressed(0)) {
		lastClickX = rgl::Mouse::x();
		lastClickY = rgl::Mouse::y();
	}


	canvas()->drawString(130, 0,
						 "Mouse X " + std::to_string(rgl::Mouse::x()) + " Y " +
						 std::to_string(rgl::Mouse::y()), rgl::Colors::WHITE, 2);
	canvas()->drawString(130, 20,
						 "B0 Pres " + std::to_string(rgl::Mouse::isPressed(0))
						 + " Held " + std::to_string(rgl::Mouse::isHeld(0))
						 + " Rels " + std::to_string(rgl::Mouse::isReleased(0)), rgl::Colors::WHITE,
						 2);
	canvas()->drawString(130, 40,
						 "B1 Pres " + std::to_string(rgl::Mouse::isPressed(1))
						 + " Held " + std::to_string(rgl::Mouse::isHeld(1))
						 + " Rels " + std::to_string(rgl::Mouse::isReleased(1)), rgl::Colors::WHITE,
						 2);

	canvas()->drawString(130, 60,
						 "B0clk X " + std::to_string(lastClickX)
						 + " Y " + std::to_string(lastClickY), rgl::Colors::WHITE, 2);


	if (rgl::Keyboard::isHeld(rgl::Keys::LEFT)) {
		nX--;
		if (nX < 0) nX = 0;
	}
	if (rgl::Keyboard::isHeld(rgl::Keys::RIGHT)) {
		nX++;
		if (nX > screenWidth() - 200) nX = screenWidth() - 200;
	}
	if (rgl::Keyboard::isHeld(rgl::Keys::UP)) {
		nY--;
		if (nY < 0) nY = 0;
	}
	if (rgl::Keyboard::isHeld(rgl::Keys::DOWN)) {
		nY++;
		if (nY > screenHeight() - 100) nY = screenHeight() - 100;
	}


	return true;
}

