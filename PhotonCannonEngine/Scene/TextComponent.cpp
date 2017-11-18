#include "TextComponent.hpp"

#include "Fnv1Hash.hpp"

using namespace Pht;

const ComponentId TextComponent::id {Hash::Fnv1a("TextComponent")};

TextComponent::TextComponent(const std::string& text, const TextProperties& properties) :
    mText {text},
    mProperties {properties} {}
