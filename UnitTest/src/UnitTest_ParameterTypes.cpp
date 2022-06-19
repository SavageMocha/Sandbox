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

    const juce::Identifier Freq("freq");

    ParameterList param_list;
    param_list
      .add(Freq, 500.f, { "filter cutoff", "hz", false, true })
      .add("Num_Taps", 4)
      .add("Enabled", true)
    ;

    // ...Assign to/from the underlying data using operator[] and operator=
    //      and compare the entry w/ a value of the same type
    beginTest("Parameter Get, Set, and Comparison");
    int x = param_list["Num_Taps"]->Get<int>();
    expect(x == 4);

    const bool bIsFour = param_list["Num_Taps"]->IsEqualTo(4);
    expect(bIsFour);

    *param_list["Num_Taps"] = 10;
    expect(param_list["Num_Taps"]->Get<int>() == 10);
    expect(param_list["Num_Taps"]->IsEqualTo(10));
    
    float y = param_list[Freq]->Get<float>();
    expect(y == 500.f);

    const bool bIsEqual = param_list[Freq]->IsEqualTo(500.f);
    expect(bIsEqual);

    *param_list[Freq] = 15.f;
    expect(param_list[Freq]->Get<float>() == 15.f);
    expect(param_list[Freq]->IsEqualTo(15.f));

    beginTest("Parameter GetRef(), (access updated value without indexing)");
    int& xRef = param_list["Num_Taps"]->GetRef<int>();
    int currX = xRef;

    *param_list["Num_Taps"] = 111;
    expect(currX != xRef);
    expect(xRef == 111);

    xRef = 222;
    expect(param_list["Num_Taps"]->IsEqualTo(222) == true);


    // ...bootstrap a juce::ValueTree from that list
    beginTest("Bootstrap juce::ValueTree from ParameterList");
    auto xmlString = param_list.GetStateAsTree().toXmlString();
    expect(xmlString.isEmpty() == false);
    
    // ...synchronize my internal parameters to a juce::ValueTree
    beginTest("Converting underlying data to/from juce::Var");
    juce::var FreqVar = param_list[Freq]->GetAsVar();
    expect(param_list[Freq]->IsEqualTo(static_cast<float>(FreqVar)));

    FreqVar = 1234.0;
    param_list[Freq]->SetAsVar(FreqVar);
    expect(param_list[Freq]->IsEqualTo(1234.f));

    bool& bEnabled = param_list["Enabled"]->GetRef<bool>();
    bEnabled = false;
    expect(param_list["Enabled"]->IsEqualTo(false));

    // get tree and sync to it
    juce::ValueTree paramListTree = param_list.GetStateAsTree();
    param_list.SyncToTree(paramListTree);
    
    // ...so that when the value tree chanegs, my parameters will update internally
    paramListTree.setProperty({"Enabled"}, true, nullptr);
    expect(param_list["Enabled"]->IsEqualTo(true));
  }
  
} // Haze


// random thought:
/*
 *  it would be badass to have separate execution edges for different threads
 *  (i.e. UI thread vs Audio thread vs debug thread?)
 *  Data could be exchanged, but execution graphs would be separate
*/
