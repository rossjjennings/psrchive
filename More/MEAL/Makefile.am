
noinst_LTLIBRARIES = libMEAL.la

nobase_include_HEADERS = \
        MEAL/Agent.h \
        MEAL/Argument.h \
        MEAL/ArgumentPolicyAdapter.h \
        MEAL/ArgumentPolicy.h \
        MEAL/Axis.h \
        MEAL/BinaryRule.h \
        MEAL/BinaryScalar.h \
        MEAL/Boost.h \
	MEAL/Boost1.h \
        MEAL/Cached.h \
        MEAL/CalculatePolicy.h \
        MEAL/ChainRule.h \
        MEAL/Coherency.h \
        MEAL/Complex2Constant.h \
        MEAL/Complex2.h \
        MEAL/Complex2Math.h \
        MEAL/Complex2Value.h \
        MEAL/Composite.h \
        MEAL/CongruenceTransformation.h \
        MEAL/Constant.h \
        MEAL/CyclicParameter.h \
	MEAL/Depolarizer.h \
        MEAL/EvaluationPolicy.h \
        MEAL/Factory.h \
        MEAL/ForwardChanged.h \
        MEAL/ForwardResult.h \
        MEAL/Function.h \
        MEAL/FunctionPolicy.h \
        MEAL/Gain.h \
        MEAL/Gaussian.h \
        MEAL/GaussJordan.h \
        MEAL/GroupRule.h \
        MEAL/Invariant.h \
        MEAL/InverseRule.h \
        MEAL/LevenbergMarquardt.h \
        MEAL/Mean.h \
	MEAL/MuellerTransformation.h \
        MEAL/NegationRule.h \
        MEAL/NoParameters.h \
        MEAL/NormalizeStokes.h \
        MEAL/NotCached.h \
        MEAL/OneParameter.h \
        MEAL/ParameterPolicy.h \
        MEAL/Parameters.h \
        MEAL/parse.h \
        MEAL/Phase.h \
        MEAL/PhysicalCoherency.h \
        MEAL/Polar.h \
        MEAL/Polynomial.h \
        MEAL/PositionAngles.h \
        MEAL/ProductRule.h \
        MEAL/ProjectGradient.h \
        MEAL/Projection.h \
	MEAL/RandomPolar.h \
	MEAL/Real4.h \
	MEAL/Real4Value.h \
        MEAL/RotatingVectorModel.h \
        MEAL/Rotation.h \
	MEAL/Rotation1.h \
        MEAL/RuleMath.h \
        MEAL/ScalarAbsolute.h \
        MEAL/ScalarArgument.h \
        MEAL/ScalarBesselI0.h \
        MEAL/ScalarConstant.h \
        MEAL/ScalarCosine.h \
        MEAL/ScalarExponential.h \
        MEAL/Scalar.h \
        MEAL/ScalarHypCosine.h \
        MEAL/ScalarHypSine.h \
        MEAL/ScalarHypTangent.h \
        MEAL/ScalarInverseHypTangent.h \
        MEAL/ScalarInverseTangent2.h \
        MEAL/ScalarLogarithm.h \
        MEAL/ScalarMath.h \
        MEAL/ScalarParameter.h \
        MEAL/ScalarPower.h \
        MEAL/ScalarRoot.h \
        MEAL/ScalarSine.h \
        MEAL/ScalarTangent.h \
        MEAL/ScalarValue.h \
        MEAL/ScaledVonMisesDeriv.h \
        MEAL/ScaledVonMises.h \
	MEAL/StokesCovariance.h \
	MEAL/StokesError.h \
        MEAL/SumRule.h \
        MEAL/Tracer.h \
        MEAL/UnaryRule.h \
        MEAL/UnaryScalar.h \
        MEAL/Univariate.h \
        MEAL/UnivariatePolicy.h \
        MEAL/VectorRule.h \
        MEAL/VelocityModel.h \
        MEAL/VonMises.h

libMEAL_la_SOURCES = \
        Argument.C \
        ArgumentPolicy.C \
        BinaryScalar.C \
        Boost.C \
	Boost1.C \
        Coherency.C \
        Complex2.C \
        Complex2Constant.C \
        Complex2Math.C \
        Complex2Value.C \
        Composite.C \
        CongruenceTransformation.C \
        CyclicParameter.C \
	Depolarizer.C \
        ForwardChanged.C \
        Function.C \
        Function_load.C \
        Function_new_Function.C \
        Function_parse.C \
        Function_print.C \
        Gain.C \
        Gaussian.C \
        header_PromoteTraits \
        Invariant.C \
        MatrixTest.h \
	MuellerTransformation.C \
        NormalizeStokes.C \
        OneParameter.C \
        ParameterPolicy.C \
        Parameters.C \
        Phase.C \
        PhysicalCoherency.C \
        Polar.C \
        Polynomial.C \
        PositionAngles.C \
        Projection.C \
	RandomPolar.C \
	Real4.C \
	Real4Value.C \
        RotatingVectorModel.C \
        Rotation.C \
	Rotation1.C \
        ScalarAbsolute.C \
        ScalarArgument.C \
        Scalar.C \
        ScalarConstant.C \
        ScalarCosine.C \
        ScalarExponential.C \
        ScalarHypCosine.C \
        ScalarHypSine.C \
        ScalarHypTangent.C \
        ScalarInverseHypTangent.C \
        ScalarInverseTangent2.C \
        ScalarLogarithm.C \
        ScalarMath.C \
        ScalarParameter.C \
        ScalarPower.C \
        ScalarRoot.C \
        ScalarSine.C \
        ScalarTangent.C \
        ScalarValue.C \
        ScaledVonMises.C \
        ScaledVonMisesDeriv.C \
	StokesCovariance.C \
	StokesError.C \
        Tracer.C \
        UnaryScalar.C \
        UnivariatePolicy.C \
        VelocityModel.C \
        VonMises.C

bin_PROGRAMS = lmfit

lmfit_SOURCES = lmfit.C

check_PROGRAMS = \
	test_copy test_Axis test_Projection test_Composite test_Scalar	\
	test_Complex2Product test_Polar test_Rotation test_Invariant \
	test_CyclicParameter test_Univariate test_StokesError \
	test_StokesCovariance

TESTS = $(check_PROGRAMS)

test_copy_SOURCES		= test_copy.C
test_Polar_SOURCES		= test_Polar.C
test_Axis_SOURCES		= test_Axis.C
test_Projection_SOURCES		= test_Projection.C
test_Composite_SOURCES		= test_Composite.C
test_Scalar_SOURCES		= test_Scalar.C
test_Complex2Product_SOURCES	= test_Complex2Product.C
test_Rotation_SOURCES		= test_Rotation.C
test_CyclicParameter_SOURCES    = test_CyclicParameter.C
test_Univariate_SOURCES		= test_Univariate.C
test_Invariant_SOURCES		= test_Invariant.C
test_StokesError_SOURCES	= test_StokesError.C 
test_StokesCovariance_SOURCES	= test_StokesCovariance.C 

##############################################################################
#
# Rule to generate MEAL.pdf
#

# convert Postscript to Abobe Acrobat Portable Document Format
%.pdf: %.ps
	ps2pdf $<

# convert dvi file format to Postscript using Adobe safe ligatures
%.ps: %.dvi
	dvips -Ppdf -G0 -o $@ $<

%.dvi: %.tex $(FIGS) $(TEXS)
	latex $*
	latex $* 

doc: MEAL.pdf

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I. -I$(top_builddir)/utils/units @PGPLOT_CFLAGS@

LDADD = libMEAL.la $(top_builddir)/utils/units/libunits.la @PGPLOT_LIBS@

include $(top_srcdir)/config/Makefile.include

