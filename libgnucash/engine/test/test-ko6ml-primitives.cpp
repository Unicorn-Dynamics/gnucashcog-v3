/********************************************************************\
 * test-ko6ml-primitives.cpp -- Tests for ko6ml Cognitive         *
 * Copyright (C) 2024 GnuCash Cognitive Engine                     *
 *                                                                  *
 * Comprehensive test suite for ko6ml primitives, AtomSpace        *
 * translation, tensor encoding, and round-trip validation.        *
 ********************************************************************/

#include <glib.h>
#include <gtest/gtest.h>
#include "gnc-ko6ml-primitives.h"
#include "gnc-cognitive-accounting.h"
#include "Account.h"
#include "Transaction.h"
#include "qof.h"

class Ko6mlPrimitivesTest : public ::testing::Test {
protected:
    void SetUp() override {
        gnc_engine_init(0, nullptr);
        ASSERT_TRUE(gnc_ko6ml_init());
    }
    
    void TearDown() override {
        gnc_ko6ml_shutdown();
    }
};

/********************************************************************\
 * Core Primitive Tests                                            *
\********************************************************************/

TEST_F(Ko6mlPrimitivesTest, PrimitiveCreationAndDestruction) {
    Ko6mlPrimitive *primitive = gnc_ko6ml_primitive_new(
        KO6ML_CONCEPT, "test_concept", "Test concept for unit testing");
    
    ASSERT_NE(primitive, nullptr);
    EXPECT_EQ(primitive->type, KO6ML_CONCEPT);
    EXPECT_STREQ(primitive->name, "test_concept");
    EXPECT_STREQ(primitive->description, "Test concept for unit testing");
    EXPECT_EQ(primitive->salience, 0.5);
    EXPECT_EQ(primitive->autonomy_index, 0.5);
    EXPECT_NE(primitive->properties, nullptr);
    
    gnc_ko6ml_primitive_free(primitive);
}

TEST_F(Ko6mlPrimitivesTest, PrimitiveProperties) {
    Ko6mlPrimitive *primitive = gnc_ko6ml_primitive_new(
        KO6ML_AGENT, "test_agent", "Test agent");
    
    gnc_ko6ml_primitive_set_property(primitive, "category", "financial");
    gnc_ko6ml_primitive_set_property(primitive, "version", "1.0");
    
    EXPECT_STREQ(gnc_ko6ml_primitive_get_property(primitive, "category"), "financial");
    EXPECT_STREQ(gnc_ko6ml_primitive_get_property(primitive, "version"), "1.0");
    EXPECT_EQ(gnc_ko6ml_primitive_get_property(primitive, "nonexistent"), nullptr);
    
    gnc_ko6ml_primitive_free(primitive);
}

TEST_F(Ko6mlPrimitivesTest, PrimitiveTypeStringConversion) {
    EXPECT_STREQ(gnc_ko6ml_primitive_type_to_string(KO6ML_CONCEPT), "concept");
    EXPECT_STREQ(gnc_ko6ml_primitive_type_to_string(KO6ML_AGENT), "agent");
    EXPECT_STREQ(gnc_ko6ml_primitive_type_to_string(KO6ML_STATE), "state");
    EXPECT_STREQ(gnc_ko6ml_primitive_type_to_string(KO6ML_PROCESS), "process");
    
    EXPECT_EQ(gnc_ko6ml_primitive_type_from_string("concept"), KO6ML_CONCEPT);
    EXPECT_EQ(gnc_ko6ml_primitive_type_from_string("agent"), KO6ML_AGENT);
    EXPECT_EQ(gnc_ko6ml_primitive_type_from_string("state"), KO6ML_STATE);
    EXPECT_EQ(gnc_ko6ml_primitive_type_from_string("invalid"), -1);
}

/********************************************************************\
 * AtomSpace Translation Tests                                     *
\********************************************************************/

TEST_F(Ko6mlPrimitivesTest, Ko6mlToAtomSpaceTranslation) {
    Ko6mlPrimitive *primitive = gnc_ko6ml_primitive_new(
        KO6ML_CONCEPT, "account_concept", "Financial account concept");
    primitive->salience = 0.8;
    primitive->autonomy_index = 0.6;
    
    Ko6mlTranslationResult *result = gnc_ko6ml_to_atomspace(primitive);
    
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->success);
    EXPECT_NE(result->atom_handle, 0);
    EXPECT_EQ(result->error_message, nullptr);
    
    // Verify tensor shape
    EXPECT_GT(result->tensor_shape.modality, 0);
    EXPECT_GT(result->tensor_shape.depth, 0);
    EXPECT_GT(result->tensor_shape.context, 0);
    EXPECT_EQ(result->tensor_shape.salience, 0.8);
    EXPECT_EQ(result->tensor_shape.autonomy_index, 0.6);
    
    gnc_ko6ml_translation_result_free(result);
    gnc_ko6ml_primitive_free(primitive);
}

TEST_F(Ko6mlPrimitivesTest, AtomSpaceToKo6mlTranslation) {
    // First create and translate a primitive
    Ko6mlPrimitive *original = gnc_ko6ml_primitive_new(
        KO6ML_RELATION, "hasBalance", "Account balance relation");
    
    Ko6mlTranslationResult *to_atomspace = gnc_ko6ml_to_atomspace(original);
    ASSERT_TRUE(to_atomspace->success);
    
    // Translate back from AtomSpace
    Ko6mlPrimitive *back_translated = gnc_ko6ml_from_atomspace(to_atomspace->atom_handle);
    
    ASSERT_NE(back_translated, nullptr);
    EXPECT_EQ(back_translated->type, KO6ML_RELATION);
    EXPECT_STREQ(back_translated->name, "hasBalance");
    EXPECT_EQ(back_translated->atom_handle, to_atomspace->atom_handle);
    
    gnc_ko6ml_primitive_free(back_translated);
    gnc_ko6ml_translation_result_free(to_atomspace);
    gnc_ko6ml_primitive_free(original);
}

TEST_F(Ko6mlPrimitivesTest, RoundTripTranslationIntegrity) {
    // Test different primitive types
    Ko6mlPrimitiveType types[] = {
        KO6ML_CONCEPT, KO6ML_RELATION, KO6ML_AGENT, 
        KO6ML_STATE, KO6ML_PROCESS, KO6ML_CONTEXT
    };
    
    for (auto type : types) {
        gchar *name = g_strdup_printf("test_%s", gnc_ko6ml_primitive_type_to_string(type));
        Ko6mlPrimitive *primitive = gnc_ko6ml_primitive_new(type, name, "Round-trip test");
        
        primitive->salience = 0.7;
        primitive->autonomy_index = 0.9;
        gnc_ko6ml_primitive_set_property(primitive, "test_property", "test_value");
        
        EXPECT_TRUE(gnc_ko6ml_round_trip_test(primitive)) 
            << "Round-trip failed for type: " << gnc_ko6ml_primitive_type_to_string(type);
        
        gnc_ko6ml_primitive_free(primitive);
        g_free(name);
    }
}

TEST_F(Ko6mlPrimitivesTest, TranslationAccuracyValidation) {
    Ko6mlPrimitive *primitive = gnc_ko6ml_primitive_new(
        KO6ML_AGENT, "trading_agent", "Automated trading agent");
    primitive->salience = 0.95;
    primitive->autonomy_index = 0.85;
    
    Ko6mlTranslationResult *result = gnc_ko6ml_to_atomspace(primitive);
    ASSERT_TRUE(result->success);
    
    gdouble accuracy = gnc_ko6ml_validate_translation_accuracy(primitive, result);
    EXPECT_GE(accuracy, 0.8) << "Translation accuracy should be at least 80%";
    
    gnc_ko6ml_translation_result_free(result);
    gnc_ko6ml_primitive_free(primitive);
}

/********************************************************************\
 * Tensor Fragment Architecture Tests                              *
\********************************************************************/

TEST_F(Ko6mlPrimitivesTest, TensorShapeCreation) {
    Ko6mlTensorShape shape = gnc_ko6ml_create_tensor_shape(
        "test_agent", nullptr, 5, 3, 7);
    
    EXPECT_GT(shape.modality, 0);
    EXPECT_GT(shape.depth, 0);
    EXPECT_GT(shape.context, 0);
    EXPECT_GE(shape.salience, 0.0);
    EXPECT_LE(shape.salience, 1.0);
    EXPECT_GE(shape.autonomy_index, 0.0);
    EXPECT_LE(shape.autonomy_index, 1.0);
}

TEST_F(Ko6mlPrimitivesTest, TensorShapeValidation) {
    Ko6mlTensorShape valid_shape = {5, 3, 7, 0.8, 0.6};
    EXPECT_TRUE(gnc_ko6ml_validate_tensor_shape(&valid_shape));
    
    Ko6mlTensorShape invalid_shape1 = {0, 3, 7, 0.8, 0.6}; // Zero modality
    EXPECT_FALSE(gnc_ko6ml_validate_tensor_shape(&invalid_shape1));
    
    Ko6mlTensorShape invalid_shape2 = {5, 3, 7, 1.5, 0.6}; // Invalid salience
    EXPECT_FALSE(gnc_ko6ml_validate_tensor_shape(&invalid_shape2));
    
    Ko6mlTensorShape invalid_shape3 = {5, 3, 7, 0.8, -0.1}; // Invalid autonomy
    EXPECT_FALSE(gnc_ko6ml_validate_tensor_shape(&invalid_shape3));
}

TEST_F(Ko6mlPrimitivesTest, HypergraphNodeEncoding) {
    Ko6mlPrimitive *primitive = gnc_ko6ml_primitive_new(
        KO6ML_STATE, "account_state", "Current account state");
    
    Ko6mlTensorShape shape = {4, 2, 6, 0.7, 0.8};
    ASSERT_TRUE(gnc_ko6ml_validate_tensor_shape(&shape));
    
    GncAtomHandle node_handle = gnc_ko6ml_encode_hypergraph_node(primitive, &shape);
    EXPECT_NE(node_handle, 0);
    EXPECT_EQ(primitive->atom_handle, node_handle);
    
    gnc_ko6ml_primitive_free(primitive);
}

TEST_F(Ko6mlPrimitivesTest, HypergraphLinkEncoding) {
    Ko6mlPrimitive *source = gnc_ko6ml_primitive_new(
        KO6ML_AGENT, "source_agent", "Source agent");
    Ko6mlPrimitive *target = gnc_ko6ml_primitive_new(
        KO6ML_STATE, "target_state", "Target state");
    
    Ko6mlTensorShape shape = {3, 2, 4, 0.6, 0.7};
    
    GncAtomHandle link_handle = gnc_ko6ml_encode_hypergraph_link(
        source, target, "influences", &shape);
    
    EXPECT_NE(link_handle, 0);
    EXPECT_NE(source->atom_handle, 0);
    EXPECT_NE(target->atom_handle, 0);
    
    gnc_ko6ml_primitive_free(source);
    gnc_ko6ml_primitive_free(target);
}

/********************************************************************\
 * Prime Factorization Mapping Tests                              *
\********************************************************************/

TEST_F(Ko6mlPrimitivesTest, TensorToPrimeFactorization) {
    Ko6mlTensorShape shape = {3, 2, 5, 0.5, 0.8};
    gsize n_factors;
    
    guint *factors = gnc_ko6ml_tensor_to_prime_factors(&shape, &n_factors);
    
    ASSERT_NE(factors, nullptr);
    EXPECT_EQ(n_factors, 5);
    
    // Verify that all factors are prime numbers
    for (gsize i = 0; i < n_factors; i++) {
        EXPECT_GT(factors[i], 1);
        // Basic primality check for small numbers
        gboolean is_prime = TRUE;
        for (guint j = 2; j * j <= factors[i]; j++) {
            if (factors[i] % j == 0) {
                is_prime = FALSE;
                break;
            }
        }
        EXPECT_TRUE(is_prime) << "Factor " << factors[i] << " is not prime";
    }
    
    g_free(factors);
}

TEST_F(Ko6mlPrimitivesTest, PrimeFactorizationRoundTrip) {
    Ko6mlTensorShape original = {4, 3, 6, 0.6, 0.9};
    gsize n_factors;
    
    guint *factors = gnc_ko6ml_tensor_to_prime_factors(&original, &n_factors);
    ASSERT_NE(factors, nullptr);
    
    Ko6mlTensorShape reconstructed = gnc_ko6ml_prime_factors_to_tensor(factors, n_factors);
    
    // Allow for small differences due to quantization
    EXPECT_LE(abs((int)original.modality - (int)reconstructed.modality), 1);
    EXPECT_LE(abs((int)original.depth - (int)reconstructed.depth), 1);
    EXPECT_LE(abs((int)original.context - (int)reconstructed.context), 1);
    EXPECT_NEAR(original.salience, reconstructed.salience, 0.1);
    EXPECT_NEAR(original.autonomy_index, reconstructed.autonomy_index, 0.1);
    
    g_free(factors);
}

/********************************************************************\
 * Performance and Optimization Tests                             *
\********************************************************************/

TEST_F(Ko6mlPrimitivesTest, TensorOperationsBenchmark) {
    Ko6mlPrimitive *primitive = gnc_ko6ml_primitive_new(
        KO6ML_PROCESS, "benchmark_process", "Process for benchmarking");
    
    gdouble avg_time = gnc_ko6ml_benchmark_tensor_operations(primitive, 100);
    
    EXPECT_GT(avg_time, 0.0);
    EXPECT_LT(avg_time, 10000.0); // Should complete within 10ms per operation
    
    g_message("Tensor operations benchmark: %.2f μs per operation", avg_time);
    
    gnc_ko6ml_primitive_free(primitive);
}

TEST_F(Ko6mlPrimitivesTest, TensorShapeOptimization) {
    Ko6mlTensorShape original = {7, 5, 9, 0.7, 0.8};
    Ko6mlTensorShape optimized = gnc_ko6ml_optimize_tensor_shape(&original);
    
    // Optimized shape should use powers of 2 for better cache efficiency
    EXPECT_TRUE((optimized.modality & (optimized.modality - 1)) == 0); // Power of 2
    EXPECT_TRUE((optimized.depth & (optimized.depth - 1)) == 0);       // Power of 2
    EXPECT_TRUE((optimized.context & (optimized.context - 1)) == 0);   // Power of 2
    
    // Should be >= original dimensions
    EXPECT_GE(optimized.modality, original.modality);
    EXPECT_GE(optimized.depth, original.depth);
    EXPECT_GE(optimized.context, original.context);
    
    // Salience and autonomy should remain unchanged
    EXPECT_EQ(optimized.salience, original.salience);
    EXPECT_EQ(optimized.autonomy_index, original.autonomy_index);
}

/********************************************************************\
 * Integration Tests                                               *
\********************************************************************/

TEST_F(Ko6mlPrimitivesTest, AccountIntegration) {
    // Create a real account
    QofBook *book = qof_book_new();
    Account *account = xaccMallocAccount(book);
    xaccAccountSetName(account, "TestAccount");
    xaccAccountSetType(account, ACCT_TYPE_BANK);
    
    // Create ko6ml primitive for the account
    Ko6mlPrimitive *account_primitive = gnc_ko6ml_primitive_new(
        KO6ML_CONCEPT, "TestAccount", "Account concept");
    gnc_ko6ml_primitive_set_property(account_primitive, "account_type", "BANK");
    
    // Test translation to AtomSpace
    Ko6mlTranslationResult *result = gnc_ko6ml_to_atomspace(account_primitive);
    ASSERT_TRUE(result->success);
    
    // Test tensor encoding
    Ko6mlTensorShape shape = gnc_ko6ml_create_tensor_shape("TestAccount", account, 3, 2, 4);
    GncAtomHandle encoded_handle = gnc_ko6ml_encode_hypergraph_node(account_primitive, &shape);
    EXPECT_NE(encoded_handle, 0);
    
    // Cleanup
    gnc_ko6ml_translation_result_free(result);
    gnc_ko6ml_primitive_free(account_primitive);
    xaccAccountDestroy(account);
    qof_book_destroy(book);
}

TEST_F(Ko6mlPrimitivesTest, MultipleAgentStates) {
    // Create multiple agents and states
    Ko6mlPrimitive *agent1 = gnc_ko6ml_primitive_new(KO6ML_AGENT, "agent1", "First agent");
    Ko6mlPrimitive *agent2 = gnc_ko6ml_primitive_new(KO6ML_AGENT, "agent2", "Second agent");
    Ko6mlPrimitive *state1 = gnc_ko6ml_primitive_new(KO6ML_STATE, "state1", "First state");
    Ko6mlPrimitive *state2 = gnc_ko6ml_primitive_new(KO6ML_STATE, "state2", "Second state");
    
    // Create tensor shapes with different dimensions
    Ko6mlTensorShape shape1 = {2, 1, 3, 0.6, 0.7};
    Ko6mlTensorShape shape2 = {4, 2, 5, 0.8, 0.9};
    
    // Encode hypergraph nodes
    GncAtomHandle agent1_handle = gnc_ko6ml_encode_hypergraph_node(agent1, &shape1);
    GncAtomHandle agent2_handle = gnc_ko6ml_encode_hypergraph_node(agent2, &shape2);
    GncAtomHandle state1_handle = gnc_ko6ml_encode_hypergraph_node(state1, &shape1);
    GncAtomHandle state2_handle = gnc_ko6ml_encode_hypergraph_node(state2, &shape2);
    
    EXPECT_NE(agent1_handle, 0);
    EXPECT_NE(agent2_handle, 0);
    EXPECT_NE(state1_handle, 0);
    EXPECT_NE(state2_handle, 0);
    
    // Create links between agents and states
    GncAtomHandle link1 = gnc_ko6ml_encode_hypergraph_link(agent1, state1, "controls", &shape1);
    GncAtomHandle link2 = gnc_ko6ml_encode_hypergraph_link(agent2, state2, "monitors", &shape2);
    
    EXPECT_NE(link1, 0);
    EXPECT_NE(link2, 0);
    
    // Cleanup
    gnc_ko6ml_primitive_free(agent1);
    gnc_ko6ml_primitive_free(agent2);
    gnc_ko6ml_primitive_free(state1);
    gnc_ko6ml_primitive_free(state2);
}

/********************************************************************\
 * Error Handling Tests                                           *
\********************************************************************/

TEST_F(Ko6mlPrimitivesTest, NullParameterHandling) {
    EXPECT_EQ(gnc_ko6ml_primitive_new(KO6ML_CONCEPT, nullptr, "test"), nullptr);
    
    Ko6mlPrimitive *primitive = gnc_ko6ml_primitive_new(KO6ML_CONCEPT, "test", "test");
    EXPECT_EQ(gnc_ko6ml_to_atomspace(nullptr), nullptr);
    EXPECT_EQ(gnc_ko6ml_from_atomspace(0), nullptr);
    EXPECT_FALSE(gnc_ko6ml_round_trip_test(nullptr));
    
    gnc_ko6ml_primitive_free(primitive);
}

TEST_F(Ko6mlPrimitivesTest, InvalidTensorShapes) {
    Ko6mlTensorShape invalid_shape = {0, 0, 0, -1.0, 2.0};
    EXPECT_FALSE(gnc_ko6ml_validate_tensor_shape(&invalid_shape));
    
    Ko6mlPrimitive *primitive = gnc_ko6ml_primitive_new(KO6ML_CONCEPT, "test", "test");
    EXPECT_EQ(gnc_ko6ml_encode_hypergraph_node(primitive, &invalid_shape), 0);
    
    gnc_ko6ml_primitive_free(primitive);
}

/********************************************************************\
 * Main Test Runner                                               *
\********************************************************************/

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    g_test_init(&argc, &argv, nullptr);
    
    return RUN_ALL_TESTS();
}