/*
* File:   SETSIK.hpp
* Author: shlomi
*
*/

#ifndef SETSIK_H
#define	SETSIK_H

#ifndef SIK_DEBUG
/**
* Set Defaul debuger output
*/
#define SIK_DEBUG false
#endif

#ifndef SIK_DEBUG_LEVEL
/**
*
*/
#define SIK_DEBUG_LEVEL 4
#endif

#ifndef SIK_DEBUG_EXPOSE_COMPILER_PARSE
/**
* Require debug level 1
*/
#define SIK_DEBUG_EXPOSE_COMPILER_PARSE true
#endif

#ifndef SIK_DEBUG_EXPOSE_COMPILER_MACRO_USE
/**
* Require debug level 1
*/
#define SIK_DEBUG_EXPOSE_COMPILER_MACRO_USE true
#endif

#ifndef SIK_DEBUG_EXPOSE_FUNCTION_TABLE
/**
* Require debug level 2
*/
#define SIK_DEBUG_EXPOSE_FUNCTION_TABLE true
#endif

#ifndef SIK_DEBUG_EXPOSE_COMPILED_CODE
/**
* Require debug level 1
*/
#define SIK_DEBUG_EXPOSE_COMPILED_CODE true
#endif

#ifndef SIK_DEBUG_EXPOSE_EXECUTION_STEPS
/**
* Require debug level 2
*/
#define SIK_DEBUG_EXPOSE_EXECUTION_STEPS true
#endif

#ifndef SIK_DEBUG_EXPOSE_EXECUTIOM_STACK_STATE
/**
* Require debug level 3
* And Require OWQ_DEBUG_EXPOSE_EXECUTION_STEPS
*/
#define SIK_DEBUG_EXPOSE_EXECUTIOM_STACK_STATE true
#endif

#endif	/* SETSIK_H */
