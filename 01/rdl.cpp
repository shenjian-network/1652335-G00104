#include "rdl.h"
#include "../common/common.h"



static void wait_for_event(event_type* e){

}

static void from_physical_layer(frame* f){

}

static void to_network_layer(packet* r){

}

void rdl(int* pidArr){
    frame r;
    event_type event;

    while(true){
        wait_for_event(&event);
        from_physical_layer(&r);
        to_network_layer(&r.info);
    }    
}