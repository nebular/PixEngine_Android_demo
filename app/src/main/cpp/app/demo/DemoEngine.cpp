//
// Created by rodo on 2020-01-24.
//

#include "olcPixelGameEngine.h"
#include <jni.h>

#include "DemoEngine.h"

bool DemoEngine::OnUserCreate() {
	Clear(olc::BLUE);
	return true;
}

bool DemoEngine::OnUserUpdate(float fElapsedTime) {
	DrawString(100,100,"HELLO OLC PGE!", olc::GREEN, 4.0);
	olc::Sprite *s = new olc::Sprite("android.png");
	SetPixelMode(olc::Pixel::ALPHA);
	DrawSprite(160,200,s, 1);
	return true;
}
