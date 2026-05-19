//
// Created by devboi on 5/17/26.
//

#include "core/detect.hpp"
#include "details.h"
#include "thirdparty/httplib.h"

bool utils::detectPortal() {
    httplib::Client networkCheckClient(consts::PORTAL_DETECT_URL);

    auto res = networkCheckClient.Get("/generate_204");
    if (res) {
        // The response should be 204, if it is anything else, there might be a portal
        return res->status != 204;
    }

    // No response? Something went wrong.
    throw std::runtime_error("Error while trying to generate 204: " + httplib::to_string(res.error()));
}