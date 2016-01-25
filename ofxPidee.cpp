#include "ofxPidee.h"
#include "Pidee.h"

// Lifecycle
// =========
ofxPidee::ofxPidee() {
    Pidee::initWiringPi();
}; 
ofxPidee::~ofxPidee() {};

// Button
// ======
unsigned int ofxPidee::getButton() {};
// Callbacks
// ---------
void ofxPidee::onButton( std::function<void(unsigned int)> ) {};
void ofxPidee::offButton( std::function<void(unsigned int)> ) {};

// Dip Switch
// ==========
unsigned int ofxPidee::getDip() {};
unsigned int ofxPidee::getDip( int index ) {};
// Callbacks
// ---------
void ofxPidee::onDip( std::function<void(unsigned int)> ) {};
void ofxPidee::onDip( int index, std::function<void(unsigned int)> ) {};
void ofxPidee::offDip( std::function<void(unsigned int)> ) {};
void ofxPidee::offDip( int index, std::function<void(unsigned int)> ) {};

// LEDs
// ====
void ofxPidee::setRed( bool value ) {};
void ofxPidee::setGreen( bool value ) {};
void ofxPidee::setBlue( bool ) {};

protected:
void ofxPidee::updateListeners() {};
void ofxPidee::enableinterupts() {};
void ofxPidee::disableInterupts() {};


