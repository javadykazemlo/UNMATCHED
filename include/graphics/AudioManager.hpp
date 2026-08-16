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

    // Background music. Only one background track is allowed to play at a time.
    void playIntroMusic();
    void playGameplayMusic();
    void playGameOverMusic();
    void stopMusic();

    // Short one-shot effects.
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

    // Every SFX owns its own sf::Sound. The corresponding SoundBuffer
    // members above outlive these objects, as required by SFML.
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

    void loadMusic(sf::Music& music, const std::string& path,
                   const std::string& label, bool& loaded);
    bool loadSfx(sf::SoundBuffer& buffer, const std::string& path,
                 const std::string& label, std::size_t index);
    void playMusic(sf::Music& target, bool loaded,
                   const std::string& label);
};
