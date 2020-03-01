//
//  Arena.cpp
//  PixEngine
//
//  Created by rodo on 21/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "ext/world/Ball.hpp"
#include "ext/world/Player.hpp"
#include "ext/world/Config.hpp"
#include "input/GyroController.hpp"
#include "core/Utils.hpp"

#include "Arena.hpp"

#include "arch/android/plugins/lonescreenkey.h"

#define DBG_NOHEIGHTMAPCOLLISIONS

namespace Pix {

	// heigh over which
	std::string Arena::TAG = "Arena";

	Arena::Arena(std::string name, WorldConfig_t config, Perspective_t perspective)
	: BallWorld(config, perspective),
	  LEVEL(new ArenaLevel_t(name)) {
		LogV(TAG, SF("Level Name %s", name.c_str()));
		add({name});
	}

	bool Arena::init(Fu *engine) {
		
		if (!World::init(engine)) return false;


		Pix::LoneScreenKey::currentInstance->clear();
		Pix::LoneScreenKey::currentInstance->reset();
		Pix::LoneScreenKey::currentInstance->addCursors(0, 120);

		pCamPlayer =
		pHumanPlayer =  new Player(this,
		{
			"kart_peach",
			{
				{150, 100, 150},		// pos
				{0.0, 0.0 ,0.0},		// rot
				8,						// radius
				10						// mass
			}
		});

		add(pHumanPlayer);
		
		camera()->setTargetMode(true);

	 #define TREES
		#ifdef TREES
			for (int i=0; i<1700; i+=200) {
				for (int j=0; j<1300; j+=200) {
					add(new GameObject(this, {
						"tree",
						{
							{ i, 0, j },			// pos
							{0.0, 0, 0.0},			// rot
							(3.0f + random() % 12 ),	// scale
							100000
						}
					}));
				}
			}
		#endif
//		GenericAxisController::enable(-1,1, -1,1, true, false, false, true);
//		engine->addInputDevice(GenericAxisController::instance());

		return true;
	}

	void Arena::driveSpline(float fElapsedTime) {
		
		sSpline spl = LEVEL->sPaths[1];

		if (fPos>spl.fTotalSplineLength) fPos = 0;
		
		fPos+=fElapsedTime;

		sPoint2D splPoint = spl.GetSplinePoint(fPos);

		glm::vec3 &pos = pHumanPlayer->position();
		pos.x = splPoint.x; //1000;
		pos.z = splPoint.y; //1000;
		
//		pHumanPlayer->fAngle = -spl.GetSplineAngle(fPos) - M_PI / 2;
			
	}

	void Arena::tick(Fu *engine,float fElapsedTime) {

		auto toDeg = [] (float rad) { return (int)(rad*180/M_PI); };

		Pix::LoneScreenKey::currentInstance->DrawSelf(engine->canvas(), Pix::Colors::WHITE, true);

	//	driveSpline(fElapsedTime);
		processCollisions(LEVEL->vecLines, fElapsedTime);
		processInput(engine, fElapsedTime);

		World::tick(engine, fElapsedTime);

//		return;
		engine->canvas()->drawString(0,70,Pix::SF("PLY spdxz %f %f, %f",
												  pHumanPlayer->velocity().x,
												  pHumanPlayer->velocity().z,
												  pHumanPlayer->speed()),
												Pix::Colors::WHITE, 2);

		engine->canvas()->drawString(0,50,Pix::SF("PLY accxz %f %f",
												  pHumanPlayer->acceleration().x,
												  pHumanPlayer->acceleration().z
												  ),
												Pix::Colors::WHITE, 2);
		
		engine->canvas()->drawString(0,30,Pix::SF("PLY ang %d x %f y %f z %f sag %d",
												  toDeg(pHumanPlayer->angle()),
										  pHumanPlayer->position().x,
										  pHumanPlayer->position().y,
										  pHumanPlayer->position().z,
												  toDeg(pHumanPlayer->fSteerAngle)
												  ),
							 Pix::Colors::WHITE, 2);
		
		engine->canvas()->drawString(0,10,Pix::SF("CAM x %f y %f z %f",
										  camera()->getPosition().x,
										  camera()->getPosition().y,
										  camera()->getPosition().z),
							 Pix::Colors::YELLOW, 2);

		
		engine->canvas()->drawRect(1024-375,576-225, 375,225,Pix::Colors::YELLOW);
		engine->canvas()->fillCircle(1024-365+pHumanPlayer->position().x/5, 576-225+pHumanPlayer->position().z / 5,10, Pix::Colors::RED);

	}

	void Arena::processInput(Fu *engine, float fElapsedTime) {

		
		if (Keyboard::isPressed(Keys::SPACE)) {
			pCamPlayer = pCamPlayer == nullptr ? pHumanPlayer : nullptr;
			camera()->setTargetMode(pCamPlayer != nullptr);
		}
		
		// Select mode with ALT/CMD
		CameraKeyControlMode_t mode =
				Keyboard::isHeld(Keys::ALT) ? Pix::ADJUST_ANGLES :
				Keyboard::isHeld(Keys::COMMAND) ? Pix::ADJUST_POSITION : Pix::MOVE;

		float xdelta = 0, ydelta = 0, K = 0.01;

		bool useInput = pCamPlayer != nullptr;

		if (pCamPlayer == nullptr) {// || mode != MOVE)
			camera()->inputKey(
						  mode,
						  Keyboard::isHeld(Keys::UP),
						  Keyboard::isHeld(Keys::DOWN),
						  Keyboard::isHeld(Keys::LEFT),
						  Keyboard::isHeld(Keys::RIGHT),
						  fElapsedTime
						  );
			if (Keyboard::isHeld(Keys::W)) ydelta=K;
			if (Keyboard::isHeld(Keys::S)) ydelta=-K;
			if (Keyboard::isHeld(Keys::A)) xdelta=-K;
			if (Keyboard::isHeld(Keys::D)) xdelta=K;
			if (ydelta !=0 || xdelta != 0) {
				GyroController::instance()->inputIncremental(xdelta, ydelta);
				useInput = true;
			}
		}
		
		if (pCamPlayer != nullptr) {
			
			camera()->follow(pHumanPlayer);
			
			if (mode != MOVE) {
				camera()->inputKey(
								  mode == ADJUST_ANGLES ? ADJUST_PLAYER_ANGLES : ADJUST_PLAYER_POSITION,
								  Keyboard::isHeld(Keys::UP),
								  Keyboard::isHeld(Keys::DOWN),
								  Keyboard::isHeld(Keys::LEFT),
								  Keyboard::isHeld(Keys::RIGHT),
								  fElapsedTime
								  );


			} else {
				if (Keyboard::isHeld(Keys::UP)) 	ydelta = K;
				if (Keyboard::isHeld(Keys::DOWN)) 	ydelta = -K;
				if (Keyboard::isHeld(Keys::LEFT)) 	xdelta = -K;
				if (Keyboard::isHeld(Keys::RIGHT)) 	xdelta = K;
			
				if (ydelta !=0 || xdelta != 0) {
					GyroController::instance()->inputIncremental(xdelta, ydelta);
					useInput = true;
				}
			}
		}

		GyroController::instance()->drawSelf(engine->canvas(), Pix::Colors::RED);
		
//		if (useInput) {
//			ASensorEvent tCurrentEvent = Pix::GyroController::currentInstance()->raw();
//			float roll = tCurrentEvent.vector.roll;
//			float azimuth = tCurrentEvent.vector.azimuth;
//			float pich = tCurrentEvent.vector.pitch;

			pHumanPlayer->steer(GyroController::instance()->x()*2, fElapsedTime);
	//		pHumanPlayer->steer(-GenericAxisController::instance()->xInterp(), fElapsedTime);
			pHumanPlayer->accelerate(GyroController::instance()->y(), fElapsedTime);
//		}
	}
};
