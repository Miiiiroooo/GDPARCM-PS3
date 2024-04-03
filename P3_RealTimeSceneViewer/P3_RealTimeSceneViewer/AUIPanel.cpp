#include"AUIPanel.h"

AUIPanel::AUIPanel(std::string name) {
    mPanelName = name;
}

std::string AUIPanel::getName() {
    return mPanelName;
}