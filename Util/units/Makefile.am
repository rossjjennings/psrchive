
noinst_LTLIBRARIES = libunits.la

libunits_la_SOURCES = Error.C HeapTracked.C ReferenceAble.C stringtok.C	      \
	Function.C Argument.C Parameters.C Projection.C Composite.C	      \
	Univariate.C Scalar.C ScalarConstant.C ScalarMath.C OptimizedScalar.C \
	ScalarValue.C UnaryScalar.C ScalarRoot.C ScalarSine.C ScalarCosine.C  \
	ScalarTangent.C ScalarHypSine.C ScalarHypCosine.C ScalarHypTangent.C  \
	ScalarAbsolute.C ScalarExponential.C ScalarLogarithm.C		      \
	ScalarInverseHypTangent.C BinaryScalar.C ScalarInverseTangent2.C      \
	ScalarPower.C ScalarArgument.C UnivariateOptimizedScalar.C	      \
	Polynomial.C Gaussian.C Complex2Value.C Complex2.C Complex2Math.C     \
	OptimizedComplex2.C Complex2Constant.C Gain.C Phase.C Boost.C	      \
	Rotation.C Polar.C Coherency.C NormalizeStokes.C Tracer.C	      \
	CongruenceTransformation.C Function_load.C Function_parse.C	      \
	Function_print.C Function_new_Function.C Pauli.C EstimatePlotter.C

nobase_include_HEADERS = Error.h HeapTracked.h ReferenceAble.h stringtok.h \
	Traits.h Reference.h ReferenceTo.h Registry.h Functor.h \
        Callback.h Estimate.h Matrix.h Vector.h Jacobi.h \
        Quaternion.h Jones.h Pauli.h Basis.h Stokes.h Cloude.h  \
        tostring.h random.h EstimatePlotter.h \
	MEAL/Agent.h                     MEAL/Pauli.h \
	MEAL/ArgumentBehaviour.h         MEAL/Phase.h \
	MEAL/Argument.h                  MEAL/Polar.h \
	MEAL/Axis.h                      MEAL/Polynomial.h \
	MEAL/BinaryRule.h                MEAL/ProductRule.h \
	MEAL/BinaryScalar.h              MEAL/ProjectGradient.h \
	MEAL/Boost.h                     MEAL/Projection.h \
	MEAL/ChainRule.h                 MEAL/Rotation.h \
	MEAL/Coherency.h                 MEAL/RuleMath.h \
	MEAL/Complex2Constant.h          MEAL/ScalarAbsolute.h \
	MEAL/Complex2.h                  MEAL/ScalarArgument.h \
	MEAL/Complex2Math.h              MEAL/ScalarConstant.h \
	MEAL/Complex2Value.h             MEAL/ScalarCosine.h \
	MEAL/Composite.h                 MEAL/ScalarExponential.h \
	MEAL/CongruenceTransformation.h  MEAL/Scalar.h \
	MEAL/Constant.h                  MEAL/ScalarHypCosine.h \
	MEAL/EvaluationBehaviour.h       MEAL/ScalarHypSine.h \
	MEAL/Factory.h                   MEAL/ScalarHypTangent.h \
	MEAL/Function.h                  MEAL/ScalarInverseHypTangent.h \
	MEAL/Gain.h                      MEAL/ScalarInverseTangent2.h \
	MEAL/Gaussian.h                  MEAL/ScalarLogarithm.h \
	MEAL/GaussJordan.h               MEAL/ScalarMath.h \
	MEAL/GroupRule.h                 MEAL/ScalarPower.h \
	MEAL/InverseRule.h               MEAL/ScalarRoot.h \
	MEAL/LevenbergMarquardt.h        MEAL/ScalarSine.h \
	MEAL/Mean.h                      MEAL/ScalarTangent.h \
	MEAL/NegationRule.h              MEAL/ScalarValue.h \
	MEAL/NoParameters.h              MEAL/SumRule.h \
	MEAL/NormalizeStokes.h           MEAL/Tracer.h \
	MEAL/OptimizedComplex2.h         MEAL/UnaryRule.h \
	MEAL/Optimized.h                 MEAL/UnaryScalar.h \
	MEAL/OptimizedScalar.h           MEAL/Univariate.h \
	MEAL/ParameterBehaviour.h        MEAL/UnivariateOptimizedScalar.h \
	MEAL/Parameters.h


bin_PROGRAMS = lmfit
lmfit_SOURCES = lmfit.C
lmfit_LDADD = libunits.la @PGPLOT_LIBS@ 
INCLUDES = @PGPLOT_CFLAGS@


