#include <fmod/fmod.h>
#include <dirent.h>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <string>
#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include "Convert.hpp"
#include "Music.hpp"

Music::Music(std::string const& file, std::map <std::string, int> const& settings): m_defaultZoom(settings.find("spectrumDefaultZoom")->second), m_zoomAdjustmentSpeed(settings.find("spectrumAdjustmentSpeed")->second),
m_minTargetScale(settings.find("spectrumMinScale")->second), m_maxTargetScale(settings.find("spectrumMaxScale")->second), m_spectrumSize(settings.find("spectrumSize")->second)
{
    FMOD_System_Create(&m_system);
    FMOD_System_Init(m_system, 1, FMOD_INIT_NORMAL, 0);

    FMOD_CHANNELGROUP *masterChannelGroup = NULL;
    FMOD_System_GetMasterChannelGroup(m_system, &masterChannelGroup);

    FMOD_System_CreateDSPByType(m_system, FMOD_DSP_TYPE_FFT, &m_dsp);
    FMOD_DSP_SetParameterInt(m_dsp, FMOD_DSP_FFT_WINDOWTYPE, FMOD_DSP_FFT_WINDOW_HANNING);
    FMOD_DSP_SetParameterInt(m_dsp, FMOD_DSP_FFT_WINDOWSIZE, m_spectrumSize * 2);
    FMOD_ChannelGroup_AddDSP(masterChannelGroup, FMOD_CHANNELCONTROL_DSP_HEAD, m_dsp);

    FMOD_System_CreateSound(m_system, file.c_str(), FMOD_2D | FMOD_CREATESTREAM, 0, &m_music);

    m_mute = settings.find("musicMute")->second;
    m_volume = settings.find("musicVolume")->second;

    play();

    m_autoplay = settings.find("musicAutoplay")->second;
    m_random = settings.find("musicRandom")->second;
    m_repeat = settings.find("musicRepeat")->second;
    m_scaleAdjustmentTime = settings.find("spectrumScaleAdjustmentTime")->second;

    m_currentTime = 0;
    m_current_scale = 0;
    m_previous_adjustment = SDL_GetTicks();
    m_max = 0;
    m_zoom = m_defaultZoom;
    m_spectrum = (float*)malloc(m_spectrumSize * sizeof(float));

    m_file = file;
}

Music::~Music()
{
    if(m_music)
        FMOD_Sound_Release(m_music);
    FMOD_System_Close(m_system);
    FMOD_System_Release(m_system);
    free(m_spectrum);
}

void Music::play() //plays the laoded file and apply the settings to it
{
    FMOD_System_PlaySound(m_system, m_music, 0, 0, &m_channel);
    FMOD_Sound_GetLength(m_music, &m_duration, FMOD_TIMEUNIT_MS);
    FMOD_Channel_SetMute(m_channel, m_mute);
    FMOD_Channel_SetVolume(m_channel, m_volume/100.0);
}

bool Music::change(int direction)
{
    m_zoom = m_defaultZoom; //Reset the spectrum to avoid big scale at the beginning of the next song
    std::string path = m_file.substr(0, m_file.find_last_of("//")); //contains only the path of the file
    std::string file = m_file.substr(m_file.find_last_of("//") + 1); //contains only the name of the file
    DIR *directory = opendir(path.c_str());
    if(directory)
    {
        struct dirent *entity;
        std::vector <long int> locations;
        int fileNumber = 0;
        long int location = telldir(directory);
        while((entity = readdir(directory)) != 0) //Seek all the audio file present in the directory and class them in the locations vector
        {
            if(strstr(entity->d_name, ".mp3") || strstr(entity->d_name, ".wav") || strstr(entity->d_name, ".flac"))
            {
                if(strcmp(entity->d_name, file.c_str()) == 0) //currently playing file
                    fileNumber = locations.size();
                locations.push_back(location);
            }
            location = telldir(directory);
        }
        
        if(locations.size() <= 1) //No other audio file found in the directory -> replay the same sound
        {
            play();
            return false;
        }
        if(direction == NEXT_MUSIC)
        {
            if(fileNumber < locations.size() - 1) //not the last music of the directory
                seekdir(directory, locations[fileNumber + 1]);
            else
                seekdir(directory, locations[0]);
            
        }
        else if(direction == PREVIOUS_MUSIC)
        {
            if(fileNumber > 0)
                seekdir(directory, locations[fileNumber - 1]); //not the first music of the directory
            else
                seekdir(directory, locations[locations.size()]);
        }
        else if(direction == RANDOM_MUSIC)
        {
            int randomFile;
            do
            {
                randomFile = rand() % locations.size();
            }
            while(randomFile == fileNumber); //Returns a music different from the music currently played
            seekdir(directory, locations[randomFile]);
        }
        entity = readdir(directory);
        m_file = convert::to_string(path) + convert::to_string("//") + convert::to_string(entity->d_name);
        closedir(directory);

        //opening the new sound
        FMOD_Sound_Release(m_music);
        FMOD_System_CreateSound(m_system, m_file.c_str(), FMOD_2D | FMOD_CREATESTREAM, 0, &m_music);
        if(!m_music)
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "erreur de lecture", "le fichier n'a pas pu etre ouvert", NULL);

        play();
        return true;
    }
    play();
    return false;
}

std::string Music::getTitle() const
{
    return m_file.substr(m_file.find_last_of("//") + 1, m_file.find_last_of('.') - m_file.find_last_of("//") - 1);
}

void Music::changeVolume(bool increase)
{
    if(increase && m_volume < 100)
        m_volume += 1;
    else if(!increase && m_volume > 0)
        m_volume -= 1;
    FMOD_Channel_SetVolume(m_channel, m_volume/100.0);
}

void Music::changeZoom(bool increase)
{
    if(increase)
        m_zoom = ceil(1.1 * m_zoom);
    else 
        m_zoom = ceil(0.9 * m_zoom);
}

void Music::changeScaleAdjustmentTime(bool increase)
{
    if(increase)
        m_scaleAdjustmentTime += 10;
    else if(!increase && m_scaleAdjustmentTime > 10)
        m_scaleAdjustmentTime -= 10;
}

int Music::getScaleAdjustmentTime() const
{
    return m_scaleAdjustmentTime;
}

int Music::getVolume() const
{
    return m_volume;
}

void Music::switchPause()
{
    FMOD_BOOL paused;
    FMOD_Channel_GetPaused(m_channel, &paused);
    FMOD_Channel_SetPaused(m_channel, !paused);
}

bool Music::getPause() const
{
    FMOD_BOOL paused;
    FMOD_Channel_GetPaused(m_channel, &paused);
    return paused;
}

void Music::setAutoplay(bool autoplay)
{
    m_autoplay = autoplay;
}

void Music::setRandom(bool random)
{
    m_random = random;
}

void Music::setRepeat(bool repeat)
{
    m_repeat = repeat;
}

bool Music::getAutoplay() const
{
    return m_autoplay;
}

bool Music::getRandom() const
{
    return m_random;
}

bool Music::getRepeat() const
{
    return m_repeat;
}

void Music::setPosition(float percent)
{
    FMOD_Channel_SetPosition(m_channel, percent * m_duration, FMOD_TIMEUNIT_MS);
}

unsigned int Music::getCurrentTime() const
{
    return m_currentTime;
}

unsigned int Music::getDuration() const
{
    return m_duration;
}

float Music::getScale() const
{
    return m_current_scale;
}

float Music::getMax() const
{
    return m_max;
}

int Music::getSpectrumZoom() const
{
    return m_zoom;
}

void Music::setMute(bool mute)
{
    m_mute = mute;
    FMOD_Channel_SetMute(m_channel, m_mute);
}

bool Music::getMute() const
{
    return m_mute;
}

void Music::update() //This function needs to be called each frame in order to provide automatic read and to analyze the spectrum
{
    FMOD_BOOL isPlaying;
    FMOD_System_Update(m_system);
    FMOD_Channel_GetPosition(m_channel, &m_currentTime, FMOD_TIMEUNIT_MS);
    FMOD_Channel_IsPlaying(m_channel, &isPlaying);
    if(!isPlaying)
    {
        m_zoom = m_defaultZoom; //Reset the spectrum to avoid big scale at the beginning of the next song
        if(m_repeat)
            play();
        else if(m_autoplay)
        {
            if(m_random)
                change(RANDOM_MUSIC);
            else
                change(NEXT_MUSIC);
        }
    }
    else
        analyzeSpectrum();
}

void Music::analyzeSpectrum()
{    
    void *spectrumData;
    FMOD_DSP_GetParameterData(m_dsp, FMOD_DSP_FFT_SPECTRUMDATA, (void **) &spectrumData, 0, 0, 0 );
    FMOD_DSP_PARAMETER_FFT* fft = (FMOD_DSP_PARAMETER_FFT *)spectrumData;
    if(fft->length != 0) //the fft could not be calculated (happens at the beginning of a music)
        memcpy(m_spectrum, fft->spectrum[0], m_spectrumSize * sizeof(float));
    
    m_max = 0;
    float calculated_scale = 0;
    //power of the signal and maximum value
    for(int i = 0; i < m_spectrumSize; i++)
    {
        calculated_scale += pow(m_spectrum[i], 2);
        if(m_spectrum[i] > m_max)
            m_max = m_spectrum[i];
    }
    calculated_scale = calculated_scale/m_spectrumSize * m_zoom;
    //compute the log and scale it so it stays between 0 and 1
    calculated_scale = log10f(1+9*(calculated_scale));

    if(calculated_scale < m_minTargetScale/100.0) //Adjust the spectrum zoom so that the power stays between those values most of the time
        m_zoom += m_zoomAdjustmentSpeed;
    else if(calculated_scale > m_maxTargetScale/100.0 && m_zoom > m_zoomAdjustmentSpeed)
        m_zoom -= m_zoomAdjustmentSpeed;

    if(calculated_scale > 1) //Prevent the power from being higher than 1 (causing the object to go higher than expected)
        calculated_scale = 1;

    m_previous_scales.push_back(calculated_scale);
    m_previous_times.push_back(SDL_GetTicks());
    while(SDL_GetTicks() - m_previous_times[0] > m_scaleAdjustmentTime) //erase the previous scales that are too old
    {
        m_previous_times.erase(m_previous_times.begin());
        m_previous_scales.erase(m_previous_scales.begin());
    }

    float delta_scale = *std::max_element(m_previous_scales.begin(), m_previous_scales.end()) - m_current_scale; //calculate the delta
    m_current_scale += delta_scale * (SDL_GetTicks() - m_previous_adjustment)/m_scaleAdjustmentTime; //adjust the current scale
    m_previous_adjustment = SDL_GetTicks();
}

bool Music::getError() const //returns if the music could not be loaded
{
    if(!m_music)
        return true;
    else
        return false;
}

float* Music::getSpectrum() const
{
    return m_spectrum;
}

int Music::getSpectrumSize() const
{
    return m_spectrumSize;
}