#include "qsp/QSP.h"
#include "qsp/CircuitBinaryAdder.h"

using namespace libff;

field_desc_tc* initField() 
{
    field_desc_tc* alt_bn128_field = new field_desc_tc;
    
    alt_bn128_field->modulus = libff::bigint<LIMBS>("21888242871839275222246405745257275088696311157297823662689037894645226208583");
    alt_bn128_field->one = bigint<LIMBS>("6350874878119819312338956282401532409788428879151445726012394534686998597021");
    alt_bn128_field->zero = bigint<LIMBS>("0");
    //assert(alt_bn128_field->modulus_is_valid());
    if (sizeof(mp_limb_t) == 8)
    {
        alt_bn128_field->Rsquared = libff::bigint<LIMBS>("3096616502983703923843567936837374451735540968419076528771170197431451843209");
        //alt_bn128_field->Rcubed = bigint_q("14921786541159648185948152738563080959093619838510245177710943249661917737183");
        alt_bn128_field->inv = 0x87d20782e4866389;
    }
    if (sizeof(mp_limb_t) == 4)
    {
        alt_bn128_field->Rsquared = libff::bigint<LIMBS>("3096616502983703923843567936837374451735540968419076528771170197431451843209");
        //alt_bn128_field->Rcubed = bigint_q("14921786541159648185948152738563080959093619838510245177710943249661917737183");
        alt_bn128_field->inv = 0xe4866389;
    }
    alt_bn128_field->num_bits = 254;
    alt_bn128_field->euler = libff::bigint<LIMBS>("10944121435919637611123202872628637544348155578648911831344518947322613104291");
    alt_bn128_field->s = 1;
    alt_bn128_field->t = libff::bigint<LIMBS>("10944121435919637611123202872628637544348155578648911831344518947322613104291");
    alt_bn128_field->t_minus_1_over_2 = bigint<LIMBS>("5472060717959818805561601436314318772174077789324455915672259473661306552145");
    //alt_bn128_field->multiplicative_generator = alt_bn128_Fq("3");
    //alt_bn128_field->root_of_unity = alt_bn128_Fq("21888242871839275222246405745257275088696311157297823662689037894645226208582");
    //alt_bn128_field->nqr = alt_bn128_Fq("3");
    //alt_bn128_field->nqr_to_t = alt_bn128_Fq("21888242871839275222246405745257275088696311157297823662689037894645226208582");

    return alt_bn128_field;
}
void runSimpleQspTests() 
{
    field_desc_tc* field_tc = initField(); 
    Field* field = new Field(field_tc);

	CircuitBinaryAdder circuit(5);

	printf("Built a circuit with %d gates\n", circuit.gates.size());

	int i = 0;
	for (BoolWireVector::iterator iter = circuit.inputs.begin();
	     iter != circuit.inputs.end();
			 iter++) {
		(*iter)->value = i++ % 2 == 0;		
	}
	
	circuit.eval();
	

	// Try constructing a QSP
	QSP qsp(&circuit, field);	
    printf("qsp's size = %d, degree= %d\n", qsp.getSize(), qsp.getDegree());
/*
	Keys* keys = qsp.genKeys(config);

	QspProof* p = qsp.doWork(keys->pk, config);

*/
}

int main()
{
    runSimpleQspTests();

    return 0;
}