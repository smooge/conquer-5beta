/*
 * Unity Configuration for Conquer C2023 Modernization Project
 * 
 * This file configures Unity testing framework for legacy C code
 * modernization with C2023 standards compliance.
 */

#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

/* Enable Unity features appropriate for legacy C modernization */
#define UNITY_INCLUDE_DOUBLE
#define UNITY_INCLUDE_FLOAT
#define UNITY_SUPPORT_64

/* Configure output for automated testing */
#define UNITY_OUTPUT_START()    
#define UNITY_OUTPUT_COMPLETE() 

/* Memory testing support for security vulnerability validation */
#define UNITY_SUPPORT_TEST_CASES

/* Platform compatibility for target systems */
#ifdef __GNUC__
#define UNITY_WEAK __attribute__((weak))
#else
#define UNITY_WEAK
#endif

/* Enable additional assertion types for modernization validation */
#define UNITY_INCLUDE_EXEC_TIME

#endif /* UNITY_CONFIG_H */