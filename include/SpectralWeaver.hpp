#ifndef CHRONOS_SPECTRAL_WEAVER_HPP
#define CHRONOS_SPECTRAL_WEAVER_HPP

#include "Biquad.hpp"
#include "FilterDesign.hpp"
#include <array>
#include <cmath>

namespace Chronos {

/**
 * @brief EQ band configuration
 */
struct EQBand {
    FilterType type;       // Filter type for this band
    double frequency;      // Center/cutoff frequency in Hz
    double Q;              // Q-factor / bandwidth
    double gainDB;         // Gain in decibels (for bell and shelf filters)
    bool enabled;          // Band enable/disable
    
    EQBand() 
        : type(FilterType::Bell)
        , frequency(1000.0)
        , Q(0.707)
        , gainDB(0.0)
        , enabled(false) {}
};

/**
 * @brief Spectral Weaver - Professional 7-Band Parametric EQ Engine
 * 
 * High-fidelity, professional-grade parametric EQ designed for:
 * - Music production
 * - Mastering
 * - Sound design
 * 
 * Features:
 * - 7 fully parametric bands
 * - Multiple filter types per band (Bell, Low/High Shelf, HPF/LPF)
 * - 64-bit double precision processing
 * - Numerically stable implementation
 * - Click-free parameter updates
 * - Phase-coherent processing
 * - Individual band enable/disable
 * 
 * Typical band allocation:
 * Band 0: HPF or Low Shelf (20-100 Hz)
 * Band 1: Low (100-250 Hz)
 * Band 2: Low-Mid (250-600 Hz)
 * Band 3: Mid (600-2000 Hz)
 * Band 4: High-Mid (2000-5000 Hz)
 * Band 5: High (5000-12000 Hz)
 * Band 6: LPF or High Shelf (12000-20000 Hz)
 */
class SpectralWeaver {
public:
    static constexpr int NUM_BANDS = 7;
    
    /**
     * @brief Constructor
     */
    SpectralWeaver() 
        : m_sampleRate(44100.0)
        , m_bypass(false) {
        initializeDefaultBands();
    }

    /**
     * @brief Initialize with specific sample rate
     * @param sampleRate Sample rate in Hz
     */
    void initialize(double sampleRate) {
        m_sampleRate = sampleRate;
        updateAllFilters();
    }

    /**
     * @brief Set sample rate and update all filters
     * @param sampleRate Sample rate in Hz
     */
    void setSampleRate(double sampleRate) {
        if (m_sampleRate != sampleRate) {
            m_sampleRate = sampleRate;
            updateAllFilters();
        }
    }

    /**
     * @brief Get current sample rate
     * @return Sample rate in Hz
     */
    double getSampleRate() const {
        return m_sampleRate;
    }

    /**
     * @brief Configure a specific EQ band
     * @param bandIndex Band index (0-6)
     * @param type Filter type
     * @param frequency Center/cutoff frequency in Hz
     * @param Q Q-factor / bandwidth
     * @param gainDB Gain in decibels
     */
    void setBand(int bandIndex, FilterType type, double frequency, double Q, double gainDB = 0.0) {
        if (bandIndex < 0 || bandIndex >= NUM_BANDS) return;
        
        m_bands[bandIndex].type = type;
        m_bands[bandIndex].frequency = frequency;
        m_bands[bandIndex].Q = Q;
        m_bands[bandIndex].gainDB = gainDB;
        
        updateFilter(bandIndex);
    }

    /**
     * @brief Enable or disable a specific band
     * @param bandIndex Band index (0-6)
     * @param enabled Enable state
     */
    void setBandEnabled(int bandIndex, bool enabled) {
        if (bandIndex < 0 || bandIndex >= NUM_BANDS) return;
        m_bands[bandIndex].enabled = enabled;
    }

    /**
     * @brief Get band configuration
     * @param bandIndex Band index (0-6)
     * @return Reference to band configuration
     */
    const EQBand& getBand(int bandIndex) const {
        static EQBand dummy;
        if (bandIndex < 0 || bandIndex >= NUM_BANDS) return dummy;
        return m_bands[bandIndex];
    }

    /**
     * @brief Set frequency for a specific band
     * @param bandIndex Band index (0-6)
     * @param frequency Frequency in Hz
     */
    void setBandFrequency(int bandIndex, double frequency) {
        if (bandIndex < 0 || bandIndex >= NUM_BANDS) return;
        m_bands[bandIndex].frequency = frequency;
        updateFilter(bandIndex);
    }

    /**
     * @brief Set Q-factor for a specific band
     * @param bandIndex Band index (0-6)
     * @param Q Q-factor
     */
    void setBandQ(int bandIndex, double Q) {
        if (bandIndex < 0 || bandIndex >= NUM_BANDS) return;
        m_bands[bandIndex].Q = Q;
        updateFilter(bandIndex);
    }

    /**
     * @brief Set gain for a specific band
     * @param bandIndex Band index (0-6)
     * @param gainDB Gain in decibels
     */
    void setBandGain(int bandIndex, double gainDB) {
        if (bandIndex < 0 || bandIndex >= NUM_BANDS) return;
        m_bands[bandIndex].gainDB = gainDB;
        updateFilter(bandIndex);
    }

    /**
     * @brief Set filter type for a specific band
     * @param bandIndex Band index (0-6)
     * @param type Filter type
     */
    void setBandType(int bandIndex, FilterType type) {
        if (bandIndex < 0 || bandIndex >= NUM_BANDS) return;
        m_bands[bandIndex].type = type;
        updateFilter(bandIndex);
    }

    /**
     * @brief Bypass the entire EQ
     * @param bypass Bypass state
     */
    void setBypass(bool bypass) {
        m_bypass = bypass;
    }

    /**
     * @brief Get bypass state
     * @return True if bypassed
     */
    bool isBypassed() const {
        return m_bypass;
    }

    /**
     * @brief Process a single sample through all enabled bands
     * @param input Input sample
     * @return Processed output sample
     */
    double processSample(double input) {
        if (m_bypass) return input;
        
        double output = input;
        for (int i = 0; i < NUM_BANDS; ++i) {
            if (m_bands[i].enabled) {
                output = m_filters[i].process(output);
            }
        }
        return output;
    }

    /**
     * @brief Process a block of samples
     * @param input Input buffer
     * @param output Output buffer
     * @param numSamples Number of samples to process
     */
    void processBlock(const double* input, double* output, int numSamples) {
        if (m_bypass) {
            // Bypass: copy input to output
            for (int i = 0; i < numSamples; ++i) {
                output[i] = input[i];
            }
            return;
        }
        
        // Process through cascaded filters
        for (int i = 0; i < numSamples; ++i) {
            output[i] = input[i];
        }
        
        for (int band = 0; band < NUM_BANDS; ++band) {
            if (m_bands[band].enabled) {
                m_filters[band].processBlock(output, output, numSamples);
            }
        }
    }

    /**
     * @brief Reset all filter states
     */
    void reset() {
        for (auto& filter : m_filters) {
            filter.reset();
        }
    }

    /**
     * @brief Get the number of bands
     * @return Number of EQ bands
     */
    static constexpr int getNumBands() {
        return NUM_BANDS;
    }

private:
    /**
     * @brief Initialize default band configuration
     */
    void initializeDefaultBands() {
        // Band 0: High-pass filter at 30 Hz (disabled by default)
        m_bands[0].type = FilterType::HighPass;
        m_bands[0].frequency = 30.0;
        m_bands[0].Q = 0.707;
        m_bands[0].gainDB = 0.0;
        m_bands[0].enabled = false;
        
        // Band 1: Low shelf at 100 Hz
        m_bands[1].type = FilterType::LowShelf;
        m_bands[1].frequency = 100.0;
        m_bands[1].Q = 0.707;
        m_bands[1].gainDB = 0.0;
        m_bands[1].enabled = false;
        
        // Band 2: Bell at 250 Hz (low-mid)
        m_bands[2].type = FilterType::Bell;
        m_bands[2].frequency = 250.0;
        m_bands[2].Q = 0.707;
        m_bands[2].gainDB = 0.0;
        m_bands[2].enabled = false;
        
        // Band 3: Bell at 1000 Hz (mid)
        m_bands[3].type = FilterType::Bell;
        m_bands[3].frequency = 1000.0;
        m_bands[3].Q = 0.707;
        m_bands[3].gainDB = 0.0;
        m_bands[3].enabled = false;
        
        // Band 4: Bell at 3000 Hz (high-mid)
        m_bands[4].type = FilterType::Bell;
        m_bands[4].frequency = 3000.0;
        m_bands[4].Q = 0.707;
        m_bands[4].gainDB = 0.0;
        m_bands[4].enabled = false;
        
        // Band 5: High shelf at 8000 Hz
        m_bands[5].type = FilterType::HighShelf;
        m_bands[5].frequency = 8000.0;
        m_bands[5].Q = 0.707;
        m_bands[5].gainDB = 0.0;
        m_bands[5].enabled = false;
        
        // Band 6: Low-pass filter at 18000 Hz (disabled by default)
        m_bands[6].type = FilterType::LowPass;
        m_bands[6].frequency = 18000.0;
        m_bands[6].Q = 0.707;
        m_bands[6].gainDB = 0.0;
        m_bands[6].enabled = false;
        
        updateAllFilters();
    }

    /**
     * @brief Update a specific filter with current band parameters
     * @param bandIndex Band index to update
     */
    void updateFilter(int bandIndex) {
        if (bandIndex < 0 || bandIndex >= NUM_BANDS) return;
        
        const auto& band = m_bands[bandIndex];
        auto& filter = m_filters[bandIndex];
        
        switch (band.type) {
            case FilterType::Bell:
                FilterDesign::designBell(filter, m_sampleRate, band.frequency, 
                                        band.Q, band.gainDB);
                break;
                
            case FilterType::LowShelf:
                FilterDesign::designLowShelf(filter, m_sampleRate, band.frequency,
                                            band.Q, band.gainDB);
                break;
                
            case FilterType::HighShelf:
                FilterDesign::designHighShelf(filter, m_sampleRate, band.frequency,
                                             band.Q, band.gainDB);
                break;
                
            case FilterType::HighPass:
                FilterDesign::designHighPass(filter, m_sampleRate, band.frequency, band.Q);
                break;
                
            case FilterType::LowPass:
                FilterDesign::designLowPass(filter, m_sampleRate, band.frequency, band.Q);
                break;
                
            case FilterType::AllPass:
                FilterDesign::designAllPass(filter, m_sampleRate, band.frequency, band.Q);
                break;
                
            case FilterType::Notch:
                FilterDesign::designNotch(filter, m_sampleRate, band.frequency, band.Q);
                break;
        }
    }

    /**
     * @brief Update all filters
     */
    void updateAllFilters() {
        for (int i = 0; i < NUM_BANDS; ++i) {
            updateFilter(i);
        }
    }

    std::array<EQBand, NUM_BANDS> m_bands;      // Band configurations
    std::array<Biquad, NUM_BANDS> m_filters;    // Biquad filters for each band
    double m_sampleRate;                         // Current sample rate
    bool m_bypass;                               // Bypass state
};

} // namespace Chronos

#endif // CHRONOS_SPECTRAL_WEAVER_HPP
