#ifndef DEF_MUSIC
#define DEF_MUSIC

#include <fmod/fmod.h>
#include <string>
#include <map>

enum
{
    NEXT_MUSIC, PREVIOUS_MUSIC, RANDOM_MUSIC
};

typedef class Music Music;
class Music
{
public:
    Music(std::string const& file, std::map <std::string, int> const& settings);
    ~Music();

    void switchPause();
    void setAutoplay(bool autoplay);
    void setRepeat(bool repeat);
    void setRandom(bool random);
    void setPosition(float percent);
    void setMute(bool mute);
    void changeVolume(bool increase);
    void changeZoom(bool increase);
    void changeScaleAdjustmentTime(bool increase);

    int getVolume() const;
    bool getAutoplay() const;
    bool getRepeat() const;
    bool getRandom() const;
    float getScale() const;
    float getMax() const;
    float* getSpectrum() const;
    int getSpectrumSize() const; 
    int getSpectrumZoom() const;
    bool getPause() const;
    unsigned int getCurrentTime() const;
    unsigned int getDuration() const;
    bool getMute() const;
    bool getError() const;
    std::string getTitle() const;
    int getScaleAdjustmentTime() const;

    bool change(int direction);
    void update();

private:
    void analyzeSpectrum();
    void play();

    FMOD_SYSTEM *m_system;
    FMOD_SOUND *m_music;
    FMOD_CHANNEL *m_channel;
    FMOD_DSP *m_dsp;
    
    bool m_autoplay;
    bool m_repeat;
    bool m_random;
    bool m_mute;
    int m_volume;

    unsigned int m_duration;
    unsigned int m_currentTime;
    float *m_spectrum;
    float m_max;
    int m_zoom;

    int m_scaleAdjustmentTime;
    int m_previous_adjustment;
    std::vector<float> m_previous_scales;
    std::vector<float> m_previous_times;
    float m_current_scale;

    const int m_defaultZoom;
    const int m_zoomAdjustmentSpeed;
    const int m_minTargetScale;
    const int m_maxTargetScale;
    const int m_spectrumSize;

    std::string m_file;
};

#endif
