//
// Created by rodo on 2020-01-24.
//

#pragma once

#include "PixFu.hpp"
#include "input/GyroController.hpp"

class DemoGyro : public rgl::PixFu {

public:

	const std::vector<std::pair<float, float>> stVecModelCar = {{1,  1},
																{1,  3},
																{3,  0},
																{0,  -3},
																{-3, 0},
																{-1, 3},
																{-1, 1}};

	bool onUserCreate(bool restarted) override;

	bool onUserUpdate(float fElapsedTime) override;

};

/**
 * HELLO GYROSCOPE
 */


inline

bool DemoGyro::onUserCreate(bool restarted) {

	rgl::GyroController::enable(100, 100);
	addInputDevice(rgl::GyroController::instance());

	return true;
}

inline bool DemoGyro::onUserUpdate(float fElapsedTime) {

	auto todeg = [](float rad) { return rad * 180 / 3.14159265; };

	canvas()->clear(rgl::Colors::BLUE);
	rgl::LoneScreenKey::currentInstance->DrawSelf(canvas(), rgl::Colors::WHITE);

	ASensorEvent tCurrentEvent = rgl::GyroController::instance()->raw();
	float roll = tCurrentEvent.vector.roll;
	float azimuth = tCurrentEvent.vector.azimuth;
	float pich = tCurrentEvent.vector.pitch;

	canvas()->drawString(250, 270, "ROLL", rgl::Colors::RED, 2.0);
	canvas()->drawString(50, 270, "AZIMUTH", rgl::Colors::RED, 2.0);
	canvas()->drawString(530, 270, "PITCH", rgl::Colors::RED, 2.0);
	canvas()->drawString(250, 300, std::to_string(todeg(roll)), rgl::Colors::YELLOW, 2.0);
	canvas()->drawString(50, 300, std::to_string(todeg(azimuth)), rgl::Colors::YELLOW, 2.0);
	canvas()->drawString(450, 300, std::to_string(todeg(pich)), rgl::Colors::YELLOW, 2.0);

	canvas()->drawWireFrameModel(stVecModelCar, 300, 125, -roll, 20.0, {rgl::Colors::YELLOW});

	rgl::GyroController::instance()->drawSelf(canvas(), rgl::Colors::CYAN);

	return true;
}
