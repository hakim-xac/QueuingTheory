#pragma once

#include <numeric>
#include <iostream>
#include <iomanip>
#include <random>
#include "InputData.h"

namespace KHAS {
    class FailureProbabilityAssessment final {

        enum class WriteToStreamType { Header, Tests };


    public:
        explicit FailureProbabilityAssessment(const InputData& data) noexcept;
        void showCalculationResult() const noexcept;

    private:

        inline size_t writeToStreamTop(std::stringstream& out, const std::string& header, size_t count) const noexcept;
        inline void writeToStreamTop(std::stringstream& out, const std::string& header, size_t count, size_t width_delim) const noexcept;

        template <typename T>
        inline void writeToStream(std::stringstream& out, const std::string& header, size_t width_delim, T&& value) const noexcept;

        template <typename Cont>
        inline void writeToStream(std::stringstream& out, const std::string& header, const Cont& cont, WriteToStreamType wt, size_t width_delim) const noexcept;

        inline void BreakConnect() noexcept;

        inline double randomRealGenerate(double lhs, double rhs) const noexcept;

        inline size_t  calcConnectedComp() const noexcept;

        std::pair<size_t, size_t> Step() noexcept;

        void startTests() noexcept;

        inline std::string delimiter(char c, size_t width) const noexcept;


    private:
        InputData data_;
        std::vector<size_t> connect_;
        std::vector<size_t> denial_;
        std::vector<size_t> connect_state_;
        double failure_probability_{};
        double left_span_{};
        double right_span_{};
    };
}

#include "FailureProbabilityAssessment.hpp"