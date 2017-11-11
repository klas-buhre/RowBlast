#include "TextComponent.hpp"

using namespace Pht;

TextComponent::TextComponent(const std::string& text, const TextProperties& properties) :
    mText {text},
    mProperties {properties} {}
