#include "graphics/AudioManager.hpp"

#include <iostream>

AudioManager::AudioManager()
    : clickSound(clickBuffer),
      hoverSound(hoverBuffer),
      confirmSound(confirmBuffer),
      effectPanelSound(effectPanelBuffer),
      turnStartSound(turnStartBuffer),
      turnEndSound(turnEndBuffer),
      attackSound(attackBuffer),
      schemeSound(schemeBuffer),
      moveSound(moveBuffer)
{
   
    loadMusic(introMusic, "assets/audio/intro_theme.ogg",
              "intro_theme.ogg", introMusicLoaded);
    loadMusic(gameplayMusic, "assets/audio/gameplay_theme.ogg",
              "gameplay_theme.ogg", gameplayMusicLoaded);
    loadMusic(gameOverMusic, "assets/audio/game_over_theme.ogg",
              "game_over_theme.ogg", gameOverMusicLoaded);

    loadSfx(clickBuffer, "assets/audio/click.wav", "click.wav", 0);
    loadSfx(hoverBuffer, "assets/audio/Hover.wav", "Hover.wav", 1);
    loadSfx(confirmBuffer, "assets/audio/confirm.wav", "confirm.wav", 2);
    loadSfx(effectPanelBuffer, "assets/audio/effect_panel_open.wav",
            "effect_panel_open.wav", 3);
    loadSfx(turnStartBuffer, "assets/audio/turn_start.wav", "turn_start.wav", 4);
    loadSfx(turnEndBuffer, "assets/audio/turn_end.wav", "turn_end.wav", 5);
    loadSfx(attackBuffer, "assets/audio/attack_panel_open.wav",
            "attack_panel_open.wav", 6);
    loadSfx(schemeBuffer, "assets/audio/scheme_panel_open.wav",
            "scheme_panel_open.wav", 7);
    loadSfx(moveBuffer, "assets/audio/move.wav", "move.wav", 8);

    introMusic.setLooping(true);
    gameplayMusic.setLooping(true);
    gameOverMusic.setLooping(true);

    playIntroMusic();
}

void AudioManager::loadMusic(sf::Music& music,
                             const std::string& path,
                             const std::string& label,
                             bool& loaded)
{
    loaded = music.openFromFile(path);
    if (!loaded)
    {
        std::cerr << "[Audio] Warning: could not load music: "
                  << label << ". Game continues without this track.\n";
    }
}

bool AudioManager::loadSfx(sf::SoundBuffer& buffer,
                           const std::string& path,
                           const std::string& label,
                           std::size_t index)
{
    const bool loaded = buffer.loadFromFile(path);
    if (!loaded)
    {
        std::cerr << "[Audio] Warning: could not load SFX: "
                  << label << ". Game continues without this effect.\n";
        if (index < sfxLoaded.size())
            sfxLoaded[index] = false;
        return false;
    }

    if (index < sfxLoaded.size())
        sfxLoaded[index] = true;

    return true;
}

void AudioManager::toggleMusicMute()
{
    musicMuted = !musicMuted;
    const float volume = musicMuted ? 0.f : 100.f;
    introMusic.setVolume(volume);
    gameplayMusic.setVolume(volume);
    gameOverMusic.setVolume(volume);
}

void AudioManager::stopMusic()
{
    introMusic.stop();
    gameplayMusic.stop();
    gameOverMusic.stop();
}

void AudioManager::playMusic(sf::Music& target,
                             bool loaded,
                             const std::string& label)
{
    introMusic.stop();
    gameplayMusic.stop();
    gameOverMusic.stop();

    if (!loaded)
    {
        std::cerr << "[Audio] Warning: music unavailable: "
                  << label << ". Game continues without this track.\n";
        return;
    }

    target.setLooping(true);
    target.setVolume(musicMuted ? 0.f : 100.f);
    target.play();
}

void AudioManager::playIntroMusic()
{
    playMusic(introMusic, introMusicLoaded, "intro_theme.ogg");
}

void AudioManager::playGameplayMusic()
{
    playMusic(gameplayMusic, gameplayMusicLoaded, "gameplay_theme.ogg");
}

void AudioManager::playGameOverMusic()
{
    playMusic(gameOverMusic, gameOverMusicLoaded, "game_over_theme.ogg");
}

void AudioManager::playSfx(Sfx effect)
{
    const std::size_t index = static_cast<std::size_t>(effect);
    if (index >= sfxLoaded.size() || !sfxLoaded[index])
        return;

    switch (effect)
    {
        case Sfx::Click:
            clickSound.play();
            break;
        case Sfx::Hover:
            hoverSound.play();
            break;
        case Sfx::Confirm:
            confirmSound.play();
            break;
        case Sfx::EffectPanel:
            effectPanelSound.play();
            break;
        case Sfx::TurnStart:
            turnStartSound.play();
            break;
        case Sfx::TurnEnd:
            turnEndSound.play();
            break;
        case Sfx::Attack:
            attackSound.play();
            break;
        case Sfx::Scheme:
            schemeSound.play();
            break;
        case Sfx::Move:
            moveSound.play();
            break;
    }
}