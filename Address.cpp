#include "Address.h"
#include "global.h"


Address::Address(){
    address = "";
    user = "";
    domain = "";
}


bool Address::SetAddress(string addr){
    
    if (!ValidateAddress(addr))
        return -1;
    
    address = addr;
    size_t pos = address.find("@");
          
    user = address.substr(0, pos);
    domain = addr.erase(0, pos + 1);
    
    return 0;
    
}

