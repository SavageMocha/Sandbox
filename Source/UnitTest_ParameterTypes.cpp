/*
  ==============================================================================

    UnitTest_ParameterTypes.cpp
    Created: 15 May 2022 2:39:07pm
    Author:  maxmo

  ==============================================================================
*/

#include "UnitTest_ParameterTypes.h"
#include "ParameterTypes.h"

namespace Haze
{
  
  // as a user I want to be able to...
  void UnitTests::UiParamTest::runTest()
  {
    // ...easily declare a list of parameters (w/ unique names) via builder pattern
    beginTest("Parameter list declaration");
    
    ParameterList paramList;
    paramList
      .add<float>("freq.", 50.f);
    

    // ...cache a read-only reference to the value
    // what will be updated in a theard safe way when value tree updates
    beginTest("ParameterList -> (default) ValueTree");
    

    // ...bootstrap a juce::ValueTree from that list
    beginTest("ParameterList -> (default) ValueTree");


    // ...synchronize my internal parameters to a juce::ValueTree
    // ...so that when the value tree chanegs, my parameters will update internally
    beginTest("ParameterList::Synchonize(juce::ValueTree);");


    // ...retrieve the up-to-date underlying data in a thread safe way (for audio processing)
    beginTest("Thread-safe get()");
    
  }


  
} // Haze


// random thought:
/*
 *  it would be badass to have separate execution edges for different threadsh
 *  (i.e. UI thread vs Audio thread)
 *  Data could be exchanged, but execution graphs would be separate
*/
