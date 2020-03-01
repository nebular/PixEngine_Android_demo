// #include "olcPixelGameEngine.h"

#include "demo/Demo.h"
#include "lonekart/inc/Main.hpp"
#include "videos/OneLoneCoder_PGE_Balls2.h"
#include "arch/android/platform_android.hpp"

// trick to execute this as Android loads the library, before anything
// else ("static library init")

struct PixFuDemo {

	PixFuDemo() {

//		Pix::PixFu *engine = new Demo1();

//		Pix::PixFu *engine = new DemoEngine();
//		Pix::PixFu *engine = new DemoGyro2();
//		Pix::PixFu *engine = new CirclePhysics();
//		Pix::PixFu *engine = new DemoGyro();
		Pix::Fu *engine = new DemoWorld();
//		Pix::PixFu *engine = new DemoControls();

		Pix::FuPlatform::init(new Pix::PixFuPlatformAndroid(engine));

	}

};

PixFuDemo rules; // !!!

