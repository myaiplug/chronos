#ifndef CHRONOS_BIQUAD_HPP
#define CHRONOS_BIQUAD_HPP

#include <cmath>
#include <algorithm>

namespace Chronos {

/**
 * @brief Professional-grade biquad filter implementation with numerical stability
 * 
 * This implementation uses Direct Form II Transposed structure which provides:
 * - Better numerical stability than Direct Form I
 * - Lower coefficient sensitivity
 * - Reduced quantization noise
 * 
 * Transfer function: H(z) = (b0 + b1*z^-1 + b2*z^-2) / (1 + a1*z^-1 + a2*z^-2)
 */
class Biquad {
public:
    Biquad() {
        reset();
        setCoefficients(1.0, 0.0, 0.0, 0.0, 0.0);
    }

    /**
     * @brief Set filter coefficients
     * @param b0 Feedforward coefficient 0
     * @param b1 Feedforward coefficient 1
     * @param b2 Feedforward coefficient 2
     * @param a1 Feedback coefficient 1 (negated in difference equation)
     * @param a2 Feedback coefficient 2 (negated in difference equation)
     */
    void setCoefficients(double b0, double b1, double b2, double a1, double a2) {
        m_b0 = b0;
        m_b1 = b1;
        m_b2 = b2;
        m_a1 = a1;
        m_a2 = a2;
    }

    /**
     * @brief Process a single sample using Direct Form II Transposed
     * @param input Input sample
     * @return Filtered output sample
     */
    double process(double input) {
        // Direct Form II Transposed implementation
        // This structure minimizes numerical errors
        double output = m_b0 * input + m_z1;
        m_z1 = m_b1 * input - m_a1 * output + m_z2;
        m_z2 = m_b2 * input - m_a2 * output;
        
        return output;
    }

    /**
     * @brief Reset filter state (clear delay lines)
     */
    void reset() {
        m_z1 = 0.0;
        m_z2 = 0.0;
    }

    /**
     * @brief Process a block of samples
     * @param input Input buffer
     * @param output Output buffer
     * @param numSamples Number of samples to process
     */
    void processBlock(const double* input, double* output, int numSamples) {
        for (int i = 0; i < numSamples; ++i) {
            output[i] = process(input[i]);
        }
    }

private:
    // Coefficients
    double m_b0, m_b1, m_b2;  // Feedforward coefficients
    double m_a1, m_a2;         // Feedback coefficients (a0 is normalized to 1)
    
    // State variables (delay lines) for Direct Form II Transposed
    double m_z1, m_z2;
};

} // namespace Chronos

#endif // CHRONOS_BIQUAD_HPP
