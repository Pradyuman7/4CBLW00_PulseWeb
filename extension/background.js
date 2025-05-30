const targetUrl = "https://pradyuman7.github.io/4CBLW00_PulseWeb/";

function showNotification() {
    chrome.notifications.create({
        type: "basic",
        iconUrl: "icon.png",
        title: "WebPulse Check-In",
        message: "How are you feeling? Tap to answer a few quick questions.",
        priority: 2
    });
}

chrome.notifications.onClicked.addListener(() => {
    chrome.tabs.create({ url: targetUrl });
});

setInterval(showNotification, 10000);
