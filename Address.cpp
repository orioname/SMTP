#include "Address.h"
#include "global.h"


Address::Address(){
    address = "";
    user = "";
    domain = "";
}

static bool ValidateAddress(string addr){
        
    if ((addr.length() > 2) && 
            (addr.find("@") != string::npos) &&
            addr.find("@") > 1)
        return true;
        
    return false;
    
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

