#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class TPBar : public CCNode {
protected:
    CCSprite* m_barTop;
    CCSprite* m_barBg;
    CCSprite* m_barFill;
    CCSprite* m_tpTextSprite;
    CCSprite* m_fillerLine;
    CCLabelBMFont* m_percentLabel;
    CCLabelBMFont* m_percentSymbol;
    
    float m_currentTP = 0.f;
    
    bool init() override;
    void updateVisuals();
    
public:
    static TPBar* create();
    
    void setTP(float percentage, bool animated = true);
    void addTP(float amount, bool animated = true);
    float getTP() const { return m_currentTP; }
    void reset();
    void animateEntry(float delay = 0.25f);
    void animateExit(float delay = 0.f);
};