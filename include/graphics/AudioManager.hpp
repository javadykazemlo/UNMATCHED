#pragma once

#include <SFML/Audio.hpp>
#include <array>
#include <string>

class AudioManager
{
public:
    enum class Sfx
    {
        Click,
        Confirm,
        EffectPanel,
        TurnStart,
        TurnEnd,
        Attack,
        Scheme,
        Move
    };

    AudioManager();

    void playIntroMusic();
    void playGameplayMusic();
    void playGameOverMusic();
    void stopMusic();
    void toggleMusicMute();
    bool isMusicMuted() const { return musicMuted; }

    void playSfx(Sfx effect);

private:
    sf::Music introMusic;
    sf::Music gameplayMusic;
    sf::Music gameOverMusic;

    sf::SoundBuffer clickBuffer;
    sf::SoundBuffer confirmBuffer;
    sf::SoundBuffer effectPanelBuffer;
    sf::SoundBuffer turnStartBuffer;
    sf::SoundBuffer turnEndBuffer;
    sf::SoundBuffer attackBuffer;
    sf::SoundBuffer schemeBuffer;
    sf::SoundBuffer moveBuffer;

    sf::Sound clickSound;
    sf::Sound confirmSound;
    sf::Sound effectPanelSound;
    sf::Sound turnStartSound;
    sf::Sound turnEndSound;
    sf::Sound attackSound;
    sf::Sound schemeSound;
    sf::Sound moveSound;

    std::array<bool, 9> sfxLoaded{};

    bool introMusicLoaded = false;
    bool gameplayMusicLoaded = false;
    bool gameOverMusicLoaded = false;
    bool musicMuted = false;

    void loadMusic(sf::Music& music, const std::string& path,
                   const std::string& label, bool& loaded);
    bool loadSfx(sf::SoundBuffer& buffer, const std::string& path,
                 const std::string& label, std::size_t index);
    void playMusic(sf::Music& target, bool loaded,
                   const std::string& label);
};