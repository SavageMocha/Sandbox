/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/


#include <JuceHeader.h>

#include "ParameterTypes.h"

//==============================================================================
int main()
{
    juce::UnitTestRunner TestRunner;

    TestRunner.setPassesAreLogged(true);
    TestRunner.runAllTests();

    return 0;
}
