#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include "main.hpp"
#include "Window.hpp"
#include "Convert.hpp"


int main(int argc, char *argv[])
{
    std::string directory;
    getDirectory(directory, argv[0]);

    std::string file;
    if(argc == 1)//only to launch it from vscode (to have some music to test the programm)
        file = "test.mp3";
    else
        file = argv[1];    

    std::map <std::string, int> settings;
    //loads the settings
    loadSettings(directory, settings);

    //create the window and checks the errors
    Window window(directory, file, settings);
    if(window.getError())
        return 1;

    window.execute();

    //save the settings before exiting
    window.getSettings(settings);
    saveSettings(directory, settings);

    return 0;
}

void getDirectory(std::string &directory, const char *path) //isolate the path of the directory containing the exe so that we can loads files even when opening directly from an audio file
{
    directory = convert::to_string(path).substr(0, convert::to_string(path).find_last_of('//') + 1);
}

bool loadSettings(std::string const& directory, std::map <std::string, int> &settings)
{
    std::string path = directory + "settings.txt";
    std::ifstream file(path.c_str());
    if(file)
    {
        std::string buffer;
        while(getline(file, buffer))
            settings[buffer.substr(0, buffer.find('='))] = strtol(buffer.substr(buffer.find('=') + 1).c_str(), NULL, 10);
        file.close();
        if(settings.size() == 34) //Seems that all settings are there... (otherwise, loads the default settings)
            return true;
    }
    //default settings
    settings["windowWidth"] = 1280;
    settings["windowHeight"] = 720;
    settings["windowMaximized"] = 1;
    settings["sceneTheme"] = 1;
    settings["sceneTimeToTop"] = 1000; //in ms
    settings["sceneRenderMethod"] = 0;
    settings["objectNumber"] = 5000;
    settings["objectSize"] = 10;
    settings["musicAutoplay"] = 0;
    settings["musicRepeat"] = 0;
    settings["musicRandom"] = 0;
    settings["musicMute"] = 0;
    settings["musicVolume"] = 100; //in %
    //Parameters than cannot be modified within the programm (no get method required !)
    settings["showIntro"] = 1;
    settings["messageDuration"] = 2000; //in ms
    settings["messageTransition"] = 500; //in ms
    settings["spectrumMinScale"] = 50; //in %
    settings["spectrumMaxScale"] = 100; //in %
    settings["spectrumAdjustmentSpeed"] = 1;
    settings["spectrumDefaultZoom"] = 100;
    settings["spectrumSize"] = 256; //needs to be a multiple of 2 (min 64)
    settings["objectColorRed"] = 25;
    settings["objectColorGreen"] = 50;
    settings["objectColorBlue"] = 90;
    settings["backgroundColorRed"] = 80;
    settings["backgroundColorGreen"] = 150;
    settings["backgroundColorBlue"] = 255;
    settings["messageColorRed"] = 0;
    settings["messageColorGreen"] = 0;
    settings["messageColorBlue"] = 0;
    settings["messageFontSize"] = 60;
    settings["progressbarDuration"] = 2000; //in ms
    settings["progressbarHeight"] = 50;
    settings["spectrumScaleAdjustmentTime"] = 100; //in ms
    return false;
}

bool saveSettings(std::string const& directory, std::map <std::string, int> &settings)
{
    std::string path = directory + "settings.txt";
    std::ofstream file(path.c_str());
    if(file)
    {
        for(std::map <std::string, int>::iterator it = settings.begin(); it != settings.end(); ++it)
            file << it->first << "=" << it->second << std::endl;
        file.close();
        return true;
    }
    return false;
}
