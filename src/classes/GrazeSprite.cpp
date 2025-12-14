#include "GrazeSprite.hpp"
#include <random>

float randFloat(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

GrazeSprite* GrazeSprite::create(PlayerObject* player) {
    auto ret = new GrazeSprite();
    if (ret && ret->init(player)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool GrazeSprite::init(PlayerObject* player) {
    if (!CCNode::init()) return false;
    
    m_targetPlayer = player;
    m_soulModeEnabled = Mod::get()->getSettingValue<bool>("enable-soul");
    
    if (!m_soulModeEnabled) {
        setupSimplePlayers();
        
        m_renderTexture = CCRenderTexture::create(static_cast<int>(m_containerSize), static_cast<int>(m_containerSize), kCCTexture2DPixelFormat_RGBA8888);
        if (!m_renderTexture) {
            log::error("Failed to create render texture for graze sprite");
            return false;
        }
        m_renderTexture->retain();
    }
    
    m_grazeSprite = CCSprite::create();
    m_grazeSprite->setOpacity(0);
    m_grazeSprite->setID("graze-effect-sprite"_spr);
    m_grazeSprite->setAnchorPoint({0.5f, 0.5f});
    
    m_initialized = true;
    
    return true;
}

void GrazeSprite::setupSimplePlayers() {
    auto manager = GameManager::sharedState();
    
    m_previewPlayer = SimplePlayer::create(0);
    m_previewPlayer->retain();
    
    m_cubeRider = SimplePlayer::create(0);
    m_cubeRider->retain();
    
    m_glowUnderlay = SimplePlayer::create(0);
    m_glowUnderlay->retain();
    
    if (m_previewPlayer) {
        m_previewPlayer->setColor({0, 0, 0});
        m_previewPlayer->setSecondColor({0, 0, 0});
        m_previewPlayer->setGlowOutline({255, 255, 255});
        m_previewPlayer->enableCustomGlowColor({255, 255, 255});
    }

    if (m_cubeRider) {
        m_cubeRider->setColor({0, 0, 0});
        m_cubeRider->setSecondColor({0, 0, 0});
        m_cubeRider->setGlowOutline({255, 255, 255});
        m_cubeRider->enableCustomGlowColor({255, 255, 255});
    }

    if (m_glowUnderlay) {
        m_glowUnderlay->setGlowOutline({255, 255, 255});
        m_glowUnderlay->enableCustomGlowColor({255, 255, 255});
    }
}

void GrazeSprite::updateSimplePlayersForGamemode(IconType iconType) {
    auto manager = GameManager::sharedState();
    
    switch(iconType) {
        case IconType::Cube:
            m_previewPlayer->updatePlayerFrame(manager->getPlayerFrame(), IconType::Cube);
            break;
        case IconType::Ship:
            m_previewPlayer->updatePlayerFrame(manager->getPlayerShip(), IconType::Ship);
            break;
        case IconType::Ball:
            m_previewPlayer->updatePlayerFrame(manager->getPlayerBall(), IconType::Ball);
            break;
        case IconType::Ufo:
            m_previewPlayer->updatePlayerFrame(manager->getPlayerBird(), IconType::Ufo);
            break;
        case IconType::Wave:
            m_previewPlayer->updatePlayerFrame(manager->getPlayerDart(), IconType::Wave);
            break;
        case IconType::Robot:
            m_previewPlayer->updatePlayerFrame(manager->getPlayerRobot(), IconType::Robot);
            break;
        case IconType::Spider:
            m_previewPlayer->updatePlayerFrame(manager->getPlayerSpider(), IconType::Spider);
            break;
        case IconType::Swing:
            m_previewPlayer->updatePlayerFrame(manager->getPlayerSwing(), IconType::Swing);
            break;
        case IconType::Jetpack:
            m_previewPlayer->updatePlayerFrame(manager->getPlayerJetpack(), IconType::Jetpack);
            break;
        default:
            break;
    }
    
    more_icons::updateSimplePlayer(m_previewPlayer, iconType);
    
    // rider gamemodes
    // welcome back icon preview, again...
    if (iconType == IconType::Ship || iconType == IconType::Ufo || iconType == IconType::Jetpack) {
        m_cubeRider->updatePlayerFrame(manager->getPlayerFrame(), IconType::Cube);
        more_icons::updateSimplePlayer(m_cubeRider, IconType::Cube);
        
        switch(iconType) {
            case IconType::Ship:
                m_glowUnderlay->updatePlayerFrame(manager->getPlayerShip(), IconType::Ship);
                break;
            case IconType::Ufo:
                m_glowUnderlay->updatePlayerFrame(manager->getPlayerBird(), IconType::Ufo);
                break;
            case IconType::Jetpack:
                m_glowUnderlay->updatePlayerFrame(manager->getPlayerJetpack(), IconType::Jetpack);
                break;
            default:
                break;
        }
        more_icons::updateSimplePlayer(m_glowUnderlay, iconType);
    }
    
    auto hideDetail = [&](SimplePlayer* p) {
        if (!p) return;
        if (p->m_detailSprite) p->m_detailSprite->setVisible(false);
    };

    auto setBlackColors = [&](SimplePlayer* p) {
        if (!p) return;
        p->setColor({0, 0, 0});
        p->setSecondColor({0, 0, 0});
    };

    // basic modes
    if (iconType == IconType::Cube || iconType == IconType::Ball || iconType == IconType::Wave || iconType == IconType::Swing) {
        hideDetail(m_previewPlayer);
        setBlackColors(m_previewPlayer);
        if (m_previewPlayer) {
            m_previewPlayer->enableCustomGlowColor({255, 255, 255});
            if (m_previewPlayer->m_outlineSprite) m_previewPlayer->m_outlineSprite->setVisible(true);
        }
        if (m_cubeRider) m_cubeRider->setVisible(false);
        if (m_glowUnderlay) m_glowUnderlay->setVisible(false);

        return;
    }

    // rider modes
    if (iconType == IconType::Ship || iconType == IconType::Ufo || iconType == IconType::Jetpack) {
        hideDetail(m_previewPlayer);
        hideDetail(m_cubeRider);
        hideDetail(m_glowUnderlay);

        setBlackColors(m_previewPlayer);
        setBlackColors(m_cubeRider);

        if (m_previewPlayer && m_previewPlayer->m_outlineSprite) m_previewPlayer->m_outlineSprite->setVisible(false);

        if (m_glowUnderlay) {
            if (m_glowUnderlay->m_firstLayer) m_glowUnderlay->m_firstLayer->setOpacity(0);
            if (m_glowUnderlay->m_secondLayer) m_glowUnderlay->m_secondLayer->setVisible(false);
            if (m_glowUnderlay->m_detailSprite) m_glowUnderlay->m_detailSprite->setVisible(false);
            if (m_glowUnderlay->m_outlineSprite) m_glowUnderlay->m_outlineSprite->setVisible(true);
            m_glowUnderlay->setGlowOutline({255,255,255});
            m_glowUnderlay->enableCustomGlowColor({255,255,255});
            m_glowUnderlay->setVisible(true);
        }

        if (m_cubeRider) {
            m_cubeRider->enableCustomGlowColor({255,255,255});
            if (m_cubeRider->m_outlineSprite) m_cubeRider->m_outlineSprite->setVisible(true);
            m_cubeRider->setVisible(true);
        }

        if (m_previewPlayer) m_previewPlayer->setVisible(true);

        return;
    }

    // these fucking modes
    // if anyone can help me make these replicate the PlayerObject's animations please do
    if (iconType == IconType::Robot) {
        setBlackColors(m_previewPlayer);
        if (m_previewPlayer) {
            m_previewPlayer->enableCustomGlowColor({255,255,255});
            if (m_previewPlayer->m_outlineSprite) m_previewPlayer->m_outlineSprite->setVisible(true);
            if (m_previewPlayer->m_robotSprite) {
                m_previewPlayer->m_robotSprite->setVisible(true);
                if (m_previewPlayer->m_robotSprite->m_extraSprite) {
                    m_previewPlayer->m_robotSprite->m_extraSprite->setVisible(false);
                }
            }
        }
        if (m_cubeRider) m_cubeRider->setVisible(false);
        if (m_glowUnderlay) m_glowUnderlay->setVisible(false);
        return;
    }

    if (iconType == IconType::Spider) {
        setBlackColors(m_previewPlayer);
        if (m_previewPlayer) {
            m_previewPlayer->enableCustomGlowColor({255,255,255});
            if (m_previewPlayer->m_outlineSprite) m_previewPlayer->m_outlineSprite->setVisible(true);
            if (m_previewPlayer->m_spiderSprite) {
                m_previewPlayer->m_spiderSprite->setVisible(true);
                if (m_previewPlayer->m_spiderSprite->m_extraSprite) {
                    m_previewPlayer->m_spiderSprite->m_extraSprite->setVisible(false);
                }
            }
        }
        if (m_cubeRider) m_cubeRider->setVisible(false);
        if (m_glowUnderlay) m_glowUnderlay->setVisible(false);
        return;
    }
}

void GrazeSprite::renderGrazeSprite() {
    if (!m_targetPlayer) return;
    
    if (m_soulModeEnabled) {
        auto soulGrazeFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("soulGraze.png"_spr);
        //auto soulGrazeFrame = CCSprite::createWithSpriteFrameName("soulGraze.png"_spr)->displayFrame();
        if (soulGrazeFrame) {
            m_grazeSprite->setDisplayFrame(soulGrazeFrame);
            m_grazeSprite->setScale(1.0f);
        } else {
            log::error("failed to load soul graze spr");
        }
        return;
    }
    
    if (!m_renderTexture) return;
    
    IconType currentMode = IconType::Cube;
    if (m_targetPlayer->m_isShip) currentMode = IconType::Ship;
    else if (m_targetPlayer->m_isBall) currentMode = IconType::Ball;
    else if (m_targetPlayer->m_isBird) currentMode = IconType::Ufo;
    else if (m_targetPlayer->m_isDart) currentMode = IconType::Wave;
    else if (m_targetPlayer->m_isRobot) currentMode = IconType::Robot;
    else if (m_targetPlayer->m_isSpider) currentMode = IconType::Spider;
    else if (m_targetPlayer->m_isSwing) currentMode = IconType::Swing;
    
    updateSimplePlayersForGamemode(currentMode);
    
    auto iconContainer = CCNode::create();
    iconContainer->setContentSize({m_containerSize, m_containerSize});
    iconContainer->setAnchorPoint({0.f, 0.f});
    
    float midX = m_containerSize / 2.f;
    float midY = m_containerSize / 2.f;
    
    bool isRiderMode = (currentMode == IconType::Ship || 
                        currentMode == IconType::Ufo || 
                        currentMode == IconType::Jetpack);
    
    if (isRiderMode) {
        if (currentMode == IconType::Ship) {
            m_glowUnderlay->setPosition({midX, midY - 5.f});
        } else {
            m_glowUnderlay->setPosition({midX, midY});
        }
        
        m_glowUnderlay->m_firstLayer->setOpacity(0);
        m_glowUnderlay->m_secondLayer->setVisible(false);
        if (m_glowUnderlay->m_detailSprite) m_glowUnderlay->m_detailSprite->setVisible(false);
        if (m_glowUnderlay->m_birdDome) m_glowUnderlay->m_birdDome->setVisible(false);
        
        if (currentMode == IconType::Ship) {
            m_previewPlayer->setPosition({midX, midY - 5.f});
        } else {
            m_previewPlayer->setPosition({midX, midY});
        }
        
        if (m_previewPlayer->m_outlineSprite) m_previewPlayer->m_outlineSprite->setVisible(false);
        
        if (currentMode == IconType::Ship) {
            m_cubeRider->setPosition({midX, midY + 5.f});
            m_cubeRider->setScale(0.55f);
        } else if (currentMode == IconType::Ufo) {
            m_cubeRider->setPosition({midX, midY + 6.f});
            m_cubeRider->setScale(0.55f);
        } else if (currentMode == IconType::Jetpack) {
            m_cubeRider->setPosition({midX + 6.f, midY + 4.f});
            m_cubeRider->setScale(0.6f);
        }
        
        iconContainer->addChild(m_glowUnderlay, 0);
        iconContainer->addChild(m_cubeRider, 1);
        iconContainer->addChild(m_previewPlayer, 2);
        
    } else {
        m_previewPlayer->setPosition({midX, midY});
        iconContainer->addChild(m_previewPlayer);
    }
    
    m_renderTexture->beginWithClear(0, 0, 0, 0);
    iconContainer->visit();
    m_renderTexture->end();
    
    auto renderedSprite = m_renderTexture->getSprite();
    if (renderedSprite) {
        m_grazeSprite->setDisplayFrame(renderedSprite->displayFrame());
        m_grazeSprite->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
        
        m_grazeSprite->setFlipY(true);
        
        float grazeScale = Mod::get()->getSettingValue<double>("graze-scale");
        m_grazeSprite->setScale(grazeScale);
    }
    
    iconContainer->removeAllChildren();
}

void GrazeSprite::updateForCurrentGamemode() {
    if (!m_initialized || !m_targetPlayer) return;
    renderGrazeSprite();
}

void GrazeSprite::showGrazeEffect(float duration) {
    if (!m_grazeSprite) return;
    
    updateForCurrentGamemode();
    
    float randomDelay = randFloat(0.05f, 0.1f);
    auto fadeAnimation = CCSequence::create(
        CCFadeIn::create(0.f),
        CCDelayTime::create(randomDelay),
        CCEaseOut::create(CCFadeOut::create(duration), 2.f),
        nullptr
    );
    
    m_grazeSprite->stopAllActions();
    m_grazeSprite->runAction(fadeAnimation);
}

void GrazeSprite::setTargetPlayer(PlayerObject* player) {
    m_targetPlayer = player;
}