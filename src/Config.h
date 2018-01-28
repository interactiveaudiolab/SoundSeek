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

#define DEFAULT_LOCAL false

namespace Config
{
using nlohmann::json;
using std::string;
using boost::filesystem::path;

static path configFilePath ()
{
    return path (string (getenv ("HOME")) + string (APP_SUPPORT) + string (CFG_PATH));
}


static void save (json config)
{
    path config_path = configFilePath ();
    try
    {
        std::ofstream o (config_path.string ());
        o << std::setw (4) << config << std::endl;
    }
    catch (...)
    {
        cerr << "Writing config file failed." << endl;
    }
}


static json load ()
{
    path config_path = configFilePath ();
    json config;

    if (is_regular_file (config_path))
    {
        try
        {
            std::ifstream i (config_path.string ());
            config << i;
        }
        catch (...)
        {
            cerr << "Reading config file failed." << endl;
        }
    }
    else
    {
        config["localAlign"] = DEFAULT_LOCAL;
        save(config);
    }

    return config;
}
}

#endif
