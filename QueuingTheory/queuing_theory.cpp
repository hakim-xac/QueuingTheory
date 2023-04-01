#include "FailureProbabilityAssessment.h"
#include "InputData.h"

namespace KHAS {
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
}

int main() {
    //KHAS::InputData data{ "./data.txt" };
    KHAS::InputData data{ "D:\\PROJECTS\\CPP\\QueuingTheory\\x64\\Debug\\data.txt" };

    KHAS::FailureProbabilityAssessment fpa{ data };
    fpa.showCalculationResult();
    system("pause");
    return 0;
}