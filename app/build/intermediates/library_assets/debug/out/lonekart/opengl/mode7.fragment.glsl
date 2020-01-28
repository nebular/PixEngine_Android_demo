/**
 *
 * A shader for:
 *
 * - Mode7 terrain rendering with heights, sky and parallax
 * - General purpose sprite support. Supports up to 8 simultaneous spritesheets with
 *   32 simultaneous sprites per spritesheets. Supports per-sprite scaling, rotation, tinting
 *
 *   (C) 2020 Rodolfo Lopez Pintor <rlp@nebular.tv>)
 *   Licence: Creative Commons with attribution.
 *
 */

///////////////////////////////////////////////////
// Settings and constants

// whether to blur sprites by default
#define BLUR false

// darken lower pixels. arguably pretty.
#define DARKEN_GROUNDHEIGHT 0.4
#define LIGHTEN_GROUNDHEIGHT 1.2

// number of maximum simultaneous active sprites by stylesheet (affects performance)
#define MAXSPRITES 32

// reduction scale of the far parallax layer (a strectched down copy of the first)
#define PARALLAX_FAR_SCALE 1.3

// color multiplier for the far parallax layer
#define PARALLAX_COLOR_FADE vec4(0.7,0.7,0.7,1)

// The Mode 7 paramers. Ideally should be sent from C++ TODO
float M7Params_space_z = 55.0;
float M7Params_horizon = 20.0;
vec2  M7Params_scale = vec2(200,200);
float M7Params_height_perspective =  -3.14159265 / 3;
//float M7Params_height_perspective = -3.14159265 / 2; // -3.14159265 / 3;

precision highp float;


///////////////////////////////////////////////////
// TEXTURES sent from C++

// OLC buffer
uniform sampler2D glbuffer;
// mode 7 bitmaps: ground, sky, trails (make ground dirty), horiz (parallax bitmap)
uniform sampler2D m7ground, m7clouds, m7trails, m7horiz;
// sprite sheets, up to 8 simultaneous. Can be expanded but should be enough !
uniform sampler2D spritebuffer0, spritebuffer1, spritebuffer2, spritebuffer3, spritebuffer4, spritebuffer5,spritebuffer6,spritebuffer7;

///////////////////////////////////////////////////
// UNIFORM VARIABLES INJECTED FROM C++

// global shader variables, common for all functionality

uniform highp vec2 iResolution;				// sceen resolution in px
uniform highp vec2 iMapSize;				// map resolutio in px
uniform highp vec4 iCamWorld;				// x,y,angle of the camera in world coordinates, and FX
bool invertGround = iCamWorld.w == 1;		// FX are sent in iCamWorld. Currently w==1 => invert

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
// MODE 7: Render ground bitmap with heights, sky and trails bimaps. Also 2-level parallax.
//

/* Defined at the beginning
float M7Params_space_z = 50.0;
float M7Params_horizon = 20.0;
vec2  M7Params_scale = vec2(200,200);
float M7Params_height_perspective = -3.14159265 / 3;
*/

float MAXHEIGHT = abs(M7Params_space_z * sin(M7Params_height_perspective));
//float MAXHEIGHT = abs(M7Params_space_z); //  * sin(M7Params_height_perspective));

// Get Ground Sample coordinates (-1,1) from screen coordinates
// honoring camera

vec2 getSample(vec2 fragCoord) {

    // first calculate the distance of the line we are drawing
    float distance = M7Params_space_z * M7Params_scale.y / (fragCoord.y*2 + M7Params_horizon);

    // then calculate the horizontal scale, or the distance between
    // space points on this horizontal line
    float horizontal_scale = distance / M7Params_scale.x;

    // calculate the dx and dy of points in space when we step
    // through all points on this line
    vec2 linedelta = vec2(
    -sin(iCamWorld.z)* horizontal_scale,
    cos(iCamWorld.z)* horizontal_scale
    );

    // calculate the starting position
    vec2 space = vec2(
    iCamWorld.x*iMapSize.x +  (distance* cos(iCamWorld.z)) - iResolution.x/2 * linedelta.x,
    iCamWorld.y*iMapSize.y +  (distance* sin(iCamWorld.z)) - iResolution.x/2 * linedelta.y
    );

    space += linedelta * fragCoord.x;
    return mod(space / iMapSize, 1.0f);

}

// mixes trails layer
vec4 mixtrail(vec4 incolor, vec2 samp) {
    vec4 trail = texture(m7trails, samp);
return mix(vec4(incolor.xyz,1), trail, trail.w);
}

// Renders a pseudo-3d bitmap
// heights: use heights

vec4 map(sampler2D bitmap, vec2 fragCoord, bool inverse, bool heights) {

    // we can paint in reverse (sky)
    if (inverse) fragCoord.y = iResolution.y/4 - fragCoord.y;

    // if no using heights, we return the pixel. All projection logic
    // is in the function getSample, that translates between world coordinates
    // and screen coordinates

    if  (!heights) {
        return texture(bitmap, getSample(fragCoord));
    }

    // If using heights, we will basically do a loop "height" times from top to bottom,
    // (like cutting the surface with a parallel plane) and in each iteration we draw the pixels
    // visible at that height.

    // So the higher our heights the more computationally intense this shader will be (we will need
    // a lot of "plane cuts". I am sure this can be done much more efficiently but that is past
    // my current abilities at the moment.

    // traverse upwards looking for a lower point with this height
    // MAXHEIGHT is the maximum height we allow
    // it cannot be very big or else this routine will kill the framerate

    for (int h=0; h<MAXHEIGHT; h++) {

        float hs = MAXHEIGHT - h; // hs: +32 +31 +30 +29 ... NORM => 1...0 down...up
        vec2 newCoord = fragCoord+vec2(0,hs);

        if (newCoord.y > 0) {

            // coordenadas negaivas para lo que queda por encima del horizonte
            // un punto mas alto puede representar a otro mas bajo con altura

            vec2 newSample = getSample(fragCoord+vec2(0,hs));

            vec4 groundpix = texture(m7ground, newSample); // ground color + height

            //			vec4 groundpix = blur5(m7ground, newSample, iMapSize.xy, normalize(vec2(1.,1.)));

            // ground pix contains height info

            float newHeight = groundpix.w;
            float distance = (iResolution.y - fragCoord.y*2)/iResolution.y; // M7Params_space_z * M7Params_scale.y / (fragCoord.y*2 + M7Params_horizon);
            float newhoffs =  sin(M7Params_height_perspective) * newHeight * M7Params_space_z  * (1 - (distance*0.5));
            //			float newhoffs =   -newHeight * M7Params_space_z  * (1 - (distance*0.25));
            //			float newhoffs =   -newHeight * M7Params_space_z * distance / 4; //  * distance*0.5;

            // lighten / darken on height
            float kg = (LIGHTEN_GROUNDHEIGHT-DARKEN_GROUNDHEIGHT) + (min(hs*2,MAXHEIGHT)/MAXHEIGHT) * DARKEN_GROUNDHEIGHT;

            if (newHeight != 0 && newhoffs<=-hs) {
                // lower point has height that projects to this height
                return heights ? vec4(mixtrail(groundpix*kg, newSample).xyz, groundpix.w) : mixtrail(groundpix*kg, newSample);
            }
        }
    }

    vec2 fSample = getSample(fragCoord);

    // sample the height at the bottom center
    vec4 groundpix =  texture(m7ground,fSample);

    //  if height at that ground pos is > 0, we do not paint a pixel, as this pixel will be upwards
    //	the below should be correct so we only paint pixels at their correct height, however pixels with
    //  height "0" render white and high. I think it's probably because of the maths but not sure is there
    //  is a flaw elsewhere
    //
    //	RENDERS ARTIFACTS BUT SHOUDLNT: if (groundpix.w > 0 || fragCoord.y<0) return vec4(-1,-1,-1,-1);

    // fragCoord.y is < 0 for pixels in the horizon
    if (fragCoord.y<0) return vec4(-1,-1,-1,-1);

    float kg0 = LIGHTEN_GROUNDHEIGHT-DARKEN_GROUNDHEIGHT;

    return heights
    ? vec4(mixtrail(vec4(groundpix.xyz * kg0,1), fSample).xyz, groundpix.w)
    : mixtrail(groundpix, fSample);

}

///////////// MODE 7 END

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

////// SUN BEGIN

vec4 mixsun(vec3 color) {

    vec2 uv = vec2(TexCoord.x, TexCoord.y) - 0.5;
    vec2 mm = vec2(sin(iCamWorld.z), -0.4); // y  - 0.5;

    if (true || mm.x>0&& mm.x<1) {

        // get angle and length of the sun (uv - mouse)
        float a = atan(uv.y-mm.y, uv.x-mm.x);
        float l = max(1.0-length(uv-mm)-0.84, 0.0);

        l = 0.3;

        float bright = 0.2;//+0.1/abs(sin(iTime/3.))/3.;//add brightness based on how the sun moves so that it is brightest
        //when it is lined up with the center


        // add the sun with the frill things
        color += max(0.1/pow(length(uv-mm)*5., 5.), 0.0)*abs(sin(a*5.+cos(a*9.)))/20.;
        color += max(0.1/pow(length(uv-mm)*10., 1./20.), .0)+abs(sin(a*3.+cos(a*9.)))/8.*(abs(sin(a*9.)))/1.;

        // add another sun in the middle (to make it brighter)  with the20color I want, and bright as the numerator.
        color += (max(bright/pow(length(uv-mm)*4., 1./2.), 0.0)*4.)*vec3(0.2, 0.21, 0.3)*4.;
        // * (0.5+.5*sin(vec3(0.4, 0.2, 0.1) + vec3(a*2., 00., a*3.)+1.3));
    }
    //multiply by the exponetial e^x ? of 1.0-length which kind of masks the brightness more so that
    //there is a sharper roll of of the light decay from the sun.
    color*= exp(1.0-length(uv-mm))/3.;
    return vec4(color,1);
}

////// SUN END

//////////////////////////////////////////////////////////////////////
//
// The Main Routine
//
// - sample GLBUFFER (the UI)
// - Mix Sky
//		- Mode7 sky itself
//		- parallax layers
// - Mix Mode7 Ground
// - Mix sprites
//

void main() {

    // the OlcPixelEngine main layer (UI)
    vec4 face = texture(glbuffer, TexCoord);

    if (face.w == 1) {

        // ui texture solid? skip all
        FragColor = face;

    } else {

        vec4 mapColor = vec4(1,1,1,1);

        // if we are above horizon (the sky)
        if (TexCoord.y<0.5) {

            // sky texture
            vec2 fragCoord = TexCoord*vec2(iResolution.x, iResolution.y/2);

            // get mode7 - projected pixel from cloud texture
            mapColor = map(m7clouds, fragCoord, true, false);

            float sc=PARALLAX_FAR_SCALE;					// far parallax layer scale
            vec4 paxcolorfade = PARALLAX_COLOR_FADE;		// darken far parallax layer
            vec4 paxcolor = vec4(0,0,0,0);

            // if we are in the parallax region (lower half of the sky)
            if (TexCoord.y > 0.25) {

                // origcoord as our origin there and will range [0.25...0.5]
                vec2 origCoord = TexCoord - vec2(0,0.25);

                // first parallax layer
                // Y coord will then range [0...1] as TexCoord is quarter the screen
                // X coord will be periodic on the cam angle
                // we now sample parallax texture using this coords
                vec2 coord = (origCoord*vec2(1,4)) +vec2(iCamWorld.z/3.14159265,0);
                vec4 hor = texture(m7horiz, mod(coord,1));

                // if the near parallax layer has pixel (not transparent)

                if (hor.w!=0 && (hor.x!=0 || hor.y!=0 || hor.z!=0)) {
                    // this is our boy
                    if (invertGround) hor=vec4(vec3(1,1,1)-hor.xyz, hor.w);
                    mapColor = hor;

                } else {

                    // the near parallax layer is transparent here
                    // so that space may be filled with the far parallax layer,
                    // so if we are in the far parallax region (determined by hardcoded
                    // scale parameter)

                    if (origCoord.y > 0.25 - 0.25 / sc) {

                        // We are in the far parallax layer
                        vec2 coord = vec2(origCoord.x + iCamWorld.z/(2*3.14159265), 4*(origCoord.y - 0.25 + 0.25/sc)) ;
                        vec4 hor = texture(m7horiz, mod(vec2(coord.x, coord.y*sc),1));

                        // if the second parallax layer has pixel (not transparent)
                        if (hor.w!=0 && (hor.x!=0 || hor.y!=0 || hor.z!=0)) {
                            if (invertGround) hor=vec4(vec3(1,1,1)-hor.xyz, hor.w);
                            // apply a fading consstant to thr far parallax pixel
                            mapColor = hor * paxcolorfade;
                        }
                    }
                }
            }
        }

        // If we are on the ground region, draw ground using heights
        if (TexCoord.y >= 0.5-(MAXHEIGHT/iResolution.y/2)) {
            vec2 fragCoord = (TexCoord-vec2(0,0.5))*vec2(iResolution.x, iResolution.y/2);
            vec4 grd= map(m7ground, fragCoord, false, true);
            if (grd!=vec4(-1,-1,-1,-1)) {
                if (invertGround) grd = vec4(vec3(1,1,1)-grd.xyz, grd.w);
                mapColor = grd;
            }
            // color comes also with height information in w
        }

        // Mix the sprites. mapColor w gives info to the sprite mixer
        // about the ground height there, so sprites can be "behind"

        mapColor = mainSprites(mapColor, TexCoord.y>0.5);
        FragColor = mapColor; // ignore alpha on UI
        //	FragColor =mix ( mixsun(mapColor.xyz), face, face.w);
        //	FragColor = mix ( mapColor, face, face.w);
    }
}
