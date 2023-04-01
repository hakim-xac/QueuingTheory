#include "FailureProbabilityAssessment.h"

namespace KHAS {

    FailureProbabilityAssessment::FailureProbabilityAssessment(const InputData& data) noexcept
        : data_{ data }
    {
        connect_.reserve(data_.getNumberOfTests());
        denial_.reserve(data_.getNumberOfTests());
        connect_state_.resize(data_.getNumberOfTests());
        startTests();
    }

    void FailureProbabilityAssessment::showCalculationResult() const noexcept {
        std::setlocale(LC_ALL, "Russian");
        std::stringstream buf;

        size_t width_delim{ writeToStreamTop(buf, std::string("Компьютер"), data_.getNumberOfMachines()) };

        writeToStream(buf, std::string("Интенсивности подключекния")
            , data_.getNumberOfConnections(), WriteToStreamType::Header, width_delim);

        writeToStream(buf, std::string("Интенсивности освобождения")
            , data_.getWorkingHours(), WriteToStreamType::Header, width_delim);

        writeToStreamTop(buf, std::string("Номер опыта"), data_.getNumberOfTests(), width_delim);

        writeToStream(buf, std::string("Попытки"), connect_, WriteToStreamType::Tests, width_delim);
        writeToStream(buf, std::string("Отказы"), denial_, WriteToStreamType::Tests, width_delim);

        writeToStream(buf, std::string("Вероятнеость отказа:"), width_delim, failure_probability_);
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3) << "(" << left_span_ << "; " << right_span_ << ")";
        writeToStream(buf, std::string("Доверительный интервал:"), width_delim, ss.str());

        std::cout << buf.str();

    }

    inline size_t FailureProbabilityAssessment::writeToStreamTop(std::stringstream& out, const std::string& header, size_t count) const noexcept {

        out << "| " << std::setw(28) << std::left << header;
        for (size_t i{}, ie{ count }; i != ie; ++i) out << " | " << std::setw(11) << std::right << i + 1;
        out << " |";

        size_t width_delim = out.tellp();

        auto delim{ delimiter('-', width_delim) };
        auto top{ out.str() };
        out.str("");

        out << delim << "\n"
            << "| " << std::setw(28) << std::left << "Студент:" << " | " << std::setw(width_delim - 32) << std::right << "Хакимов А.С. |\n"
            << "| " << std::setw(28) << std::left << "Группа №:" << " | " << std::setw(width_delim - 32) << std::right << "ПБ-11 |\n"
            << "| " << std::setw(28) << std::left << "Задание:" << " | " << std::setw(width_delim - 32) << std::right << "Необходимо оценить вероятность отказа при подключении |\n"
            << delim << "\n"
            << top << "\n"
            << delim << "\n";

        return width_delim;
    }

    inline void FailureProbabilityAssessment::writeToStreamTop(std::stringstream& out, const std::string& header, size_t count, size_t width_delim) const noexcept {
        auto delim{ delimiter('-', width_delim) };
        out << delim << "\n| " << std::setw(28) << std::left << header;
        for (size_t i{}, ie{ count }; i != ie; ++i) out << " | " << std::setw(4) << std::right << i + 1;
        out << " |\n" << delim << "\n";
    }

    inline void FailureProbabilityAssessment::BreakConnect() noexcept {
        std::fill(connect_state_.begin(), connect_state_.end(), 0);
    }

    inline double FailureProbabilityAssessment::randomRealGenerate(double lhs, double rhs) const noexcept {
        std::random_device rd;
        std::default_random_engine re{ rd() };
        std::uniform_real_distribution<double> un_dist(lhs, rhs);

        return un_dist(re);
    }

    inline size_t  FailureProbabilityAssessment::calcConnectedComp() const noexcept {
        return std::count(connect_state_.begin(), connect_state_.end(), 1);
    }

    std::pair<size_t, size_t> FailureProbabilityAssessment::Step() noexcept {

        size_t connect{};
        size_t denial{};

        for (size_t i{}, ie{ data_.getNumberOfMachines() }; i != ie; ++i) {
            double x{ randomRealGenerate(0, 1) };
            if (connect_state_[i] == 0) {
                if (x <= (data_.getNumberOfConnections()[i] * data_.getFailureProbability())) {
                    ++connect;
                    if (calcConnectedComp() < data_.getChannelsOfConnection()) connect_state_[i] = 1;
                    else ++denial;
                }
            }
            else if (x <= data_.getWorkingHours()[i] * data_.getFailureProbability()) connect_state_[i] = 0;
        }
        return { connect, denial };
    }

    void FailureProbabilityAssessment::startTests() noexcept {
        for (size_t i{}, ie{ data_.getNumberOfTests() }; i != ie; ++i) {
            BreakConnect();
            double c{ data_.getFailureProbability() };
            double t{ data_.getObservationInterval() };
            size_t cn{};
            size_t den{};
            while (c < t) {
                auto&& [cn_tmp, den_tmp] { Step() };
                cn += cn_tmp;
                den += den_tmp;
                c += data_.getFailureProbability();
            }
            connect_.emplace_back(cn);
            denial_.emplace_back(den);
        }
        auto connect_sum{ std::accumulate(connect_.begin(), connect_.end(), size_t{}) };
        auto denial_sum{ std::accumulate(denial_.begin(), denial_.end(), size_t{}) };
        if (denial_sum == 0 || connect_sum == 0) return;

        failure_probability_ = 1. * denial_sum / connect_sum;
        auto tmp{ data_.getQuintileOfNormalDistribution() * std::sqrt(failure_probability_ * (1 - failure_probability_) / connect_sum) };
        left_span_ = failure_probability_ - tmp;
        right_span_ = failure_probability_ + tmp;

    }

    inline std::string FailureProbabilityAssessment::delimiter(char c, size_t width) const noexcept {
        return std::string(width, c);
    }
}