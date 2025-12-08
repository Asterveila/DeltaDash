#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <geode.custom-keybinds/include/Keybinds.hpp>
#include <random>
#include "BattleTab.hpp"
#include "TPBar.hpp"

using namespace geode::prelude;
#ifndef GEODE_IS_IOS
using namespace keybinds;
#endif

bool enableDeltarune = Mod::get()->getSettingValue<bool>("enable-deltarune");

bool hpCooldown = false;

ccColor3B getPastelCol(const ccColor3B& color, float factor = 0.4f) {
    factor = std::max(0.0f, std::min(1.0f, factor));
    
    GLubyte r = static_cast<GLubyte>(color.r + (255 - color.r) * factor);
    GLubyte g = static_cast<GLubyte>(color.g + (255 - color.g) * factor);
    GLubyte b = static_cast<GLubyte>(color.b + (255 - color.b) * factor);

    ccColor3B pastelized = ccColor3B({r, g, b});
    
    return pastelized;
}

CCSpriteFrame* getGamemodeFrame(PlayerObject* player) {
    CCSpriteFrame* ret;

    if (player->m_isShip || player->m_isBird) {
        ret = player->m_vehicleSprite->displayFrame();
    } else if (player->m_isSpider) {
        ret = player->m_spiderSprite->m_headSprite->displayFrame();
    } else if (player->m_isRobot) {
        ret = player->m_robotSprite->m_headSprite->displayFrame();
    } else {
        ret = player->m_iconSprite->displayFrame();
    }

    return ret;
}

CharacterAttributes getCharAttributes(int stars, int isDemon, std::string character) {
    CharacterAttributes attrs;
    auto gm = GameManager::sharedState();
    auto statsManager = GameStatsManager::sharedState();
    int playerStars = statsManager->getStat("6");
    bool demonQualifies = false;

    attrs.magicLv = std::clamp(playerStars / 1000, 1, 25);

    if (stars == 12022004) {
        attrs.minDamage = 20.f;
        attrs.maxDamage = 40.f;
        log::warn("we're using placeholder values!! you're either on a main level or something went pretty wrong.");
    } else if (stars < 10) {
        switch (stars) {
            case 6:
                attrs.minDamage = 10.f;
                attrs.maxDamage = 20.f;
                break;
            case 7:
                attrs.minDamage = 15.f;
                attrs.maxDamage = 30.f;
                break;
            case 8:
                attrs.minDamage = 15.f;
                attrs.maxDamage = 35.f;
                break;
            case 9:
                attrs.minDamage = 20.f;
                attrs.maxDamage = 40.f;
                break;
            default:
                attrs.minDamage = 25.f;
                attrs.maxDamage = 45.f;
                break;
        }
    } else {
        switch (isDemon) {
            case 3: // easy demon
                attrs.minDamage = 30.f;
                attrs.maxDamage = 60.f;
                break;
            case 4: // medium demon
                attrs.minDamage = 35.f;
                attrs.maxDamage = 65.f;
                demonQualifies = true;
                break;
            case 0: // hard demon
                attrs.minDamage = 40.f;
                attrs.maxDamage = 70.f;
                demonQualifies = true;
                break;
            case 5: // insane demon
                attrs.minDamage = 45.f;
                attrs.maxDamage = 75.f;
                demonQualifies = true;
                break;
            case 6: // extreme demon
                attrs.minDamage = 50.f;
                attrs.maxDamage = 80.f;
                demonQualifies = true;
                break;
            default:
                attrs.minDamage = 35.f;
                attrs.maxDamage = 65.f;
                demonQualifies = true;
                break;
        }
    }

    if (demonQualifies) attrs.bonusHealth = 40.f;

    if (character == "kris") {
        attrs.tabContainerColor = ccColor3B({0, 255, 255});
        attrs.tabElementsColor = ccColor3B({0, 255, 255});
        attrs.maxHealth = 160.f + attrs.bonusHealth;
    } else if (character == "susie") {
        attrs.tabContainerColor = ccColor3B({255, 0, 255});
        attrs.tabElementsColor = ccColor3B({255, 0, 255});
        attrs.maxHealth = 190.f + attrs.bonusHealth;
    } else if (character == "ralsei") {
        attrs.tabContainerColor = ccColor3B({0, 255, 0});
        attrs.tabElementsColor = ccColor3B({0, 255, 0});
        attrs.maxHealth = 140.f + attrs.bonusHealth;
    } else if (character == "noelle") {
        attrs.tabContainerColor = ccColor3B({255, 255, 0});
        attrs.tabElementsColor = ccColor3B({255, 255, 0});
        attrs.maxHealth = 90.f + attrs.bonusHealth;
    } else if (character == "player") {
        auto playerColor = gm->colorForIdx(gm->getPlayerColor());
        attrs.tabContainerColor = playerColor
        attrs.tabElementsColor = getPastelCol(playerColor, 0.5f);
        int playerLv = Mod::get()->getSavedValue<int>("player-lv", 0);
        int bonus = 20 * playerLv;
        attrs.maxHealth = 50.f + bonus;
    } else if (character == "true-player") {
        auto playerColor = gm->colorForIdx(gm->getPlayerColor());
        attrs.tabContainerColor = playerColor;
        attrs.tabElementsColor = playerColor;
        attrs.maxHealth = 1.f;
    }

    return attrs;
}

float getRandomHPFloat(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

$on_mod(Loaded){
    #ifndef GEODE_IS_IOS
    BindManager::get()->registerBindable({
        "heal-prayer-key"_spr,
        "Heal Prayer keybind",
        "",
        { Keybind::create(KEY_C, Modifier::None) },
        Mod::get()->getName()
    });

    BindManager::get()->registerBindable({
        "defend-key"_spr,
        "Defend keybind",
        "",
        { Keybind::create(KEY_V, Modifier::None) },
        Mod::get()->getName()
    });

    BindManager::get()->registerBindable({
        "activate-midgame"_spr,
        "In-Game Activate keybind",
        "",
        { Keybind::create(KEY_Q, Modifier::None) },
        Mod::get()->getName()
    });
    #endif

    listenForSettingChanges("enable-deltarune", [](bool value) {
        enableDeltarune = value;
    });
}

class $modify(DeltaPlayLayer, PlayLayer) {
    struct Fields {
        CCNode* partyContainer = nullptr;
        std::vector<BattleTab*> partyMembers;
        int activePartyMemberIndex = 0;
        
        TPBar* tpBar = nullptr;
        
        CCSprite* downSpr = nullptr;
        CCSprite* healSpr = nullptr;
        CCLabelBMFont* damageLabel = nullptr;
        CCLabelBMFont* healingLabel = nullptr;
        
        std::vector<CCSprite*> downSprites;
        std::vector<CCLabelBMFont*> damageLabels;
        
        GJGameLevel* currentLevel = nullptr;
        int levelStars = 12022004;
        int levelDemon = 3;
        bool hasDied = false;
        
        bool isDemonLevel = false;
        int alivePartyMembers = 0;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
    
        if (!enableDeltarune) return true;
        
        auto fields = m_fields.self();
        fields->currentLevel = level;
        
        return true;
    }

	void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        if (!enableDeltarune) return;
        
        auto fields = m_fields.self();
        if (!m_uiLayer) return;

        Mod::get()->setSavedValue<bool>("battle-mode-active", false);
        
        if (fields->currentLevel) {
            fields->levelStars = fields->currentLevel->m_stars;
            fields->levelDemon = fields->currentLevel->m_demonDifficulty;
        }
        
        fields->isDemonLevel = (fields->levelStars >= 10);
        
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // this is better here actually
        fields->tpBar = TPBar::create();
        if (fields->tpBar) {
            m_uiLayer->addChild(fields->tpBar);
            fields->tpBar->setZOrder(20);
            log::info("created TP bar");
        } else {
            log::error("couldn't create tp bar");
        }
        
        // party container
        fields->partyContainer = CCNode::create();
        fields->partyContainer->setID("party-container"_spr);
        fields->partyContainer->setContentSize({400.f, 100.f});
        fields->partyContainer->setAnchorPoint({0.5f, 0.f});
        fields->partyContainer->setScale(Mod::get()->getSettingValue<double>("ui-scale"));
        fields->partyContainer->setPosition({winSize.width / 2.f, 0.f});
        
        m_uiLayer->addChild(fields->partyContainer);
        fields->partyContainer->setZOrder(50);
        
        // party members
        std::string partyMember1 = Mod::get()->getSettingValue<std::string>("party-char1");
        std::string partyMember2 = Mod::get()->getSettingValue<std::string>("party-char2");
        std::string partyMember3 = Mod::get()->getSettingValue<std::string>("party-char3");
        std::vector<std::string> partyChars;
        
        if (partyMember1 == "true-player") {
            partyChars.push_back(partyMember1);
        } else {
            if (!partyMember1.empty() && partyMember1 != "none") {
                partyChars.push_back(partyMember1);
            }
            if (!partyMember2.empty() && partyMember2 != "none") {
                partyChars.push_back(partyMember2);
            }
            if (!partyMember3.empty() && partyMember3 != "none") {
                partyChars.push_back(partyMember3);
            }
        }

        log::info("using {} party members", partyChars.size());
        
        // make all tabs
        for (const auto& character : partyChars) {
            CharacterAttributes charAttrs = getCharAttributes(fields->levelStars, fields->levelDemon, character);
            
            auto battleTab = BattleTab::create(character, charAttrs);
            if (!battleTab) {
                log::error("couldn't create {}'s battle tab", character);
                continue;
            }
            
            fields->partyContainer->addChild(battleTab);
            
            fields->partyMembers.push_back(battleTab);
            log::info("created tab for {}", character);
        }
        
        fields->alivePartyMembers = fields->partyMembers.size();

        fields->partyContainer->setLayout(
            RowLayout::create()
                ->setGap(0.f)
                ->setAxisAlignment(AxisAlignment::Center)
                ->setAxisReverse(false)
                ->setCrossAxisOverflow(true)
                ->setAutoScale(false)
                ->setGrowCrossAxis(false)
                ->setAutoGrowAxis(true)
        );
        fields->partyContainer->updateLayout();
        
        for (auto* member : fields->partyMembers) if (member) member->setToHiddenState(true);

        setupIndicators();
        setupButtonCallbacks();
        #ifndef GEODE_IS_IOS
        setupKeybinds();
        #endif
        
        std::string activationMode = Mod::get()->getSettingValue<std::string>("activation-mode");
        
        if (activationMode == "on-start") {
            this->scheduleOnce(schedule_selector(DeltaPlayLayer::delayStartSound), 0.25f);
            this->scheduleOnce(schedule_selector(DeltaPlayLayer::animateTabsToInactive), 0.25f);
            
            if (fields->tpBar) fields->tpBar->animateEntry(0.25f);
        } else {
            if (fields->tpBar) fields->tpBar->setPosition({-10.f, winSize.height / 2.f});
        }
    }

    void startGame() {
        PlayLayer::startGame();
        
        if (!enableDeltarune) return;
        
        auto fields = m_fields.self();
        
        std::string activationMode = Mod::get()->getSettingValue<std::string>("activation-mode");
        
        if (activationMode == "on-start") {
            Mod::get()->setSavedValue<bool>("battle-mode-active", true);
            
            if (!fields->partyMembers.empty()) {
                fields->activePartyMemberIndex = 0;
                auto firstMember = fields->partyMembers[0];
                
                if (firstMember) {
                    firstMember->setToActiveState(true);
                } else {
                    log::error("first party member is null?????");
                }
            } else {
                log::error("no party members exist, something's probably wrong...");
            }
        }
    }

    void resetLevel() {
        PlayLayer::resetLevel();

        if (!enableDeltarune) return;
        
        auto fields = m_fields.self();
        
        hpCooldown = false;
        bool battleModeActive = Mod::get()->getSavedValue<bool>("battle-mode-active", false);
        std::string activationMode = Mod::get()->getSettingValue<std::string>("activation-mode");
        
        // reset party
        fields->alivePartyMembers = fields->partyMembers.size();
        for (auto* member : fields->partyMembers) {
            if (member) {
                member->reset();
            
                if (activationMode == "midgame-keybind" && !battleModeActive) {
                    member->setToHiddenState(false);
                } else {
                    member->setToInactiveState(false);
                }
            }
        }
        
        fields->activePartyMemberIndex = 0;
        if (!fields->partyMembers.empty() && fields->partyMembers[0]) {
            if (battleModeActive) fields->partyMembers[0]->setToActiveState(false);
        }

        if (activationMode == "midgame-keybind" && battleModeActive) {
            if (fields->tpBar) fields->tpBar->reset();
            deactivateBattleMode();
        }
        
        // reset other visuals
        if (fields->downSpr) {
            fields->downSpr->stopAllActions();
            fields->downSpr->setOpacity(0);
            fields->downSpr->setScale(1.f);
            fields->downSpr->setPosition({0.f, 0.f});
        }
        
        if (fields->damageLabel) {
            fields->damageLabel->stopAllActions();
            fields->damageLabel->setOpacity(0);
            fields->damageLabel->setScale(1.f);
            fields->damageLabel->setPosition({0.f, 0.f});
        }
        
        if (fields->healingLabel) {
            fields->healingLabel->stopAllActions();
            fields->healingLabel->setOpacity(0);
            fields->healingLabel->setScale(1.f);
            fields->healingLabel->setPosition({0.f, 0.f});
        }
        
        for (auto* label : fields->damageLabels) {
            if (label) {
                label->stopAllActions();
                label->setOpacity(0);
                label->setScale(1.f);
                label->setPosition({0.f, 0.f});
            }
        }
        
        for (auto* sprite : fields->downSprites) {
            if (sprite) {
                sprite->stopAllActions();
                sprite->setOpacity(0);
                sprite->setScale(1.f);
                sprite->setPosition({0.f, 0.f});
            }
        }
        
        if (m_player1 && fields->healSpr && (battleModeActive || activationMode == "on-start")) {
            auto screenPos = getPlayerScreenPosition(m_player1);
            fields->healSpr->setPosition({screenPos.x + 15.f, screenPos.y});
            fields->healSpr->setOpacity(255);
            
            auto animation = createHealSpriteAnimation(0.3f);
            fields->healSpr->runAction(animation);
            
            FMODAudioEngine::sharedEngine()->playEffect("snd_heal_c.ogg"_spr);
        }
    }

    void postUpdate(float p0) {
        PlayLayer::postUpdate(p0);
        if (!enableDeltarune) return;
        
        auto fields = m_fields.self();
        
        for (auto* member : fields->partyMembers) {
            if (!member) continue;
            
            std::string character = member->getCharacter();
            bool validChars = character == "player" || character == "true-player";
            if (!validChars) continue;
            
            auto frame = getGamemodeFrame(m_player1);
            member->updateCharacterIcon(frame);
            member->updatePlayerColors(m_player1->m_playerColor1, m_player1->m_playerColor1);
        }
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) override {
        if (!enableDeltarune) return PlayLayer::destroyPlayer(player, obj);
        
        auto fields = m_fields.self();
        
        bool battleModeActive = Mod::get()->getSavedValue<bool>("battle-mode-active");
        if (!battleModeActive) return PlayLayer::destroyPlayer(player, obj);
        
        if (fields->partyMembers.empty()) return PlayLayer::destroyPlayer(player, obj);
        
        if (obj == m_anticheatSpike) {
            return PlayLayer::destroyPlayer(player, obj);
        }
        
        int aliveBefore = 0;
        for (auto* member : fields->partyMembers) {
            if (member && !member->isDead()) {
                aliveBefore++;
            }
        }
        
        if (aliveBefore <= 0) {
            return PlayLayer::destroyPlayer(player, obj);
            if (Mod::get()->getSettingValue<std::string>("activation-mode") == "midgame-keybind") {
                deactivateBattleMode();
            }
        }
        
        if (!player->m_isDead && !hpCooldown) {
            hpCooldown = true;
            this->scheduleOnce(schedule_selector(DeltaPlayLayer::cooldownHPFunction), 1.6f);
            
            auto flashAction = createInvincibilityFlash();
            player->runAction(flashAction);
            
            FMODAudioEngine::sharedEngine()->playEffect("snd_hurt.ogg"_spr);
            
            if (fields->isDemonLevel) {
                handleDemonDamage(player, obj);
            } else {
                handleNonDemonDamage(player, obj);
            }
        }
    }

    void activateBattleMode(float dt = 0.f) {
        auto fields = m_fields.self();
        
        bool battleModeActive = Mod::get()->getSavedValue<bool>("battle-mode-active", false);
        if (battleModeActive) return;
        
        Mod::get()->setSavedValue<bool>("battle-mode-active", true);
        
        FMODAudioEngine::sharedEngine()->playEffect("snd_weaponpull_fast.ogg"_spr);
        
        for (auto* member : fields->partyMembers) {
            if (member) {
                member->setToInactiveState(true);
            }
        }
        
        if (fields->tpBar) {
            if (dt > 0.f) fields->tpBar->reset();
            fields->tpBar->animateEntry(0.3f);
        }
        
        this->scheduleOnce(schedule_selector(DeltaPlayLayer::activateFirstMemberScheduled), 0.3f);
    }

    void deactivateBattleMode() {
        auto fields = m_fields.self();
        
        bool battleModeActive = Mod::get()->getSavedValue<bool>("battle-mode-active", false);
        if (!battleModeActive) return;
        
        Mod::get()->setSavedValue("battle-mode-active", false);
        
        for (auto* member : fields->partyMembers) {
            if (member) member->setToHiddenState(false);
        }
        
        if (fields->tpBar) fields->tpBar->animateExit(0.f);
    }

    void activateFirstMemberScheduled(float dt) {
        auto fields = m_fields.self();
        if (!fields->partyMembers.empty() && fields->partyMembers[0]) fields->partyMembers[0]->setToActiveState(true);
    }

    void activateBattleModeKeybind() {
        auto fields = m_fields.self();
        
        bool battleModeActive = Mod::get()->getSavedValue<bool>("battle-mode-active", false);
        
        if (battleModeActive) return;
        activateBattleMode(0.001f);
    }

    void animateTabsToInactive(float dt) {
        auto fields = m_fields.self();
        
        for (auto* member : fields->partyMembers) {
            if (member) member->setToInactiveState(true);
        }
    }

    void handleNonDemonDamage(PlayerObject* player, GameObject* obj) {
        auto fields = m_fields.self();
        
        std::vector<BattleTab*> aliveMembers;
        for (auto* member : fields->partyMembers) {
            if (member && !member->isDead()) aliveMembers.push_back(member);
        }
        
        if (aliveMembers.empty()) {
            std::string activationMode = Mod::get()->getSettingValue<std::string>("activation-mode");
            if (activationMode == "midgame-keybind") deactivateBattleMode();
            
            PlayLayer::destroyPlayer(player, obj);
            return;
        }
        
        int randomIndex = rand() % aliveMembers.size();
        auto* targetMember = aliveMembers[randomIndex];
        
        auto charAttrs = targetMember->getAttributes();
        float damageAmount = getRandomHPFloat(charAttrs.minDamage, charAttrs.maxDamage);
        
        if (fields->partyMembers.size() == 2) {
            damageAmount *= 1.25f;
        } else if (fields->partyMembers.size() == 3) {
            damageAmount *= 1.5f;
        }
        
        bool wasDefending = targetMember->isDefending();
        
        if (targetMember->isDefending()) {
            damageAmount *= 0.4f;
            bool stillDefending = targetMember->processDefendedHit();
            checkAllDefenseComplete();
        }
        
        if (!targetMember->isDead()) targetMember->takeDamage(damageAmount);
        
        int actualAlive = 0;
        for (auto* member : fields->partyMembers) {
            if (member && !member->isDead()) {
                actualAlive++;
            }
        }
        fields->alivePartyMembers = actualAlive;
        
        if (fields->alivePartyMembers <= 0) {
            showDownSprite(player);
    
            if (Mod::get()->getSettingValue<std::string>("activation-mode") == "midgame-keybind") deactivateBattleMode();
            
            PlayLayer::destroyPlayer(player, obj);
            return;
        } else if (targetMember->isDead()) {
            showDownSprite(player);
            
            if (fields->activePartyMemberIndex < fields->partyMembers.size() &&
                fields->partyMembers[fields->activePartyMemberIndex] == targetMember) {
                tryActivateNextMember();
            }
        } else {
            showDamageIndicator(player, damageAmount);
        }
    }

    void handleDemonDamage(PlayerObject* player, GameObject* obj) {
        auto fields = m_fields.self();
        
        std::vector<float> damages;
        float totalDamage = 0.f;
        
        for (auto* member : fields->partyMembers) {
            if (!member) continue;
            
            auto charAttrs = member->getAttributes();
            float damage = getRandomHPFloat(charAttrs.minDamage, charAttrs.maxDamage);
            
            if (fields->partyMembers.size() == 2) {
                damage *= 1.5f;
            } else if (fields->partyMembers.size() == 3) {
                damage *= 1.75f;
            }
            
            if (member->isDefending()) {
                damage *= 0.5f;
                bool stillDefending = member->processDefendedHit();
            }
            
            damages.push_back(damage);
            totalDamage += damage;
            
            if (!member->isDead()) member->takeDamage(damage);
        }
        
        checkAllDefenseComplete();
        
        if (fields->partyMembers.size() > 1) {
            showMultipleDamageIndicators(player, damages);
        } else {
            showDamageIndicator(player, totalDamage);
        }
        
        int actualAlive = 0;
        for (auto* member : fields->partyMembers) {
            if (member && !member->isDead()) actualAlive++;
        }
        fields->alivePartyMembers = actualAlive;
        
        if (fields->alivePartyMembers <= 0) {
            if (Mod::get()->getSettingValue<std::string>("activation-mode") == "midgame-keybind") deactivateBattleMode();
            PlayLayer::destroyPlayer(player, obj);

            return;
        }
        
        bool activeIsDead = false;
        if (fields->activePartyMemberIndex < fields->partyMembers.size()) {
            auto* activeMember = fields->partyMembers[fields->activePartyMemberIndex];
            if (activeMember && activeMember->isDead()) {
                activeIsDead = true;
            }
        }
        
        if (activeIsDead) tryActivateNextMember();
    }

    bool shouldWaitForDefendedHits() {
        auto fields = m_fields.self();
        
        for (auto* member : fields->partyMembers) {
            if (member && member->isDefending() && member->getDefendHitsLeft() > 0) {
                return true;
            }
        }
        return false;
    }

    bool isPartyWaiting() {
        auto fields = m_fields.self();

        if (fields->activePartyMemberIndex < fields->partyMembers.size()) {
            auto* current = fields->partyMembers[fields->activePartyMemberIndex];

            if (current && !current->isDead() && !current->isActive()) return true;
        }
        return false;
    }

    void checkAllDefenseComplete() {
        auto fields = m_fields.self();
        
        bool anyStillDefending = false;
        
        for (auto* member : fields->partyMembers) {
            if (!member) continue;
            
            if (member->isDefending()) anyStillDefending = true;
        }
        
        if (!anyStillDefending && isPartyWaiting()) this->scheduleOnce(schedule_selector(DeltaPlayLayer::activateMemberAfterDefenseComplete), 0.35f);
    }

    void activateMemberAfterDefenseComplete(float dt) {
        auto fields = m_fields.self();
        
        if (fields->activePartyMemberIndex < fields->partyMembers.size()) {
            auto* memberToActivate = fields->partyMembers[fields->activePartyMemberIndex];
            if (memberToActivate && !memberToActivate->isDead()) {
                memberToActivate->setToActiveState(true);
            } else {
                tryActivateNextMember();
            }
        }
    }

    BattleTab* getNextAvailableMemberImmediate(int startIndex) {
        auto fields = m_fields.self();
        
        if (fields->partyMembers.empty()) return nullptr;
        
        int attempts = 0;
        int nextIndex = startIndex;
        
        do {
            nextIndex = (nextIndex + 1) % fields->partyMembers.size();
            attempts++;
            
            if (attempts > fields->partyMembers.size()) return nullptr;
            
            auto* member = fields->partyMembers[nextIndex];
            if (member && !member->isDead()) return member;

        } while (true);
    }

    BattleTab* getNextAvailableMember(int startIndex) {
        auto fields = m_fields.self();
        
        if (fields->partyMembers.empty()) return nullptr;
        
        int attempts = 0;
        int nextIndex = startIndex;
        
        do {
            nextIndex = (nextIndex + 1) % fields->partyMembers.size();
            attempts++;
            
            if (attempts > fields->partyMembers.size()) return nullptr;
            
            auto* member = fields->partyMembers[nextIndex];
            if (member && !member->isDead() && !member->isDefending()) return member;

        } while (true);
    }

    void tryActivateNextMember() {
        auto fields = m_fields.self();
        
        if (shouldWaitForDefendedHits()) {
            
            if (fields->activePartyMemberIndex < fields->partyMembers.size()) {
                auto* current = fields->partyMembers[fields->activePartyMemberIndex];
                if (current && current->isDefending()) {
                    if (current->getButtonMenu()) current->getButtonMenu()->setEnabled(false);
                    current->setActive(false);
                }
            }
            return;
        }

        if (fields->activePartyMemberIndex < fields->partyMembers.size()) {
            auto* current = fields->partyMembers[fields->activePartyMemberIndex];
            if (current) current->setToInactiveState(true);
        }
        
        auto* nextMember = getNextAvailableMember(fields->activePartyMemberIndex);
        
        if (!nextMember) return;
        
        for (size_t i = 0; i < fields->partyMembers.size(); i++) {
            if (fields->partyMembers[i] == nextMember) {
                fields->activePartyMemberIndex = i;
                break;
            }
        }
        
        nextMember->setToActiveState(true);
    }

    void cycleToNextMemberImmediately() {
        auto fields = m_fields.self();
        
        if (fields->activePartyMemberIndex < fields->partyMembers.size()) {
            auto* current = fields->partyMembers[fields->activePartyMemberIndex];
            if (current) {
                if (current->isDefending()) {
                    if (current->getButtonMenu()) current->getButtonMenu()->setEnabled(false);
                    current->setActive(false);
                } else {
                    current->setToInactiveState(true);
                }
            }
        }
        
        bool anyDefending = false;
        for (auto* member : fields->partyMembers) {
            if (member && member->isDefending() && member->getDefendHitsLeft() > 0) {
                anyDefending = true;
            }
        }
        
        auto* nextMember = getNextAvailableMemberImmediate(fields->activePartyMemberIndex);
        
        if (!nextMember) return;
        
        int nextIndex = -1;
        for (size_t i = 0; i < fields->partyMembers.size(); i++) {
            if (fields->partyMembers[i] == nextMember) {
                nextIndex = i;
                break;
            }
        }
        
        if (nextIndex == -1) return;
        
        bool hasLoopedAround = (nextIndex <= fields->activePartyMemberIndex);
        
        if (hasLoopedAround && anyDefending) {
            
            fields->activePartyMemberIndex = nextIndex;
            
            for (auto* member : fields->partyMembers) {
                if (member) {
                    if (member->getButtonMenu()) member->getButtonMenu()->setEnabled(false);
                    member->setActive(false);
                    
                }
            }
            return;
        }
        
        fields->activePartyMemberIndex = nextIndex;
        nextMember->setToActiveState(true);
    }

    void cooldownHPFunction(float dt) {
        hpCooldown = false;
    }

    CCAction* createInvincibilityFlash() {
        auto flash = CCRepeat::create(
            CCSequence::create(
                CCFadeTo::create(0.1f, 40),
                CCFadeTo::create(0.1f, 255),
                nullptr
            ),
            8
        );
        
        return flash;
    }

    void buttonTintEffect(CCSprite* sprite) {
        auto tintToYellow = CCTintTo::create(0.f, 255, 255, 0);
        auto tintToNormal = CCTintTo::create(0.3f, 255, 127, 39);
        auto tintSequence = CCSequence::create(tintToYellow, tintToNormal, nullptr);
        sprite->runAction(tintSequence);
    }

    void delayStartSound(float dt){
        FMODAudioEngine::sharedEngine()->playEffect("snd_weaponpull_fast.ogg"_spr);
    }

    void playerDefend(CCObject* sender) {
        auto fields = m_fields.self();
        
        if (fields->activePartyMemberIndex >= fields->partyMembers.size()) return;
        
        auto* activeMember = fields->partyMembers[fields->activePartyMemberIndex];
        if (!activeMember) return;
        
        if (!activeMember->isActive() || !activeMember->getButtonMenu() || 
            !activeMember->getButtonMenu()->isEnabled()) {
            FMODAudioEngine::sharedEngine()->playEffect("snd_cantselect_1.ogg"_spr);
            return;
        }
        
        if (activeMember->isDefending()) {
            FMODAudioEngine::sharedEngine()->playEffect("snd_cantselect_1.ogg"_spr);
            return;
        }
        
        auto menuItem = static_cast<CCMenuItemSpriteExtra*>(sender);
        auto sprite = static_cast<CCSprite*>(menuItem->getNormalImage());
        buttonTintEffect(sprite);
        
        FMODAudioEngine::sharedEngine()->playEffect("snd_select.ogg"_spr);
        this->scheduleOnce(schedule_selector(DeltaPlayLayer::delayedDefendAction), 0.3f);
    }

    void playerDefendKeybindVer() {
        auto fields = m_fields.self();
        
        if (fields->activePartyMemberIndex >= fields->partyMembers.size()) return;
        
        auto* activeMember = fields->partyMembers[fields->activePartyMemberIndex];
        if (!activeMember) return;
        
        auto menu = activeMember->getButtonMenu();
        if (!activeMember->isActive() || !menu || !menu->isEnabled()) {
            FMODAudioEngine::sharedEngine()->playEffect("snd_cantselect_1.ogg"_spr);
            return;
        }
        
        if (activeMember->isDefending()) {
            FMODAudioEngine::sharedEngine()->playEffect("snd_cantselect_1.ogg"_spr);
            return;
        }
        
        auto defendBtn = menu->getChildByID("defend-btn"_spr);
        if (defendBtn) playerDefend(defendBtn);
    }

    void delayedDefendAction(float dt) {
        auto fields = m_fields.self();
        
        if (fields->activePartyMemberIndex >= fields->partyMembers.size()) return;
        
        auto* activeMember = fields->partyMembers[fields->activePartyMemberIndex];
        if (!activeMember) return;
        
        if (!fields->tpBar) return;
        
        activeMember->startDefending(3);
        activeMember->hideTab(true);
        
        fields->tpBar->addTP(16.f);

        cycleToNextMemberImmediately();
    }

    void healPrayer(CCObject* sender) {
        auto fields = m_fields.self();
        
        if (fields->activePartyMemberIndex >= fields->partyMembers.size()) return;
        
        auto* activeMember = fields->partyMembers[fields->activePartyMemberIndex];
        if (!activeMember) return;
        
        if (!fields->tpBar) return;
        
        if (!activeMember->isActive() || !activeMember->getButtonMenu() || 
            !activeMember->getButtonMenu()->isEnabled()) {
            FMODAudioEngine::sharedEngine()->playEffect("snd_cantselect_1.ogg"_spr);
            return;
        }
        
        if (activeMember->isDefending()) {
            FMODAudioEngine::sharedEngine()->playEffect("snd_cantselect_1.ogg"_spr);
            return;
        }
        
        if (fields->tpBar->getTP() < 32.f) {
            FMODAudioEngine::sharedEngine()->playEffect("snd_cantselect_1.ogg"_spr);
            return;
        }
        
        auto menuItem = static_cast<CCMenuItemSpriteExtra*>(sender);
        auto sprite = static_cast<CCSprite*>(menuItem->getNormalImage());
        buttonTintEffect(sprite);
        
        FMODAudioEngine::sharedEngine()->playEffect("snd_select.ogg"_spr);
        this->scheduleOnce(schedule_selector(DeltaPlayLayer::delayedHealAction), 0.3f);
    }

    void healPrayerKeybindVer() {
        auto fields = m_fields.self();
        
        if (fields->activePartyMemberIndex >= fields->partyMembers.size()) return;
        
        auto* activeMember = fields->partyMembers[fields->activePartyMemberIndex];
        if (!activeMember) return;
        
        auto menu = activeMember->getButtonMenu();
        if (!menu) return;
        
        if (!activeMember->isActive() || !menu || !menu->isEnabled()) {
            FMODAudioEngine::sharedEngine()->playEffect("snd_cantselect_1.ogg"_spr);
            return;
        }
        
        if (activeMember->isDefending()) {
            FMODAudioEngine::sharedEngine()->playEffect("snd_cantselect_1.ogg"_spr);
            return;
        }
        
        if (!fields->tpBar) return;
        
        if (fields->tpBar->getTP() < 32.f) {
            FMODAudioEngine::sharedEngine()->playEffect("snd_cantselect_1.ogg"_spr);
            return;
        }
        
        auto healBtn = menu->getChildByID("heal-btn"_spr);
        if (healBtn) healPrayer(healBtn);
    }

    void delayedHealAction(float dt) {
        auto fields = m_fields.self();
        
        if (fields->activePartyMemberIndex >= fields->partyMembers.size()) return;
        
        auto* activeMember = fields->partyMembers[fields->activePartyMemberIndex];
        if (!activeMember) return;
        
        if (!fields->tpBar) return;
        
        auto charAttrs = activeMember->getAttributes();
        
        float magicBonus = 1.f;
        if (m_player1->m_isSpider || m_player1->m_isDart) {
            magicBonus = 6.f;
        } else if (m_player1->m_isBird || m_player1->m_isRobot) {
            magicBonus = 5.f;
        } else if (m_player1->m_isSwing) {
            magicBonus = 7.f;
        } else if (m_player1->m_isShip) {
            magicBonus = 3.f;
        } else if (m_player1->m_isBall) {
            magicBonus = 4.f;
        }
        
        float currentTP = fields->tpBar->getTP();
        fields->tpBar->addTP(-32.f);
        
        int fullMagicLv = charAttrs.magicLv + static_cast<int>(magicBonus);
        float healAmount = fullMagicLv * 5.f;
        
        float oldHP = activeMember->getCurrentHP();
        activeMember->heal(healAmount);
        float newHP = activeMember->getCurrentHP();
        
        bool reachedMax = (newHP >= activeMember->getMaxHP());
        
        if (reachedMax) {
            showHealSprite(m_player1);
        } else {
            showHealingIndicator(m_player1, healAmount);
        }
        
        FMODAudioEngine::sharedEngine()->playEffect("snd_heal_c.ogg"_spr);

        cycleToNextMemberImmediately();
    }

    void setupIndicators() {
        auto fields = m_fields.self();
        
        fields->downSpr = CCSprite::createWithSpriteFrameName("downMsg.png"_spr);
        fields->healSpr = CCSprite::createWithSpriteFrameName("revivedText.png"_spr);
        fields->damageLabel = CCLabelBMFont::create("0", "damageFont.fnt"_spr);
        fields->healingLabel = CCLabelBMFont::create("0", "damageFont.fnt"_spr);
        
        fields->downSpr->getTexture()->setAliasTexParameters();
        fields->healSpr->getTexture()->setAliasTexParameters();
        
        fields->downSpr->setColor({255, 0, 0});
        fields->healSpr->setColor({0, 255, 0});
        fields->healingLabel->setColor({0, 255, 0});
        
        fields->downSpr->setOpacity(0);
        fields->healSpr->setOpacity(0);
        fields->damageLabel->setOpacity(0);
        fields->healingLabel->setOpacity(0);
        
        fields->downSpr->setZOrder(1000);
        fields->healSpr->setZOrder(1000);
        
        fields->downSpr->setID("down-sprite"_spr);
        fields->healSpr->setID("heal-sprite"_spr);
        fields->damageLabel->setID("damage-label"_spr);
        fields->healingLabel->setID("healing-label"_spr);
        
        this->addChild(fields->downSpr);
        this->addChild(fields->healSpr);
        this->addChild(fields->damageLabel);
        this->addChild(fields->healingLabel);
        
        for (int i = 0; i < 3; i++) {
            auto damageLabel = CCLabelBMFont::create("0", "damageFont.fnt"_spr);
            damageLabel->setOpacity(0);
            damageLabel->setID(fmt::format("damage-label-{}", i).c_str());
            this->addChild(damageLabel);
            fields->damageLabels.push_back(damageLabel);
            
            auto downSprite = CCSprite::createWithSpriteFrameName("downMsg.png"_spr);
            downSprite->getTexture()->setAliasTexParameters();
            downSprite->setColor({255, 0, 0});
            downSprite->setOpacity(0);
            downSprite->setZOrder(1000);
            downSprite->setID(fmt::format("down-sprite-{}", i).c_str());
            this->addChild(downSprite);
            fields->downSprites.push_back(downSprite);
        }
    }
    
    void setupButtonCallbacks() {
        auto fields = m_fields.self();
        
        for (auto* member : fields->partyMembers) {
            if (!member) continue;
            
            auto menu = member->getButtonMenu();
            if (!menu) continue;
            
            auto healBtn = static_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("heal-btn"_spr));
            auto defendBtn = static_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("defend-btn"_spr));
            
            if (healBtn) {
                healBtn->setTarget(this, menu_selector(DeltaPlayLayer::healPrayer));
            }
            
            if (defendBtn) {
                defendBtn->setTarget(this, menu_selector(DeltaPlayLayer::playerDefend));
            }
        }
    }
    
    #ifndef GEODE_IS_IOS
    void setupKeybinds() {
        this->addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                healPrayerKeybindVer();
            }
            return ListenerResult::Propagate;
        }, "heal-prayer-key"_spr);
        
        this->addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                playerDefendKeybindVer();
            }
            return ListenerResult::Propagate;
        }, "defend-key"_spr);

        this->addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                activateBattleModeKeybind();
            }
            return ListenerResult::Propagate;
        }, "activate-midgame"_spr);
    }
    #endif
    
    void showDamageIndicator(PlayerObject* player, float damage) {
        auto fields = m_fields.self();
        if (!fields->damageLabel) return;
        
        auto screenPos = getPlayerScreenPosition(player);
        
        fields->damageLabel->setPosition(screenPos);
        fields->damageLabel->setString(fmt::format("{}", static_cast<int>(damage)).c_str(), true);
        fields->damageLabel->setOpacity(255);
        
        auto animation = createFloatingTextAnimation();
        fields->damageLabel->stopAllActions();
        fields->damageLabel->runAction(animation);
    }

    void showMultipleDamageIndicators(PlayerObject* player, const std::vector<float>& damages) {
        auto fields = m_fields.self();
        auto screenPos = getPlayerScreenPosition(player);
        
        float yOffset = 20.f;
        for (size_t i = 0; i < damages.size() && i < fields->damageLabels.size(); i++) {
            auto* label = fields->damageLabels[i];
            auto* member = fields->partyMembers[i];
            
            if (!label || !member) continue;
            
            if (member->isDead()) {
                if (i < fields->downSprites.size()) {
                    auto* downSprite = fields->downSprites[i];
                    downSprite->setPosition({screenPos.x, screenPos.y + yOffset});
                    downSprite->setOpacity(255);
                    downSprite->setScale(1.f);
                    
                    auto animation = createDownSpriteAnimation();
                    downSprite->stopAllActions();
                    downSprite->runAction(animation);
                }
            } else {
                label->setPosition({screenPos.x, screenPos.y + yOffset});
                label->setString(fmt::format("{}", static_cast<int>(damages[i])).c_str(), true);
                label->setOpacity(255);
                
                auto animation = createFloatingTextAnimation();
                label->stopAllActions();
                label->runAction(animation);
            }
            
            yOffset -= 15.f;
        }
    }
    
    void showHealingIndicator(PlayerObject* player, float healAmount) {
        auto fields = m_fields.self();
        if (!fields->healingLabel) return;
        
        auto screenPos = getPlayerScreenPosition(player);
        
        fields->healingLabel->setPosition(screenPos);
        fields->healingLabel->setString(fmt::format("{}", static_cast<int>(healAmount)).c_str(), true);
        fields->healingLabel->setOpacity(255);
        
        auto animation = createFloatingTextAnimation();
        fields->healingLabel->stopAllActions();
        fields->healingLabel->runAction(animation);
    }
    
    void showDownSprite(PlayerObject* player) {
        auto fields = m_fields.self();
        if (!fields->downSpr) return;
        
        auto screenPos = getPlayerScreenPosition(player);
        fields->downSpr->setPosition(screenPos);
        fields->downSpr->setOpacity(255);
        
        auto animation = createDownSpriteAnimation();
        fields->downSpr->stopAllActions();
        fields->downSpr->runAction(animation);
    }
    
    void showHealSprite(PlayerObject* player) {
        auto fields = m_fields.self();
        if (!fields->healSpr) return;
        
        auto screenPos = getPlayerScreenPosition(player);
        fields->healSpr->setPosition({screenPos.x + 15.f, screenPos.y});
        fields->healSpr->setOpacity(255);
        
        auto animation = createHealSpriteAnimation();
        fields->healSpr->stopAllActions();
        fields->healSpr->runAction(animation);
    }
    
    CCPoint getPlayerScreenPosition(PlayerObject* player) {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        
        if (Mod::get()->getSettingValue<bool>("always-imitate-deltarune-pos")) return {winSize.width / 2.f - 60.f, winSize.height / 2.f};
        
        auto mainNode = this->getChildByID("main-node");
        if (!mainNode || !m_objectLayer) return {winSize.width / 2.f - 60.f, winSize.height / 2.f};
        
        auto playerWorldPos = player->getPosition();
        auto worldPos = m_objectLayer->convertToWorldSpace(playerWorldPos);
        return this->convertToNodeSpace(worldPos);
    }
    
    CCAction* createFloatingTextAnimation() {
        auto downAnim = CCSequence::create(
            CCEaseOut::create(CCMoveBy::create(0.15f, {9.f, 20.f}), 2.f),
            CCEaseBounceOut::create(CCMoveBy::create(0.4f, {0.f, -18.f})),
            CCDelayTime::create(0.25f),
            CCMoveBy::create(0.3f, {0.f, 50.f}),
            nullptr
        );
        auto fadeAnim = CCSequence::create(
            CCDelayTime::create(0.8f),
            CCFadeOut::create(0.3f),
            nullptr
        );
        auto stretchAnim = CCSequence::create(
            CCDelayTime::create(0.8f),
            CCScaleTo::create(0.3f, 1.f, 2.5f),
            nullptr
        );
        auto resetAnim = CCSequence::create(
            CCDelayTime::create(1.1f),
            CCFadeOut::create(0.f),
            CCScaleTo::create(0.f, 1.f, 1.f),
            CCMoveTo::create(0.f, {0.f, 0.f}),
            nullptr
        );
        
        return CCSpawn::create(downAnim, fadeAnim, stretchAnim, resetAnim, nullptr);
    }
    
    CCAction* createDownSpriteAnimation() {
        auto moveAnim = CCSequence::create(
            CCEaseOut::create(CCMoveBy::create(0.15f, {9.f, 20.f}), 2.f),
            CCEaseBounceOut::create(CCMoveBy::create(0.4f, {0.f, -18.f})),
            CCDelayTime::create(0.25f),
            CCMoveBy::create(0.3f, {0.f, 50.f}),
            nullptr
        );
        auto fadeAnim = CCSequence::create(
            CCDelayTime::create(0.8f),
            CCFadeOut::create(0.3f),
            nullptr
        );
        auto stretchAnim = CCSequence::create(
            CCDelayTime::create(0.8f),
            CCScaleTo::create(0.3f, 1.f, 2.5f),
            nullptr
        );
        auto resetAnim = CCSequence::create(
            CCDelayTime::create(1.2f),
            CCScaleTo::create(0.f, 1.f, 1.f),
            CCMoveTo::create(0.f, {0.f, 0.f}),
            nullptr
        );
        
        return CCSpawn::create(moveAnim, fadeAnim, stretchAnim, resetAnim, nullptr);
    }
    
    CCAction* createHealSpriteAnimation(float extraDelay = 0.f) {
        auto moveAnim = CCSequence::create(
            CCEaseOut::create(CCMoveBy::create(0.15f, {0.f, 20.f}), 2.f),
            CCEaseBounceOut::create(CCMoveBy::create(0.4f, {0.f, -18.f})),
            CCDelayTime::create(0.25f + extraDelay),
            CCMoveBy::create(0.3f, {0.f, 50.f}),
            nullptr
        );
        auto fadeAnim = CCSequence::create(
            CCDelayTime::create(0.8f + extraDelay),
            CCFadeOut::create(0.3f),
            nullptr
        );
        auto stretchAnim = CCSequence::create(
            CCDelayTime::create(0.8f + extraDelay),
            CCScaleTo::create(0.3f, 1.f, 2.5f),
            nullptr
        );
        auto resetAnim = CCSequence::create(
            CCDelayTime::create(1.1f + extraDelay),
            CCFadeOut::create(0.f),
            CCScaleTo::create(0.f, 1.f, 1.f),
            CCMoveTo::create(0.f, {0.f, 0.f}),
            nullptr
        );
        
        return CCSpawn::create(moveAnim, fadeAnim, stretchAnim, resetAnim, nullptr);
    }

};

class $modify(DeltaEndLevelLayer, EndLevelLayer) {
    struct Fields {
        CCLabelBMFont* strongerLabel = nullptr;
        bool gotStronger = false;
    };

    void customSetup() {
        EndLevelLayer::customSetup();

        if (!enableDeltarune) return;

        auto accManager = GJAccountManager::get();
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto x = winSize.width / 2.f;
        auto y = winSize.height / 2.f;
        auto fields = m_fields.self();
        auto mainLayer = this->getChildByID("main-layer");

        int starsProgress = Mod::get()->getSavedValue<int>("stars-progress", 0);

        Mod::get()->setSavedValue<int>("stars-progress", starsProgress + m_stars);

        int newStars = Mod::get()->getSavedValue<int>("stars-progress", 0);
        int starsLeft = 100 - newStars;

        if (starsLeft <= 0) fields->gotStronger = true;

        std::string gotStronger = fmt::format("* {} got stronger.", accManager->m_username);
        std::string notYetStronger = fmt::format("* {} LEFT.", starsLeft);
        std::string strongerLabelText = (fields->gotStronger) ? gotStronger : notYetStronger;

        fields->strongerLabel = CCLabelBMFont::create(strongerLabelText.c_str(), "deltarune.fnt"_spr);
        fields->strongerLabel->setOpacity(0);
        fields->strongerLabel->setScale(0.55f);
        fields->strongerLabel->setPosition({x, y + 55.f});

        mainLayer->addChild(fields->strongerLabel);
    }

    void lvlUpStuff(float dt) {
        auto fields = m_fields.self();
        int currentLevel = Mod::get()->getSavedValue<int>("player-lv", 0);

        auto showLabel = CCEaseIn::create(CCFadeIn::create(0.4f), 2.f);

        if (fields->gotStronger) {
            Mod::get()->setSavedValue<int>("stars-progress", 0);
            Mod::get()->setSavedValue<int>("player-lv", currentLevel + 1);

            auto tintToRed = CCEaseIn::create(CCTintTo::create(0.45f, {255}, {0}, {0}), 2.f);
            fields->strongerLabel->runAction(tintToRed);

            FMODAudioEngine::sharedEngine()->playEffect("gettingStronger.ogg"_spr);
        }

        fields->strongerLabel->runAction(showLabel);
        
    }

    void showLayer(bool p0) {
        EndLevelLayer::showLayer(p0);
        if (!enableDeltarune) return;

        this->scheduleOnce(schedule_selector(DeltaEndLevelLayer::lvlUpStuff), 0.4f);
    }
};