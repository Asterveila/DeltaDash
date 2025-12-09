#include "BattleTab.hpp"

ccColor3B pastelizeColor(const ccColor3B& color, float factor = 0.4f) {
    factor = std::max(0.0f, std::min(1.0f, factor));
    
    GLubyte r = static_cast<GLubyte>(color.r + (255 - color.r) * factor);
    GLubyte g = static_cast<GLubyte>(color.g + (255 - color.g) * factor);
    GLubyte b = static_cast<GLubyte>(color.b + (255 - color.b) * factor);

    ccColor3B pastelized = ccColor3B({r, g, b});
    
    return pastelized;
}

BattleTab* BattleTab::create(const std::string& character, const CharacterAttributes& attrs) {
    auto ret = new BattleTab();
    if (ret && ret->init(character, attrs)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool BattleTab::init(const std::string& character, const CharacterAttributes& attrs) {
    if (!CCNode::init()) return false;
    
    m_character = character;
    m_charAttrs = attrs;
    m_maxHP = attrs.maxHealth;
    m_currentHP = m_maxHP;
    
    setupVisuals();
    setupButtons();
    
    return true;
}

void BattleTab::setupVisuals() {
    m_tabTop = CCSprite::createWithSpriteFrameName("deltaTab_top.png"_spr);
    m_tabBottom = CCSprite::create("linesTabLoop.gif"_spr);
    m_hpOverlay = CCSprite::createWithSpriteFrameName("hpUI.png"_spr);
    m_hpBarFill = CCSprite::createWithSpriteFrameName("hpBarFiller.png"_spr);
    m_defendIcon = CCSprite::createWithSpriteFrameName("defendIconGlobal.png"_spr);
    
    m_hpOverlay->getTexture()->setAliasTexParameters();
    m_hpBarFill->getTexture()->setAliasTexParameters();
    m_tabTop->getTexture()->setAliasTexParameters();
    m_tabBottom->getTexture()->setAliasTexParameters();
    m_defendIcon->getTexture()->setAliasTexParameters();
    
    bool isPlayerChar = (m_character == "player" || m_character == "true-player");
    
    if (isPlayerChar) {
        m_nameLabel = CCSprite::createWithSpriteFrameName("nameLabel_player.png"_spr);
        m_nameLabel->getTexture()->setAliasTexParameters();
        
        auto playLayer = PlayLayer::get();
        if (playLayer && playLayer->m_player1) {
            m_charIcon = CCSprite::createWithSpriteFrame(playLayer->m_player1->m_iconSprite->displayFrame());
            m_charIcon->setPosition({13.25f, 25.75f});
            m_charIcon->setScale(0.4f);
            m_charIcon->setColor(pastelizeColor(m_charAttrs.tabElementsColor));
        }
    } else {
        std::string iconFilename = fmt::format("{}Icon_idle.png"_spr, m_character);
        m_charIcon = CCSprite::createWithSpriteFrameName(iconFilename.c_str());
        
        std::string labelFilename = fmt::format("nameLabel_{}.png"_spr, m_character);
        m_nameLabel = CCSprite::createWithSpriteFrameName(labelFilename.c_str());
        
        std::string hurtFilename = fmt::format("{}Icon_hurt.png"_spr, m_character);
        m_hurtIcon = CCSprite::createWithSpriteFrameName(hurtFilename.c_str());
        
        m_charIcon->getTexture()->setAliasTexParameters();
        m_nameLabel->getTexture()->setAliasTexParameters();
        m_hurtIcon->getTexture()->setAliasTexParameters();
        m_hurtIcon->setVisible(false);
    }
    
    m_hpLabel = CCLabelBMFont::create(fmt::format("{}", static_cast<int>(m_maxHP)).c_str(), "hpNumbers.fnt"_spr);
    m_hpLabel->setAlignment(kCCTextAlignmentRight);
    m_hpLabel->setAnchorPoint({1.f, 1.f});
    
    m_maxHpLabel = CCLabelBMFont::create(fmt::format("{}", static_cast<int>(m_maxHP)).c_str(), "hpNumbers.fnt"_spr);
    m_maxHpLabel->setAlignment(kCCTextAlignmentRight);
    m_maxHpLabel->setAnchorPoint({1.f, 1.f});
    
    if (m_character == "true-player") {
        m_hpLabel->setColor({255, 255, 0});
        m_maxHpLabel->setColor({255, 255, 0});
    }
    
    m_tabTop->setID("tab-top"_spr);
    m_tabBottom->setID("tab-bottom"_spr);
    m_charIcon->setID("character-icon"_spr);
    m_nameLabel->setID("character-name"_spr);
    m_hpBarFill->setID("hp-bar-filler"_spr);
    m_defendIcon->setID("defend-icon"_spr);
    m_hpLabel->setID("hp-label"_spr);
    m_maxHpLabel->setID("max-hp-label"_spr);
    
    m_tabBottom->setAnchorPoint({0.5f, 0.f});
    m_hpBarFill->setAnchorPoint({0.f, 0.5f});
    
    m_tabTop->setColor(m_charAttrs.tabContainerColor);
    m_tabBottom->setColor(m_charAttrs.tabContainerColor);
    m_hpBarFill->setColor(m_charAttrs.tabElementsColor);
    m_defendIcon->setColor(pastelizeColor(m_charAttrs.tabElementsColor));
    m_defendIcon->setVisible(false);
    
    setContentSize(m_tabTop->getContentSize());
    setAnchorPoint({0.5f, 0.f});
    setScale(1.3f);
    
    addChild(m_tabTop);
    addChild(m_tabBottom);
    addChild(m_charIcon);
    addChild(m_nameLabel);
    addChild(m_hpOverlay);
    addChild(m_hpBarFill);
    addChild(m_hpLabel);
    addChild(m_maxHpLabel);
    addChild(m_defendIcon);
    
    if (m_hurtIcon) {
        m_hurtIcon->setID("hurt-icon"_spr);
        addChild(m_hurtIcon);
    }
    
    auto nodeSize = getContentSize();
    
    m_tabTop->setPosition(nodeSize / 2);
    m_tabBottom->setPosition({nodeSize.width / 2.f, 0.f});
    m_nameLabel->setPosition(nodeSize / 2);
    m_hpOverlay->setPosition(nodeSize / 2);
    m_defendIcon->setPosition(nodeSize / 2);
    m_hpBarFill->setPosition({64.05f, 22.7f});
    m_hpLabel->setPosition({80.f, 31.5f});
    m_maxHpLabel->setPosition({102.5f, 31.5f});
    
    if (!isPlayerChar) {
        m_charIcon->setPosition(nodeSize / 2);
        if (m_hurtIcon) m_hurtIcon->setPosition(nodeSize / 2);
    }
    
    m_hpBarFill->setZOrder(2);
    
    setID("deltarune-ui-node"_spr);
}

void BattleTab::setupButtons() {
    m_buttonMenu = CCMenu::create();
    
    auto healSpr = CCSprite::createWithSpriteFrameName("magicBtn.png"_spr);
    healSpr->setColor({255, 127, 39});
    healSpr->setID("heal-spr"_spr);
    
    auto healBtn = CCMenuItemSpriteExtra::create(healSpr, nullptr, nullptr);
    healBtn->setID("heal-btn"_spr);
    
    auto defendSpr = CCSprite::createWithSpriteFrameName("defendBtn.png"_spr);
    defendSpr->setColor({255, 127, 39});
    defendSpr->setID("defend-spr"_spr);
    
    auto defendBtn = CCMenuItemSpriteExtra::create(defendSpr, nullptr, nullptr);
    defendBtn->setID("defend-btn"_spr);
    
    m_buttonMenu->addChild(healBtn);
    m_buttonMenu->addChild(defendBtn);
    
    m_buttonMenu->setLayout(
        RowLayout::create()
            ->setGap(2.f)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setAxisReverse(false)
            ->setCrossAxisOverflow(true)
            ->setAutoGrowAxis(5.f)
    );
    m_buttonMenu->setAnchorPoint({0.5f, 0.f});
    m_buttonMenu->setID("buttons-menu"_spr);
    m_buttonMenu->setPosition({getContentSize().width / 2.f, 0.f});
    
    addChild(m_buttonMenu);
    
    m_buttonMenu->setEnabled(false);
}

void BattleTab::takeDamage(float amount, bool showIndicator) {
    m_currentHP = m_currentHP - amount;
    updateHPBar();
    updateHPLabel();
    
    if (showIndicator && !m_isDefending && m_character != "player" && m_character != "true-player") {
        showHurtIcon();
    }
}

void BattleTab::heal(float amount) {
    m_currentHP = std::min(m_maxHP, m_currentHP + amount);
    updateHPBar();
    updateHPLabel();
}

void BattleTab::setHP(float hp) {
    m_currentHP = std::clamp(hp, 0.f, m_maxHP);
    updateHPBar(false);
    updateHPLabel();
}

void BattleTab::updateHPBar(bool animated) {
    float newScaleX = std::max(0.f, m_currentHP / m_maxHP);
    
    if (animated) {
        auto scaleAction = CCEaseInOut::create(CCScaleTo::create(0.1f, newScaleX, 1.f), 2.0f);
        m_hpBarFill->runAction(scaleAction);
    } else {
        m_hpBarFill->setScaleX(newScaleX);
    }
}

void BattleTab::updateHPLabel() {
    int displayHP = static_cast<int>(std::round(m_currentHP));
    m_hpLabel->setString(fmt::format("{}", displayHP).c_str(), true);
    
    if (m_currentHP <= 0) {
        m_hpLabel->setColor({255, 0, 0});
        m_maxHpLabel->setColor({255, 0, 0});
    } else if (m_currentHP <= m_maxHP / 4) {
        m_hpLabel->setColor({255, 255, 0});
        m_maxHpLabel->setColor({255, 255, 0});
    } else {
        m_hpLabel->setColor({255, 255, 255});
        m_maxHpLabel->setColor({255, 255, 255});
    }
    
    if (m_character == "true-player") {
        m_hpLabel->setColor({255, 255, 0});
        m_maxHpLabel->setColor({255, 255, 0});
    }
}

void BattleTab::showHurtIcon(float duration) {
    if (!m_hurtIcon || m_isDefending) return;
    
    m_charIcon->setVisible(false);
    m_hurtIcon->setVisible(true);
    
    auto resetAction = CCSequence::create(
        CCDelayTime::create(duration),
        CCCallFunc::create(this, callfunc_selector(BattleTab::resetHurtIconCallback)),
        nullptr
    );
    
    this->runAction(resetAction);
}

void BattleTab::resetHurtIconCallback() {
    if (m_charIcon) m_charIcon->setVisible(true);
    if (m_hurtIcon) m_hurtIcon->setVisible(false);
}

void BattleTab::startDefending(int hits) {
    m_isDefending = true;
    m_defendHitsLeft = hits;
    
    if (m_defendIcon) m_defendIcon->setVisible(true);
    if (m_charIcon) m_charIcon->setVisible(false);
    
    updateContainerColor( {42, 32, 42});
}

void BattleTab::endDefending() {
    m_isDefending = false;
    m_defendHitsLeft = 0;
    
    if (m_defendIcon) m_defendIcon->setVisible(false);
    if (m_charIcon) m_charIcon->setVisible(true);
}

bool BattleTab::processDefendedHit() {
    if (!m_isDefending || m_defendHitsLeft <= 0) return false;
    
    m_defendHitsLeft--;
    if (m_defendHitsLeft <= 0) {
        endDefending();
        return false;
    }
    return true;
}

void BattleTab::setActive(bool active) {
    m_isActive = active;
    
    if (m_buttonMenu) {
        m_buttonMenu->setEnabled(active);
    }
    
    if (active) {
        updateContainerColor(m_charAttrs.tabContainerColor);
    } else {
        updateContainerColor({42, 32, 42});
    }

}

void BattleTab::hideTab(bool animated) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    
    if (animated) {
        auto hideAction = CCEaseOut::create(CCMoveTo::create(0.25f, {getPositionX(), -21.5f}), 3.f);
        stopAllActions();
        runAction(hideAction);
    } else {
        setPositionY(-21.5f);
    }
    
    m_isHidden = true;
}

void BattleTab::showTab(bool animated) {
    if (animated) {
        auto showAction = CCEaseOut::create(CCMoveTo::create(0.3f, {getPositionX(), 0.f}), 2.f);
        stopAllActions();
        runAction(showAction);
    } else {
        setPositionY(0.f);
    }
    
    m_isHidden = false;
}

void BattleTab::updateContainerColor(const ccColor3B& color) {
    if (m_tabTop) m_tabTop->setColor(color);
    if (m_tabBottom) m_tabBottom->setColor(color);
}

void BattleTab::updatePlayerColors(const ccColor3B& containerColor, const ccColor3B& elementsColor) {
    m_charAttrs.tabContainerColor = containerColor;
    m_charAttrs.tabElementsColor = elementsColor;
    
    m_tabTop->setColor(containerColor);
    m_tabBottom->setColor(containerColor);
    m_hpBarFill->setColor(elementsColor);
    m_charIcon->setColor(pastelizeColor(elementsColor));
    m_defendIcon->setColor(pastelizeColor(elementsColor));
}

void BattleTab::updateCharacterIcon(CCSpriteFrame* frame) {
    if (m_charIcon) {
        m_charIcon->setDisplayFrame(frame);
    }
}

void BattleTab::setToHiddenState(bool instant) {
    if (instant) {
        setPositionY(-50.f);
    } else {
        auto hideAction = CCEaseOut::create(CCMoveTo::create(0.25f, {getPositionX(), -50.f}), 3.f);
        stopAllActions();
        runAction(hideAction);
    }
    m_isHidden = true;
    updateContainerColor({42, 32, 42});
}

void BattleTab::setToInactiveState(bool animated) {
    if (animated) {
        auto moveAction = CCEaseOut::create(CCMoveTo::create(0.3f, {getPositionX(), -21.5f}), 2.f);
        stopAllActions();
        runAction(moveAction);
    } else {
        setPositionY(-21.5f);
    }
    
    m_isHidden = false;
    m_isActive = false;
    if (m_buttonMenu) m_buttonMenu->setEnabled(false);
    updateContainerColor({42, 32, 42});
}

void BattleTab::setToActiveState(bool animated) {
    if (animated) {
        auto moveAction = CCEaseOut::create(CCMoveTo::create(0.3f, {getPositionX(), 0.f}), 2.f);
        stopAllActions();
        runAction(moveAction);
    } else {
        setPositionY(0.f);
    }
    
    m_isHidden = false;
    m_isActive = true;
    if (m_buttonMenu) m_buttonMenu->setEnabled(true);
    updateContainerColor(m_charAttrs.tabContainerColor);
}

void BattleTab::reset() {
    m_currentHP = m_maxHP;
    m_isDefending = false;
    m_defendHitsLeft = 0;
    m_isHidden = false;
    
    setHP(m_maxHP);
    endDefending();
    
    if (m_character == "true-player") {
        m_hpLabel->setColor({255, 255, 0});
        m_maxHpLabel->setColor({255, 255, 0});
    }
}