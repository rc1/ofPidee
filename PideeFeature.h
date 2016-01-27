#include <wiringPi.h>
#include <string>

// Feature Types
// =============
enum PideeFeatureType { PIDEE_DIP, PIDEE_BUTTON, PIDEE_LED };

// Features
// ========      
struct PideeFeature {
    unsigned int physical_pin;
    PideeFeatureType type;
    void(*interupt_handler)(PideeFeature*);
}

PideeFeature pidee_feature_button     = { PIDEE_BUTTON, 12, false };
PideeFeature pidee_feature_dip_1      = { PIDEE_DIP,    11, false };
PideeFeature pidee_feature_dip_2      = { PIDEE_DIP,    13, false };
PideeFeature pidee_feature_dip_3      = { PIDEE_DIP,    15, false };
PideeFeature pidee_feature_dip_4      = { PIDEE_DIP,    16, false };
PideeFeature pidee_feature_dip_5      = { PIDEE_DIP,    18, false };
PideeFeature pidee_feature_dip_6      = { PIDEE_DIP,    22, false };
PideeFeature pidee_feature_dip_7      = { PIDEE_DIP,    29, false };
PideeFeature pidee_feature_dip_8      = { PIDEE_DIP,    31, false };
PideeFeature pidee_feature_led_red    = { PIDEE_LED,    33, false };
PideeFeature pidee_feature_led_green  = { PIDEE_LED,    32, false };
PideeFeature pidee_feature_led_yellow = { PIDEE_LED,    35, false };

// Interupts
// =========
void pidee_button_interupt() { if ( pidee_feature_button.interupt_handler != NULL ) { &pidee_feature_button.interupt_handler( &pidee_feature_button ); } };
void pidee_dip_1_interupt()  { if ( pidee_feature_dip_1.interupt_handler != NULL ) { &pidee_feature_dip_1.interupt_handler( &pidee_feature_dip_1 ); } };
void pidee_dip_2_interupt()  { if ( pidee_feature_dip_2.interupt_handler != NULL ) { &pidee_feature_dip_2.interupt_handler( &pidee_feature_dip_2 ); }};
void pidee_dip_3_interupt()  { if ( pidee_feature_dip_3.interupt_handler != NULL ) { &pidee_feature_dip_3.interupt_handler( &pidee_feature_dip_3 ); }};
void pidee_dip_4_interupt()  { if ( pidee_feature_dip_4.interupt_handler != NULL ) { &pidee_feature_dip_4.interupt_handler( &pidee_feature_dip_4 ); }};
void pidee_dip_5_interupt()  { if ( pidee_feature_dip_5.interupt_handler != NULL ) { &pidee_feature_dip_5.interupt_handler( &pidee_feature_dip_5 ); }};
void pidee_dip_6_interupt()  { if ( pidee_feature_dip_6.interupt_handler != NULL ) { &pidee_feature_dip_6.interupt_handler( &pidee_feature_dip_6 ); }};
void pidee_dip_7_interupt()  { if ( pidee_feature_dip_7.interupt_handler != NULL ) { &pidee_feature_dip_7.interupt_handler( &pidee_feature_dip_7 ); }};
void pidee_dip_8_interupt()  { if ( pidee_feature_dip_8.interupt_handler != NULL ) { &pidee_feature_dip_8.interupt_handler( &pidee_feature_dip_8 ); }};

// Setup/Read/Write 
// ================
void pidee_feature_setup( PideeFeature *feature,  ) {
    if ( feature->type == PIDEE_BUTTON || feature->type == PIDEE_DIP ) {
        system( "gpio export " + physPinToGpio( feature->physical_pin ) + " in" );
        system( "gpio -g mode " + physPinToGpio( feature->physical_pin ) + " up" );
    } else {
        system( "gpio export " + physPinToGpio( feature->physical_pin ) + " out" );
    }
}

int pidee_feature_read( PideeFeature *feature ) {
    return digitalRead( physPinToGpio( feature.physical_pin ) );
}

void pidee_feature_write( PideeFeature *feature, int value ) {
    return digitalRead( physPinToGpio( feature.physical_pin ) );
}

// Interupts
// ========= 
void pidee_feature_enable_interupt( PideeFeature *feature ) {
    int pin = physPinToGpio( eature->physical_pin );
    switch ( feature->physical_pin ) {
        case pidee_feature_button: wiringPiISR( pin, INT_EDGE_BOTH, &pidee_button_interupt ); break;
        case pidee_feature_dip_1.physical_pin: wiringPiISR( pin, INT_EDGE_BOTH, &pidee_dip_1_interupt ); break;
        case pidee_feature_dip_2.physical_pin: wiringPiISR( pin, INT_EDGE_BOTH, &pidee_dip_2_interupt ); break;
        case pidee_feature_dip_3.physical_pin: wiringPiISR( pin, INT_EDGE_BOTH, &pidee_dip_3_interupt ); break;
        case pidee_feature_dip_4.physical_pin: wiringPiISR( pin, INT_EDGE_BOTH, &pidee_dip_4_interupt ); break;
        case pidee_feature_dip_5.physical_pin: wiringPiISR( pin, INT_EDGE_BOTH, &pidee_dip_5_interupt ); break;
        case pidee_feature_dip_6.physical_pin: wiringPiISR( pin, INT_EDGE_BOTH, &pidee_dip_6_interupt ); break;
        case pidee_feature_dip_7.physical_pin: wiringPiISR( pin, INT_EDGE_BOTH, &pidee_dip_7_interupt ); break;
        case pidee_feature_dip_8.physical_pin: wiringPiISR( pin, INT_EDGE_BOTH, &pidee_dip_8_interupt ); break;
    }
}


