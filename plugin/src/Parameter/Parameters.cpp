#include "Parameters.h"
#include "LambdaListener.h"
#include "Modulation/ModulationData.h"




Parameters::Parameters() = default;



void Parameters::connectTo(juce::AudioProcessor &processorToConnectTo) {
    jassert(layoutInCreation);
    layoutInCreation = false;

    treeState = std::make_shared<juce::AudioProcessorValueTreeState>(
            processorToConnectTo,
            &undoManager,
            "Parameters",
            std::move(layout));

}



List<std::shared_ptr<Modulation>> &Parameters::addModulationSlots(size_t number, ModulatedParameterFloat* frequencyParameter) {
    jassert(layoutInCreation); // Call this before calling connectTo()
    jassert(number > 0); // number must be greater 0
    jassert(modulations.empty()); // Call this method only once, or change implementation to support multiple calls

    const juce::String modulationTargetName = "Modulation Target";
    const juce::String modulationSourceName = "Modulation Source";
    const juce::String modulationAmountName = "Modulation Amount";

    const juce::String frequencyParameterID = frequencyParameter->getParameterID();

    // Get a List of all existing modulatable parameters for possible modulation targets
    List<juce::String> modulationTargetIds = {"None"};
    for (auto&  parameter: sortedModulatedParameters) modulationTargetIds.push_back(parameter->getParameterID());

    // Add all Parameter amounts as targets as well
    for (size_t i = 0; i < number; i++) modulationTargetIds.push_back(modulationAmountName + " " + juce::String(i + 1));

    // Add all modulations
    for (size_t i = 0; i < number; i++) modulations.push_back(std::make_shared<Modulation>());

    // Create the Parameters for Modulation
    for (size_t i = 0; i < number; i++) {

        // Modulation Target
        auto* modulationTargetParameter = add<juce::AudioParameterChoice>(juce::ParameterID(modulationTargetName + " " + juce::String(i + 1), Parameters::VERSION), modulationTargetName + " " + juce::String(i + 1), modulationTargetIds.toStringArray(), i == 0 ? modulationTargetIds.indexOf(frequencyParameterID) : 0);
        modulationTargetParameter->addListener(new LambdaListener([this, i, modulationTargetName, modulationSourceName, modulationAmountName, modulationTargetIds] (float newValue) {

            auto& modulation = this->modulations[i];

            // Cleanup old Modulation
            juce::String oldModulatedParameterId = this->modulations[i]->getModulatedParameterId();
            if (this->modulatedParameters.contains(oldModulatedParameterId)) {
                this->modulatedParameters[oldModulatedParameterId]->modulations.remove(modulation);
            }

            // Make new Modulation
            auto newIndex = static_cast<size_t>(round(newValue * static_cast<float>(modulationTargetIds.size() - 1)));
            if (newIndex != 0) {
                const juce::String &modulatedParameterId = modulationTargetIds[newIndex];
                this->modulatedParameters[modulatedParameterId]->withModulation(modulation);
                modulation->setModulatedParameterId(modulatedParameterId);
            } else {
                modulation->setModulatedParameterId("");
            }

        }));
        if (i == 0) {
            this->modulatedParameters[frequencyParameterID]->withModulation(this->modulations[0]);
            this->modulations[0]->setModulatedParameterId(frequencyParameterID);
        }

        // Modulation Source
        auto* modulationSourceParameter = add<juce::AudioParameterChoice>(
                juce::ParameterID(modulationSourceName + " " + juce::String(i + 1), Parameters::VERSION),
                modulationSourceName + " " + juce::String(i + 1),
                ModulationData::Sources::ALL.map<juce::String>([](const ModulationData::Source& s) { return s.name; }).toStringArray(),
                i == 0 ? ModulationData::Sources::X.id : 0);

        modulationSourceParameter->addListener(new LambdaListener([this, i] (float newValue) {

            auto& modulation = this->modulations[i];

            modulation->setModulationSourceId(static_cast<size_t>(round(newValue * static_cast<float>(ModulationData::Sources::ALL.size() - 1))));

        }));
        if (i == 0) {
            this->modulations[i]->setModulationSourceId(ModulationData::Sources::X.id);
        }

        // Modulation Amount
        auto* modulationAmountParameter = add<ModulatedParameterFloat>(modulationAmountName + " " + juce::String(i + 1), juce::NormalisableRange<float>(-1, 1, 0, 0.66f, true), i == 0 ? 0.1 : 0);
        modulations[i]->setAmountParameter(modulationAmountParameter);

    }

    return modulations;
}



void Parameters::getStateInformation(juce::MemoryBlock &destData) {
    juce::MemoryOutputStream memoryStream(destData, true);
    treeState->state.writeToStream(memoryStream);
}



void Parameters::setStateInformation(const void *data, int sizeInBytes) {
    auto inputTree = juce::ValueTree::readFromData(data, static_cast<size_t>(sizeInBytes));
    if (inputTree.isValid()) {
        treeState->replaceState(inputTree);
    }
}



void Parameters::prepareToPlay(Decimal sampleRate, Eigen::Index samplesPerBlock) {
    jassert(!layoutInCreation); // Call connectTo() before any audio processing

    for (auto& [id, parameter]: modulatedParameters) {
        parameter->prepareToPlay(sampleRate, samplesPerBlock);
    }

    for (auto& modulation: modulations) {
        modulation->prepareToPlay(samplesPerBlock);
    }
}



void Parameters::processBlock() {
    jassert(!layoutInCreation); // Call connectTo() before any audio processing

    for (auto& [id, parameter]: modulatedParameters) {
        parameter->processBlock();
    }
}