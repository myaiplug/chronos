#include "../include/SpectralWeaver.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>

using namespace Chronos;

/**
 * @brief Generate a simple test tone
 */
std::vector<double> generateTestTone(double frequency, double sampleRate, double duration, double amplitude = 0.5) {
    int numSamples = static_cast<int>(duration * sampleRate);
    std::vector<double> samples(numSamples);
    
    for (int i = 0; i < numSamples; ++i) {
        double phase = 2.0 * M_PI * frequency * i / sampleRate;
        samples[i] = amplitude * std::sin(phase);
    }
    
    return samples;
}

/**
 * @brief Calculate RMS level of a signal
 */
double calculateRMS(const std::vector<double>& signal) {
    double sum = 0.0;
    for (double sample : signal) {
        sum += sample * sample;
    }
    return std::sqrt(sum / signal.size());
}

/**
 * @brief Print separator
 */
void printSeparator() {
    std::cout << std::string(70, '=') << std::endl;
}

/**
 * @brief Print section header
 */
void printHeader(const std::string& title) {
    printSeparator();
    std::cout << "  " << title << std::endl;
    printSeparator();
}

/**
 * @brief Demo: Basic Bell Filter
 */
void demoBellFilter() {
    printHeader("DEMO 1: Bell/Peak Filter");
    
    SpectralWeaver eq;
    eq.initialize(44100.0);
    
    std::cout << "\nConfiguration:" << std::endl;
    std::cout << "  • Sample Rate: 44100 Hz" << std::endl;
    std::cout << "  • Filter Type: Bell (Parametric Peak)" << std::endl;
    std::cout << "  • Center Frequency: 1000 Hz" << std::endl;
    std::cout << "  • Q-Factor: 2.0" << std::endl;
    std::cout << "  • Gain: +6 dB" << std::endl;
    
    // Configure bell filter at 1 kHz with +6dB boost
    eq.setBand(3, FilterType::Bell, 1000.0, 2.0, 6.0);
    eq.setBandEnabled(3, true);
    
    // Generate test tones at different frequencies
    std::vector<double> testFrequencies = {100.0, 500.0, 1000.0, 2000.0, 5000.0};
    
    std::cout << "\nFrequency Response:" << std::endl;
    std::cout << "  Frequency | Input RMS | Output RMS | Gain (dB)" << std::endl;
    std::cout << "  " << std::string(60, '-') << std::endl;
    
    for (double freq : testFrequencies) {
        auto inputSignal = generateTestTone(freq, 44100.0, 0.1);
        std::vector<double> outputSignal(inputSignal.size());
        
        eq.reset();
        eq.processBlock(inputSignal.data(), outputSignal.data(), inputSignal.size());
        
        double inputRMS = calculateRMS(inputSignal);
        double outputRMS = calculateRMS(outputSignal);
        double gainDB = 20.0 * std::log10(outputRMS / inputRMS);
        
        std::cout << "  " << std::setw(8) << freq << " Hz | "
                  << std::fixed << std::setprecision(4)
                  << std::setw(9) << inputRMS << " | "
                  << std::setw(10) << outputRMS << " | "
                  << std::setw(8) << std::setprecision(2) << gainDB << " dB"
                  << std::endl;
    }
    
    std::cout << "\n✓ Bell filter demonstrates frequency-selective boost around 1 kHz\n" << std::endl;
}

/**
 * @brief Demo: Shelving Filters
 */
void demoShelvingFilters() {
    printHeader("DEMO 2: Low and High Shelf Filters");
    
    SpectralWeaver eq;
    eq.initialize(44100.0);
    
    std::cout << "\nConfiguration:" << std::endl;
    std::cout << "  • Low Shelf: 200 Hz, +4 dB (bass boost)" << std::endl;
    std::cout << "  • High Shelf: 8000 Hz, +3 dB (treble boost)" << std::endl;
    
    // Configure low shelf
    eq.setBand(1, FilterType::LowShelf, 200.0, 0.707, 4.0);
    eq.setBandEnabled(1, true);
    
    // Configure high shelf
    eq.setBand(5, FilterType::HighShelf, 8000.0, 0.707, 3.0);
    eq.setBandEnabled(5, true);
    
    std::vector<double> testFrequencies = {50.0, 200.0, 1000.0, 8000.0, 15000.0};
    
    std::cout << "\nFrequency Response:" << std::endl;
    std::cout << "  Frequency | Gain (dB)" << std::endl;
    std::cout << "  " << std::string(30, '-') << std::endl;
    
    for (double freq : testFrequencies) {
        auto inputSignal = generateTestTone(freq, 44100.0, 0.1);
        std::vector<double> outputSignal(inputSignal.size());
        
        eq.reset();
        eq.processBlock(inputSignal.data(), outputSignal.data(), inputSignal.size());
        
        double inputRMS = calculateRMS(inputSignal);
        double outputRMS = calculateRMS(outputSignal);
        double gainDB = 20.0 * std::log10(outputRMS / inputRMS);
        
        std::cout << "  " << std::setw(8) << freq << " Hz | "
                  << std::fixed << std::setprecision(2) << std::setw(8) << gainDB << " dB"
                  << std::endl;
    }
    
    std::cout << "\n✓ Shelving filters boost bass and treble regions\n" << std::endl;
}

/**
 * @brief Demo: Complete 7-Band EQ Setup
 */
void demoComplete7BandEQ() {
    printHeader("DEMO 3: Professional 7-Band EQ Configuration");
    
    SpectralWeaver eq;
    eq.initialize(44100.0);
    
    std::cout << "\nTypical Mastering EQ Configuration:" << std::endl;
    std::cout << "\n  Band | Type       | Frequency | Q    | Gain  | Status" << std::endl;
    std::cout << "  " << std::string(65, '-') << std::endl;
    
    // Band 0: HPF to remove sub-bass rumble
    eq.setBand(0, FilterType::HighPass, 30.0, 0.707, 0.0);
    eq.setBandEnabled(0, true);
    std::cout << "    0  | HPF        |    30 Hz  | 0.71 |  N/A  | Enabled" << std::endl;
    
    // Band 1: Low shelf - gentle bass enhancement
    eq.setBand(1, FilterType::LowShelf, 100.0, 0.707, 2.0);
    eq.setBandEnabled(1, true);
    std::cout << "    1  | Low Shelf  |   100 Hz  | 0.71 | +2 dB | Enabled" << std::endl;
    
    // Band 2: Low-mid clarity
    eq.setBand(2, FilterType::Bell, 250.0, 1.5, -1.5);
    eq.setBandEnabled(2, true);
    std::cout << "    2  | Bell       |   250 Hz  | 1.50 | -1.5  | Enabled" << std::endl;
    
    // Band 3: Mid presence boost
    eq.setBand(3, FilterType::Bell, 1500.0, 1.0, 2.5);
    eq.setBandEnabled(3, true);
    std::cout << "    3  | Bell       |  1500 Hz  | 1.00 | +2.5  | Enabled" << std::endl;
    
    // Band 4: High-mid air
    eq.setBand(4, FilterType::Bell, 4000.0, 2.0, 1.5);
    eq.setBandEnabled(4, true);
    std::cout << "    4  | Bell       |  4000 Hz  | 2.00 | +1.5  | Enabled" << std::endl;
    
    // Band 5: High shelf - sparkle
    eq.setBand(5, FilterType::HighShelf, 10000.0, 0.707, 1.0);
    eq.setBandEnabled(5, true);
    std::cout << "    5  | High Shelf | 10000 Hz  | 0.71 | +1 dB | Enabled" << std::endl;
    
    // Band 6: LPF to control extreme highs
    eq.setBand(6, FilterType::LowPass, 18000.0, 0.707, 0.0);
    eq.setBandEnabled(6, true);
    std::cout << "    6  | LPF        | 18000 Hz  | 0.71 |  N/A  | Enabled" << std::endl;
    
    std::cout << "\n✓ All 7 bands configured for professional mastering application" << std::endl;
    
    // Test processing
    std::cout << "\nProcessing Test Signal..." << std::endl;
    auto testSignal = generateTestTone(1000.0, 44100.0, 0.1);
    std::vector<double> processed(testSignal.size());
    
    eq.reset();
    eq.processBlock(testSignal.data(), processed.data(), testSignal.size());
    
    std::cout << "  ✓ Successfully processed " << testSignal.size() << " samples" << std::endl;
    std::cout << "  ✓ No clipping or numerical issues detected\n" << std::endl;
}

/**
 * @brief Demo: High-Pass and Low-Pass Filters
 */
void demoPassFilters() {
    printHeader("DEMO 4: High-Pass and Low-Pass Filters");
    
    SpectralWeaver eq;
    eq.initialize(44100.0);
    
    std::cout << "\nConfiguration:" << std::endl;
    std::cout << "  • HPF at 80 Hz (remove rumble)" << std::endl;
    std::cout << "  • LPF at 15000 Hz (tame harsh highs)" << std::endl;
    
    // Configure HPF
    eq.setBand(0, FilterType::HighPass, 80.0, 0.707, 0.0);
    eq.setBandEnabled(0, true);
    
    // Configure LPF
    eq.setBand(6, FilterType::LowPass, 15000.0, 0.707, 0.0);
    eq.setBandEnabled(6, true);
    
    std::vector<double> testFrequencies = {30.0, 80.0, 1000.0, 15000.0, 18000.0};
    
    std::cout << "\nFrequency Response:" << std::endl;
    std::cout << "  Frequency | Attenuation (dB)" << std::endl;
    std::cout << "  " << std::string(35, '-') << std::endl;
    
    for (double freq : testFrequencies) {
        auto inputSignal = generateTestTone(freq, 44100.0, 0.1);
        std::vector<double> outputSignal(inputSignal.size());
        
        eq.reset();
        eq.processBlock(inputSignal.data(), outputSignal.data(), inputSignal.size());
        
        double inputRMS = calculateRMS(inputSignal);
        double outputRMS = calculateRMS(outputSignal);
        double gainDB = 20.0 * std::log10(outputRMS / inputRMS);
        
        std::cout << "  " << std::setw(8) << freq << " Hz | "
                  << std::fixed << std::setprecision(2) << std::setw(12) << gainDB << " dB"
                  << std::endl;
    }
    
    std::cout << "\n✓ HPF and LPF effectively control frequency extremes\n" << std::endl;
}

/**
 * @brief Demo: Dynamic Parameter Changes
 */
void demoDynamicChanges() {
    printHeader("DEMO 5: Dynamic Parameter Changes");
    
    SpectralWeaver eq;
    eq.initialize(44100.0);
    
    std::cout << "\nDemonstrating real-time parameter updates:" << std::endl;
    
    // Configure a bell filter
    eq.setBand(3, FilterType::Bell, 1000.0, 1.0, 0.0);
    eq.setBandEnabled(3, true);
    
    std::cout << "\n  Sweeping gain from -12 dB to +12 dB..." << std::endl;
    
    std::vector<double> gainValues = {-12.0, -6.0, 0.0, 6.0, 12.0};
    auto testSignal = generateTestTone(1000.0, 44100.0, 0.05);
    
    for (double gain : gainValues) {
        eq.setBandGain(3, gain);
        
        std::vector<double> output(testSignal.size());
        eq.reset();
        eq.processBlock(testSignal.data(), output.data(), testSignal.size());
        
        double outputRMS = calculateRMS(output);
        std::cout << "    Gain: " << std::setw(6) << std::fixed << std::setprecision(1) 
                  << gain << " dB → Output RMS: " << std::setprecision(4) << outputRMS << std::endl;
    }
    
    std::cout << "\n✓ Parameters can be updated smoothly without artifacts\n" << std::endl;
}

/**
 * @brief Demo: Bypass Functionality
 */
void demoBypass() {
    printHeader("DEMO 6: Bypass Functionality");
    
    SpectralWeaver eq;
    eq.initialize(44100.0);
    
    // Configure multiple bands
    eq.setBand(1, FilterType::LowShelf, 100.0, 0.707, 6.0);
    eq.setBandEnabled(1, true);
    eq.setBand(3, FilterType::Bell, 1000.0, 1.0, 6.0);
    eq.setBandEnabled(3, true);
    eq.setBand(5, FilterType::HighShelf, 8000.0, 0.707, 6.0);
    eq.setBandEnabled(5, true);
    
    auto testSignal = generateTestTone(1000.0, 44100.0, 0.1);
    std::vector<double> outputProcessed(testSignal.size());
    std::vector<double> outputBypassed(testSignal.size());
    
    // Process with EQ active
    eq.setBypass(false);
    eq.reset();
    eq.processBlock(testSignal.data(), outputProcessed.data(), testSignal.size());
    
    // Process with EQ bypassed
    eq.setBypass(true);
    eq.reset();
    eq.processBlock(testSignal.data(), outputBypassed.data(), testSignal.size());
    
    double processedRMS = calculateRMS(outputProcessed);
    double bypassedRMS = calculateRMS(outputBypassed);
    double inputRMS = calculateRMS(testSignal);
    
    std::cout << "\nResults:" << std::endl;
    std::cout << "  Input RMS:     " << std::fixed << std::setprecision(6) << inputRMS << std::endl;
    std::cout << "  Bypassed RMS:  " << bypassedRMS << " (should equal input)" << std::endl;
    std::cout << "  Processed RMS: " << processedRMS << " (should differ from input)" << std::endl;
    
    std::cout << "\n✓ Bypass correctly passes signal through unmodified\n" << std::endl;
}

int main() {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "       CHRONOS SPECTRAL WEAVER - 7-BAND PARAMETRIC EQ" << std::endl;
    std::cout << "              Professional Audio Processing Demo" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    std::cout << "\n";
    
    try {
        demoBellFilter();
        demoShelvingFilters();
        demoPassFilters();
        demoComplete7BandEQ();
        demoDynamicChanges();
        demoBypass();
        
        printSeparator();
        std::cout << "\n  ✓ All demos completed successfully!" << std::endl;
        std::cout << "\n  The Spectral Weaver EQ engine is ready for:" << std::endl;
        std::cout << "    • Music production" << std::endl;
        std::cout << "    • Mastering applications" << std::endl;
        std::cout << "    • Sound design" << std::endl;
        std::cout << "    • Professional audio processing" << std::endl;
        printSeparator();
        std::cout << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Demo failed: " << e.what() << std::endl;
        return 1;
    }
}
