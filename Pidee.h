#pragma once
#include <functional>
#if __cplusplus > 199711L
#include <pthread.h>
#endif

// Description
// ===========
// C/C++Ox wrapper for Pidee

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

    // Numbering Mode Mappings
    // -----------------------
    struct PideeGPIOPin {
        enum Direction { PIDEE_OUTPUT, PIDEE_INPUT };
        GPIOPin( FeatureType featureType, unsigned int physical, unsigned int bcm, unsigned int wiringPi )
        :featureType(featureType),physical(physical),bcm(bcm),wiringPi(wiringPi) {
            if ( featureType == FEATURE_TYPE_BUTTON || FEATURE_TYPE_DIP ) {
                direction = PIDEE_INPUT;
            } 
            else {
                direction = PIDEE_OUTPUT;
            }
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
    // - `enableAutoWiringPiSetup` - setups up wiring pi using sys class usesing BDM 
    // numbering and does not need require the application to be run with root 
    // privillages
    struct PideeSettings () {
        NumberingMode pinNumberingMode = NUMBERING_MODE_BCM; 
        bool enableAutoWiringPiSetup = true;
        // Sets up interupts and a thread to check for changes. `updateEvents` should
        // be called in the user thread to when it is safe for the lambdas to be 
        // triggered
        bool enableEvents = true; 
        PideePinMap pinMap;
        PideeSettings () {
            // Configutation, order of similar feature
            pinMap.insert( "button",    PideeGPIOPin( FEATURE_TYPE_BUTTON,  1, 1, 1 ) );
            pinMap.insert( "dip-1",     PideeGPIOPin( FEATURE_TYPE_DIP, 1, 1, 1 ) );
            pinMap.insert( "dip-2",     PideeGPIOPin( FEATURE_TYPE_DIP, 1, 1, 1 ) );
            pinMap.insert( "dip-3",     PideeGPIOPin( FEATURE_TYPE_DIP, 1, 1, 1 ) );
            pinMap.insert( "dip-4",     PideeGPIOPin( FEATURE_TYPE_DIP, 1, 1, 1 ) );
            pinMap.insert( "dip-5",     PideeGPIOPin( FEATURE_TYPE_DIP, 1, 1, 1 ) );
            pinMap.insert( "dip-6",     PideeGPIOPin( FEATURE_TYPE_DIP, 1, 1, 1 ) );
            pinMap.insert( "dip-7",     PideeGPIOPin( FEATURE_TYPE_DIP, 1, 1, 1 ) );
            pinMap.insert( "dip-8",     PideeGPIOPin( FEATURE_TYPE_DIP, 1, 1, 1 ) );
            pinMap.insert( "led-red",   PideeGPIOPin( FEATURE_TYPE_LED, 1, 1, 1 ) );
            pinMap.insert( "led-green", PideeGPIOPin( FEATURE_TYPE_LED, 1, 1, 1 ) );
            pinMap.insert( "led-blue",  PideeGPIOPin( FEATURE_TYPE_LED, 1, 1, 1 ) );
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

    void updateEvents() {
        bool shouldUpdateDipListeners = false;
        bool shouldUpdateButtonListeners = false;
        pthread_mutex_lock( &interuptMutex );
        if ( dipChangedFlag ) {
            shouldUpdateDipListeners = true;
            dipChangedFlag = false;
        }
        if ( buttonChangedFlag ) {
            shouldUpdateButtonListeners = true;
            buttonChangedFlag = false;
        }
        pthread_mutex_unlock( &interuptMutex );
        ditch the FeatureType... just go by domain and set the fn maps accordingly
        if ( shouldUpdateDipListeners ) {
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

    // Settings
    // --------
    PideeSettings getSettings { return settings; };

    // Button
    // -------
    int getButton() {
        return digitalRead( settings.pinMap.at( "button" ).get( settings.pinNumberingMode ) );
    };

    // ### Events
    #if __cplusplus > 199711L
    void onButtonChange( std::function<void(unsigned int)> );
    void offButtonChange( std::function<void(unsigned int)> );
    #endif

    // Dip Switch
    // ----------
    int getDipSwitch() { 
        // to do, voodoo
        return 0;
    };
    int getDipSwitchAtIndex( int index ) { return get( "domain" + std::to_string( index + 1 ) ); };

    // #### Events
    #if __cplusplus > 199711L
    void onDipChange( std::function<void(unsigned int)> );
    void offDipChange( std::function<void(unsigned int)> );
    #endif

    // LEDs
    // ----
    void setLedRed( bool ) { return set( "led-red" ); }
    void setLedGreen( bool ) { return set( "led-green" ); }
    void setLedBlue( bool ) { return set( "led-blue" ); }

    // Get and Set
    // -----------
    int set( const string domain ) {
        return digitalWrite( settings.pinMap.at( domain ).get( settings.pinNumberingMode ), value ? HIGH : LOW );
    }
    int get( const string domain ) {
        return digitalRead( settings.pinMap.at( domain ).get( settings.pinNumberingMode ) );
    }

protected:
    // Init
    // ----
    void init() {
        dipChangedFlag = false;
        buttonChangedFlag = false;
        if ( settings.enableAutoWiringPiSetup ) {
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
        }
        #if __cplusplus > 199711L
        if ( settings.enableEvents ) {
            for ( auto& pinMap : settings.pinMap ) {
                if ( pinMap.second.featureType == FEATURE_TYPE_BUTTON ) {
                    wiringPiISR( 18, INT_EDGE_BOTH, &setButtonDidChange );
                }
                else if ( pinMap.second.featureType == FEATURE_TYPE_DIP ) {
                    wiringPiISR( 18, INT_EDGE_BOTH, &setDipDidChange );
                }
            }
        }
        #endif
    }
    // Settings
    // --------
    PideeSettings settings;
    // Events
    // ------
    #if __cplusplus > 199711L
    pthread_mutex_t interuptMutex = PTHREAD_MUTEX_INITIALIZER;
    void setDipDidChange() { 
        pthread_mutex_lock( &interuptMutex );
        dipChangedFlag = true;
        pthread_mutex_unlock() &interuptMutex ;
    };
    bool dipChangedFlag;
    vector<unsigned int, std::function<void(unsigned int)> dipListners; 
    void setButtonDidChange() { 
        pthread_mutex_lock( &interuptMutex );
        buttonChangedFlag = true;
        pthread_mutex_unlock() &interuptMutex ;
    };
    bool buttonChangedFlag;
    vector<unsigned int, std::function<void(unsigned int)> buttonListeners; 
    #endif

}