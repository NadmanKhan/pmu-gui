#ifndef QPMU_COMMON_H
#define QPMU_COMMON_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <complex>
#include <string>

namespace qpmu {

constexpr auto OrgName = PROJECT_ORG_NAME;
constexpr auto AppName = PROJECT_APP_NAME;
constexpr auto AppDisplayName = PROJECT_APP_NAME;

using Float = FLOAT_TYPE;
using Complex = std::complex<Float>;
using USize = std::size_t;
using U64 = std::uint64_t;
using U32 = std::uint16_t;
using U16 = std::uint16_t;
using ISize = ssize_t;
using I64 = std::int64_t;
using I32 = std::int16_t;
using I16 = std::int16_t;

/// Voltage, Current
constexpr USize CountSignalTypes = 2;

/// A, B, C
constexpr USize CountSignalPhases = 3;

/// 3 phases * 2 types = 6 channels
constexpr USize CountSignals = CountSignalPhases * CountSignalTypes;

/// Voltage, Current
constexpr std::array<char const *, CountSignalTypes> SignalTypeNames = { "Voltage", "Current" };

/// V for Voltage, I for Current
constexpr std::array<char, CountSignalTypes> SignalTypeSymbols = { 'V', 'I' };

/// Volts, Amperes
constexpr std::array<char const *, CountSignalTypes> SignalTypeUnitNames = { "Volts", "Amperes" };

/// V for Volts, A for Amperes
constexpr std::array<char, CountSignalTypes> SignalTypeUnitSymbols = { 'V', 'A' };

/// A, B, C
constexpr std::array<char, CountSignalPhases> SignalPhaseNames = { 'A', 'B', 'C' };

/// { { VA, IA }, { VB, IB }, { VC, IC } }
constexpr std::array<std::pair<USize, USize>, CountSignalPhases> SignalPhasePairs = {
    { { 0, 3 }, { 1, 4 }, { 2, 5 } }
};

/// V for Volts, A for Amperes
constexpr std::array<char, CountSignalTypes> SignalTypeUnit = { 'V', 'A' };

/// Number of fields in a sample generated by the Analog-to-Digital Converter (ADC) driver
constexpr USize CountSampleFields = 9;

/// Names of fields in a sample generated by the Analog-to-Digital Converter (ADC) driver
constexpr std::array<char const *, CountSampleFields> SampleFieldNames = {
    "Sequence number", "VA", "VB", "VC", "IA", "IB", "IC", "Timestamp (µs)", "Time-delta (µs)"
};

/// Types of fields in a sample generated by the Analog-to-Digital Converter (ADC) driver
enum class SampleField {

    /// Sequence number (e.g., 1, 2, 3, ...)
    /// to identify a sample
    SequenceNumber = 0,

    /// Voltage of phase A
    VA,

    /// Voltage of phase B
    VB,

    /// Voltage of phase C
    VC,

    /// Current of phase A
    IA,

    /// Current of phase B
    IB,

    /// Current of phase C
    IC,

    /// Timestamp (in microseconds)
    Timestamp,

    /// Time difference from the previous sample (in microseconds)
    TimeDelta
};

/// Shape (i.e. field order) of a sample generated by the Analog-to-Digital Converter (ADC) driver
using SampleFieldOrderingConfig = std::array<SampleField, CountSampleFields>;

/// Sample generated by the Analog-to-Digital Converter (ADC) driver
struct Sample
{
    /// Sequence number
    U64 seqNo;

    /// Channel values
    std::array<U64, CountSignals> channels;

    /// Timestamp (in microseconds)
    U64 timestampUs;

    /// Timestamp difference from previous sample (in microseconds)
    U64 timeDeltaUs;
};

struct Signal
{
    /// Signal index
    USize index;

    /// Signal name ("VA", "VB", "VC", "IA", "IB", "IC")
    char const *name;

    /// Signal type (Voltage or Current)
    enum class Type { Voltage, Current } type;

    /// Signal phase (A, B, or C)
    enum class Phase { A, B, C } phase;
};

/// Estimation of phasors, frequency, and rate of change of frequency
struct Synchrophasor
{
    /// Timestamp (in microseconds)
    U64 timestampUs;

    /// Phasor magnitudes
    std::array<Float, CountSignals> magnitudes;

    /// Phasor angles (in radians)
    std::array<Float, CountSignals> phaseAngles;

    /// Frequency (in Hz)
    Float frequency;

    /// Rate of change of frequency (in Hz/s)
    Float rocof;
};

constexpr std::array<Signal, CountSignals> Signals = {
    { { 0, "VA", Signal::Type::Voltage, Signal::Phase::A },
      { 1, "VB", Signal::Type::Voltage, Signal::Phase::B },
      { 2, "VC", Signal::Type::Voltage, Signal::Phase::C },
      { 3, "IA", Signal::Type::Current, Signal::Phase::A },
      { 4, "IB", Signal::Type::Current, Signal::Phase::B },
      { 5, "IC", Signal::Type::Current, Signal::Phase::C } }
};

/// Order of fields in a sample generated by the ADC driver
constexpr SampleFieldOrderingConfig SampleFieldOrdering = { SampleField::SequenceNumber,
                                                            SampleField::VA,
                                                            SampleField::VB,
                                                            SampleField::VC,
                                                            SampleField::IA,
                                                            SampleField::IB,
                                                            SampleField::IC,
                                                            SampleField::Timestamp,
                                                            SampleField::TimeDelta };

namespace util {

std::string phasor_to_string(const Complex &phasor);

std::string phasor_polar_to_string(const Complex &phasor);

std::string csv_header_for_synchrophasor();

std::string to_csv(const Synchrophasor &synchrophasor);

std::string to_string(const Synchrophasor &synchrophasor);

bool parse_as_sample(Sample &out_sample, const char *const s,
                     const SampleFieldOrderingConfig &shape = SampleFieldOrdering);

std::string csv_header_for_sample();

std::string to_csv(const Sample &sample);

std::string to_string(const Sample &sample);

} // namespace util
} // namespace qpmu

#endif // QPMU_COMMON_H