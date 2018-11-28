#include "NoLivesDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "TextComponent.hpp"
#include "QuadMesh.hpp"
#include "ObjMesh.hpp"

// Game includes.
#include "UserServices.hpp"
#include "StringUtils.hpp"
#include "UiLayer.hpp"
#include "IGuiLightProvider.hpp"

using namespace RowBlast;

namespace {
    constexpr auto countdownNumChars {5};
}

NoLivesDialogView::NoLivesDialogView(Pht::IEngine& engine,
                                     const CommonResources& commonResources,
                                     const UserServices& userServices,
                                     PotentiallyZoomedScreen zoom) :
    mUserServices {userServices} {
    
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetMediumDarkMenuWindow(zoom)};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    CreateText({-2.2f, 4.95f, UiLayer::text},
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
    
    auto& heart {
        CreateSceneObject(Pht::ObjMesh {"heart_112.obj", 3.25f},
                          commonResources.GetMaterials().GetRedMaterial(),
                          sceneManager)
    };
    auto& heartTransform {heart.GetTransform()};
    heartTransform.SetPosition({0.0f, 2.5f, UiLayer::root});
    heartTransform.SetScale(2.0f);
    GetRoot().AddChild(heart);
    
    Pht::TextProperties zeroTextProperties {
        commonResources.GetHussarFontSize35(zoom),
        1.0f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.27f, 0.27f, 0.27f, 0.5f}
    };
    
    CreateText({-0.4, 2.2f, UiLayer::text}, "0", zeroTextProperties);
    
    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    
    CreateText({-4.5f, 0.5f, UiLayer::text}, "Time until next life:", textProperties);
    mCountdownText = &CreateText({2.3f, 0.5f, UiLayer::text}, "00:00", textProperties);

    auto& line2SceneObject {
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager)
    };
    line2SceneObject.GetTransform().SetPosition({0.0f, -0.2f, UiLayer::textRectangle});
    GetRoot().AddChild(line2SceneObject);

    CreateText({-4.9f, -1.5f, UiLayer::text}, "Get 5 more lives and continue", textProperties);
    CreateText({-1.3f, -2.575f, UiLayer::text}, "playing", textProperties);

    Pht::Vec2 refillLivesInputSize {194.0f, 50.0f};

    MenuButton::Style refillLivesButtonStyle;
    refillLivesButtonStyle.mPressedScale = 1.05f;
    refillLivesButtonStyle.mTextScale = 1.05f;
    refillLivesButtonStyle.mRenderableObject = &guiResources.GetLargeGreenGlossyButton(zoom);
    refillLivesButtonStyle.mSelectedRenderableObject = &guiResources.GetLargeDarkGreenGlossyButton(zoom);

    mRefillLivesButton = std::make_unique<MenuButton>(engine,
                                                      *this,
                                                      Pht::Vec3 {0.0f, -4.8f, UiLayer::textRectangle},
                                                      refillLivesInputSize,
                                                      refillLivesButtonStyle);

    Pht::TextProperties buttonTextProperties {
        commonResources.GetHussarFontSize27(zoom),
        1.05f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}
    };

    mRefillLivesButton->CreateText({-3.1f, -0.24f, UiLayer::buttonText},
                                   "CONTINUE           12",
                                   buttonTextProperties);

    auto& coin {
        CreateSceneObject(Pht::ObjMesh {"coin_852.obj", 3.15f},
                          commonResources.GetMaterials().GetGoldMaterial(),
                          engine.GetSceneManager())
    };
    auto& coinTransform {coin.GetTransform()};
    coinTransform.SetPosition({1.85f, 0.0f, UiLayer::text});
    coinTransform.SetRotation({0.0f, 40.0f, 0.0f});
    coinTransform.SetScale(0.9f);
    mRefillLivesButton->GetSceneObject().AddChild(coin);
}

void NoLivesDialogView::SetUp() {
    if (mGuiLightProvider) {
        mGuiLightProvider->SetDefaultGuiLightDirections();
    }
}

void NoLivesDialogView::Update() {
    auto secondsUntilNewLife {mUserServices.GetLifeService().GetDurationUntilNewLife()};
    
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
