//
// Created by devboi on 5/19/26.
//

#ifndef CAPTIVELOGINLINUX_FUN_H
#define CAPTIVELOGINLINUX_FUN_H

#include "thirdparty/httplib.h"

namespace fun {

void funStuff(httplib::Client& httpClient);
bool login(httplib::Client& httpClient);
void logout(httplib::Client& httpClient);
void startHeartbeat(httplib::Client& httpClient);


}


#endif //CAPTIVELOGINLINUX_FUN_H
