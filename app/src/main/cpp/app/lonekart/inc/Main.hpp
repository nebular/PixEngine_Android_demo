//
//  Main.hpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#ifndef Main_hpp
#define Main_hpp

#include "items/Canvas2D.hpp"
#include "ext/world/World.hpp"
#include "Arena.hpp"

using namespace rgl;


class DemoWorld : public rgl::PixFu {
	
	float pop = 0;
	Arena *pArena;

public:
	
	DemoWorld();
	~DemoWorld();

	bool onUserCreate(bool restarted) override ;
	
	bool onUserUpdate(float fElapsedTime) override;
	
};

#endif /* Main_hpp */
