#include "FailureProbabilityAssessment.h"
#include "InputData.h"

int main() {
    KHAS::InputData data{ "./data.txt" };
    //KHAS::InputData data{ "D:\\PROJECTS\\CPP\\QueuingTheory\\x64\\Debug\\data.txt" };

    KHAS::FailureProbabilityAssessment fpa{ data };
    fpa.showCalculationResult();
    system("pause");
    return 0;
}