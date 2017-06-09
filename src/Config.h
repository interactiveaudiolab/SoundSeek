//
//  config.h
//  AudioSearchCLI
//
//  Created by Michael Donovan on 5/17/17.
//

#ifndef CONFIG_H
#define CONFIG_H

#include "Common.h"
#include "libs/json.hpp"
#include <iostream>

#define DEFAULT_LOCAL true

namespace Config {
using nlohmann::json;
using std::string;
using boost::filesystem::path;

static path configFilePath()
{
    return path(string(getenv("HOME")) + string(APP_SUPPORT) + string(CFG_PATH));
}

static json load()
{
    path config_path = configFilePath();
    json config;

    if (is_regular_file(config_path)) {
        try {
            std::ifstream i(config_path.string());
            config << i;
        } catch (...) {
            std::cerr << "Reading config file failed." << std::endl;
        }
    }

    return config;
}

static void save(json config)
{
    path config_path = configFilePath();
    try {
        std::ofstream o(config_path.string());
        o << std::setw(4) << config << std::endl;
    } catch (...) {
        std::cerr << "Writing config file failed." << std::endl;
    }
}
}

#endif
