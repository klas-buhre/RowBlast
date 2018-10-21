#include "NoLivesDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "TextComponent.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "UserData.hpp"
#include "StringUtils.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto countdownNumChars {5};
}

NoLivesDialogView::NoLivesDialogView(Pht::IEngine& engine,
                                     const CommonResources& commonResources,
                                     const UserData& userData,
                                     PotentiallyZoomedScreen zoom) :
    mUserData {userData} {
    
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetMediumDarkMenuWindow(zoom)};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    CreateText({-2.2f, 5.25f, UiLayer::text},
               "NO LIVES",
               guiResources.GetLargeWhiteTextProperties(zoom));
    
    Pht::Vec3 closeButtonPosition {
        GetSize().x / 2.0f - 1.3f,
        GetSize().y / 2.0f - 1.3f,
        UiLayer::textRectangle
    };
    
    Pht::Vec2 closeButtonInputSize {55.0f, 55.0f};

    MenuButton::Style closeButtonStyle;
    closeButtonStyle.mPressedScale = 1.05f;
    closeButtonStyle.mRenderableObject = &guiResources.GetCloseButton(zoom);
    
    mCloseButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                closeButtonPosition,
                                                closeButtonInputSize,
                                                closeButtonStyle);

    Pht::Material lineMaterial {Pht::Color{0.6f, 0.8f, 1.0f}};
    lineMaterial.SetOpacity(0.3f);
    auto& sceneManager {engine.GetSceneManager()};
    auto& lineSceneObject {
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager)
    };
    lineSceneObject.GetTransform().SetPosition({0.0f, GetSize().y / 2.0f - 2.6f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject);
    
    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    
    CreateText({-1.4f, 2.8f, UiLayer::text}, "Lives: 0", textProperties);
    CreateText({-4.5f, 1.8f, UiLayer::text}, "Time until next life:", textProperties);
    mCountdownText = &CreateText({2.3f, 1.8f, UiLayer::text}, "00:00", textProperties);

    auto& line2SceneObject {
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager)
    };
    line2SceneObject.GetTransform().SetPosition({0.0f, 0.5f, UiLayer::textRectangle});
    GetRoot().AddChild(line2SceneObject);

    CreateText({-4.4f, -1.0f, UiLayer::text}, "Purchase 5 lives for $0.99", textProperties);

    Pht::Vec2 refillLivesInputSize {205.0f, 59.0f};
    
    MenuButton::Style refillLivesButtonStyle;
    refillLivesButtonStyle.mMeshFilename = GuiResources::mBigButtonMeshFilename;
    refillLivesButtonStyle.mColor = GuiResources::mGreenButtonColor;
    refillLivesButtonStyle.mSelectedColor = GuiResources::mGreenSelectedButtonColor;
    refillLivesButtonStyle.mPressedScale = 1.05f;

    mRefillLivesButton = std::make_unique<MenuButton>(engine,
                                                      *this,
                                                      Pht::Vec3 {0.0f, -4.0f, UiLayer::textRectangle},
                                                      refillLivesInputSize,
                                                      refillLivesButtonStyle);
    mRefillLivesButton->CreateText({-1.4f, -0.31f, UiLayer::buttonText},
                                   "$0.99",
                                   guiResources.GetLargeWhiteTextProperties(zoom));
}

void NoLivesDialogView::Update() {
    auto secondsUntilNewLife {mUserData.GetLifeManager().GetDurationUntilNewLife()};
    
    if (secondsUntilNewLife != mSecondsUntilNewLife) {
        StaticStringBuffer countdownBuffer;
        StringUtils::FormatToMinutesAndSeconds(countdownBuffer, secondsUntilNewLife);
        
        auto numChars {std::strlen(countdownBuffer.data())};
        assert(numChars == countdownNumChars);
        
        auto& text {mCountdownText->GetText()};
        auto textLength {text.size()};
        assert(textLength == countdownNumChars);
        
        StringUtils::StringCopy(text, 0, countdownBuffer, countdownNumChars);
        
        mSecondsUntilNewLife = secondsUntilNewLife;
    }
}
