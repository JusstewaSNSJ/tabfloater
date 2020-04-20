window.onload = function () {
    setButtonStates();
    setCompanionStatusIndicator();
}

floatTabButton.onclick = function () {
    window.close();

    chrome.runtime.getBackgroundPage(function (backgroundPage) {
        backgroundPage.floatTab();
    });
};

unfloatTabButton.onclick = function () {
    window.close();
    chrome.runtime.getBackgroundPage(function (backgroundPage) {
        backgroundPage.unfloatTab();
    });
};

setButtonStates = function () {
    chrome.runtime.getBackgroundPage(function (backgroundPage) {
        backgroundPage.tryGetFloatingTab(function (floatingTab) {
            let floatingTabAlreadyExists = floatingTab != undefined;
            floatTabButton.disabled = floatingTabAlreadyExists;
            unfloatTabButton.disabled = !floatingTabAlreadyExists;
        });
    });
}

setCompanionStatusIndicator = function () {
    chrome.runtime.getBackgroundPage(function (backgroundPage) {
        backgroundPage.getCompanionStatus(function (status) {
            companionStatusConnecting.classList.add("is-hidden");
            
            if (status == "connected") {
                companionStatusConnected.classList.remove("is-hidden");
            } else if (status == "inactive") {
                companionStatusInactive.classList.remove("is-hidden");
            } else if (status == "error") {
                companionStatusError.classList.remove("is-hidden");
            } else {
                companionStatusUnavailable.classList.remove("is-hidden");
            }
        });
    });
}