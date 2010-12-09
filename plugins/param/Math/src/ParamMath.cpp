#include <ofxsParamEffect.h>

namespace tuttle {

template <class ParamType>
class ParamMathPlugin : public OFX::ParamEffect
{
enum EOperation
{
	eMultiply,
	eDivide,
	eAddition,
	eSubtraction,
	eModulo,
};

public:
	ParamMathPlugin( OfxParamEffectHandle handle )
	{
		_operation = fetchChoice( "operation" );
		_a         = fetch<ParamType>( "a" );
		_b         = fetch<ParamType>( "b" );
		_output    = fetch<ParamType>( "output" );
	}

public:
	void process( const OFX::RenderArguments& args )
	{
		// test valeurs min/max ?
		switch( static_cast<EOperation>( _operation.value() ) )
		{
			case eParamMathMultiply:
				_output->setValue( _a->value() * _b->value() );
				return;
			case eParamMathDivide:
				if( b->value() == 0 )
				{
					_output->setValue( 0 );
					return;
				}
				_output->setValue( _a->value() / _b->value() );
				return;
			case eAddition:
				_output->setValue( _a->value() + _b->value() );
				return;
			case eSubtraction:
				_output->setValue( _a->value() - _b->value() );
				return;
			case eModulo:
				_output->setValue( _a->value() % _b->value() );
				return;
		}
		_output->setValue( 0 );
	}

protected:
	OFX::ChoiceParam* _operation;
	ParamType* _a;
	ParamType* _b;
	ParamType* _output;
};

mDeclarePluginFactory( ParamMathPluginFactory<DoubleParam>, {}, {}
                       );
mDeclarePluginFactory( ParamMathPluginFactory<IntParam>, {}, {}
                       );

template <class ParamType>
void ParamMathPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleMathParam", "MathParam",
	                "Math Param" );
	desc.setPluginGrouping( "tuttle/param" );

	OFX::ChoiceParamDescriptor* op = desc.defineChoiceParam( "operation" );
	op->appendOption( "Multiply" );
	op->appendOption( "Divide" );
	op->appendOption( "Addition" );
	op->appendOption( "Subtraction" );
	op->appendOption( "Modulo" );
	op->setDefault( 0 );

	OFX::Descriptor<ParamType>* a      = desc.define<ParamType>( "a" );
	OFX::Descriptor<ParamType>* b      = desc.define<ParamType>( "b" );
	OFX::Descriptor<ParamType>* output = desc.define<ParamType>( "Output" );
}

template <class ParamType>
OFX::ParamEffect* ParamMathPluginFactory::createInstance( OfxImageEffectHandle handle )
{
	return new ParamMathPlugin( handle );
}

}

namespace OFX
{
namespace Plugin
{
void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	static tuttle::ParamMathPluginFactory<DoubleParam> d( "fr.tuttle.parammath.double", 0, 0 );
	static tuttle::ParamMathPluginFactory<IntParam> i( "fr.tuttle.parammath.int", 0, 0 );
	ids.push_back( &d );
	ids.push_back( &i );
}

}
}

