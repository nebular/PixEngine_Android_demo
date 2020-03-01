//
// Created by rodo on 2020-01-24.
//

#pragma once

#include "Fu.hpp"
#include "input/GyroController.hpp"


class DemoGyro2 : public Pix::Fu {

public:
	DemoGyro2();

	float fAngle = 0, fPosX = 100, fPosY = 100;

	const std::vector<std::pair<float, float>> stVecModelCar = {{1,  1},
																{1,  3},
																{3,  0},
																{0,  -3},
																{-3, 0},
																{-1, 3},
																{-1, 1}};

	bool onUserUpdate(float fElapsedTime) override;

};


/**
 * HELLO GYROSCOPE
 */


DemoGyro2::DemoGyro2() {
	Pix::GyroController::enable(100, 100);
	addInputDevice(Pix::GyroController::instance());
}


inline bool DemoGyro2::onUserUpdate(float fElapsedTime) {

	canvas()->clear(Pix::Colors::BLUE);

	float xn = Pix::GyroController::instance()->x();
	float yn = Pix::GyroController::instance()->y();

	fAngle += xn * fElapsedTime * 10;

	canvas()->drawString(50, 270, "xn", Pix::Colors::RED, 2.0);
	canvas()->drawString(250, 270, "yn", Pix::Colors::RED, 2.0);
	canvas()->drawString(50, 300, std::to_string(xn), Pix::Colors::YELLOW, 2.0);
	canvas()->drawString(250, 300, std::to_string(yn), Pix::Colors::YELLOW, 2.0);


	fPosX = screenWidth() / 2 +
			(screenWidth() / 2) * Pix::GyroController::instance()->xInterp() * 1.4F;

	fPosY = screenHeight() / 2 +
			(screenHeight() / 2) * Pix::GyroController::instance()->yInterp() * 1.4F;

	canvas()->drawWireFrameModel(stVecModelCar, fPosX, fPosY, fAngle, 20.0, {Pix::Colors::YELLOW});

	Pix::GyroController::instance()->drawSelf(canvas(), Pix::Colors::CYAN);

	return true;

}
