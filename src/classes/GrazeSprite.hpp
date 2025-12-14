#pragma once
#include <Geode/Geode.hpp>
#include <Geode/binding/SimplePlayer.hpp>
#include <hiimjustin000.more_icons/include/MoreIconsV2.hpp>

using namespace geode::prelude;

class GrazeSprite : public CCNode {
protected:
    SimplePlayer* m_previewPlayer = nullptr;
    SimplePlayer* m_cubeRider = nullptr;
    SimplePlayer* m_glowUnderlay = nullptr;
    CCSprite* m_grazeSprite = nullptr;
    CCRenderTexture* m_renderTexture = nullptr;
    
    PlayerObject* m_targetPlayer = nullptr;
    bool m_initialized = false;

    float m_containerSize = 80.f;
    
    bool init(PlayerObject* player);
    void setupSimplePlayers();
    void updateSimplePlayersForGamemode(IconType iconType);
    void renderGrazeSprite();
    
public:
    static GrazeSprite* create(PlayerObject* player);
    
    void updateForCurrentGamemode();
    void showGrazeEffect(float duration = 0.3f);
    void setTargetPlayer(PlayerObject* player);
    
    CCSprite* getGrazeSprite() const { return m_grazeSprite; }
};