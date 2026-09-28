# Introduction
![GitHub](https://img.shields.io/github/license/LGLTeam/Android-Mod-Menu?style=flat-square)

Floating mod menu for il2cpp and other native android games. KittyMemory, MSHook, And64InlineHook and basic string obfuscator (AY obfuscator) included. Assets are stored as base64 in cpp and does not need to be stored under assets folder.

Support Android 4.4.x up to Android S Preview. ARMv7, x86 and ARM64 architecture supported.

**Modified & Customized by Raj Vishwakarma**

![](https://i.imgur.com/zeumkBG.gif)

# Features
- 🎯 Floating Mod Menu overlay
- 🔧 KittyMemory patching
- 🪝 MSHook & And64InlineHook hooking
- 🔐 AY string obfuscator
- 📦 Base64 assets (no assets folder needed)
- 📱 Multi-architecture support (ARMv7 / x86 / ARM64)
- 🎨 Menu controls: Spinner, Toggle, SeekBar, Button, Category, InputValue, CheckBox
- 💰 Money Hack (Gold, Silver, XP, Karma, Gas)
- 🏃 Speed Hack
- 💀 Auto Kill
- 🖼️ ESP (Line, Box, Name, Distance, Object Counter)
- 🔔 Missing Offset Notifications
- 🛡️ Safe Patching (null-checks to prevent crashes)

# How It Works
`lib_main()` constructor starts a hack thread on library load → `hack_thread()` waits for `libil2cpp.so` to load (`sleep(5)`) → then finds classes via `LoadClass`, gets method offsets via `GetMethodOffsetByName`, hooks functions using `HOOK_LIB`/`HOOK_AU`, applies patches via `PATCH_LIB` → `GetFeatureList()` returns all menu toggles → `Changes()` handles user input to enable/disable patches → `DrawOn()` draws ESP overlay (boxes, lines, names, distance) every frame.

# Known bug
- Spinner does not show on some devices running Android 11. Should work again on Android 12
- On some games, menu is using old layout such as Kitkat or Gingerbread when launched without permission. We have not found a way to fix it.

# Download
Download this repo as ZIP, or clone using any git tools

Or download Releases here https://github.com/LGLTeam/Android-Mod-Menu/releases

# Getting started
**Go to this Wiki page to start reading:**

https://github.com/LGLTeam/Android-Mod-Menu/wiki

# Help, Support, FAQ

See: [Frequently Asked Questions (FAQ)](https://github.com/LGLTeam/Android-Mod-Menu/wiki/FAQ) where common questions are answered.

If you have installation or usage problems, try asking your questions on the forum sites, [Platinmods](https://platinmods.com/forums/modding-questions-discussions.11/), or [UnknownCheats](https://www.unknowncheats.me/forum/android/) or others.

For example, if you have an issue with Hooking and game crashes, you should go to the **support forums**. Here there are no teachers who can help you with such issues.

Issues are disabled permanently due to peoples who have no mind (mostly newbies) aren't even able to fill proper issue templates nor are they able to read the instructions. I get so many useless issues, even useless pull-requests.

As a result, the contact infomation has been removed as well. However you can find me in our Telegram channel. I will only talk to peoples who are very skilled, have proper brain and have patience! I will BLOCK if I feel like you are annoying, disrespectful and acting like a kid

# Credits
Thanks to the following individuals whose code helped me develop this mod menu

* Octowolve/Escanor - Mod menu: https://github.com/z3r0Sec/Substrate-Template-With-Mod-Menu and Hooking: https://github.com/z3r0Sec/Substrate-Hooking-Example
* VanHoevenTR - Mod menu - https://github.com/LGLTeam/VanHoevenTR_Android_Mod_Menu
* MrIkso - First mod menu template https://github.com/MrIkso/FloatingModMenu
* MJx0 A.K.A Ruit - https://github.com/MJx0/KittyMemory
* Rprop - https://github.com/Rprop/And64InlineHook
* LGLTeam - Original Android-Mod-Menu project - https://github.com/LGLTeam/Android-Mod-Menu
* And everyone else who provided input and contributions to this project!

# Modified By
**Raj Vishwakarma** — Added Money Hack (Gold, Silver, XP, Karma, Gas), Speed Hack, Auto Kill, Missing Offset Notifications, Safe Patching with null-checks to prevent crashes, ESP improvements (Line, Box, Name, Distance, Object Counter) and 32-bit ARM (armeabi-v7a) support.

# License
**GNU General Public License 3**

# Disclaimer
This project is for Educational Use only. We do not condone this project being used to gain an advantage against other people. This project was made for fun

While commecial use/selling is allowed, we still strongly refrain you from buying any source codes on Telegram even if the author can be trusted, there is always a risk getting scammed. We will not be responsible for that. This project is always FREE to use
