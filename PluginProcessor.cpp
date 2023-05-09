#include "PluginProcessor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
      treeState (*this, nullptr, "Parameters", createParameters())
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
    time = 0;

    // Converting double to a float.
    rate = static_cast<float> (sampleRate);
    upDownSwitch = 0;
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

    // Calling parameter values tempo, quaver and tempo.
    auto tempo = getParameterValue ("BPM");
    auto mode = getParameterValue ("MODE");
    auto quaver = static_cast<int> (getParameterValue ("QUAV"));


    // Converting from float to int
    auto noteDuration = static_cast<int> (std::ceil (rate/((tempo * (quaver + 1))/60)));

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
        // Up Mode
        if (mode == 0)
        {
            upNoteChanger (time, midiMessages, offset, numSamples, noteDuration, mode);
        }

        // Down Mode
        if (mode == 1)
        {
            downNoteChanger (time, midiMessages, offset, numSamples, noteDuration, mode);
        }

        // Up-Down Mode
        if (mode == 2)
        {
            upDownNoteChanger(numSamples, noteDuration, midiMessages, offset, mode);
        }
    }

    if (notesAreNotHeld(notes))
    {
        lastNoteOffMessageSender (midiMessages, lastNoteValue, offset, time, currentNote);
    }

    time = timeUpdater(time, numSamples, noteDuration);

}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

// Using generic audio processor to generate GUI.
juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
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

float AudioPluginAudioProcessor::getParameterValue (juce::String parameterID)
{
    auto atomicFloat = treeState.getRawParameterValue(parameterID);
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

bool AudioPluginAudioProcessor:: notesAreNotHeld (juce::SortedSet<int> notes)
{
    return ((notes.size() == 0) && (lastNoteValue != -1));
}

int AudioPluginAudioProcessor::timeUpdater(int time, int numSamples, int noteDuration)
{
    time = (time + numSamples) % noteDuration;
    return time;
}

void AudioPluginAudioProcessor::lastNoteOffMessageSender (juce::MidiBuffer& midiMessages, int& lastNoteValue, int offset, int& time, int& currentNote)
{
    midiMessages.addEvent (juce::MidiMessage::noteOff (1, lastNoteValue), offset);
    lastNoteValue = -1;
    time = 0;
    currentNote = -1;
}

void AudioPluginAudioProcessor::noteOnSenderFromNextNote (int& currentNote, juce::SortedSet<int>& notes, int& lastNoteValue, juce::MidiBuffer& midiMessages, int offset, int mode)
{
    // Up Mode
    if (mode == 0)
    {
        moveUpOneInSortedSet();
    }

    // Down Mode
    else if (mode == 1)
    {
        moveDownOneInSortedSet();
    }

    // Up-Down Mode
    else if (mode == 2)
    {
        if (upDownSwitch == 0){
            moveUpOneInSortedSet();
        }

        if (upDownSwitch == 1){
            moveDownOneInSortedSet();
        }
    }

    // Setting to new note in sorted set then sending note on message.
    lastNoteValue = notes[currentNote];
    midiMessages.addEvent (juce::MidiMessage::noteOn (1, lastNoteValue, (juce::uint8) 127), offset);
}

bool AudioPluginAudioProcessor::timeForNoteChange (int time, int numSamples, int noteDuration)
{
    return (time + numSamples) >= noteDuration;
}

void AudioPluginAudioProcessor::upNoteChanger (int& time, juce::MidiBuffer& midiMessages, int offset, int numSamples, int noteDuration, int mode)
{
    if (timeForNoteChange (time, numSamples, noteDuration))
    {
        // Insert note off message from note before
        insertNoteOffMessage(midiMessages, offset);

        // Send next note message
        noteOnSenderFromNextNote (currentNote, notes, lastNoteValue, midiMessages, offset, mode);
    }
}

void AudioPluginAudioProcessor::downNoteChanger (int& time, juce::MidiBuffer& midiMessages, int offset, int numSamples, int noteDuration, int mode)
{
    if (timeForNoteChange (time, numSamples, noteDuration))
    {
            insertNoteOffMessage(midiMessages, offset);

            // First note of down note changer
            if (currentNote == -1)
            {
                // First note is the highest in sorted set. -1 because first element is 0.
                currentNote = notes.size() - 1;
                lastNoteValue = notes[currentNote];
                midiMessages.addEvent (juce::MidiMessage::noteOn  (1, lastNoteValue, (juce::uint8) 127), offset);
            }
            else
            {
                noteOnSenderFromNextNote (currentNote, notes, lastNoteValue, midiMessages, offset, mode);
            }
    }
}

void AudioPluginAudioProcessor::insertNoteOffMessage(juce::MidiBuffer& midiMessages, int offset)
{
    if (lastNoteValue > 0)
    {
            midiMessages.addEvent (juce::MidiMessage::noteOff (1, lastNoteValue), offset);
            lastNoteValue = -1;
    }
}

void AudioPluginAudioProcessor::upDownNoteChanger(int numSamples, int noteDuration, juce::MidiBuffer& midiMessages, int offset, int mode)
{
    // If current note is either 0 (Lowest Note) or less than 0 (-1 before first note is hit) then upNoteChanger is used.
    if (currentNote <= 0)
    {
            upDownSwitch = 0;
    }

    // If current note is higest in sorted set, then downNoteChanger is used.
    if (currentNote == (notes.size() - 1))
    {
            upDownSwitch = 1;
    }

    if (upDownSwitch == 0)
    {
            if (timeForNoteChange (time, numSamples, noteDuration))
            {
                upNoteChanger (time, midiMessages, offset, numSamples, noteDuration, mode);
            }
    }

    if (upDownSwitch == 1)
    {
            if (timeForNoteChange (time, numSamples, noteDuration))
            {
                downNoteChanger (time, midiMessages, offset, numSamples, noteDuration, mode);
            }
    }
}

void AudioPluginAudioProcessor::moveUpOneInSortedSet()
{
    // Equation to go up one element in sorted set then wrap back to the start
    currentNote = (currentNote + 1) % notes.size();
}

void AudioPluginAudioProcessor::moveDownOneInSortedSet()
{
    // Equation to go down one element in sorted set then wrap back to the top
    currentNote = (currentNote - 1 + notes.size()) % notes.size();
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

    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"BPM", 1}, "Bpm", 20, 200, 120));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"MODE", 1}, "Mode", juce::StringArray{"Up", "Down", "Up-Down"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"QUAV", 1}, "Quaver Mode", 0));

    return{ params.begin(), params.end() };
}
