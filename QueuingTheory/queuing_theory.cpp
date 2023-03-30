#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>




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
        double getFailureProbability() const noexcept {
            return failure_probability_;
        }

        bool isLoad() const & noexcept {
            return is_load_;
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
                    [&](auto n) { return number_of_connection_columns_ * 24. / n; });
            
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

    public:
        FailureProbabilityAssessment(const InputData& id) noexcept
            : number_of_machines_{ id.getNumberOfMachines()}
            , channels_of_connection_{ id.getChannelsOfConnection()}
            , failure_probability_{ id.getFailureProbability()}
        {
            init();
        }
        
    private:
        void init() {
            number_of_connections_ = { 5.0833333333,  8.1666666666, 10.6666666666, 9.5833333333, 7.4166666666 };
            working_hours_ = { 47.3684210526, 24.76354256233, 26.9410664172, 28.3185840707, 25.0652741514 };
        }
    private:
        size_t number_of_machines_;
        size_t channels_of_connection_;
        double failure_probability_;


        std::vector<double> number_of_connections_;
        std::vector<double> working_hours_;


    };

}






int main() {
    KHAS::InputData data{ "D:\\PROJECTS\\CPP\\QueuingTheory\\x64\\Debug\\data.txt" };

    std::cout << data << "\n\n";


    KHAS::FailureProbabilityAssessment fpa{ data };
    return 0;
}