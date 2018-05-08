#include "GuiResources.hpp"

using namespace RowBlast;

GuiResources::GuiResources(Pht::IEngine& engine, const CommonResources& commonResources) :
    mLargeMenuWindow {engine, commonResources, MenuWindow::Size::Large} {}
