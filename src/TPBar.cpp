#include "TPBar.hpp"

TPBar* TPBar::create() {
    auto ret = new TPBar();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool TPBar::init() {
    if (!CCNode::init()) return false;
    
    // create elements
    m_barTop = CCSprite::createWithSpriteFrameName("tpBar_top.png"_spr);
    m_barBg = CCSprite::createWithSpriteFrameName("tpBar_bg.png"_spr);
    m_barFill = CCSprite::createWithSpriteFrameName("tpBar_filler.png"_spr);
    m_tpTextSprite = CCSprite::createWithSpriteFrameName("tpBar_tp.png"_spr);
    m_fillerLine = CCSprite::createWithSpriteFrameName("tpBar_line.png"_spr);
    
    m_percentLabel = CCLabelBMFont::create("0", "deltarune.fnt"_spr);
    m_percentSymbol = CCLabelBMFont::create("%", "deltarune.fnt"_spr);
    
    // ids
    m_barTop->setID("tp-bar-top"_spr);
    m_barBg->setID("tp-bar-bg"_spr);
    m_barFill->setID("tp-bar-fill"_spr);
    m_tpTextSprite->setID("tp-bar-text"_spr);
    m_fillerLine->setID("tp-bar-filler-line"_spr);
    m_percentLabel->setID("tp-bar-percent-label"_spr);
    m_percentSymbol->setID("tp-bar-percent-simbol"_spr);
    
    // misc
    m_barTop->getTexture()->setAliasTexParameters();
    m_tpTextSprite->getTexture()->setAliasTexParameters();
    
    m_barTop->setZOrder(4);
    m_fillerLine->setZOrder(3);
    m_barFill->setZOrder(2);
    m_barBg->setZOrder(1);
    
    setContentSize(m_barTop->getContentSize());
    setAnchorPoint({0.5f, 0.5f});
    
    // adding
    addChild(m_barBg);
    addChild(m_barFill);
    addChild(m_barTop);
    addChild(m_tpTextSprite);
    addChild(m_fillerLine);
    addChild(m_percentLabel);
    addChild(m_percentSymbol);
    
    auto nodeSize = getContentSize();
    
    // positions and stuff
    m_barTop->setPosition(nodeSize / 2);
    m_barBg->setPosition(nodeSize / 2);
    
    m_barFill->setAnchorPoint({0.5f, 0.f});
    m_barFill->setPosition({nodeSize.width / 2.f, 0.f});
    m_barFill->setScaleY(0.f);
    
    m_tpTextSprite->setPosition({(nodeSize.width / 2.f) - 19.f, nodeSize.height - nodeSize.height / 4.f});
    
    m_percentLabel->setPosition({m_tpTextSprite->getPositionX() - 9.f, m_tpTextSprite->getPositionY() - 18.f});
    m_percentLabel->setScale(0.7f);
    m_percentLabel->setAlignment(kCCTextAlignmentLeft);
    m_percentLabel->setExtraKerning(-5);
    m_percentLabel->setAnchorPoint({0.f, 0.5f});
    
    m_percentSymbol->setPosition({m_tpTextSprite->getPositionX(), m_percentLabel->getPositionY() - 13.f});
    m_percentSymbol->setScale(0.7f);
    m_percentSymbol->setAnchorPoint({0.7f, 0.5f});
    
    m_fillerLine->setAnchorPoint({0.5f, 1.f});
    m_fillerLine->setPosition({nodeSize.width / 2.f, 1.f});
    
    setScale(1.5f);
    setID("tp-bar-container"_spr);
    
    return true;
}

void TPBar::setTP(float percentage, bool animated) {
    m_currentTP = std::clamp(percentage, 0.f, 100.f);
    
    float newScaleY = m_currentTP / 100.f;
    
    if (animated) {
        auto scaleAction = CCEaseInOut::create(CCScaleTo::create(0.1f, 1.f, newScaleY), 2.0f);
        m_barFill->runAction(scaleAction);
    } else {
        m_barFill->setScaleY(newScaleY);
    }
    
    updateVisuals();
}

void TPBar::addTP(float amount, bool animated) {
    setTP(m_currentTP + amount, animated);
}

void TPBar::updateVisuals() {
    auto fillSize = m_barFill->getContentSize();
    float lineY = fillSize.height * (m_currentTP / 100.f);
    m_fillerLine->setPosition({m_barFill->getPositionX(), lineY});
    
    int tpAmount = static_cast<int>(m_currentTP);
    if (tpAmount >= 100) {
        m_percentLabel->setString("MAX", true);
        m_percentLabel->setScale(0.6f);
    } else {
        m_percentLabel->setString(fmt::format("{}", tpAmount).c_str(), true);
        m_percentLabel->setScale(0.7f);
    }
}

void TPBar::reset() {
    setTP(0.f, false);
}

void TPBar::animateEntry(float delay) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    setPosition({-10.f, winSize.height / 2.f});
    
    auto moveInBar = CCSequence::create(
        CCDelayTime::create(delay),
        CCEaseOut::create(CCMoveTo::create(0.3f, {45.f, winSize.height / 2.f}), 3.f),
        nullptr
    );
    
    runAction(moveInBar);
}

void TPBar::animateExit(float delay) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    
    auto moveOutBar = CCSequence::create(
        CCDelayTime::create(delay),
        CCEaseIn::create(CCMoveTo::create(0.25f, {-10.f, winSize.height / 2.f}), 3.f),
        nullptr
    );
    
    runAction(moveOutBar);
}