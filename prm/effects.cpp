#include "globals.hpp"
#include <math.h>
#include "lib/color.h"
#include "lib/effect.h"
#include "lib/effect_runner.h"
#include "lib/noise.h"



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

  FastEffect e;
  SlowEffect e2;
	StopEffect e3;
  r.setEffect(&e);

  r.setMaxFrameRate(100);
  r.setLayout("layouts/grid32x16z.json");
	
  if (!r.parseArguments(*(arguments.argc), arguments.argv)) {
      
		return (void*)1;
  }

	while(loop) {
	  pthread_mutex_lock(&lock);
		switch(effects.effect) {
			case 48:
				r.setEffect(&e);
				break;
			case 49:
				r.setEffect(&e2);
				break;
			default:
				r.setEffect(&e3);
		}	
		pthread_mutex_unlock(&lock);
		r.doFrame();
	}
	r.setEffect(&e3);
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
