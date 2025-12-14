#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <random>
#include "helper.hpp"
#include "classes/TPBar.hpp"
#include "classes/GrazeSprite.hpp"

using namespace geode::prelude;

float sensitivity = 14.0f;
bool tpCooldown = false;
bool isPlayer2 = false;
static const std::set<int> sawblades = {88, 89, 98, 183, 184, 185, 186, 187, 188, 397, 398, 399, 678, 679, 680, 740, 741, 742, 1619, 1620, 1701, 1702, 1703, 1705, 1706, 1707, 1708, 1709, 1710, 1734, 1735, 1736};

bool enableDeltaruneMod = Mod::get()->getSettingValue<bool>("enable-deltarune");

void SarahsTweaks::cooldownTP(float dt) {
    tpCooldown = false;
}

$on_mod(Loaded) {
    listenForSettingChanges("enable-deltarune", [](bool value) {
        enableDeltaruneMod = value;
    });
}

float getRandomFloat(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
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
        
        showTPGrazeEffect(isPlayer2 ? m_player2 : m_player1);
    }

	void showTPGrazeEffect(PlayerObject* targetPlayer) {
        if (!targetPlayer) return;
        
        auto grazeSprite = static_cast<GrazeSprite*>(targetPlayer->getUserObject("graze-sprite-handler"_spr));
        bool soulMode = Mod::get()->getSettingValue<bool>("enable-soul");
        if (!grazeSprite) {
            grazeSprite = GrazeSprite::create(targetPlayer);
            if (grazeSprite) {
                targetPlayer->setUserObject("graze-sprite-handler"_spr, grazeSprite);
                if (!soulMode) {
                    targetPlayer->m_mainLayer->addChild(grazeSprite->getGrazeSprite(), -6);
                } else {
                    targetPlayer->addChild(grazeSprite->getGrazeSprite());
                }
                
            } else {
                log::error("Failed to create GrazeSprite");
                return;
            }
        }
        
        grazeSprite->showGrazeEffect(0.3f);
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
        bool soulMode = Mod::get()->getSettingValue<bool>("enable-soul");
        
        auto grazeSprite = GrazeSprite::create(this);
        if (grazeSprite) {
            this->setUserObject("graze-sprite-handler"_spr, grazeSprite);
            if (!soulMode) {
                m_mainLayer->addChild(grazeSprite->getGrazeSprite(), -6);
            } else {
                this->addChild(grazeSprite->getGrazeSprite());
            }
            
        }

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
