/**
 *
 * A shader for:
 *
 * - General purpose sprite support. Supports up to 8 simultaneous spritesheets with
 *   32 simultaneous sprites per spritesheets. Supports per-sprite scaling, rotation, tinting
 *
 *   (C) 2020 Rodolfo Lopez Pintor <rlp@nebular.tv>)
 *   Licence: Creative Commons with attribution.
 *
 */

#version 330 core

///////////////////////////////////////////////////
// Settings and constants

// whether to blur sprites by default
#define BLUR false

// number of maximum simultaneous active sprites by stylesheet (affects performance)
#define MAXSPRITES 32

precision highp float;


///////////////////////////////////////////////////
// TEXTURES sent from C++

// OLC buffer
uniform sampler2D glbuffer;
// sprite sheets, up to 8 simultaneous. Can be expanded but should be enough !
uniform sampler2D spritebuffer0, spritebuffer1, spritebuffer2, spritebuffer3, spritebuffer4, spritebuffer5,spritebuffer6,spritebuffer7;

///////////////////////////////////////////////////
// UNIFORM VARIABLES INJECTED FROM C++

// global shader variables, common for all functionality

uniform highp vec2 iResolution;				// sceen resolution in px
uniform highp vec4 iCamWorld;				// x,y,angle of the camera in world coordinates, and FX

// SPRITES VARIABLES (common)

uniform highp int  iSpriteSheets;			// number of loaded sprite sheets
uniform highp vec4 iColorKey;				// sprites tint color and tolerance

// SPRITES VARIABLES (Per-Spritesheet)

// vec4(sheetwidth, sheetheight, numspr_x, numspr_y)
uniform highp vec4 iSpriteSheet0, iSpriteSheet1, iSpriteSheet2, iSpriteSheet3, iSpriteSheet4, iSpriteSheet5, iSpriteSheet6, iSpriteSheet7, iSpriteSheet8;

// Sprite positions array
uniform highp vec4 iSpritePos0[MAXSPRITES], iSpritePos1[MAXSPRITES], iSpritePos2[MAXSPRITES], iSpritePos3[MAXSPRITES], iSpritePos4[MAXSPRITES], iSpritePos5[MAXSPRITES], iSpritePos6[MAXSPRITES], iSpritePos7[MAXSPRITES];

// Sprite status (on/off) array and height
uniform highp vec4 iSpriteDef0[MAXSPRITES], iSpriteDef1[MAXSPRITES], iSpriteDef2[MAXSPRITES], iSpriteDef3[MAXSPRITES], iSpriteDef4[MAXSPRITES], iSpriteDef5[MAXSPRITES], iSpriteDef6[MAXSPRITES], iSpriteDef7[MAXSPRITES];

// Sprite FX (tint currently)
uniform highp vec4 iSpriteFx0[MAXSPRITES], iSpriteFx1[MAXSPRITES], iSpriteFx2[MAXSPRITES], iSpriteFx3[MAXSPRITES], iSpriteFx4[MAXSPRITES], iSpriteFx5[MAXSPRITES], iSpriteFx6[MAXSPRITES], iSpriteFx7[MAXSPRITES];

out vec4 FragColor;
in vec2 TexCoord;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Utilities

// cnvert YUV<>RGB
vec4 yuv(vec4 rgb) {
	float Y = 0.2989 * rgb.r + 0.5866 * rgb.g + 0.1145 * rgb.b;
	float Cr = 0.492 * (rgb.r - Y);
	float Cb = 0.877 * (rgb.b - Y);
	return vec4(Y,Cb,Cr,rgb.w);
}

// cnvert YUV<>RGB
vec4 yuv2rgb(vec4 ycol) {
	return vec4(
	ycol.x+1.5958*ycol.z,
	ycol.x-0.39173*ycol.y - 0.81290*ycol.z,
	ycol.x+2.017*ycol.y,
	ycol.w
	);
}

// we use YUV color space for chroma key, results should be better
vec4 yuvColorKey = yuv(iColorKey);

// a blur function used for ground smoothing
vec4 blur5(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
    vec4 color = vec4(0.0);
    vec4 orig = texture(image, uv);
    vec2 off1 = vec2(1.3333333333333333) * direction;
    color += orig * 0.29411764705882354;
    color += texture(image, uv + (off1 / resolution)) * 0.35294117647058826;
    color += texture(image, uv - (off1 / resolution)) * 0.35294117647058826;
    return vec4(color.xyz, orig.w);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// SPRITES SECTION
//

// convenience: whether sprite is activated. at the moment is the trivial situation,
// but we can add more complex logic here to hide the sprites on additional situations
// ie. computed not visible for any reason, etc..

bool hasImage(int sprite) {
    return sprite != -1;
}

// generic 2d rotate
mat2 rotate2d(float _angle){
    return mat2(cos(_angle), -sin(_angle),
    sin(_angle), cos(_angle));
}

// generic dimensions rotating
vec2 rotateDimensions(vec2 target, float angle) {
    target = rotate2d(angle) * target;
    target.x = abs(target.x);
    target.y = abs(target.y);
    return target;
}

// the main routine: draws an sprite from a spritesheet
// spriteSheet: vec4(width,height,numspritesx,numspritesy)
// uv: the fragment coords
// imageVector: vec4(worldx,worldy,scale,rotation)
// source: the spritesheet texture
// spriteDef: vec4(on_flag, height, unused, unused)
// tintInfo: color to tint the sprite
// groundpix: ground pix that maybe will be replaced with a pixel from this sprite
// blur: whether to blur the image
// useHeight: whether to process height information

vec4 drawImage(vec4 spriteSheet, vec2 uv, vec4 imageVector, sampler2D source, vec4 spriteDef, vec4 tintInfo, vec4 groundPix, bool blur, bool useHeight) {

	int spriteIndex = int(spriteDef.x);
	
	// for simplicity, base sprite size is calculated
	vec2 __spriteSize =  spriteSheet.xy/spriteSheet.zw;

    if (hasImage(spriteIndex)) {
		
		// x,y cell in spritesheet
		vec2 sprite = vec2(spriteIndex%int(spriteSheet.z), spriteIndex/int(spriteSheet.z));

		// offset in sprite sheet
		vec2 __spsheetoff = sprite*__spriteSize * imageVector.z;

        float t = 0.0;
        vec4 fincolor = vec4(0,0,0,0);

        vec2 pos = iResolution.xy * imageVector.xy;
        
		vec2 odimens = __spriteSize * imageVector.z * spriteDef.zw;
        vec2 dimens = odimens;
      
		float angle = imageVector.w;
		vec2 halfdimens = dimens / 2.0;
		vec2 halforig = dimens / 2.0;

        if ((uv.x > pos.x - halfdimens.x ) && (uv.x < pos.x + halfdimens.x)
        && (uv.y > pos.y - halfdimens.y) && (uv.y < pos.y +  halfdimens.y)) {

			// point belongs inside the rectangle: render the sprite

			float heightinvperc = 1 - (uv.y - pos.y - halfdimens.y)/dimens.y;
			float k = (0.01 / 5) * spriteDef.w; // TODO i multiply for sprites higher that 1 block, but dont know if this is right how it is
			float height = useHeight ? spriteDef.y + heightinvperc*dimens.y*k : -1;

			if (height >=0 && groundPix.w > height)
				return vec4(0,0,0,0);
			
            t = 1.0;

            // DIMENS dimensiones del agujero con las dimensiones de la cara
            // image rescale factor "fit xy"
            // adjustaremos desde fuera xa q ese FITXY sea justo el tamano del bitmap
            // en lugar del tamano de la cara

            // position in texture
         	vec2 textpos = vec2(uv.x-(pos.x - halfdimens.x), uv.y-(pos.y -  halfdimens.y));

			if (angle != 0) {

				// apply rotation r/center of sprite
				textpos -= halforig;
				textpos = rotate2d(angle) * textpos;

				// these points at the corner of the rotated sprite
				// belong to neighboring textures
				if (textpos.x < -halfdimens.x || textpos.x>halfdimens.x || textpos.y < -halfdimens.y || textpos.y>halfdimens.y)
					return vec4(0,0,0,0);

				textpos += halforig;
			}
			
			// add offset in sheet
			textpos += __spsheetoff;

			// divide by number of sprites
			textpos /= (spriteSheet.zw / spriteDef.zw);

			// final texture coordinates
            vec2 textcoord =  textpos / odimens;
	
			if (blur) {
				
				// we do not support blurring & tinting at the same time
				// anyway blur is pretty bold for sprites at the moment
				
				fincolor = blur5(source, textcoord, __spriteSize.xy, normalize(vec2(1.,1.)));

			} else {

				// pick sprite pixel
				fincolor = texture(source, textcoord);

				// process tinting, we have a constant with the chromakey and tolerance
				// we use YUV space for better results

				vec2 ckey=vec2(yuvColorKey.y, yuvColorKey.z);
				if (fincolor.w > 0) {
					if (tintInfo.w == 1) {
						vec4 co = yuv(fincolor);
						if (distance(ckey, vec2(co.y, co.z)) < iColorKey.w) {
							// tint the sprite to the new color computing the new shade
							fincolor = vec4((fincolor.xyz - iColorKey.xyz) + tintInfo.xyz * 0.8, fincolor.w);
						}
					} else if (tintInfo.w == 2) {
						fincolor = vec4((fincolor.xyz ) + tintInfo.xyz * 0.8, fincolor.w);
					}
				}
			}
		}

		return fincolor;

    } else return vec4(0, 0, 0, 0);

}

// mixes a sprite if there. parameters refer to drawImage, and destinationLayer is the incoming color
// that will be replaced by a sprite color or not.

vec4 mixSprite(in vec4 spritesheet, in vec2 fragCoord, in vec4 imageVector, in sampler2D channel, in vec4 destinationLayer, in vec4 spriteDef, in vec4 tintInfo, bool blur, bool useHeights) {
	vec4 faceColor = drawImage(spritesheet, fragCoord, imageVector, channel, spriteDef, tintInfo, destinationLayer, blur, useHeights);
	return mix(destinationLayer, faceColor, faceColor.a);
}

// mixes active sprites in active spritesheets

vec4 mainSprites(vec4 face, bool useHeights) {

	vec2 fragCoord = vec2(TexCoord*iResolution.xy);

	// if there are active spritesheets
	if (iSpriteSheets > 0)
		
		// mix all sprites from all spritesheets
		// MAXSPRITES control how many sprites in a stylesheet can be active at the same time.
		// as you see it linearly affects performance so keep the value near your requirements.
		// also consider using more stylesheets rather than incrementing this number
		
		for (int i=0; i<MAXSPRITES; i++) {

			// if sprite "i" in spritesheet 0 is activated ... mix it!
			if (iSpriteDef0[i][0]!=-1) 					  face = mixSprite(iSpriteSheet0, fragCoord, iSpritePos0[i], spritebuffer0, face, iSpriteDef0[i],iSpriteFx0[i], BLUR, useHeights);

			// if spritesheet 1 is active, and its sprite "i" is activated ... mix it! ... etc
			if (iSpriteSheets>1 && iSpriteDef1[i][0]!=-1) face = mixSprite(iSpriteSheet1, fragCoord, iSpritePos1[i], spritebuffer1, face, iSpriteDef1[i],iSpriteFx1[i], BLUR, useHeights);
			
			// etc ... for all 8 maximum supported spritesheets
			if (iSpriteSheets>2 && iSpriteDef2[i][0]!=-1) face = mixSprite(iSpriteSheet2, fragCoord, iSpritePos2[i], spritebuffer2, face, iSpriteDef2[i],iSpriteFx2[i], BLUR, useHeights);
			if (iSpriteSheets>3 && iSpriteDef3[i][0]!=-1) face = mixSprite(iSpriteSheet3, fragCoord, iSpritePos3[i], spritebuffer3, face, iSpriteDef3[i],iSpriteFx3[i], BLUR, useHeights);
			if (iSpriteSheets>4 && iSpriteDef4[i][0]!=-1) face = mixSprite(iSpriteSheet4, fragCoord, iSpritePos4[i], spritebuffer4, face, iSpriteDef4[i],iSpriteFx4[i], BLUR, useHeights);
			if (iSpriteSheets>5 && iSpriteDef5[i][0]!=-1) face = mixSprite(iSpriteSheet5, fragCoord, iSpritePos5[i], spritebuffer5, face, iSpriteDef5[i],iSpriteFx5[i], BLUR, useHeights);
			if (iSpriteSheets>6 && iSpriteDef6[i][0]!=-1) face = mixSprite(iSpriteSheet6, fragCoord, iSpritePos6[i], spritebuffer6, face, iSpriteDef6[i],iSpriteFx6[i], BLUR, useHeights);
			if (iSpriteSheets>7 && iSpriteDef7[i][0]!=-1) face = mixSprite(iSpriteSheet7, fragCoord, iSpritePos7[i], spritebuffer7, face, iSpriteDef7[i],iSpriteFx7[i], BLUR, useHeights);
		}

	return face;

}

///////////// SPRITES END



//////////////////////////////////////////////////////////////////////
//
// The Main Routine
//
// - sample GLBUFFER (the UI)
// - Mix sprites
//

void main() {

	// the OlcPixelEngine main layer (UI)
	vec4 face = texture(glbuffer, TexCoord);
	FragColor = mainSprites(vec4(face.xyz,0), true);

}
