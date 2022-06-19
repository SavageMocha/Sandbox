
#include <JuceHeader.h>

#include "ParameterTypes.h"

//==============================================================================
int main()
{
    juce::UnitTestRunner TestRunner;

    TestRunner.setPassesAreLogged(false);
    TestRunner.runAllTests();

    return 0;
}
