#include "../include/SpectralWeaver.hpp"
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <iomanip>

using namespace Chronos;

// Tolerance for floating-point comparisons
constexpr double EPSILON = 1e-9;

bool areClose(double a, double b, double tolerance = EPSILON) {
    return std::abs(a - b) < tolerance;
}

void testBiquadBasic() {
    std::cout << "Testing Biquad basic functionality..." << std::endl;
    
    Biquad filter;
    
    // Test passthrough (all b0=1, others=0)
    filter.setCoefficients(1.0, 0.0, 0.0, 0.0, 0.0);
    double output = filter.process(1.0);
    assert(areClose(output, 1.0));
    
    // Test reset
    filter.reset();
    output = filter.process(1.0);
    assert(areClose(output, 1.0));
    
    std::cout << "  ✓ Biquad basic tests passed" << std::endl;
}

void testFilterDesign() {
    std::cout << "Testing FilterDesign..." << std::endl;
    
    Biquad filter;
    double sampleRate = 44100.0;
    
    // Test Bell filter design
    FilterDesign::designBell(filter, sampleRate, 1000.0, 1.0, 6.0);
    double impulse = filter.process(1.0);
    // Should produce non-zero output
    assert(!areClose(impulse, 0.0));
    filter.reset();
    
    // Test Low Shelf
    FilterDesign::designLowShelf(filter, sampleRate, 200.0, 0.707, 3.0);
    impulse = filter.process(1.0);
    assert(!areClose(impulse, 0.0));
    filter.reset();
    
    // Test High Shelf
    FilterDesign::designHighShelf(filter, sampleRate, 8000.0, 0.707, 3.0);
    impulse = filter.process(1.0);
    assert(!areClose(impulse, 0.0));
    filter.reset();
    
    // Test HPF
    FilterDesign::designHighPass(filter, sampleRate, 100.0, 0.707);
    impulse = filter.process(1.0);
    assert(!areClose(impulse, 0.0));
    filter.reset();
    
    // Test LPF
    FilterDesign::designLowPass(filter, sampleRate, 10000.0, 0.707);
    impulse = filter.process(1.0);
    assert(!areClose(impulse, 0.0));
    filter.reset();
    
    std::cout << "  ✓ FilterDesign tests passed" << std::endl;
}

void testSpectralWeaverInitialization() {
    std::cout << "Testing SpectralWeaver initialization..." << std::endl;
    
    SpectralWeaver eq;
    eq.initialize(44100.0);
    
    assert(eq.getSampleRate() == 44100.0);
    assert(eq.getNumBands() == 7);
    assert(!eq.isBypassed());
    
    // Check default bands are disabled
    for (int i = 0; i < 7; ++i) {
        assert(!eq.getBand(i).enabled);
    }
    
    std::cout << "  ✓ SpectralWeaver initialization tests passed" << std::endl;
}

void testBandConfiguration() {
    std::cout << "Testing band configuration..." << std::endl;
    
    SpectralWeaver eq;
    eq.initialize(48000.0);
    
    // Test setting a bell filter
    eq.setBand(3, FilterType::Bell, 1000.0, 1.0, 6.0);
    eq.setBandEnabled(3, true);
    
    const auto& band = eq.getBand(3);
    assert(band.type == FilterType::Bell);
    assert(areClose(band.frequency, 1000.0));
    assert(areClose(band.Q, 1.0));
    assert(areClose(band.gainDB, 6.0));
    assert(band.enabled);
    
    // Test individual parameter updates
    eq.setBandFrequency(3, 2000.0);
    assert(areClose(eq.getBand(3).frequency, 2000.0));
    
    eq.setBandQ(3, 2.0);
    assert(areClose(eq.getBand(3).Q, 2.0));
    
    eq.setBandGain(3, -3.0);
    assert(areClose(eq.getBand(3).gainDB, -3.0));
    
    eq.setBandType(3, FilterType::HighShelf);
    assert(eq.getBand(3).type == FilterType::HighShelf);
    
    std::cout << "  ✓ Band configuration tests passed" << std::endl;
}

void testProcessing() {
    std::cout << "Testing audio processing..." << std::endl;
    
    SpectralWeaver eq;
    eq.initialize(44100.0);
    
    // Test bypass
    eq.setBypass(true);
    double input = 0.5;
    double output = eq.processSample(input);
    assert(areClose(output, input));
    
    // Test with bands disabled (should pass through)
    eq.setBypass(false);
    output = eq.processSample(input);
    assert(areClose(output, input));
    
    // Enable a band and test processing
    eq.setBand(3, FilterType::Bell, 1000.0, 1.0, 6.0);
    eq.setBandEnabled(3, true);
    
    // Process impulse
    eq.reset();
    double impulseResponse = eq.processSample(1.0);
    // Should produce different output than input
    assert(!areClose(impulseResponse, 0.0));
    
    // Process silence (should remain silent)
    eq.reset();
    for (int i = 0; i < 100; ++i) {
        output = eq.processSample(0.0);
    }
    assert(areClose(output, 0.0, 1e-6)); // Very small tolerance for numerical precision
    
    std::cout << "  ✓ Audio processing tests passed" << std::endl;
}

void testBlockProcessing() {
    std::cout << "Testing block processing..." << std::endl;
    
    SpectralWeaver eq;
    eq.initialize(44100.0);
    
    const int blockSize = 512;
    std::vector<double> input(blockSize);
    std::vector<double> output(blockSize);
    
    // Create impulse
    input[0] = 1.0;
    for (int i = 1; i < blockSize; ++i) {
        input[i] = 0.0;
    }
    
    // Test bypass
    eq.setBypass(true);
    eq.processBlock(input.data(), output.data(), blockSize);
    assert(areClose(output[0], 1.0));
    for (int i = 1; i < blockSize; ++i) {
        assert(areClose(output[i], 0.0));
    }
    
    // Enable processing
    eq.setBypass(false);
    eq.setBand(2, FilterType::Bell, 500.0, 0.707, 6.0);
    eq.setBandEnabled(2, true);
    
    eq.reset();
    eq.processBlock(input.data(), output.data(), blockSize);
    
    // Output should be different from input
    bool hasNonZeroOutput = false;
    for (int i = 0; i < blockSize; ++i) {
        if (!areClose(output[i], input[i], 0.01)) {
            hasNonZeroOutput = true;
            break;
        }
    }
    assert(hasNonZeroOutput);
    
    std::cout << "  ✓ Block processing tests passed" << std::endl;
}

void testMultipleBands() {
    std::cout << "Testing multiple active bands..." << std::endl;
    
    SpectralWeaver eq;
    eq.initialize(44100.0);
    
    // Configure multiple bands
    eq.setBand(0, FilterType::HighPass, 30.0, 0.707, 0.0);
    eq.setBandEnabled(0, true);
    
    eq.setBand(1, FilterType::LowShelf, 100.0, 0.707, 3.0);
    eq.setBandEnabled(1, true);
    
    eq.setBand(3, FilterType::Bell, 1000.0, 1.0, 6.0);
    eq.setBandEnabled(3, true);
    
    eq.setBand(5, FilterType::HighShelf, 8000.0, 0.707, 2.0);
    eq.setBandEnabled(5, true);
    
    eq.setBand(6, FilterType::LowPass, 18000.0, 0.707, 0.0);
    eq.setBandEnabled(6, true);
    
    // Process impulse through all bands
    eq.reset();
    double impulse = eq.processSample(1.0);
    
    // Should produce significant output
    assert(!areClose(impulse, 0.0));
    
    std::cout << "  ✓ Multiple bands test passed" << std::endl;
}

void testNumericalStability() {
    std::cout << "Testing numerical stability..." << std::endl;
    
    SpectralWeaver eq;
    eq.initialize(44100.0);
    
    // Configure extreme settings
    eq.setBand(0, FilterType::Bell, 50.0, 10.0, 12.0);
    eq.setBandEnabled(0, true);
    
    eq.setBand(6, FilterType::Bell, 18000.0, 10.0, -12.0);
    eq.setBandEnabled(6, true);
    
    // Process many samples to check for numerical issues
    eq.reset();
    double output = 0.0;
    for (int i = 0; i < 10000; ++i) {
        output = eq.processSample(0.0);
        // Check for NaN or Inf
        assert(!std::isnan(output));
        assert(!std::isinf(output));
        // Output should decay to zero
        assert(std::abs(output) < 1000.0); // Reasonable bound
    }
    
    // After processing silence, output should be very close to zero
    assert(std::abs(output) < 1e-5);
    
    std::cout << "  ✓ Numerical stability tests passed" << std::endl;
}

void testAllFilterTypes() {
    std::cout << "Testing all filter types..." << std::endl;
    
    SpectralWeaver eq;
    eq.initialize(44100.0);
    
    std::vector<FilterType> types = {
        FilterType::Bell,
        FilterType::LowShelf,
        FilterType::HighShelf,
        FilterType::HighPass,
        FilterType::LowPass,
        FilterType::AllPass,
        FilterType::Notch
    };
    
    for (auto type : types) {
        eq.reset();
        eq.setBand(3, type, 1000.0, 1.0, 6.0);
        eq.setBandEnabled(3, true);
        
        // Process impulse
        double impulse = eq.processSample(1.0);
        
        // All filters should produce valid output
        assert(!std::isnan(impulse));
        assert(!std::isinf(impulse));
        assert(std::abs(impulse) < 10.0); // Reasonable bound
        
        eq.setBandEnabled(3, false);
    }
    
    std::cout << "  ✓ All filter types test passed" << std::endl;
}

void testSampleRateChange() {
    std::cout << "Testing sample rate changes..." << std::endl;
    
    SpectralWeaver eq;
    eq.initialize(44100.0);
    
    eq.setBand(3, FilterType::Bell, 1000.0, 1.0, 6.0);
    eq.setBandEnabled(3, true);
    
    // Change sample rate
    eq.setSampleRate(48000.0);
    assert(eq.getSampleRate() == 48000.0);
    
    // Should still process correctly
    eq.reset();
    double output = eq.processSample(1.0);
    assert(!std::isnan(output));
    assert(!std::isinf(output));
    
    // Change to higher sample rate
    eq.setSampleRate(96000.0);
    eq.reset();
    output = eq.processSample(1.0);
    assert(!std::isnan(output));
    assert(!std::isinf(output));
    
    std::cout << "  ✓ Sample rate change tests passed" << std::endl;
}

void printTestResults() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "   CHRONOS SPECTRAL WEAVER - TEST RESULTS" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "\nAll tests passed successfully! ✓\n" << std::endl;
    std::cout << "7-Band Parametric EQ Engine verified:" << std::endl;
    std::cout << "  • Biquad filter implementation" << std::endl;
    std::cout << "  • Bell/Peak filters" << std::endl;
    std::cout << "  • Low/High shelf filters" << std::endl;
    std::cout << "  • High-pass/Low-pass filters" << std::endl;
    std::cout << "  • All-pass and notch filters" << std::endl;
    std::cout << "  • 7-band cascaded processing" << std::endl;
    std::cout << "  • Numerical stability" << std::endl;
    std::cout << "  • Sample rate adaptation" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

int main() {
    std::cout << "\n=== CHRONOS SPECTRAL WEAVER TEST SUITE ===\n" << std::endl;
    
    try {
        testBiquadBasic();
        testFilterDesign();
        testSpectralWeaverInitialization();
        testBandConfiguration();
        testProcessing();
        testBlockProcessing();
        testMultipleBands();
        testNumericalStability();
        testAllFilterTypes();
        testSampleRateChange();
        
        printTestResults();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n✗ Test failed with unknown exception" << std::endl;
        return 1;
    }
}
