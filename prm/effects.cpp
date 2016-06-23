#include "globals.hpp"
#include <math.h>
#include "lib/color.h"
#include "lib/effect.h"
#include "lib/effect_runner.h"
#include "lib/noise.h"
using namespace std;


class StopEffect : public Effect {

public:
		virtual void beginFrame(const FrameInfo &f) {
		}

		virtual void shader(Vec3& rgb, const PixelInfo &p) const {
			rgb[0]=0.0;
			rgb[1]=0.0;
			rgb[2]=0.0;
		}

};

class FastEffect : public Effect
{
public:
    FastEffect()
        : cycle (0) {}

    float cycle;

    virtual void beginFrame(const FrameInfo &f)
    {
        const float speed = 10.0;
        cycle = fmodf(cycle + f.timeDelta * speed, 2 * M_PI);
    }

    virtual void shader(Vec3& rgb, const PixelInfo &p) const
    {
        float distance = len(p.point);
        float wave = sinf(3.0 * distance - cycle) + noise3(p.point);
        hsv2rgb(rgb, 0.2, 0.3, wave);
    }
};

class SlowEffect : public Effect
{
public:
    SlowEffect()
        : cycle (0) {}

    float cycle;

    virtual void beginFrame(const FrameInfo &f)
    {
        const float speed = 1.0;
        cycle = fmodf(cycle + f.timeDelta * speed, 2 * M_PI);
    }

    virtual void shader(Vec3& rgb, const PixelInfo &p) const
    {
        float distance = len(p.point);
        float wave = sinf(3.0 * distance - cycle) + noise3(p.point);
        hsv2rgb(rgb, 0.2, 0.3, wave);
    }
};

void* effects_main(void* memes)
{
  EffectRunner r;

  FastEffect fastEffect;
  SlowEffect slowEffect;
  StopEffect stopEffect;
  
  r.setMaxFrameRate(100);
  r.setLayout("layouts/grid32x16z.json");
	
  if (!r.parseArguments(*(arguments.argc), arguments.argv)) {
		return (void*)1;
  }
 
  unsigned int noteCounter = 0;

  while(loop) {
    pthread_mutex_lock(&lock);
      if(effect.midiReceived){
        switch(effect.byte1) {
          case 144:
            if(effect.byte2 == 36) {
              r.setEffect(&fastEffect);
              noteCounter++;
            } else if(effect.byte2 == 37) {
                r.setEffect(&slowEffect);
                noteCounter++;
            }
            std::cout << "Note on " << noteCounter << endl; 
            break;
          case 128:
            if(effect.byte2 == 36){
              noteCounter--;
              if(noteCounter == 0)
                r.setEffect(&stopEffect);
            }
            else if(effect.byte2 == 37){
              noteCounter--;
              if(noteCounter == 0)
                r.setEffect(&stopEffect);
            }
            std::cout << "Note off " << noteCounter << endl; 
            break;
          case 176:
            break;
          default:
            break;		
         }
      }  
      effect.midiReceived = false;
      pthread_mutex_unlock(&lock);
      r.doFrame();
	}
	r.setEffect(&stopEffect);
	r.doFrame();
	r.doFrame();
	r.doFrame();
	
  return (void*)0;

  //return r.main(argc, argv);
}

/*int main (int argc, char **argv) {

	loop = 1;
	return effects_main(argc, argv);

}*/
