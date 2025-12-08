#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

struct CharacterAttributes {
    float minDamage;
    float maxDamage;
    float maxHealth;
    float bonusHealth = 0.f;
    int magicLv = 1;
    ccColor3B tabContainerColor;
    ccColor3B tabElementsColor;
};

class BattleTab : public CCNode {
protected:
    CCSprite* m_tabTop;
    CCSprite* m_tabBottom;
    CCSprite* m_hpOverlay;
    CCSprite* m_hpBarFill;
    CCSprite* m_nameLabel;
    CCSprite* m_charIcon;
    CCSprite* m_hurtIcon;
    CCSprite* m_defendIcon;
    
    CCLabelBMFont* m_hpLabel;
    CCLabelBMFont* m_maxHpLabel;
    
    CCMenu* m_buttonMenu;
    
    float m_currentHP;
    float m_maxHP;
    CharacterAttributes m_charAttrs;
    std::string m_character;
    
    bool m_isDefending = false;
    int m_defendHitsLeft = 0;
    bool m_isHidden = false;
    bool m_isActive = false;
    
    bool init(const std::string& character, const CharacterAttributes& attrs);
    void setupVisuals();
    void setupButtons();
    
public:
    static BattleTab* create(const std::string& character, const CharacterAttributes& attrs);
    
    void takeDamage(float amount, bool showIndicator = true);
    void heal(float amount);
    void setHP(float hp);
    float getCurrentHP() const { return m_currentHP; }
    float getMaxHP() const { return m_maxHP; }
    bool isDead() const { return m_currentHP <= 0.f; }
    
    void startDefending(int hits = 3);
    void endDefending();
    bool isDefending() const { return m_isDefending; }
    bool processDefendedHit();
    int getDefendHitsLeft() const { return m_defendHitsLeft; }
    
    void setActive(bool active);
    bool isActive() const { return m_isActive; }
    
    void updateHPBar(bool animated = true);
    void updateHPLabel();
    void showHurtIcon(float duration = 0.5f);
    void resetHurtIconCallback();
    void hideTab(bool animated = true);
    void showTab(bool animated = true);
    void updateContainerColor(const ccColor3B& color);
    
    void updatePlayerColors(const ccColor3B& containerColor, const ccColor3B& elementsColor);
    void updateCharacterIcon(CCSpriteFrame* frame);

    void setToHiddenState(bool instant = true);
    void setToInactiveState(bool animated = false);
    void setToActiveState(bool animated = false);
    
    void reset();
    
    CCMenu* getButtonMenu() const { return m_buttonMenu; }
    const CharacterAttributes& getAttributes() const { return m_charAttrs; }
    std::string getCharacter() const { return m_character; }
};