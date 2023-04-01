#include "InputData.h"


namespace KHAS {
    InputData::InputData(const std::string& filename) : filename_{ filename } {
        if (!loadDataFromFile()) {
            clear();
            std::cerr << "Can not read file from filename: " << filename;
            return;
        }
        is_load_ = true;
    }

    size_t InputData::getNumberOfMachines() const noexcept {
        return number_of_machines_;
    }

    size_t InputData::getChannelsOfConnection() const noexcept {
        return channels_of_connection_;
    }

    size_t InputData::getNumberOfTests() const noexcept {
        return number_of_tests_;
    }

    double InputData::getFailureProbability() const noexcept {
        return failure_probability_;
    }

    double InputData::getObservationInterval() const noexcept {
        return observation_interval_;
    }

    double InputData::getQuintileOfNormalDistribution() const noexcept {
        return quintile_of_normal_distribution_;
    }

    bool InputData::isLoad() const& noexcept {
        return is_load_;
    }

    const std::vector<double>& InputData::getNumberOfConnections() const& noexcept {
        return number_of_connections_;
    }

    const std::vector<double>& InputData::getWorkingHours() const& noexcept {
        return working_hours_;
    }

    void InputData::clear() noexcept {
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

    bool InputData::loadDataFromFile() noexcept {

        std::ifstream fs{ filename_ };
        if (!fs.is_open()) return false;

        size_t tmp{};
        size_t status{ 1 };
        std::tie(number_of_machines_, tmp) = readFromStream<size_t>(fs);
        status &= tmp;
        std::tie(channels_of_connection_, tmp) = readFromStream<size_t>(fs);
        status &= tmp;
        std::tie(failure_probability_, tmp) = readFromStream<double>(fs);
        status &= tmp;
        std::tie(number_of_tests_, tmp) = readFromStream<size_t>(fs);
        status &= tmp;
        std::tie(observation_interval_, tmp) = readFromStream<double>(fs);
        status &= tmp;
        std::tie(quintile_of_normal_distribution_, tmp) = readFromStream<double>(fs);
        status &= tmp;
        std::tie(number_of_connection_columns_, tmp) = readFromStream<size_t>(fs);
        status &= tmp;
        std::tie(number_of_working_times_, tmp) = readFromStream<size_t>(fs);
        status &= tmp;
        if (status != 1) return false;

        return readFromStream(number_of_connections_, fs, number_of_machines_, number_of_connection_columns_,
            [&](auto n) { return 1. * n / number_of_connection_columns_; })
            && readFromStream(working_hours_, fs, number_of_machines_, number_of_working_times_,
                [&](auto n) { return number_of_working_times_ * 24. / n; });

    }

}