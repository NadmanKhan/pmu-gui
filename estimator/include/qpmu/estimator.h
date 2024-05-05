#ifndef ESTIMATORS_H
#define ESTIMATORS_H

#include <array>
#include <complex>
#include <vector>

#include <fftw3.h>
#include <sdft/sdft.h>

#include "qpmu/common.h"

namespace qpmu {

template <typename _FloatType = void>
struct FFTW
{
};

#define SPECIALIZE(type, suffix)                                        \
  template <>                                                           \
  struct FFTW<type>                                                     \
  {                                                                     \
    using ComplexType = fftw##suffix##_complex;                         \
    using PlanType = fftw##suffix##_plan;                               \
    static constexpr auto alloc_complex = fftw##suffix##_alloc_complex; \
    static constexpr auto malloc = fftw##suffix##_malloc;               \
    static constexpr auto plan_dft_1d = fftw##suffix##_plan_dft_1d;     \
    static constexpr auto execute = fftw##suffix##_execute;             \
    static constexpr auto destroy_plan = fftw##suffix##_destroy_plan;   \
    static constexpr auto free = fftw##suffix##_free;                   \
  };

SPECIALIZE(float, f)
SPECIALIZE(double, )
SPECIALIZE(long double, l)
#undef SPECIALIZE

enum class EstimationStrategy {
    FFT, // Fast Fourier Transform
    SDFT, // Sliding-window Discrete Fourier Transform
};

class Estimator
{
public:
    // ****** Type definitions ******

    using FloatType = qpmu::FloatType;
    using SizeType = qpmu::SizeType;
    using ComplexType = qpmu::ComplexType;
    using SdftType = sdft::SDFT<FloatType, FloatType>;
    static constexpr SizeType NumChannels = qpmu::NumChannels;

    struct FftwState
    {
        FFTW<FloatType>::ComplexType *inputs[NumChannels];
        FFTW<FloatType>::ComplexType *outputs[NumChannels];
        FFTW<FloatType>::PlanType plans[NumChannels];
    };

    struct SdftState
    {
        std::vector<SdftType> workers;
        std::vector<std::vector<ComplexType>> outputs;
    };

    // ****** Constructors and destructors ******
    Estimator() : Estimator(64) { }
    Estimator(const Estimator &) = default;
    Estimator(Estimator &&) = default;
    Estimator &operator=(const Estimator &) = default;
    Estimator &operator=(Estimator &&) = default;
    ~Estimator();
    Estimator(SizeType window_size, EstimationStrategy strategy = EstimationStrategy::FFT,
              std::pair<FloatType, FloatType> voltage_params = { 1.0, 0.0 },
              std::pair<FloatType, FloatType> current_params = { 1.0, 0.0 });

    // ****** Public member functions ******
    qpmu::Estimations estimate_measurements(qpmu::AdcSample sample);

private:
    // ****** Private member functions ******

    // ****** Private member variables ******
    EstimationStrategy m_strategy = EstimationStrategy::FFT;
    SizeType m_size = 0;
    FloatType m_scale_voltage = 1.0;
    FloatType m_offset_voltage = 0.0;
    FloatType m_scale_current = 1.0;
    FloatType m_offset_current = 0.0;
    std::vector<qpmu::AdcSample> m_samples = {};
    std::vector<qpmu::Estimations> m_estimations = {};
    SizeType m_index = 0;
    FftwState m_fftw_state = {};
    SdftState m_sdft_state = {};
};

} // namespace qpmu

#endif // ESTIMATORS_H