//
// Created by rodo on 2020-01-24.
//

#pragma once

#include "Fu.hpp"
#include "arch/android/plugins/lonescreenkey.h"
#include "input/Keyboard.hpp"

class DemoControls : public Pix::Fu {

	int nX, nY, lastClickX, lastClickY;

public:
	bool onUserCreate(bool restarted) override;

	bool onUserUpdate(float fElapsedTime) override;
};


/**
 * HELLO CONTROLS
 */


inline bool DemoControls::onUserCreate(bool restarted) {

	Pix::LoneScreenKey::currentInstance->clear();
	Pix::LoneScreenKey::currentInstance->reset();
	Pix::LoneScreenKey::currentInstance->addCursors(0, 120);
	Pix::LoneScreenKey::currentInstance->addMouse(screenWidth() - 100, 0);

	nX = 160;
	nY = 200;

	return true;
}

inline bool DemoControls::onUserUpdate(float fElapsedTime) {

	canvas()->clear(Pix::Colors::BLUE);

	Pix::LoneScreenKey::currentInstance->DrawSelf(canvas(), Pix::Colors::WHITE, true);

	canvas()->fillCircle(nX, nY, 14, Pix::Colors::CYAN);

	if (Pix::Mouse::isPressed(0)) {
		lastClickX = Pix::Mouse::x();
		lastClickY = Pix::Mouse::y();
	}


	canvas()->drawString(130, 0,
						 "Mouse X " + std::to_string(Pix::Mouse::x()) + " Y " +
						 std::to_string(Pix::Mouse::y()), Pix::Colors::WHITE, 2);
	canvas()->drawString(130, 20,
						 "B0 Pres " + std::to_string(Pix::Mouse::isPressed(0))
						 + " Held " + std::to_string(Pix::Mouse::isHeld(0))
						 + " Rels " + std::to_string(Pix::Mouse::isReleased(0)), Pix::Colors::WHITE,
						 2);
	canvas()->drawString(130, 40,
						 "B1 Pres " + std::to_string(Pix::Mouse::isPressed(1))
						 + " Held " + std::to_string(Pix::Mouse::isHeld(1))
						 + " Rels " + std::to_string(Pix::Mouse::isReleased(1)), Pix::Colors::WHITE,
						 2);

	canvas()->drawString(130, 60,
						 "B0clk X " + std::to_string(lastClickX)
						 + " Y " + std::to_string(lastClickY), Pix::Colors::WHITE, 2);


	if (Pix::Keyboard::isHeld(Pix::Keys::LEFT)) {
		nX--;
		if (nX < 0) nX = 0;
	}
	if (Pix::Keyboard::isHeld(Pix::Keys::RIGHT)) {
		nX++;
		if (nX > screenWidth() - 200) nX = screenWidth() - 200;
	}
	if (Pix::Keyboard::isHeld(Pix::Keys::UP)) {
		nY--;
		if (nY < 0) nY = 0;
	}
	if (Pix::Keyboard::isHeld(Pix::Keys::DOWN)) {
		nY++;
		if (nY > screenHeight() - 100) nY = screenHeight() - 100;
	}


	return true;
}

