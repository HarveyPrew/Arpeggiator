#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/student/Documents/code/Arpeggiator/Builds/JUCE/tools
  make -f /Users/student/Documents/code/Arpeggiator/Builds/JUCE/tools/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/student/Documents/code/Arpeggiator/Builds/JUCE/tools
  make -f /Users/student/Documents/code/Arpeggiator/Builds/JUCE/tools/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/student/Documents/code/Arpeggiator/Builds/JUCE/tools
  make -f /Users/student/Documents/code/Arpeggiator/Builds/JUCE/tools/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/student/Documents/code/Arpeggiator/Builds/JUCE/tools
  make -f /Users/student/Documents/code/Arpeggiator/Builds/JUCE/tools/CMakeScripts/ReRunCMake.make
fi

