#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <random>
#include "helper.hpp"
#include "TPBar.hpp"

using namespace geode::prelude;

float sensitivity = 14.0f;
bool tpCooldown = false;
bool isPlayer2 = false;
static const std::set<int> sawblades = {88, 89, 98, 183, 184, 185, 186, 187, 188, 397, 398, 399, 678, 679, 680, 740, 741, 742, 1619, 1620, 1701, 1702, 1703, 1705, 1706, 1707, 1708, 1709, 1710, 1734, 1735, 1736};

bool enableDeltaruneMod = Mod::get()->getSettingValue<bool>("enable-deltarune");

float getRandomFloat(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

void SarahsTweaks::cooldownTP(float dt) {
    tpCooldown = false;
}

$on_mod(Loaded) {
    listenForSettingChanges("enable-deltarune", [](bool value) {
        enableDeltaruneMod = value;
    });
}

class $modify(TPBaseLayer, GJBaseGameLayer) {

    void doTheWhateverThing() {
        auto playLayer = PlayLayer::get();
        if (!playLayer) return;
        
        tpCooldown = true;
        this->scheduleOnce(schedule_selector(SarahsTweaks::cooldownTP), 0.125f);
        
        FMODAudioEngine::sharedEngine()->playEffect("snd_graze.ogg"_spr);
        
        if (!m_uiLayer) return;
        
        auto tpBar = static_cast<TPBar*>(m_uiLayer->getChildByID("tp-bar-container"_spr));
        if (!tpBar) return;
        
        float tpGain = getRandomFloat(1.f, 9.f);
        tpBar->addTP(tpGain);
        
        showTPGrazeEffect(isPlayer2 ? m_player2 : m_player1, 0.35f);
    }

	void showTPGrazeEffect(PlayerObject* targetPlayer, float extraScale) {
        if (!targetPlayer) return;
        
        auto tpSprite = static_cast<CCSprite*>(targetPlayer->m_mainLayer->getChildByID("tp-effect-sprite"_spr));
        auto riderTpSprite = static_cast<CCSprite*>(targetPlayer->m_mainLayer->getChildByID("rider-tp-effect-sprite"_spr));
        
        if (Mod::get()->getSettingValue<bool>("enable-soul")) {
            auto soul = targetPlayer->getChildByID("soul-sprite"_spr);
            if (soul) {
                tpSprite = static_cast<CCSprite*>(soul->getChildByID("soul-graze-sprite"_spr));
            }
        }
        
        if (!tpSprite || !riderTpSprite) return;
        
        if (!Mod::get()->getSettingValue<bool>("enable-soul")) {
            if (targetPlayer->m_isBird || targetPlayer->m_isShip) {
                if (targetPlayer->m_vehicleGlow) {
                    tpSprite->setDisplayFrame(targetPlayer->m_vehicleGlow->displayFrame());
                    tpSprite->setPosition(targetPlayer->m_vehicleGlow->getPosition());
                    tpSprite->setScale(targetPlayer->m_vehicleGlow->getScale() + extraScale);
                }
                if (targetPlayer->m_iconGlow) {
                    riderTpSprite->setDisplayFrame(targetPlayer->m_iconGlow->displayFrame());
                    riderTpSprite->setPosition(targetPlayer->m_iconGlow->getPosition());
                    riderTpSprite->setScale(targetPlayer->m_iconGlow->getScale() + extraScale);
                }
            } else {
                if (targetPlayer->m_iconGlow) {
                    tpSprite->setDisplayFrame(targetPlayer->m_iconGlow->displayFrame());
                    tpSprite->setPosition(targetPlayer->m_iconGlow->getPosition());
                    tpSprite->setScale(targetPlayer->m_iconGlow->getScale() + extraScale);
                }
            }
        }
        
        float randomDelay = getRandomFloat(0.05f, 0.1f);
        auto fadeAnimation = CCSequence::create(
            CCFadeIn::create(0.f),
            CCDelayTime::create(randomDelay),
            CCEaseOut::create(CCFadeOut::create(0.3f), 2.f),
            nullptr
        );
        
        tpSprite->stopAllActions();
        tpSprite->runAction(fadeAnimation);
        
        if (targetPlayer->m_isBird || targetPlayer->m_isShip) {
            auto riderFadeAnimation = static_cast<CCSequence*>(fadeAnimation->copy());
            riderTpSprite->stopAllActions();
            riderTpSprite->runAction(riderFadeAnimation);
        } else {
            riderTpSprite->setOpacity(0);
        }
    }

    void collisionCheckObjects(PlayerObject* player, gd::vector<GameObject*>* objs, int objectCount, float dt) {
        GJBaseGameLayer::collisionCheckObjects(player, objs, objectCount, dt);

        auto playLayer = PlayLayer::get();
        if (!enableDeltaruneMod || !playLayer || tpCooldown) return;

        bool battleModeActive = Mod::get()->getSavedValue<bool>("battle-mode-active", false);
        if (!battleModeActive) return;
        
        for (int i = 0; i < objectCount; i++) {
            GameObject* obj = objs->at(i);
            if (!obj || obj->m_isGroupDisabled || obj == m_anticheatSpike) continue;
            if (obj->m_objectType != GameObjectType::Hazard && 
                obj->m_objectType != GameObjectType::AnimatedHazard && 
                obj->m_objectType != GameObjectType::Solid) continue;
            if (obj->m_objectType == GameObjectType::Solid && !player->m_isDart) continue;
            
            const bool isSawblade = std::binary_search(sawblades.begin(), sawblades.end(), obj->m_objectID);
            const float multiplier = isSawblade ? -2.5f : 2.f;
            CCRect sensitivityRect = CCRect(
                obj->getObjectRect().origin - CCPoint(sensitivity, sensitivity), 
                obj->getObjectRect().size + CCPoint(sensitivity * multiplier, sensitivity * multiplier)
            );
            
            if (player->getObjectRect().intersectsRect(sensitivityRect)) {
                doTheWhateverThing();
                isPlayer2 = player->m_isSecondPlayer;
            }
        }
    }
};

class $modify(TPPlayLayer, PlayLayer) {
    
    void resetLevel() {
        PlayLayer::resetLevel();
        if (!enableDeltaruneMod) return;
        tpCooldown = false;
    }
};

class $modify(TPPlayerObject, PlayerObject) {
    bool init(int player, int ship, GJBaseGameLayer* gameLayer, CCLayer* layer, bool playLayer) {
        if (!PlayerObject::init(player, ship, gameLayer, layer, playLayer)) return false;

        if (!enableDeltaruneMod) return true;
    
        auto tpSprite = CCSprite::create();
        tpSprite->setID("tp-effect-sprite"_spr);
        tpSprite->setOpacity(0);
        tpSprite->setZOrder(-6);
        
        auto riderTPSprite = CCSprite::create();
        riderTPSprite->setID("rider-tp-effect-sprite"_spr);
        riderTPSprite->setOpacity(0);
        riderTPSprite->setZOrder(-5);
        
        m_mainLayer->addChild(tpSprite);
        m_mainLayer->addChild(riderTPSprite);

        tpCooldown = false;
        
        return true;
    }

    void switchedToMode(GameObjectType p0) {
        PlayerObject::switchedToMode(p0);

        if (!enableDeltaruneMod) return;
        
        auto playLayer = PlayLayer::get();
        if (!playLayer) return;
        
        auto uiLayer = UILayer::get();
        if (!uiLayer) return;
        
        auto tpBar = static_cast<TPBar*>(uiLayer->getChildByID("tp-bar-container"_spr));
        if (!tpBar) return;
        
        float currentTP = tpBar->getTP();
        
        if (currentTP <= 0.f) return;
        
        float minDrain = currentTP * 0.1f;
        float maxDrain = currentTP * 0.7f;
        float drainAmount = getRandomFloat(minDrain, maxDrain);
        
        tpBar->addTP(-drainAmount);
    }
};
