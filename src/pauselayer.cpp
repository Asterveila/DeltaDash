#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/loader/SettingV3.hpp>

using namespace geode::prelude;

class $modify(MyPauseLayer, PauseLayer) {
    void openModSettings(CCObject*) {
        geode::openSettingsPopup(Mod::get());
    }

    void customSetup() {
        PauseLayer::customSetup();

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto menu = this->getChildByID("left-button-menu");

        if (!menu) {
            menu = CCMenu::create();
            menu->setLayout(
                ColumnLayout::create()
                    ->setGap(4.f)
                    ->setAxisAlignment(AxisAlignment::End)
                    ->setAxisReverse(true)
                    ->setCrossAxisOverflow(false)
            );
            menu->setPosition({36.f, winSize.height / 2});
            menu->setContentSize({40, winSize.height - 40.f});
            menu->setZOrder(10);
            this->addChild(menu);
        }

        auto spr = CCSprite::createWithSpriteFrameName("openModSettings.png"_spr);
        auto btn = CCMenuItemSpriteExtra::create(
            spr,
            this,
            menu_selector(MyPauseLayer::openModSettings)
        );
        spr->setScale(0.7f);

        btn->setPosition({menu->getContentSize().width / 2, btn->getContentSize().height / 2});
        btn->setID("deltadash-open-settings"_spr);
        menu->addChild(btn);
        menu->updateLayout();
    }
};