namespace KHAS {
    template <typename T, typename Func>
    bool InputData::readFromStream(T& container, std::ifstream& fs, const size_t cols, const size_t rows, Func func) noexcept {

        using value_type = typename T::value_type;
        container.reserve(cols);
        std::string tmp{};
        fs.get();
        std::getline(fs, tmp);
        std::getline(fs, tmp);

        for (size_t i{}, ie{ cols }; i != ie; ++i) {
            value_type sum{};
            value_type tmp;
            for (size_t j{}, je{ rows }; j != je; ++j) {
                fs >> tmp;
                sum += tmp;
                if (fs.fail()) return false;
            }
            container.emplace_back(func(sum));
        }
        return true;
    }

    template <typename T>
    inline std::pair<T, size_t> InputData::readFromStream(std::ifstream& fs) const noexcept {
        using value_type = T;

        std::string tmp;
        value_type result{};
        if (fs >> result) std::getline(fs, tmp);

        return { result, fs.good() };
    }
}