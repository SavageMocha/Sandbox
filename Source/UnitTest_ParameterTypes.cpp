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

    const juce::Identifier Freq("freq.");

    ParameterList param_list;
    param_list
      .add(Freq, 500.f)
      .add("Num Taps", 4)
    ;

    // ...Assign to/from the underlying data using operator[] and operator=
    //      and compare the entry w/ a value of the same type
    int x = param_list["Num Taps"]->Get<int>();
    expect(x == 4);

    const bool bIsFour = param_list["Num Taps"]->IsEqualTo(4);
    expect(bIsFour);

    *param_list["Num Taps"] = 10;
    expect(param_list["Num Taps"]->Get<int>() == 10);
    expect(param_list["Num Taps"]->IsEqualTo(10));


    float y = param_list[Freq]->Get<float>();
    expect(y == 500.f);

    const bool bIsEqual = param_list[Freq]->IsEqualTo(500.f);
    expect(bIsEqual);

    *param_list[Freq] = 15.f;
    expect(param_list[Freq]->Get<float>() == 15.f);
    expect(param_list[Freq]->IsEqualTo(15.f));
    

    

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
