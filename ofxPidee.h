#pragma once
#include <functional>

class ofxPidee () {
public:
    ofxPidee() {}; 
    virtual ~ofxPidee() {};

    // Button
    // ======
    unsigned int getButton() {};
    // Callbacks
    // ---------
    void onButton( std::function<void(unsigned int)> ) {};
    void offButton( std::function<void(unsigned int)> ) {};

    // Dip Switch
    // ==========
    unsigned int getDip() {};
    unsigned int getDip( int index ) {};
    // Callbacks
    // ---------
    void onDip( std::function<void(unsigned int)> ) {};
    void onDip( int index, std::function<void(unsigned int)> ) {};
    void offDip( std::function<void(unsigned int)> ) {};
    void offDip( int index, std::function<void(unsigned int)> ) {};

    // LEDs
    // ====
    void setRed( bool value ) {};
    void setGreen( bool value ) {};
    void setBlue( bool ) {};

protected:
}


