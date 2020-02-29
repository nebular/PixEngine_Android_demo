// #include "olcPixelGameEngine.h"

#include "demo/Demo.h"
#include "lonekart/inc/Main.hpp"
#include "videos/OneLoneCoder_PGE_Balls2.h"
#include "arch/android/platform_android.hpp"

// trick to execute this as Android loads the library, before anything
// else ("static library init")

struct PixFuDemo {

	PixFuDemo() {

//		rgl::PixFu *engine = new Demo1();

//		rgl::PixFu *engine = new DemoEngine();
//		rgl::PixFu *engine = new DemoGyro2();
//		rgl::PixFu *engine = new CirclePhysics();
		rgl::PixFu *engine = new DemoGyro();
//		rgl::PixFu *engine = new DemoWorld();
//		rgl::PixFu *engine = new DemoControls();

		rgl::PixFuPlatform::init(new rgl::PixFuPlatformAndroid(engine));

	}

};

PixFuDemo rules; // !!!

