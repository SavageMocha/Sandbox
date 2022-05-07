/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>

//==============================================================================

// Max sandbox type area (todo: setup unit test environment)

// for now pretend get/set only uses float, will templatize that later
class Parameter
{

private:
    class ParamConcept
    {
    public:
        // dtor
        virtual ~ParamConcept() = default;

        // public interface
        virtual float get() const = 0;
        virtual void set(float&& value) = 0;
    };

    template<typename ParamType>
    class ParamModel : ParamConcept
    {
        
    };


public:
    
}; // class Parameter




int main (int argc, char* argv[])
{

    // ..your code goes here!
    DBG("This is a test");
    


    return 0;
}
