#include "bitmap.h"
#include "quantize.h"
//#include <string>
#include <vector>
#define X_DISPLAY_MISSING
#ifdef HAVE_IMLIB2
#include <Imlib2.h>
#endif
#ifdef HAVE_MAGICK
#include <Magick++.h>
using namespace Magick;
#endif




#ifdef HAVE_MAGICK
cBitmap* LoadMagick(const char *Filename, int height, int width, int colors, bool Quiet) {
	std::vector<Image> images;
	cBitmap *bmp = NULL;
	try {
		int w, h;
		std::vector<Image>::iterator it;
		readImages(&images, Filename);
		dsyslog("DEBUG: weatherng: load image %s\n", Filename);

		if (images.size() == 0) {
			esyslog("ERROR: weatherng: Couldn't load %s\n", Filename);
			return false;
		}
//		mDelay = images[0].animationDelay() * 10;
		for (it = images.begin(); it != images.end(); ++it) {
			if (colors != 0){
				(*it).opacity(OpaqueOpacity);
				(*it).backgroundColor( Color ( 0,0,0,0) );
				(*it).quantizeColorSpace( RGBColorspace );
				(*it).quantizeColors( colors );
				(*it).quantize();
			} 

			 if (height != 0 || width != 0)
				(*it).sample(Geometry(width,height));
			w = (*it).columns();
			h = (*it).rows();
/*			if ((*it).depth() > 8) {
				printf("ERROR: weatherng: More than 8bpp images are not supported\n");
				return NULL;
			} */
			bmp = new cBitmap(w, h, (*it).depth());

			const PixelPacket *pix = (*it).getConstPixels(0, 0, w, h);
			for (int iy = 0; iy < h; ++iy) {
				for (int ix = 0; ix < w; ++ix) {
					tColor col = (~(pix->opacity * 255 / MaxRGB) << 24) | ((pix->red * 255 / MaxRGB) << 16) | ((pix->green * 255 / MaxRGB) << 8) | (pix->blue * 255 / MaxRGB);
					bmp->DrawPixel(ix, iy, col);

					++pix;
				}

			}

//			mBitmaps.push_back(bmp);
		}
	} catch (Exception &e) {
		  if (!Quiet)
			esyslog("ERROR: weatherng: Couldn't load %s: %s\n", Filename, e.what());
		  delete bmp;
		  return false;
	} catch (...) {
                if (!Quiet)
                    esyslog("ERROR: weatherng: Couldn't load %s: Unknown exception caught!", Filename);
	         delete bmp;
                return false;
       }
       return bmp;
}

#else
#ifdef HAVE_IMLIB2
cBitmap* LoadImlib(const char *Filename, int height, int width, int colors, bool Quiet) {
        Imlib_Image image;
        unsigned char * outputImage = NULL;
        unsigned int * outputPalette = NULL;
        cQuantizeWu* quantizer = new cQuantizeWu();
        cBitmap *bmp = NULL;

        image = imlib_load_image(Filename);
	dsyslog("DEBUG: weatherng: load image %s\n", Filename);

        if (!image) {
		esyslog("ERROR: weatherng: Couldn't load %s\n", Filename);
                return false;
	}

        Imlib_Context ctx = imlib_context_new();
        imlib_context_push(ctx);
        if (height != 0 || width != 0){
                imlib_context_set_image(image);
                image = imlib_create_cropped_scaled_image(0,0,imlib_image_get_width(), imlib_image_get_height() ,width , height);
        }
        imlib_context_set_image(image);
        bmp = new cBitmap(imlib_image_get_width(), imlib_image_get_height(), 8);
        uint8_t *data = (uint8_t*)imlib_image_get_data_for_reading_only();
        if ( colors != 0 ){
                quantizer->Quantize(data, imlib_image_get_width()* imlib_image_get_height(), colors);
                outputImage = quantizer->OutputImage();
                outputPalette = quantizer->OutputPalette();
        }
        int pos = 0;
        for (int y = 0; y < bmp->Height(); ++y) {
                for (int x = 0; x < bmp->Width(); ++x) {
                        if ( colors != 0 ){
                                bmp->DrawPixel(x, y ,  outputPalette[outputImage[y * bmp->Width() + x]] | 0xFF000000 );
                        }else{
                                tColor col = (data[pos + 3] << 24) | (data[pos + 2] << 16) | (data[pos + 1] << 8) | data[pos + 0];
                                bmp->DrawPixel(x, y, col);
                                pos += 4;
                        }
                }
        }

        imlib_free_image();
        imlib_context_free(ctx);
//        mBitmaps.push_back(bmp);
        delete(quantizer);
        return bmp;
}

#else
cBitmap* LoadXpm(const char *Filename) {
	cBitmap *bmp = new cBitmap(1,1,1);
	if (!(bmp->LoadXpm(Filename))) {
	    esyslog("ERROR : weatherng : Couldn't load %s\n",Filename);
	}    
    return bmp;	
}	


#endif
#endif

