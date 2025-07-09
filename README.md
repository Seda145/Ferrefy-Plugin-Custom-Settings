

# Ferrefy: Custom Settings

![BannerCustomSettingsPlugin](https://github.com/user-attachments/assets/7c93cbcf-7eef-42f0-b2c1-6e6b6f6de34e)

---
### Introduction
---

I made this plugin for Unreal Engine. 

It makes implementation of engine and custom settings simple and efficient.

Settings are classes which are registered into the Player Settings Subsystem, also referred to as the "setting solver", providing information about what they do and require.

The setting solver builds and tracks setting compatibility and cooperation. The solver makes it possible to allow, test and inject complex setting behavior. Settings can even be injected by other plugins.

Applied setting values are written to an ini file, stored per player profile. Profiles can be set through code, or managed by players in-game. Players can also play together with settings personalized by their profiles.

50+ c++ settings are already provided, including settings controls, graphics, audio accessibility, and keybinding. Unreal Engine's experimental "Enhanced Input" is not used.

On top of that, it comes with a complete main menu UI, no code required, which is incredibly easy to design and maintain because it's built with the [Central UI](https://www.fab.com/listings/23a2d4dd-dfbb-42aa-ad90-126fc38e8723) and [UI Additions](https://github.com/Seda145/Ferrefy-Plugin-UI-Additions) plugins. This main menu lets you manage settings and profiles. Everything is supported in split-screen. It contains extras (intro, credits animation system) to further speed up development.

---
### Notice
---

[Ferrefy](https://ferrefy.com/) ([chamber of commerce](https://www.kvk.nl/bestellen/#/95694269000061090395) number 95694269) retains all intellectual property rights (including copyright, patent rights, trademark rights, design and design rights, etc.) on all designs, drawings, writings, data carriers or other information, quotations, images, sketches, models, scale models, etc.

The name 'Ferrefy', the Ferrefy logo, and associated branding are protected and may not be used without permission. Any presence of it in the data does not grant any permissions. If present for demonstration purposes (logo, name, branding etc. on places like an in-game UI), then those placeholders are there to let you easily replace them with your own data. 

---
### Documentation
---

[Read the documentation on my website](https://ferrefy.com/documentation/custom-settings-plugin/)

The documentation covers plugin requirements, installation, features, media, examples, the compact "how and why" of class implementations, and more.

It also lists dependencies. Dependencies are other Ferrefy or Unreal Engine plugins that are required for this plugin to work. 

---
### Availability
---

Unlike some Ferrefy plugins, this plugin is not yet available on FAB. You can currently download it here from Github and read the included readme, license and other documentation to get started. 

This plugin can not be published on FAB, because EPIC's build pipeline is not advanced enough. It seems not to be in their interest to modernize. EPIC / FAB can not process plugins that have (non UE) plugin dependencies, which makes it impossible to publish modular software there. 

To professional software developers, software modularity is a necessity. That is why I decided to still publish this plugin (and others), but on Github, for free.

To support me and future software development, you can buy [Ferrefy plugins on FAB](https://www.fab.com/sellers/Ferrefy).

---
### Find more plugins
---

Find Ferrefy plugins on [FAB](https://www.fab.com/sellers/Ferrefy) and [GitHub](https://github.com/Seda145?tab=repositories).

---
### Media
---

Take a quick look at some of the key features of this plugin. Interested? Click the documentation link to watch all media and learn more.

https://github.com/user-attachments/assets/831430f1-9c92-4607-a6f3-d2ca7d1590e8

![MediaGallery_CustomSettingsPlugin_1](https://github.com/user-attachments/assets/991fe7af-e315-4db4-a2c4-401d9923fbea)

![MediaGallery_CustomSettingsPlugin_2](https://github.com/user-attachments/assets/77424649-07ed-4517-9a43-882fbbdc2541)

![MediaGallery_CustomSettingsPlugin_3](https://github.com/user-attachments/assets/ad4a63d1-7a74-4349-8768-f0f0a991fb34)

![MediaGallery_CustomSettingsPlugin_4](https://github.com/user-attachments/assets/cbc89723-0dd2-44e7-be47-0c56c6a54d09)

![MediaGallery_CustomSettingsPlugin_5](https://github.com/user-attachments/assets/ab23f023-42af-4d33-9141-89d96a0c7533)

![MediaGallery_CustomSettingsPlugin_6](https://github.com/user-attachments/assets/bc61be49-a8ee-4e45-9d7e-9fdff557e78a)

![MediaGallery_CustomSettingsPlugin_7](https://github.com/user-attachments/assets/6858de33-78a4-4c7e-a1cd-38d1fa24b804)

---
### About me
---

Hi! I'm Roy Wierer. I am a solo developer specialized in game and website programming. I enjoy being creative. When not programming, I'm likely working on visual art or playing musical instruments. A mix of many skills and interests, and a high level of specialization, makes me effective as a developer. I'm there from vision to end product. My Github nickname "Seda145", which I used for personal projects became well known with project [Road To The North](https://www.moddb.com/mods/zoneexpanded-road-to-the-north). 

I decided to make game development my full-time job because it offers full freedom to combine my skill set. I began developing plugins for Unreal Engine, then registered my own company, [Ferrefy](https://ferrefy.com). As Ferrefy I participate on marketplaces like [FAB](https://www.fab.com/sellers/Ferrefy). 

As Ferrefy, I release highly modular systems as Unreal Engine plugins. Plugins that excel in usability both independently and when combined into larger systems. I provide the parts, the tools and features, for your project. 
