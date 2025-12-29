#ifndef CHRONOS_FILTER_DESIGN_HPP
#define CHRONOS_FILTER_DESIGN_HPP

#include <cmath>
#include <algorithm>
#include "Biquad.hpp"

namespace Chronos {

/**
 * @brief Filter types supported by the parametric EQ
 */
enum class FilterType {
    Bell,        // Parametric peak/bell filter
    LowShelf,    // Low frequency shelf
    HighShelf,   // High frequency shelf
    LowPass,     // Low-pass filter (LPF)
    HighPass,    // High-pass filter (HPF)
    AllPass,     // All-pass filter (phase adjustment)
    Notch        // Notch filter
};

/**
 * @brief Professional filter coefficient calculator
 * 
 * Implements industry-standard cookbook formulas with careful attention to:
 * - Numerical stability across all parameter ranges
 * - Proper frequency warping for bilinear transform
 * - Correct Q-factor handling
 * - Gain staging for shelving filters
 */
class FilterDesign {
public:
    static constexpr double PI = 3.14159265358979323846;
    static constexpr double MIN_Q = 0.1;
    static constexpr double MAX_Q = 18.0;

    /**
     * @brief Calculate biquad coefficients for a bell/peak filter
     * @param biquad Biquad filter to configure
     * @param sampleRate Sample rate in Hz
     * @param frequency Center frequency in Hz
     * @param Q Q-factor (bandwidth control)
     * @param gainDB Gain in decibels
     */
    static void designBell(Biquad& biquad, double sampleRate, double frequency, 
                          double Q, double gainDB) {
        Q = std::clamp(Q, MIN_Q, MAX_Q);
        frequency = std::clamp(frequency, 1.0, sampleRate * 0.49);
        
        double A = std::pow(10.0, gainDB / 40.0);  // sqrt of linear gain
        double omega = 2.0 * PI * frequency / sampleRate;
        double sn = std::sin(omega);
        double cs = std::cos(omega);
        double alpha = sn / (2.0 * Q);
        
        double b0 = 1.0 + alpha * A;
        double b1 = -2.0 * cs;
        double b2 = 1.0 - alpha * A;
        double a0 = 1.0 + alpha / A;
        double a1 = -2.0 * cs;
        double a2 = 1.0 - alpha / A;
        
        // Normalize by a0
        biquad.setCoefficients(b0/a0, b1/a0, b2/a0, a1/a0, a2/a0);
    }

    /**
     * @brief Calculate biquad coefficients for a low shelf filter
     * @param biquad Biquad filter to configure
     * @param sampleRate Sample rate in Hz
     * @param frequency Transition frequency in Hz
     * @param Q Slope control (typically 0.707 for smooth transition)
     * @param gainDB Gain in decibels
     */
    static void designLowShelf(Biquad& biquad, double sampleRate, double frequency,
                               double Q, double gainDB) {
        Q = std::clamp(Q, MIN_Q, MAX_Q);
        frequency = std::clamp(frequency, 1.0, sampleRate * 0.49);
        
        double A = std::pow(10.0, gainDB / 40.0);
        double omega = 2.0 * PI * frequency / sampleRate;
        double sn = std::sin(omega);
        double cs = std::cos(omega);
        double beta = std::sqrt(A) / Q;
        
        double b0 = A * ((A + 1.0) - (A - 1.0) * cs + beta * sn);
        double b1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * cs);
        double b2 = A * ((A + 1.0) - (A - 1.0) * cs - beta * sn);
        double a0 = (A + 1.0) + (A - 1.0) * cs + beta * sn;
        double a1 = -2.0 * ((A - 1.0) + (A + 1.0) * cs);
        double a2 = (A + 1.0) + (A - 1.0) * cs - beta * sn;
        
        biquad.setCoefficients(b0/a0, b1/a0, b2/a0, a1/a0, a2/a0);
    }

    /**
     * @brief Calculate biquad coefficients for a high shelf filter
     * @param biquad Biquad filter to configure
     * @param sampleRate Sample rate in Hz
     * @param frequency Transition frequency in Hz
     * @param Q Slope control (typically 0.707 for smooth transition)
     * @param gainDB Gain in decibels
     */
    static void designHighShelf(Biquad& biquad, double sampleRate, double frequency,
                                double Q, double gainDB) {
        Q = std::clamp(Q, MIN_Q, MAX_Q);
        frequency = std::clamp(frequency, 1.0, sampleRate * 0.49);
        
        double A = std::pow(10.0, gainDB / 40.0);
        double omega = 2.0 * PI * frequency / sampleRate;
        double sn = std::sin(omega);
        double cs = std::cos(omega);
        double beta = std::sqrt(A) / Q;
        
        double b0 = A * ((A + 1.0) + (A - 1.0) * cs + beta * sn);
        double b1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * cs);
        double b2 = A * ((A + 1.0) + (A - 1.0) * cs - beta * sn);
        double a0 = (A + 1.0) - (A - 1.0) * cs + beta * sn;
        double a1 = 2.0 * ((A - 1.0) - (A + 1.0) * cs);
        double a2 = (A + 1.0) - (A - 1.0) * cs - beta * sn;
        
        biquad.setCoefficients(b0/a0, b1/a0, b2/a0, a1/a0, a2/a0);
    }

    /**
     * @brief Calculate biquad coefficients for a high-pass filter
     * @param biquad Biquad filter to configure
     * @param sampleRate Sample rate in Hz
     * @param frequency Cutoff frequency in Hz
     * @param Q Resonance control
     */
    static void designHighPass(Biquad& biquad, double sampleRate, double frequency, double Q) {
        Q = std::clamp(Q, MIN_Q, MAX_Q);
        frequency = std::clamp(frequency, 1.0, sampleRate * 0.49);
        
        double omega = 2.0 * PI * frequency / sampleRate;
        double sn = std::sin(omega);
        double cs = std::cos(omega);
        double alpha = sn / (2.0 * Q);
        
        double b0 = (1.0 + cs) / 2.0;
        double b1 = -(1.0 + cs);
        double b2 = (1.0 + cs) / 2.0;
        double a0 = 1.0 + alpha;
        double a1 = -2.0 * cs;
        double a2 = 1.0 - alpha;
        
        biquad.setCoefficients(b0/a0, b1/a0, b2/a0, a1/a0, a2/a0);
    }

    /**
     * @brief Calculate biquad coefficients for a low-pass filter
     * @param biquad Biquad filter to configure
     * @param sampleRate Sample rate in Hz
     * @param frequency Cutoff frequency in Hz
     * @param Q Resonance control
     */
    static void designLowPass(Biquad& biquad, double sampleRate, double frequency, double Q) {
        Q = std::clamp(Q, MIN_Q, MAX_Q);
        frequency = std::clamp(frequency, 1.0, sampleRate * 0.49);
        
        double omega = 2.0 * PI * frequency / sampleRate;
        double sn = std::sin(omega);
        double cs = std::cos(omega);
        double alpha = sn / (2.0 * Q);
        
        double b0 = (1.0 - cs) / 2.0;
        double b1 = 1.0 - cs;
        double b2 = (1.0 - cs) / 2.0;
        double a0 = 1.0 + alpha;
        double a1 = -2.0 * cs;
        double a2 = 1.0 - alpha;
        
        biquad.setCoefficients(b0/a0, b1/a0, b2/a0, a1/a0, a2/a0);
    }

    /**
     * @brief Calculate biquad coefficients for an all-pass filter
     * @param biquad Biquad filter to configure
     * @param sampleRate Sample rate in Hz
     * @param frequency Center frequency in Hz
     * @param Q Q-factor
     */
    static void designAllPass(Biquad& biquad, double sampleRate, double frequency, double Q) {
        Q = std::clamp(Q, MIN_Q, MAX_Q);
        frequency = std::clamp(frequency, 1.0, sampleRate * 0.49);
        
        double omega = 2.0 * PI * frequency / sampleRate;
        double sn = std::sin(omega);
        double cs = std::cos(omega);
        double alpha = sn / (2.0 * Q);
        
        double b0 = 1.0 - alpha;
        double b1 = -2.0 * cs;
        double b2 = 1.0 + alpha;
        double a0 = 1.0 + alpha;
        double a1 = -2.0 * cs;
        double a2 = 1.0 - alpha;
        
        biquad.setCoefficients(b0/a0, b1/a0, b2/a0, a1/a0, a2/a0);
    }

    /**
     * @brief Calculate biquad coefficients for a notch filter
     * @param biquad Biquad filter to configure
     * @param sampleRate Sample rate in Hz
     * @param frequency Center frequency in Hz
     * @param Q Bandwidth control
     */
    static void designNotch(Biquad& biquad, double sampleRate, double frequency, double Q) {
        Q = std::clamp(Q, MIN_Q, MAX_Q);
        frequency = std::clamp(frequency, 1.0, sampleRate * 0.49);
        
        double omega = 2.0 * PI * frequency / sampleRate;
        double sn = std::sin(omega);
        double cs = std::cos(omega);
        double alpha = sn / (2.0 * Q);
        
        double b0 = 1.0;
        double b1 = -2.0 * cs;
        double b2 = 1.0;
        double a0 = 1.0 + alpha;
        double a1 = -2.0 * cs;
        double a2 = 1.0 - alpha;
        
        biquad.setCoefficients(b0/a0, b1/a0, b2/a0, a1/a0, a2/a0);
    }
};

} // namespace Chronos

#endif // CHRONOS_FILTER_DESIGN_HPP
