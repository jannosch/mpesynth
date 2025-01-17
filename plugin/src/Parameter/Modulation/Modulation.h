#pragma once
#include "../../Types.h"

class ModulatedParameterFloat;
class ModulationData;

class Modulation {
public:

    Modulation();

    Modulation(size_t newModulationSourceId, ModulatedParameterFloat* newAmount);

    Eigen::ArrayX<Decimal> getModulatedNormalized(const ModulationData* modulationData, int startSample, int numSamples);

    void setModulatedParameterId(juce::String newModulatedParameterId);

    juce::String getModulatedParameterId();

    void setModulationSourceId(size_t newModulationSourceId);

    void setAmountParameter(ModulatedParameterFloat* newAmount);

    void prepareToPlay(Eigen::Index newSamplesPerBlock);

protected:

    Eigen::Index samplesPerBlock = 0;

    juce::String modulatedParameterId;
    size_t modulationSourceId;
    ModulatedParameterFloat* amount = nullptr;

};