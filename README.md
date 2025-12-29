# Chronos - Spectral Weaver

**Professional 7-Band Parametric EQ Engine**

[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

High-fidelity, professional-grade parametric EQ designed for music production, mastering, and sound design.

## Features

✨ **7 Fully Parametric Bands** with independent frequency, Q, and gain control  
🎛️ **Multiple Filter Types**: Bell, Low/High Shelf, HPF/LPF, All-Pass, Notch  
🔬 **64-bit Double Precision** processing for maximum audio quality  
⚡ **Numerically Stable** implementation using Direct Form II Transposed  
🎵 **Phase Coherent** with minimal phase distortion  
🎚️ **Click-Free** parameter updates for smooth automation  

## Quick Start

```cpp
#include "SpectralWeaver.hpp"

using namespace Chronos;

int main() {
    // Create and initialize EQ
    SpectralWeaver eq;
    eq.initialize(44100.0);
    
    // Configure a parametric bell filter
    eq.setBand(3, FilterType::Bell, 1000.0, 1.0, 6.0);
    eq.setBandEnabled(3, true);
    
    // Process audio
    double output = eq.processSample(input);
    
    return 0;
}
```

## Build & Run

```bash
# Build and run everything
make

# Run tests only
make test

# Run demo only
make demo
```

## Documentation

See [DOCUMENTATION.md](DOCUMENTATION.md) for comprehensive API reference, examples, and technical details.

## Project Structure

```
chronos/
├── include/              # Header-only library
│   ├── Biquad.hpp       # Core biquad filter implementation
│   ├── FilterDesign.hpp # Filter coefficient calculators
│   └── SpectralWeaver.hpp # 7-band EQ engine
├── tests/               # Test suite
├── examples/            # Demo applications
└── Makefile            # Build system
```

## Technical Highlights

- **Direct Form II Transposed** biquad structure for optimal numerical stability
- **Industry-standard cookbook formulas** for professional filter design
- **Cascaded processing** through 7 independent filter stages
- **Proper frequency warping** for accurate bilinear transform
- **Header-only implementation** for maximum compiler optimization

## Applications

- 🎼 Music Production
- 🎚️ Mastering
- 🔊 Sound Design
- 🎤 Vocal Processing
- 🎸 Instrument Enhancement

## Requirements

- C++17 compatible compiler
- Standard library support

---

**Chronos** - Professional audio DSP for the modern era. 
