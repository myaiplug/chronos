# Chronos Spectral Weaver

## Professional 7-Band Parametric EQ Engine

**High-fidelity, professional-grade parametric EQ designed for music production, mastering, and sound design.**

---

## Features

### Core Capabilities
- **7 Fully Parametric Bands** - Independent control of frequency, Q, and gain
- **Multiple Filter Types** - Bell, Low/High Shelf, HPF/LPF, All-Pass, Notch
- **Professional Quality** - 64-bit double precision processing
- **Numerically Stable** - Robust implementation across all parameter ranges
- **Phase Coherent** - Minimal phase distortion
- **Click-Free** - Smooth parameter updates without audio artifacts

### Technical Excellence
- **Direct Form II Transposed** biquad implementation for numerical stability
- **Industry-standard cookbook formulas** for filter design
- **Proper frequency warping** for bilinear transform
- **Individual band enable/disable** for CPU efficiency
- **Zero-latency processing** for real-time applications

---

## Architecture

### Filter Structure
```
Input → Band 0 → Band 1 → Band 2 → Band 3 → Band 4 → Band 5 → Band 6 → Output
         (HPF)    (Low)    (Low-   (Mid)    (High-  (High)   (LPF)
                           Mid)             Mid)
```

### Typical Band Allocation
| Band | Type       | Frequency Range | Typical Use            |
|------|------------|----------------|------------------------|
| 0    | HPF/Bell   | 20-100 Hz      | Rumble removal         |
| 1    | Low Shelf  | 100-250 Hz     | Bass adjustment        |
| 2    | Bell       | 250-600 Hz     | Low-mid clarity        |
| 3    | Bell       | 600-2000 Hz    | Midrange presence      |
| 4    | Bell       | 2000-5000 Hz   | High-mid detail        |
| 5    | High Shelf | 5000-12000 Hz  | Treble/Air             |
| 6    | LPF/Bell   | 12000-20000 Hz | Sparkle/Harshness ctrl |

---

## Filter Types

### Bell (Parametric Peak)
- **Use**: Surgical frequency correction or enhancement
- **Parameters**: Frequency, Q (bandwidth), Gain (±dB)
- **Characteristics**: Symmetrical boost/cut around center frequency

### Low Shelf
- **Use**: Bass adjustment, warmth control
- **Parameters**: Frequency (transition), Q (slope), Gain (±dB)
- **Characteristics**: Affects all frequencies below transition point

### High Shelf
- **Use**: Treble adjustment, air, sparkle
- **Parameters**: Frequency (transition), Q (slope), Gain (±dB)
- **Characteristics**: Affects all frequencies above transition point

### High-Pass Filter (HPF)
- **Use**: Rumble removal, clean up low end
- **Parameters**: Frequency (cutoff), Q (resonance)
- **Characteristics**: Attenuates frequencies below cutoff

### Low-Pass Filter (LPF)
- **Use**: Tame harsh highs, warmth
- **Parameters**: Frequency (cutoff), Q (resonance)
- **Characteristics**: Attenuates frequencies above cutoff

### All-Pass Filter
- **Use**: Phase correction without affecting magnitude
- **Parameters**: Frequency, Q
- **Characteristics**: Unity gain, phase shift only

### Notch Filter
- **Use**: Feedback elimination, resonance removal
- **Parameters**: Frequency, Q (bandwidth)
- **Characteristics**: Deep cut at center frequency

---

## Quick Start

### Basic Usage

```cpp
#include "SpectralWeaver.hpp"

using namespace Chronos;

int main() {
    // Create EQ instance
    SpectralWeaver eq;
    eq.initialize(44100.0);  // Set sample rate
    
    // Configure a bell filter at 1 kHz
    eq.setBand(3, FilterType::Bell, 1000.0, 1.0, 6.0);
    eq.setBandEnabled(3, true);
    
    // Process audio
    double input = ...; // Your input sample
    double output = eq.processSample(input);
    
    return 0;
}
```

### Multi-Band Configuration

```cpp
SpectralWeaver eq;
eq.initialize(48000.0);

// Band 0: HPF to remove rumble
eq.setBand(0, FilterType::HighPass, 30.0, 0.707, 0.0);
eq.setBandEnabled(0, true);

// Band 1: Low shelf boost
eq.setBand(1, FilterType::LowShelf, 100.0, 0.707, 3.0);
eq.setBandEnabled(1, true);

// Band 3: Mid presence
eq.setBand(3, FilterType::Bell, 1500.0, 1.0, 2.5);
eq.setBandEnabled(3, true);

// Band 5: High shelf sparkle
eq.setBand(5, FilterType::HighShelf, 10000.0, 0.707, 2.0);
eq.setBandEnabled(5, true);
```

### Block Processing

```cpp
const int blockSize = 512;
double input[blockSize];
double output[blockSize];

// Fill input buffer with audio...

// Process entire block
eq.processBlock(input, output, blockSize);
```

---

## API Reference

### SpectralWeaver Class

#### Initialization
```cpp
void initialize(double sampleRate);
void setSampleRate(double sampleRate);
double getSampleRate() const;
```

#### Band Configuration
```cpp
void setBand(int bandIndex, FilterType type, double frequency, 
             double Q, double gainDB = 0.0);

void setBandEnabled(int bandIndex, bool enabled);
void setBandFrequency(int bandIndex, double frequency);
void setBandQ(int bandIndex, double Q);
void setBandGain(int bandIndex, double gainDB);
void setBandType(int bandIndex, FilterType type);

const EQBand& getBand(int bandIndex) const;
```

#### Processing
```cpp
double processSample(double input);
void processBlock(const double* input, double* output, int numSamples);
void reset();  // Clear filter states
```

#### Bypass
```cpp
void setBypass(bool bypass);
bool isBypassed() const;
```

### Parameter Ranges

| Parameter | Minimum | Maximum | Typical | Notes |
|-----------|---------|---------|---------|-------|
| Frequency | 1 Hz    | 0.49×SR | -       | Auto-clamped to valid range |
| Q         | 0.1     | 18.0    | 0.707   | Higher = narrower bandwidth |
| Gain      | -∞ dB   | +∞ dB   | ±12 dB  | Practical range for mastering |

---

## Building

### Requirements
- C++17 compatible compiler
- Standard library with `<cmath>`, `<array>`, `<algorithm>`

### Compile and Run
```bash
# Build and run everything
make

# Run tests only
make test

# Run demo only
make demo

# Build without running
make build

# Clean build artifacts
make clean
```

### Compiler Flags
```bash
g++ -std=c++17 -O3 -march=native -ffast-math -I./include \
    tests/test_spectral_weaver.cpp -o test_spectral_weaver -lm
```

---

## Implementation Details

### Numerical Stability

The implementation prioritizes numerical stability through:

1. **Direct Form II Transposed** structure
   - Minimizes coefficient sensitivity
   - Reduces quantization noise
   - Better than Direct Form I for fixed-point and floating-point

2. **Double precision (64-bit)** throughout the signal path
   - Maintains accuracy in long filter chains
   - Prevents accumulation of rounding errors

3. **Careful coefficient calculation**
   - Proper handling of extreme parameter values
   - Frequency clamping to valid range (0 < f < SR/2)
   - Q-factor limiting to stable range

4. **State variable initialization**
   - Clean reset functionality
   - No DC offset accumulation

### Filter Design

Based on Robert Bristow-Johnson's "Audio EQ Cookbook":

- Bilinear transform for analog→digital conversion
- Pre-warped frequency for accurate placement
- Proper gain staging for shelving filters
- Normalized coefficients (a0 = 1)

### Performance Considerations

- **Header-only implementation** for compiler optimization
- **Inline processing** for minimal function call overhead
- **Cascaded structure** for efficient multi-band processing
- **Conditional processing** - disabled bands skip computation

---

## Example Applications

### Mastering EQ
```cpp
// Subtle enhancements for final mix
eq.setBand(0, FilterType::HighPass, 30.0, 0.707, 0.0);
eq.setBand(1, FilterType::LowShelf, 80.0, 0.707, 1.5);
eq.setBand(3, FilterType::Bell, 1500.0, 1.0, 1.0);
eq.setBand(5, FilterType::HighShelf, 10000.0, 0.707, 1.0);
// Enable all bands...
```

### Vocal Processing
```cpp
// Remove mud, add presence and air
eq.setBand(0, FilterType::HighPass, 80.0, 0.707, 0.0);
eq.setBand(2, FilterType::Bell, 300.0, 1.5, -3.0);  // Reduce mud
eq.setBand(3, FilterType::Bell, 3000.0, 1.0, 4.0);  // Presence
eq.setBand(5, FilterType::HighShelf, 8000.0, 0.707, 2.0);  // Air
```

### Bass Enhancement
```cpp
// Tight, punchy bass
eq.setBand(0, FilterType::HighPass, 40.0, 0.707, 0.0);
eq.setBand(1, FilterType::Bell, 60.0, 1.0, 3.0);    // Sub punch
eq.setBand(2, FilterType::Bell, 120.0, 1.5, 2.0);   // Body
eq.setBand(3, FilterType::Bell, 400.0, 2.0, -2.0);  // Reduce boxiness
```

---

## Testing

The included test suite validates:
- ✓ Biquad filter implementation
- ✓ All filter type calculations
- ✓ Parameter updates
- ✓ Block processing
- ✓ Numerical stability
- ✓ Bypass functionality
- ✓ Sample rate changes
- ✓ Multi-band cascading

Run tests with:
```bash
make test
```

---

## Demo Application

The demo application showcases:
1. Bell filter frequency response
2. Shelving filter characteristics
3. Pass filter behavior
4. Complete 7-band configuration
5. Dynamic parameter changes
6. Bypass functionality

Run demo with:
```bash
make demo
```

---

## License

This is a professional DSP implementation for the Chronos audio plugin project.

---

## Technical Notes

### Q-Factor Guidelines
- **0.5 - 0.8**: Gentle, musical curves (mastering)
- **1.0 - 2.0**: Focused adjustments (mixing)
- **3.0 - 10.0**: Surgical corrections (problem frequencies)

### Gain Guidelines
- **±1-3 dB**: Subtle, transparent adjustments
- **±3-6 dB**: Moderate, noticeable changes
- **±6-12 dB**: Dramatic reshaping
- **>±12 dB**: Extreme, use with caution

### CPU Optimization
- Disable unused bands for better performance
- Use bypass when EQ is not needed
- Consider using single-band processing when appropriate

---

## Contributing

This is part of the Chronos project. For improvements or bug reports, please refer to the main project repository.

---

**Chronos Spectral Weaver** - Professional audio processing at your fingertips.
