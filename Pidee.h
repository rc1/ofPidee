#pragma once
#include <wiringPi.h>
#if __cplusplus > 199711L
#include <functional>
#include <pthread.h>
#endif

// Description
// ===========
// C/C++11x Pidee class.  
// Needs to be complied with the wiring pi librarys eg: `gcc -Wall -o pideeExampleApp myFile.c -lwiringPi`.
//
// Event System
// ------------
// An event system can be used on C++11 compilers. Call `pidee.enabled`

class Pidee {
public:

    // GPIO Pings
    // ==========

    // Numbering Mode
    // --------------
    enum PinNumberingMode { NUMBERING_MODE_BCM, NUMBERING_MODE_WIRING_PI, NUMBERING_MODE_PHYSICAL };
    // Feature type
    // ------------
    enum FeatureType { FEATURE_TYPE_BUTTON, FEATURE_TYPE_DIP, FEATURE_TYPE_LED };
    // Direction
    enum Direction { PIDEE_OUTPUT, PIDEE_INPUT };

    // Numbering Mode Mappings
    // -----------------------
    struct PideeGPIOPinFeature {
        GPIOPin( FeatureType featureType, int featureIndex, unsigned int physical, unsigned int bcm, unsigned int wiringPi )
        :featureType(featureType),physical(physical),bcm(bcm),wiringPi(wiringPi) {
            direction = ( featureType == FEATURE_TYPE_BUTTON || FEATURE_TYPE_DIP ) ? PIDEE_INPUT : PIDEE_OUTPUT;
        };
        FeatureType featureType;
        Direction direction;
        unsigned int physical;
        unsigned int bcm;
        unsigned int wiringPi;
        unsigned int get( NumberingMode mode ) {
            if ( mode == NUMBERING_MODE_PHYSICAL ) { return physical; }
            else if ( mode == NUMBERING_MODE_BCM ) { return bcm; }
            else { return wiringPi; }
        }
    }

    // Mappings
    // --------
    typedef map<string, PideeGPIOPin> PideePinMap;

    // Settings
    // ========
    // If you are using the wiring pi library or gpio in your library you may wish to
    // customize settings, for example:
    // - `enableAutoWiringPiSetup` - setups up wiring pi using sys class useing BDM 
    // numbering and does not need require the application to be run with root 
    // privillages
    struct PideeSettings () {
        NumberingMode pinNumberingMode = NUMBERING_MODE_BCM; 
        PideePinMap pinMap;

        PideeSettings () {
            // Configutation, order of similar feature                      idx  phys  bcm  wir
            pinMap.insert( "button",    PideeGPIOPin( FEATURE_TYPE_BUTTON,  0,   1,    1,   1 ) );
            pinMap.insert( "dip-1",     PideeGPIOPin( FEATURE_TYPE_DIP,     0,   1,    1,   1 ) );
            pinMap.insert( "dip-2",     PideeGPIOPin( FEATURE_TYPE_DIP,     1,   1,    1,   1 ) );
            pinMap.insert( "dip-3",     PideeGPIOPin( FEATURE_TYPE_DIP,     2,   1,    1,   1 ) );
            pinMap.insert( "dip-4",     PideeGPIOPin( FEATURE_TYPE_DIP,     3,   1,    1,   1 ) );
            pinMap.insert( "dip-5",     PideeGPIOPin( FEATURE_TYPE_DIP,     4,   1,    1,   1 ) );
            pinMap.insert( "dip-6",     PideeGPIOPin( FEATURE_TYPE_DIP,     5,   1,    1,   1 ) );
            pinMap.insert( "dip-7",     PideeGPIOPin( FEATURE_TYPE_DIP,     6,   1,    1,   1 ) );
            pinMap.insert( "dip-8",     PideeGPIOPin( FEATURE_TYPE_DIP,     7,   1,    1,   1 ) );
            pinMap.insert( "led-red",   PideeGPIOPin( FEATURE_TYPE_LED,     0,   1,    1,   1 ) );
            pinMap.insert( "led-green", PideeGPIOPin( FEATURE_TYPE_LED,     1,   1,    1,   1 ) );
            pinMap.insert( "led-blue",  PideeGPIOPin( FEATURE_TYPE_LED,     2,   1,    1,   1 ) );
        };
    }

    // Pidee
    // ======

    // Lifecycle
    // ---------
    Pidee() { init(); }
    Pidee( PideeSettings customSettings ) { 
        settings = customSettings;
        init();
    }; 

    virtual ~Pidee() {};

    // Setup
    // -----
    //
    // ### Configure Wiring Pi
    // Setup up configures wiring pi to use BCM number and does not use
    // require sudo to be used with the features of pidee
    void setup () {
        if ( settings.pinNumberingMode != NUMBERING_MODE_BCM ) {
            throw "Pidee error: enableAutoWiringPiSetup needs the pin mode to be set to BCM numbering ";
        } else {
                    // inputs
            for ( auto& pinMap : settings.pinMap ) {
                if ( pinMap.second.direction == PideeGPIOPin.PIDEE_INPUT ) {
                    system( "gpio export " + pinMap.second.bcm + " in" );
                    system( "gpio -g mode " + pinMap.second.bcm + " up" );
                } else {
                    system( "gpio export " + pinMap.second.bcm + " out" );
                }
            }
            wiringPiSetupSys();
        }
    };

    // ### Enable Event System
    void enabledEventSystem () {
        for ( auto& pinMap : settings.pinMap ) {
            if ( pinMap.second.featureType == FEATURE_TYPE_BUTTON ) {
                wiringPiISR( 18, INT_EDGE_BOTH, &setButtonDidChange );
            }
            else if ( pinMap.second.featureType == FEATURE_TYPE_DIP ) {
                wiringPiISR( 18, INT_EDGE_BOTH, &setDipDidChange );
            }
        }
    };

    #if __cplusplus > 199711L
    // Update Event System
    // -------------------
    // Triggers any event handlers which need to be triggered.
    // Should be called for the same thread as the event handlers
    void updateEventSystem() {
        bool shouldNotifyDipListeners = false;
        bool shouldNotifyButtonListeners = false;
        pthread_mutex_lock( &interuptMutex );
        if ( dipChangedFlag ) {
            shouldNotifyDipListeners = true;
            dipChangedFlag = false;
        }
        if ( buttonChangedFlag ) {
            shouldUpdateButtonListeners = true;
            buttonChangedFlag = false;
        }
        pthread_mutex_unlock( &interuptMutex );
        if ( shouldNotifyButtonListeners ) {
            for ( auto fn : dipListners ) {
                int value = getDipSwitch();
                for ( auto fn : dipListners ) {
                    fn( value );
                }
            }
        }
        if ( shouldUpdateButtonListeners ) {
            int value = getButton();
            for ( auto fn : buttonListeners ) {
                fn( value );
            }
        }
    }
    #endif

    // Settings
    // --------
    const PideeSettings getSettings { return settings; };

    // Button
    // -------
    bool getButtonValue() { return get( "button" ); };
    // ### Events
    #if __cplusplus > 199711L
    void onButtonChange( std::function<void(bool)> fn );
    void offButtonChange( std::function<void(bool)> fn );
    #endif

    // Dip Switch
    // ----------
    unsigned int getDipSwitchValue() { 
        // to do, voodoo
        return 0;
    };
    bool getDipSwitchValueAtIndex( int index ) { return get( "domain" + std::to_string( index + 1 ) ); }; // Zero based

    // #### Events
    #if __cplusplus > 199711L
    // Example:
    //     pidee.onDipChange( [] ( unsigned int switchIndex, unsigned int switchValue, unsigned int dipSwitchValue ) {
    //         printf( "Switch %i, changed to %s. Dip switch value now", switchIndex + 1, switchValue === 1 ? "on" : "off", dipSwitchValue ); 
    //     });
    void onDipChange( std::function<void(unsigned int, bool, unsigned int)> fn );
    void offDipChange( std::function<void(unsigned int, bool, unsigned int)> fn );
    #endif

    // LEDs
    // ----
    void setLedRed( bool value )   { set( "led-red", value ); }
    void setLedGreen( bool value ) { set( "led-green", value ); }
    void setLedBlue( bool value )  { set( "led-blue", value ); }

    // Get and Set
    // -----------
    void set( const string domain, bool value ) {
        return digitalWrite( settings.pinMap.at( domain ).get( settings.pinNumberingMode ), value ? HIGH : LOW );
    }
    bool get( const string domain ) {
        return digitalRead( settings.pinMap.at( domain ).get( settings.pinNumberingMode ) );
    }

protected:
    // Init
    // ----
    void init() {
        dipChangedFlag = false;
        buttonChangedFlag = false;
    }

    // Settings
    // --------
    PideeSettings settings;

    #if __cplusplus > 199711L
    // Events
    // ------
    pthread_mutex_t interuptMutex = PTHREAD_MUTEX_INITIALIZER;

    bool dipChangedFlag;
    bool buttonChangedFlag;

    vector<unsigned int, std::function<void(unsigned int)> dipListeners; 
    vector<unsigned int, std::function<void(unsigned int)> buttonListeners; 
    
    void setDipDidChange() { 
        pthread_mutex_lock( &interuptMutex );
        dipChangedFlag = true;
        pthread_mutex_unlock() &interuptMutex ;
    };

    void setButtonDidChange() { 
        pthread_mutex_lock( &interuptMutex );
        buttonChangedFlag = true;
        pthread_mutex_unlock() &interuptMutex ;
    };

    #endif

}