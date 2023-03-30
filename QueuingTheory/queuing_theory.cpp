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

        size_t getNumberOfMachines() const & noexcept {
            return number_of_machines_;
        }
        size_t getChannelsOfConnection() const & noexcept {
            return channels_of_connection_;
        }
        double getFailureProbability() const & noexcept {
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
            failure_probability_ = 0;
            observation_interval_ = 0;
            quintile_of_normal_distribution_ = 0;

            number_of_connections_.clear();
            working_hours_.clear();
        }

        bool loadDataFromFile() noexcept {
            std::ifstream fs{ filename_ };
            if (!fs.is_open()) return false;
            fs >> number_of_machines_ >> channels_of_connection_ 
                >> failure_probability_ >> number_of_tests_ 
                >> observation_interval_ >> quintile_of_normal_distribution_
                >> number_of_connection_columns_;

            if (fs.fail()) return false;
            number_of_connections_.reserve(number_of_connection_columns_);
            for (size_t i{}, ie{ number_of_machines_ }; i != ie; ++i) {
                size_t sum{};
                for (size_t j{}, je{ number_of_connection_columns_ }; j != je; ++j) {
                    size_t tmp;
                    fs >> tmp;
                    sum += tmp;
                    if (fs.fail()) return false;
                }
            }
            

            return fs.good();
        }

    private:
        bool is_load_{};
        size_t number_of_machines_{};
        size_t number_of_tests_{};
        size_t number_of_connection_columns_{};
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
            << std::setw(40) << "quintile_of_normal_distribution_: " << data.quintile_of_normal_distribution_ << "\n";
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

    KHAS::InputData data{ "./data.txt" };

    std::cout << data << "\n\n";


    KHAS::FailureProbabilityAssessment fpa{ data };
    return 0;
}