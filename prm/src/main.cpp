/*
 * main.cpp
 *
 *  Created on: Jun 19, 2016
 *      Author: luxcapto
 */

/* RtMidi includes */
#include <iostream>
#include <cstdlib>
#include "lib/RtMidi.h"

// Fadecandy includes
#include <math.h>
#include "lib/color.h"
#include "lib/effect.h"
#include "lib/effect_runner.h"
#include "lib/noise.h"

// Fadecandy "Simple" Effect class
class MyEffect : public Effect
{
public:
    MyEffect()
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

// RtMidi argument error function
void usage( void ) {
  std::cout << "\nuseage: cmidiin <port>\n";
  std::cout << "    where port = the device to use (default = 0).\n\n";
  exit( 0 );
}

// RtMidi callback function (prints out received MIDI data)
void mycallback( double deltatime, std::vector< unsigned char > *message, void */*userData*/ )
{
  unsigned int nBytes = message->size();
  for ( unsigned int i=0; i<nBytes; i++ )
    std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
  if ( nBytes > 0 )
    std::cout << "stamp = " << deltatime << std::endl;
}

// This function should be embedded in a try/catch block in case of
// an exception.  It offers the user a choice of MIDI ports to open.
// It returns false if there are no ports available.
bool chooseMidiPort( RtMidiIn *rtmidi );


int main(int argc, char **argv)
{
    // Fadecandy
		EffectRunner r;

		MyEffect e;
		r.setEffect(&e);

		// Defaults, overridable with command line options
		r.setMaxFrameRate(100);
		r.setLayout("../layouts/grid32x16z.json");

		return r.main(argc, argv);

	// RtMIDI
		 RtMidiIn *midiin = 0;

		  // Minimal command-line check.
		  if ( argc > 2 ) usage();

		  try {

		    // RtMidiIn constructor
		    midiin = new RtMidiIn();

		    // Call function to select port.
		    if ( chooseMidiPort( midiin ) == false ) goto cleanup;

		    // Set our callback function.  This should be done immediately after
		    // opening the port to avoid having incoming messages written to the
		    // queue instead of sent to the callback function.
		    midiin->setCallback( &mycallback );

		    // Don't ignore sysex, timing, or active sensing messages.
		    midiin->ignoreTypes( false, false, false );

		    std::cout << "\nReading MIDI input ... press <enter> to quit.\n";
		    char input;
		    std::cin.get(input);

		  } catch ( RtMidiError &error ) {
		    error.printMessage();
		  }

		 cleanup:

		  delete midiin;

		  return 0;
}

// Command line for opening MIDI port and out putting notes
bool chooseMidiPort( RtMidiIn *rtmidi )
{
  std::cout << "\nWould you like to open a virtual input port? [y/N] ";

  std::string keyHit;
  std::getline( std::cin, keyHit );
  if ( keyHit == "y" ) {
    rtmidi->openVirtualPort();
    return true;
  }

  std::string portName;
  unsigned int i = 0, nPorts = rtmidi->getPortCount();
  if ( nPorts == 0 ) {
    std::cout << "No input ports available!" << std::endl;
    return false;
  }

  if ( nPorts == 1 ) {
    std::cout << "\nOpening " << rtmidi->getPortName() << std::endl;
  }
  else {
    for ( i=0; i<nPorts; i++ ) {
      portName = rtmidi->getPortName(i);
      std::cout << "  Input port #" << i << ": " << portName << '\n';
    }

    do {
      std::cout << "\nChoose a port number: ";
      std::cin >> i;
    } while ( i >= nPorts );
    std::getline( std::cin, keyHit );  // used to clear out stdin
  }

  rtmidi->openPort( i );

  return true;
}


