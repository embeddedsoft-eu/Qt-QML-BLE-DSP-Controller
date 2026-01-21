# 🎛️ Qt QML BLE DSP Controller

A **relatively simple (as much as possible)** Qt 5 / QML application for controlling a **DSP audio system over BLE**.

The application communicates with a BLE device (HM-10 compatible modules or ESP32 on the controller side) and provides a **custom QML-based UI** for adjusting DSP parameters such as volume, EQ bands and sound profiles.

The project focuses on being a **practical, working solution** for standard BLE modules on **Android and iOS**, while still allowing desktop UI debugging.

---

## ✨ Features

- Qt 5 + QML application
- Custom QML components (sliders, spin boxes, buttons)
- BLE communication via QtBluetooth
- Designed for DSP audio control
- Works with:
  - HM-10 compatible BLE modules
  - ESP32 (BLE mode)
- Vertical screen orientation
- Touch-friendly mobile UI
- Practical, real-world BLE implementation

---
![photo_2026-01-21 12 26 22](https://github.com/user-attachments/assets/4748e455-7e23-4411-ae4d-d552a245c79a)
![photo_2026-01-21 12 26 21](https://github.com/user-attachments/assets/5a021f0d-ca9a-461d-933c-bdb5be51931a)
![photo_2026-01-21 12 26 20](https://github.com/user-attachments/assets/a22fe1e5-dcdf-4a06-a87d-cb43ae197062)

## Demo Video

[https://www.youtube.com/watch?v=VIDEO_ID](https://youtube.com/shorts/Fqf-MxSoJDk?feature=share)

---

## 📱 Supported Platforms

| Platform  | Status |
|---------|--------|
| Android | ✅ Supported |
| iOS     | ✅ Supported (requires macOS + Xcode) |
| Desktop | ✅ UI debugging & visual development |

> ⚠️ BLE functionality on desktop depends on platform and adapter support.  
> Desktop builds are mainly intended for **UI development and debugging**.

---

## 📐 UI Design

- Designed primarily for **vertical orientation**
- Custom QML controls instead of standard Qt widgets
- Optimized for mobile touch input
- Desktop builds can be used to debug UI logic

---

## 🔧 BLE & Protocol

- BLE communication implemented using **QtBluetooth**
- Uses a **custom and intentionally simple BLE protocol**
- The entire protocol implementation is located at the **bottom of `ble.cpp`**

### Current protocol functionality:
- Request current sound profile
- Read firmware version
- Read current DSP profile parameters

> 🔧 **Important**  
> The protocol is intended as a **base implementation** and should be **modified or extended** for your own DSP system.
>
> For this project, the protocol was implemented to:
> - request the current profile
> - read firmware version
> - read characteristics of the active DSP profile
>
> The focus is on a **working solution** for common BLE modules used with **Android and iOS**.

## 🧩 Project Structure (Simplified)

```text
.
├── qml/
│   ├── Classic/
│   │   ├── Slider.qml
│   │   ├── Spin.qml
│   │   └── ...
│   ├── Start.qml
│   └── ...
├── ble.cpp        // BLE logic and protocol implementation
├── ble.h
├── deviceinfo.*
└── README.md






