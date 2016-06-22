#include <iostream>
#include <cstdlib>
#include "lib/RtMidi.h"
#include "globals.hpp"
#include "effects.hpp"
using namespace std;

char loop;
args_t arguments;
effects_t effect;
pthread_mutex_t lock;

void usage( void ) {
// Error function in case of incorrect command-line
// argument specifications.
std::cout << "\nuseage: cmidiin <port>\n";
std::cout << "    where port = the device to use (default = 0).\n\n";
exit( 0 );
}

void mycallback( double deltatime, std::vector< unsigned char > *message, void */*userData*/ )
{
  /*
  1st Byte           2nd Byte               3rd byte
    Note on: 144       CC control number      CC control value
    Note off: 128      Note value             Note velocity
    CC: 176
  */

  unsigned int nBytes = message->size();
  for ( unsigned int i=0; i<nBytes; i++ )
    std::cout << "Byte " << i << " = " << (int)message->at(i) << " ";
  std::cout << endl;
  if (nBytes == 3) {
		unsigned char firstByte = message->at(0);
		unsigned char secondByte = message->at(1);
		unsigned char thirdByte = message->at(2);
		pthread_mutex_lock(&lock);
		effect.byte1 = firstByte;
		effect.byte2 = secondByte;
		effect.byte3 = thirdByte;
		pthread_mutex_unlock(&lock);
  } 
          
}

// This function should be embedded in a try/catch block in case of
// an exception.  It offers the user a choice of MIDI ports to open.
// It returns false if there are no ports available.
bool chooseMidiPort( RtMidiIn *rtmidi );

int main( int argc, char **argv )
{
	int return_value;
	pthread_t effects_thread;
	loop = 1;
	lock = PTHREAD_MUTEX_INITIALIZER;
	arguments.argc = &argc;
	arguments.argv = argv;


  RtMidiIn *midiin = 0;

  // Minimal command-line check.
  //if ( argc > 2 ) usage();

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
		
		return_value = pthread_create(&effects_thread, NULL, effects_main, NULL);

    std::cout << "\nReading MIDI input ... press <enter> to quit.\n";
    char input;
    std::cin.get(input);
		
		loop = 0;
		pthread_join(effects_thread, NULL);
		

  } catch ( RtMidiError &error ) {
    error.printMessage();
  }

  cleanup:
  delete midiin;
  return 0;
}


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

