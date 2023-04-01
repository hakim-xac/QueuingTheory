#pragma once
#include <vector>
#include <fstream>
#include <sstream>

namespace KHAS {
    class InputData final {

    public:
        explicit InputData(const std::string& filename);
        friend std::ostream& operator << (std::ostream& os, const InputData& data);

        size_t getNumberOfMachines()                const noexcept;
        size_t getChannelsOfConnection()            const noexcept;
        size_t getNumberOfTests()                   const noexcept;
        double getFailureProbability()              const noexcept;
        double getObservationInterval()             const noexcept;
        double getQuintileOfNormalDistribution()    const noexcept;

        bool isLoad()                   const& noexcept;
        const std::vector<double>& getNumberOfConnections()  const& noexcept;
        const std::vector<double>& getWorkingHours()         const& noexcept;


    private:

        void clear() noexcept;
        bool loadDataFromFile() noexcept;

        template <typename T, typename Func>
        bool readFromStream(T& container, std::ifstream& fs, const size_t cols, const size_t rows, Func func) noexcept;

        template <typename T>
        inline std::pair<T, size_t> readFromStream(std::ifstream& fs) const noexcept;


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
}

#include "InputData.hpp"