#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
#include <numeric>
#include <map>
#include <sstream>
#include <iterator>




namespace KHAS {

    /*
    
    */
    class InputData final {

    public:
        explicit InputData(const std::string& filename) : filename_{ filename } {
            if (!loadDataFromFile()) {
                clear();
                std::cerr << "Can not read file from filename: " << filename;
                return;
            }
            is_load_ = true;
        }

        friend std::ostream& operator << (std::ostream& os, const InputData& data);

        size_t getNumberOfMachines() const noexcept {
            return number_of_machines_;
        }
        size_t getChannelsOfConnection() const noexcept {
            return channels_of_connection_;
        }
        size_t getNumberOfTests() const noexcept {
            return number_of_tests_;
        }
        double getFailureProbability() const noexcept {
            return failure_probability_;
        }
        double getObservationInterval() const noexcept {
            return observation_interval_;
        }
        double getQuintileOfNormalDistribution() const noexcept {
            return quintile_of_normal_distribution_;
        }

        bool isLoad() const & noexcept {
            return is_load_;
        }

        auto& getNumberOfConnections() const& noexcept {
            return number_of_connections_;
        }
        auto& getWorkingHours() const& noexcept {
            return working_hours_;
        }



    private:

        void clear() noexcept {
            is_load_ = false;
            number_of_machines_ = 0;
            number_of_tests_ = 0;
            number_of_connection_columns_ = 0;
            number_of_working_times_ = 0;
            failure_probability_ = 0;
            observation_interval_ = 0;
            quintile_of_normal_distribution_ = 0;
            channels_of_connection_ = 0;

            number_of_connections_.clear();
            working_hours_.clear();
        }

        template <typename T, typename Func>
        bool readFromStream(T& container, std::ifstream& fs, const size_t cols, const size_t rows, Func func) noexcept {

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
        inline std::pair<T, size_t> readFromStream(std::ifstream& fs) const noexcept {
            using value_type = T;

            std::string tmp;
            value_type result{};
            if (fs >> result) std::getline(fs, tmp);

            return { result, fs.good() };
        }

        bool loadDataFromFile() noexcept {

            std::ifstream fs{ filename_ };
            if (!fs.is_open()) return false;

            size_t tmp{};
            size_t status{ 1 };
            std::tie(number_of_machines_, tmp)               = readFromStream<size_t>(fs);
            status &= tmp;
            std::tie(channels_of_connection_, tmp)           = readFromStream<size_t>(fs);
            status &= tmp;
            std::tie(failure_probability_, tmp)              = readFromStream<double>(fs);
            status &= tmp;
            std::tie(number_of_tests_, tmp)                  = readFromStream<size_t>(fs);
            status &= tmp;
            std::tie(observation_interval_, tmp)             = readFromStream<double>(fs);
            status &= tmp;
            std::tie(quintile_of_normal_distribution_, tmp)  = readFromStream<double>(fs);
            status &= tmp;
            std::tie(number_of_connection_columns_, tmp)     = readFromStream<size_t>(fs);
            status &= tmp;
            std::tie(number_of_working_times_, tmp)          = readFromStream<size_t>(fs);
            status &= tmp;
            if (status != 1) return false;

            return readFromStream(number_of_connections_, fs, number_of_machines_, number_of_connection_columns_,
                [&](auto n) { return 1. * n / number_of_connection_columns_; })
                && readFromStream(working_hours_, fs, number_of_machines_, number_of_working_times_,
                    [&](auto n) { return number_of_working_times_ * 24. / n; });
            
        }

    private:
        bool is_load_{};
        size_t number_of_machines_{};
        size_t number_of_tests_{};
        size_t number_of_connection_columns_{};
        size_t number_of_working_times_{};
        size_t channels_of_connection_{};
        double failure_probability_{};
        double observation_interval_{};
        double quintile_of_normal_distribution_{};
        std::vector<double> number_of_connections_{};
        std::vector<double> working_hours_{};
        std::string filename_{};
    };

    std::ostream& operator << (std::ostream& os, const InputData& data) {
        os << std::left << std::setw(40) << "\nis_load_: " << data.is_load_ << "\n"
            << std::setw(40) << "number_of_machines_: " << data.number_of_machines_ << "\n"
            << std::setw(40) << "number_of_tests_: " << data.number_of_tests_ << "\n"
            << std::setw(40) << "number_of_connection_columns_: " << data.number_of_connection_columns_ << "\n"
            << std::setw(40) << "channels_of_connection_: " << data.channels_of_connection_ << "\n"
            << std::setw(40) << "failure_probability_: " << data.failure_probability_ << "\n"
            << std::setw(40) << "observation_interval_: " << data.observation_interval_ << "\n"
            << std::setw(40) << "quintile_of_normal_distribution_: " << data.quintile_of_normal_distribution_ << "\n"
            << std::setw(40) << "number_of_connections_:\n";
        for (auto&& elem : data.number_of_connections_) std::cout << std::setw(8) << elem << " ";
        std::cout << "\n" << std::setw(40) << "working_hours_:\n";
        for (auto&& elem : data.working_hours_) std::cout << std::setw(8) << elem << " ";

        return os;
    }








    class FailureProbabilityAssessment final {

        enum class WriteToStreamType { Header, Tests };


    public:
        explicit FailureProbabilityAssessment(const InputData& data) noexcept
            : data_{ data } 
        {
            connect_.reserve(data_.getNumberOfTests());
            denial_.reserve(data_.getNumberOfTests());
            connect_state_.resize(data_.getNumberOfTests());
            startTests();
        }

    public:
        void showCalculationResult() const noexcept{
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

        
    private:

        inline size_t writeToStreamTop(std::stringstream& out, const std::string& header, size_t count) const noexcept {
            
            out << "| " << std::setw(28) << std::left << header;
            for (size_t i{}, ie{ count }; i != ie; ++i) out << " | " << std::setw(11) << std::right << i+1;
            out << " |";

            size_t width_delim = out.tellp();

            auto delim{ delimiter('-', width_delim) };
            auto top{ out.str() };
            out.str("");
            out << delim << "\n"
                << top << "\n"
                << delim << "\n";

            return width_delim;
        }
        inline void writeToStreamTop(std::stringstream& out, const std::string& header, size_t count, size_t width_delim) const noexcept {
            auto delim{ delimiter('-', width_delim) };
            out << delim << "\n| " << std::setw(28) << std::left << header;
            for (size_t i{}, ie{ count }; i != ie; ++i) out << " | " << std::setw(4) << std::right << i+1;
            out << " |\n" << delim << "\n";
        }

        template <typename T>
        inline void writeToStream(std::stringstream& out, const std::string& header, size_t width_delim, T&& value) const noexcept {
            auto delim{ delimiter('-', width_delim) };
            out << delim << "\n| " << std::setw(28) << std::left << header;
            out << " | " << std::setw(width_delim - 35) << std::right << std::fixed << std::setprecision(3) << std::forward<T>(value) << " |\n" << delim << "\n";
        }

        template <typename Cont>
        inline void writeToStream(std::stringstream& out, const std::string& header, const Cont& cont, WriteToStreamType wt, size_t width_delim) const noexcept {
            
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

        inline void BreakConnect() noexcept {
            std::fill(connect_state_.begin(), connect_state_.end(), 0);
        }

        inline double randomRealGenerate(double lhs, double rhs) {
            std::random_device rd;
            std::default_random_engine re{ rd() };
            std::uniform_real_distribution<double> un_dist(lhs, rhs);

            return un_dist(re);
        }

        inline size_t  calcConnectedComp() {
            return std::count(connect_state_.begin(), connect_state_.end(), 1);
        }

        std::pair<size_t, size_t> Step() {

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

        void startTests() noexcept {
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

            failure_probability_ = 1. * denial_sum / connect_sum;
            auto tmp{ data_.getQuintileOfNormalDistribution() * std::sqrt(failure_probability_ * (1 - failure_probability_)) };
            left_span_ = failure_probability_ - tmp;
            right_span_ = failure_probability_ + tmp;

        }

        inline std::string delimiter(char c, size_t width) const noexcept {
            return std::string(width, c);
        }
        

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






int main() {
    //KHAS::InputData data{ "./data.txt" };
    KHAS::InputData data{ "D:\\PROJECTS\\CPP\\QueuingTheory\\x64\\Debug\\data.txt" };

    KHAS::FailureProbabilityAssessment fpa{ data };
    fpa.showCalculationResult();
    system("pause");
    return 0;
}