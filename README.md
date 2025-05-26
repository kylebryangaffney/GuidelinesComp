# GuideLinesComp

**GuideLinesComp** is a two-stage audio compressor plugin built with JUCE. It is designed for clean, intuitive vocal or dialogue compression, combining fast transient control with warm, musical leveling inspired by classic optical compressors.

---

## 🎛️ Features

- **Stage 1: Control Compression**
  - Faster compressor for managing peaks and transients
  - User-defined attack, threshold, ratio, and release (via mapped controls)

- **Stage 2: Tone Compression**
  - Fixed-character opto-style compressor
  - Driven by the output of Stage 1 for smooth, musical gain leveling

- **Low Cut Filter**
  - Adjustable high-pass filter to remove low-end rumble (20 Hz – 1 kHz)

- **Output Gain**
  - Clean, smoothed output level control with ±18 dB range

- **Gain Reduction Warning System**
  - Visual indicator that intensifies from yellow to red if gain reduction exceeds 6 dB
  - Helps users avoid over-compression and maintain dynamic integrity

- **Responsive UI & Smoothed Parameters**
  - Parameter smoothing for zipper-free changes and realtime safety
  - Sleek black/orange interface with visual feedback

---

## 🧱 Built With

- [JUCE](https://juce.com) – C++ framework for developing audio plugins
- C++17
- Git & GitHub for version control

---

## 🛠️ Getting Started

To build this plugin:

1. Clone the repository:
   ```bash
   git clone https://github.com/kylebryangaffney/GuideLinesComp.git
   cd GuideLinesComp
