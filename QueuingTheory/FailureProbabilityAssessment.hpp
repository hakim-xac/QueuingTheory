namespace KHAS {
    template <typename T>
    inline void FailureProbabilityAssessment::writeToStream(std::stringstream& out, const std::string& header, size_t width_delim, T&& value) const noexcept {
        auto delim{ delimiter('-', width_delim) };
        out << delim << "\n| " << std::setw(28) << std::left << header;
        out << " | " << std::setw(width_delim - 35) << std::right << std::fixed << std::setprecision(3) << std::forward<T>(value) << " |\n" << delim << "\n";
    }

    template <typename Cont>
    inline void FailureProbabilityAssessment::writeToStream(std::stringstream& out, const std::string& header, const Cont& cont, WriteToStreamType wt, size_t width_delim) const noexcept {

        auto delim{ delimiter('-', width_delim) };
        out << "| " << std::setw(28) << std::left << header;
        for (const auto& elem : cont)
        {
            if (wt == WriteToStreamType::Header) {
                out << " | " << std::setw(11) << std::right << std::setprecision(2) << std::fixed << elem;
            }
            else if (wt == WriteToStreamType::Tests) {
                out << " | " << std::setw(4) << std::right << elem;
            }
        }
        out << " |\n" << delim << "\n";
    }
}