#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters())
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int)
{
    // Emptying notes from sorted set.
    notes.clear();

    // Initially has current Note set to first instance of sorted set.
    currentNote = -1;

    // Holds information to be able to stop the notes
    lastNoteValue = -1;

    // Time used to keep track of note duration with respect to buffer size and sample rate.
    time = -1;

    // Converting double to a float.
    rate = static_cast<float> (sampleRate);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    // no audio i/o so buffer will have zeros
    jassert (buffer.getNumChannels() == 0);

    // however we use the buffer to get timing information
    auto numSamples = buffer.getNumSamples();

    auto tempo = pointerToFloat("BPM");

    auto mode = pointerToFloat("MODE");

    // Converting from float to int
    auto noteDuration = static_cast<int> (std::ceil (rate/(tempo/60)));

    // Iterate through each midi message metadata in the midiMessages buffer
    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();

        // Adding note to sorted set if it's a note-on message, deleting it if it's a note-off.
        if (msg.isNoteOn())
        {
            notes.add (msg.getNoteNumber());
        }
        else if (msg.isNoteOff())
        {
            notes.removeValue (msg.getNoteNumber());
        }
    }


    auto offset = calculateOffSet(time, numSamples, noteDuration);

    // Empty midi buffer to prepare the sorted set -> midi buffer transition.
    midiMessages.clear();


    if (notesAreHeld(notes))
    {
        if (time == -1)
        {
            noteChanger(currentNote, notes, lastNoteValue, midiMessages, offset);
            time = 0;
        }
        // TODO: Simplify comment.
        // Argument is met when it's time to change notes and edit the midi buffer. IF WE ARE JUST ABOUT TO WRAP
        // TODO: If conditions turn into functions.
        if (timeForNoteChange (time, numSamples, noteDuration))
        {
            if (lastNoteValue > 0)
            {
                midiMessages.addEvent (juce::MidiMessage::noteOff (1, lastNoteValue), offset);
                lastNoteValue = -1;
            }
            noteChanger(currentNote, notes, lastNoteValue, midiMessages, offset);
            time = timeUpdater(time, numSamples, noteDuration);
        }
        else if (0 < (time + numSamples) < noteDuration)
        {
            time = timeUpdater(time, numSamples, noteDuration);
        }
    }

    // TODO: Make to function. Whole block could be a function
    if ((notes.size() == 0) && (lastNoteValue != -1))
        {
            midiMessages.addEvent (juce::MidiMessage::noteOff (1, lastNoteValue), offset);
            lastNoteValue = -1;
            time = -1;
            currentNote = -1;
        }

    // TODO: TEST IN XCODE
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

float AudioPluginAudioProcessor::pointerToFloat(juce::String parameterID)
{
    auto atomicFloat =apvts.getRawParameterValue(parameterID);
    auto floatValue = atomicFloat -> load();
    return floatValue;
}

int AudioPluginAudioProcessor::calculateOffSet(int time, int numSamples, int noteDuration)
{
    auto offsetEquation = std::min(noteDuration - time, numSamples -1);
    auto offset = std::max(0, offsetEquation);
    return offset;
}

bool AudioPluginAudioProcessor:: notesAreHeld (juce::SortedSet<int> notes)
{
    return notes.size() > 0;
}

int AudioPluginAudioProcessor::timeUpdater(int time, int numSamples, int noteDuration)
{
    time = (time + numSamples) % noteDuration;
    return time;
}

bool AudioPluginAudioProcessor::timeForNoteChange (int time, int numSamples, int noteDuration)
{
    return (time + numSamples) >= noteDuration;
}

void AudioPluginAudioProcessor::noteChanger (int& currentNote, juce::SortedSet<int>& notes, int& lastNoteValue, juce::MidiBuffer& midiMessages, int offset)
{
    currentNote = (currentNote + 1) % notes.size();
    lastNoteValue = notes[currentNote];
    midiMessages.addEvent (juce::MidiMessage::noteOn (1, lastNoteValue, (juce::uint8) 127), offset);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}

// Declaring function that makes parameters
juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameters()
{
    // Storing parameters as a vector
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterInt>("BPM", "Bpm", 20, 200, 120));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("MODE", "Mode", juce::StringArray{"Up", "Down", "Up-Down"}, 0));

    return{ params.begin(), params.end() };
}
