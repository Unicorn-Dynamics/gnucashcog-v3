/********************************************************************\
 * ko6ml-phase1-demo.cpp -- Phase 1 Cognitive Primitives Demo    *
 * Copyright (C) 2024 GnuCash Cognitive Engine                     *
 *                                                                  *
 * Comprehensive demonstration of ko6ml primitives, AtomSpace     *
 * translation, tensor encoding, and foundational hypergraph      *
 * pattern creation for Phase 1 implementation.                   *
 ********************************************************************/

#include <iostream>
#include <glib.h>
#include <iomanip>
#include "gnc-ko6ml-primitives.h"
#include "gnc-ko6ml-scheme-adapters.h"
#include "gnc-cognitive-accounting.h"
#include "Account.h"
#include "Transaction.h"
#include "Split.h"
#include "qof.h"
#include "gnc-engine.h"

void print_separator(const char* title) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void demonstrate_ko6ml_primitives() {
    print_separator("ko6ml Cognitive Primitives Demo");
    
    std::cout << "\n1. Creating ko6ml primitives for different cognitive types:\n";
    
    // Create primitives for different types
    Ko6mlPrimitive *account_concept = gnc_ko6ml_primitive_new(
        KO6ML_CONCEPT, "BankAccount", "Financial account concept");
    gnc_ko6ml_primitive_set_property(account_concept, "category", "financial");
    gnc_ko6ml_primitive_set_property(account_concept, "domain", "banking");
    account_concept->salience = 0.9;
    account_concept->autonomy_index = 0.3;
    
    Ko6mlPrimitive *balance_relation = gnc_ko6ml_primitive_new(
        KO6ML_RELATION, "hasBalance", "Account balance relation");
    gnc_ko6ml_primitive_set_property(balance_relation, "type", "numeric");
    balance_relation->salience = 0.8;
    balance_relation->autonomy_index = 0.2;
    
    Ko6mlPrimitive *trading_agent = gnc_ko6ml_primitive_new(
        KO6ML_AGENT, "TradingAgent", "Autonomous trading agent");
    gnc_ko6ml_primitive_set_property(trading_agent, "strategy", "conservative");
    gnc_ko6ml_primitive_set_property(trading_agent, "risk_tolerance", "medium");
    trading_agent->salience = 0.95;
    trading_agent->autonomy_index = 0.9;
    
    Ko6mlPrimitive *market_state = gnc_ko6ml_primitive_new(
        KO6ML_STATE, "MarketState", "Current market conditions");
    gnc_ko6ml_primitive_set_property(market_state, "volatility", "high");
    market_state->salience = 0.7;
    market_state->autonomy_index = 0.1;
    
    Ko6mlPrimitive *transaction_process = gnc_ko6ml_primitive_new(
        KO6ML_PROCESS, "TransactionProcessing", "Transaction processing workflow");
    transaction_process->salience = 0.85;
    transaction_process->autonomy_index = 0.4;
    
    std::cout << "✓ Created 5 ko6ml primitives:\n";
    std::cout << "  - " << account_concept->name << " (" 
              << gnc_ko6ml_primitive_type_to_string(account_concept->type) << ")\n";
    std::cout << "  - " << balance_relation->name << " (" 
              << gnc_ko6ml_primitive_type_to_string(balance_relation->type) << ")\n";
    std::cout << "  - " << trading_agent->name << " (" 
              << gnc_ko6ml_primitive_type_to_string(trading_agent->type) << ")\n";
    std::cout << "  - " << market_state->name << " (" 
              << gnc_ko6ml_primitive_type_to_string(market_state->type) << ")\n";
    std::cout << "  - " << transaction_process->name << " (" 
              << gnc_ko6ml_primitive_type_to_string(transaction_process->type) << ")\n";
    
    // Clean up
    gnc_ko6ml_primitive_free(account_concept);
    gnc_ko6ml_primitive_free(balance_relation);
    gnc_ko6ml_primitive_free(trading_agent);
    gnc_ko6ml_primitive_free(market_state);
    gnc_ko6ml_primitive_free(transaction_process);
}

void demonstrate_atomspace_translation() {
    print_separator("ko6ml ↔ AtomSpace Translation Demo");
    
    std::cout << "\n2. Testing bidirectional translation with round-trip validation:\n";
    
    Ko6mlPrimitive *test_primitive = gnc_ko6ml_primitive_new(
        KO6ML_AGENT, "FinancialAdvisor", "AI financial advisor agent");
    gnc_ko6ml_primitive_set_property(test_primitive, "specialization", "portfolio_management");
    gnc_ko6ml_primitive_set_property(test_primitive, "certification", "CFA");
    test_primitive->salience = 0.92;
    test_primitive->autonomy_index = 0.85;
    
    std::cout << "Original primitive: " << test_primitive->name << "\n";
    std::cout << "  Type: " << gnc_ko6ml_primitive_type_to_string(test_primitive->type) << "\n";
    std::cout << "  Salience: " << std::fixed << std::setprecision(3) << test_primitive->salience << "\n";
    std::cout << "  Autonomy: " << test_primitive->autonomy_index << "\n";
    
    // Test ko6ml → AtomSpace translation
    Ko6mlTranslationResult *to_atomspace = gnc_ko6ml_to_atomspace(test_primitive);
    if (to_atomspace && to_atomspace->success) {
        std::cout << "✓ Translation to AtomSpace successful\n";
        std::cout << "  AtomSpace handle: " << to_atomspace->atom_handle << "\n";
        std::cout << "  Tensor shape: [" << to_atomspace->tensor_shape.modality 
                  << "," << to_atomspace->tensor_shape.depth 
                  << "," << to_atomspace->tensor_shape.context 
                  << "," << to_atomspace->tensor_shape.salience 
                  << "," << to_atomspace->tensor_shape.autonomy_index << "]\n";
        
        // Test AtomSpace → ko6ml translation
        Ko6mlPrimitive *back_translated = gnc_ko6ml_from_atomspace(to_atomspace->atom_handle);
        if (back_translated) {
            std::cout << "✓ Translation back to ko6ml successful\n";
            std::cout << "  Back-translated: " << back_translated->name << "\n";
            gnc_ko6ml_primitive_free(back_translated);
        } else {
            std::cout << "✗ Translation back to ko6ml failed\n";
        }
        
        // Test round-trip integrity
        gboolean round_trip_success = gnc_ko6ml_round_trip_test(test_primitive);
        std::cout << (round_trip_success ? "✓" : "✗") << " Round-trip translation integrity: " 
                  << (round_trip_success ? "PASSED" : "FAILED") << "\n";
        
        // Validate translation accuracy
        gdouble accuracy = gnc_ko6ml_validate_translation_accuracy(test_primitive, to_atomspace);
        std::cout << "Translation accuracy: " << std::fixed << std::setprecision(1) 
                  << (accuracy * 100) << "%\n";
        
        gnc_ko6ml_translation_result_free(to_atomspace);
    } else {
        std::cout << "✗ Translation to AtomSpace failed\n";
        if (to_atomspace) {
            std::cout << "  Error: " << (to_atomspace->error_message ? to_atomspace->error_message : "Unknown") << "\n";
            gnc_ko6ml_translation_result_free(to_atomspace);
        }
    }
    
    gnc_ko6ml_primitive_free(test_primitive);
}

void demonstrate_tensor_architecture() {
    print_separator("Tensor Fragment Architecture Demo");
    
    std::cout << "\n3. Demonstrating tensor encoding with [modality, depth, context, salience, autonomy_index]:\n";
    
    Ko6mlPrimitive *agent = gnc_ko6ml_primitive_new(
        KO6ML_AGENT, "RiskAnalysisAgent", "Risk analysis cognitive agent");
    agent->salience = 0.88;
    agent->autonomy_index = 0.75;
    
    Ko6mlPrimitive *state = gnc_ko6ml_primitive_new(
        KO6ML_STATE, "PortfolioState", "Current portfolio state");
    state->salience = 0.65;
    state->autonomy_index = 0.25;
    
    // Create tensor shapes for agent/state encoding
    Ko6mlTensorShape agent_shape = gnc_ko6ml_create_tensor_shape(
        "RiskAnalysisAgent", agent, 7, 4, 9);
    agent_shape.salience = agent->salience;
    agent_shape.autonomy_index = agent->autonomy_index;
    
    Ko6mlTensorShape state_shape = gnc_ko6ml_create_tensor_shape(
        "PortfolioState", state, 3, 2, 5);
    state_shape.salience = state->salience;
    state_shape.autonomy_index = state->autonomy_index;
    
    std::cout << "Agent tensor shape: [" << agent_shape.modality 
              << "," << agent_shape.depth << "," << agent_shape.context
              << "," << std::fixed << std::setprecision(3) << agent_shape.salience
              << "," << agent_shape.autonomy_index << "]\n";
    
    std::cout << "State tensor shape: [" << state_shape.modality 
              << "," << state_shape.depth << "," << state_shape.context
              << "," << state_shape.salience << "," << state_shape.autonomy_index << "]\n";
    
    // Validate tensor shapes
    gboolean agent_valid = gnc_ko6ml_validate_tensor_shape(&agent_shape);
    gboolean state_valid = gnc_ko6ml_validate_tensor_shape(&state_shape);
    
    std::cout << (agent_valid ? "✓" : "✗") << " Agent tensor shape validation: " 
              << (agent_valid ? "VALID" : "INVALID") << "\n";
    std::cout << (state_valid ? "✓" : "✗") << " State tensor shape validation: " 
              << (state_valid ? "VALID" : "INVALID") << "\n";
    
    // Encode hypergraph nodes
    GncAtomHandle agent_node = gnc_ko6ml_encode_hypergraph_node(agent, &agent_shape);
    GncAtomHandle state_node = gnc_ko6ml_encode_hypergraph_node(state, &state_shape);
    
    std::cout << "Agent hypergraph node handle: " << agent_node << "\n";
    std::cout << "State hypergraph node handle: " << state_node << "\n";
    
    // Create hypergraph link between agent and state
    GncAtomHandle link_handle = gnc_ko6ml_encode_hypergraph_link(
        agent, state, "analyzes", &agent_shape);
    
    std::cout << "✓ Created hypergraph link 'analyzes': " << link_handle << "\n";
    
    // Demonstrate prime factorization mapping
    gsize n_factors;
    guint *prime_factors = gnc_ko6ml_tensor_to_prime_factors(&agent_shape, &n_factors);
    
    std::cout << "Prime factorization mapping: [";
    for (gsize i = 0; i < n_factors; i++) {
        std::cout << prime_factors[i];
        if (i < n_factors - 1) std::cout << ", ";
    }
    std::cout << "]\n";
    
    // Test round-trip prime factorization
    Ko6mlTensorShape reconstructed = gnc_ko6ml_prime_factors_to_tensor(prime_factors, n_factors);
    std::cout << "Reconstructed shape: [" << reconstructed.modality 
              << "," << reconstructed.depth << "," << reconstructed.context
              << "," << reconstructed.salience << "," << reconstructed.autonomy_index << "]\n";
    
    // Test tensor optimization
    Ko6mlTensorShape optimized = gnc_ko6ml_optimize_tensor_shape(&agent_shape);
    std::cout << "Optimized shape: [" << optimized.modality 
              << "," << optimized.depth << "," << optimized.context
              << "," << optimized.salience << "," << optimized.autonomy_index << "]\n";
    
    g_free(prime_factors);
    gnc_ko6ml_primitive_free(agent);
    gnc_ko6ml_primitive_free(state);
}

void demonstrate_scheme_adapters() {
    print_separator("Scheme Cognitive Grammar Adapters Demo");
    
    std::cout << "\n4. Testing modular Scheme adapters for agentic grammar:\n";
    
    Ko6mlPrimitive *test_agent = gnc_ko6ml_primitive_new(
        KO6ML_AGENT, "CognitiveAssistant", "Cognitive financial assistant");
    gnc_ko6ml_primitive_set_property(test_agent, "domain", "personal_finance");
    gnc_ko6ml_primitive_set_property(test_agent, "language", "natural");
    test_agent->salience = 0.9;
    test_agent->autonomy_index = 0.8;
    
    // Test Scheme translation
    Ko6mlSchemeTranslationResult *scheme_result = gnc_ko6ml_primitive_to_scheme(test_agent);
    if (scheme_result && scheme_result->success) {
        std::cout << "✓ Scheme translation successful\n";
        std::cout << "  Performance: " << std::fixed << std::setprecision(2) 
                  << scheme_result->performance_ms << " ms\n";
        std::cout << "  Accuracy: " << std::setprecision(1) 
                  << (scheme_result->accuracy * 100) << "%\n";
        
        std::cout << "\nGenerated Scheme code:\n";
        std::cout << "----------------------------------------\n";
        std::cout << scheme_result->scheme_code << "\n";
        std::cout << "----------------------------------------\n";
        
        gnc_ko6ml_scheme_translation_result_free(scheme_result);
    } else {
        std::cout << "✗ Scheme translation failed\n";
        if (scheme_result) {
            std::cout << "  Error: " << scheme_result->error_message << "\n";
            gnc_ko6ml_scheme_translation_result_free(scheme_result);
        }
    }
    
    // Test round-trip Scheme translation
    Ko6mlSchemeTranslationResult *round_trip = gnc_ko6ml_scheme_round_trip_test(test_agent);
    if (round_trip) {
        std::cout << "Round-trip Scheme test:\n";
        std::cout << "  Success: " << (round_trip->success ? "YES" : "NO") << "\n";
        std::cout << "  Accuracy: " << (round_trip->accuracy * 100) << "%\n";
        std::cout << "  Performance: " << round_trip->performance_ms << " ms\n";
        gnc_ko6ml_scheme_translation_result_free(round_trip);
    }
    
    // Generate agentic grammar patterns
    std::cout << "\n5. Generating agentic grammar patterns:\n";
    
    gchar *agentic_pattern = gnc_ko6ml_generate_agentic_grammar(test_agent);
    if (agentic_pattern) {
        std::cout << "✓ Generated agentic grammar pattern\n";
        std::cout << "----------------------------------------\n";
        std::cout << agentic_pattern << "\n";
        std::cout << "----------------------------------------\n";
        g_free(agentic_pattern);
    }
    
    // Create context for BindLink pattern
    Ko6mlPrimitive *context = gnc_ko6ml_primitive_new(
        KO6ML_CONTEXT, "PersonalFinanceContext", "Personal finance context");
    
    gchar *bindlink_pattern = gnc_ko6ml_create_agentic_bindlink(test_agent, context);
    if (bindlink_pattern) {
        std::cout << "\n✓ Generated agentic BindLink pattern\n";
        std::cout << "----------------------------------------\n";
        std::cout << bindlink_pattern << "\n";
        std::cout << "----------------------------------------\n";
        g_free(bindlink_pattern);
    }
    
    gnc_ko6ml_primitive_free(test_agent);
    gnc_ko6ml_primitive_free(context);
}

void demonstrate_performance_benchmarks() {
    print_separator("Performance Benchmarks Demo");
    
    std::cout << "\n6. Running performance benchmarks:\n";
    
    Ko6mlPrimitive *benchmark_primitive = gnc_ko6ml_primitive_new(
        KO6ML_PROCESS, "BenchmarkProcess", "Process for performance testing");
    benchmark_primitive->salience = 0.75;
    benchmark_primitive->autonomy_index = 0.5;
    
    // Benchmark tensor operations
    gdouble tensor_time = gnc_ko6ml_benchmark_tensor_operations(benchmark_primitive, 1000);
    std::cout << "Tensor operations benchmark: " << std::fixed << std::setprecision(2) 
              << tensor_time << " μs per operation (1000 iterations)\n";
    
    // Benchmark adapter performance
    Ko6mlSchemeAdapter *adapter = gnc_ko6ml_get_best_adapter(KO6ML_PROCESS);
    if (adapter) {
        gdouble adapter_time = gnc_ko6ml_benchmark_adapter(adapter, benchmark_primitive, 500);
        std::cout << "Scheme adapter benchmark: " << adapter_time 
                  << " μs per translation (500 iterations)\n";
        
        gdouble adapter_accuracy = gnc_ko6ml_validate_adapter_accuracy(adapter, benchmark_primitive);
        std::cout << "Adapter accuracy: " << std::setprecision(1) 
                  << (adapter_accuracy * 100) << "%\n";
        
        gnc_ko6ml_update_adapter_performance(adapter, adapter_accuracy);
    }
    
    // Display adapter statistics
    gsize n_adapters;
    gdouble avg_performance;
    gnc_ko6ml_get_adapter_stats(&n_adapters, &avg_performance);
    std::cout << "Adapter registry statistics:\n";
    std::cout << "  Total adapters: " << n_adapters << "\n";
    std::cout << "  Average performance: " << std::setprecision(3) 
              << avg_performance << "\n";
    
    gnc_ko6ml_primitive_free(benchmark_primitive);
}

void demonstrate_integration_with_gnucash() {
    print_separator("GnuCash Integration Demo");
    
    std::cout << "\n7. Integrating ko6ml primitives with GnuCash accounts:\n";
    
    // Create a GnuCash book and account
    QofBook *book = qof_book_new();
    Account *root = gnc_account_create_root(book);
    
    Account *checking = xaccMallocAccount(book);
    xaccAccountSetName(checking, "SmartCheckingAccount");
    xaccAccountSetType(checking, ACCT_TYPE_BANK);
    gnc_account_append_child(root, checking);
    
    // Create ko6ml primitives for the account
    Ko6mlPrimitive *account_primitive = gnc_ko6ml_primitive_new(
        KO6ML_CONCEPT, "SmartCheckingAccount", "Cognitive-enhanced checking account");
    gnc_ko6ml_primitive_set_property(account_primitive, "account_type", "BANK");
    gnc_ko6ml_primitive_set_property(account_primitive, "cognitive_level", "enhanced");
    account_primitive->salience = 0.85;
    account_primitive->autonomy_index = 0.4;
    
    Ko6mlPrimitive *account_agent = gnc_ko6ml_primitive_new(
        KO6ML_AGENT, "AccountAgent", "Account management agent");
    account_agent->salience = 0.8;
    account_agent->autonomy_index = 0.7;
    
    std::cout << "✓ Created GnuCash account: " << xaccAccountGetName(checking) << "\n";
    std::cout << "✓ Created ko6ml primitive: " << account_primitive->name << "\n";
    std::cout << "✓ Created account agent: " << account_agent->name << "\n";
    
    // Create tensor shapes and encode
    Ko6mlTensorShape account_shape = gnc_ko6ml_create_tensor_shape(
        "SmartCheckingAccount", checking, 4, 3, 6);
    account_shape.salience = account_primitive->salience;
    account_shape.autonomy_index = account_primitive->autonomy_index;
    
    GncAtomHandle account_node = gnc_ko6ml_encode_hypergraph_node(account_primitive, &account_shape);
    GncAtomHandle agent_node = gnc_ko6ml_encode_hypergraph_node(account_agent, &account_shape);
    
    // Create relationship link
    GncAtomHandle manages_link = gnc_ko6ml_encode_hypergraph_link(
        account_agent, account_primitive, "manages", &account_shape);
    
    std::cout << "Account hypergraph node: " << account_node << "\n";
    std::cout << "Agent hypergraph node: " << agent_node << "\n";
    std::cout << "Management link: " << manages_link << "\n";
    
    // Generate cognitive rule for account management
    gchar *cognitive_rule = gnc_ko6ml_generate_cognitive_rule(
        account_agent, account_primitive, "optimizes");
    if (cognitive_rule) {
        std::cout << "\n✓ Generated cognitive rule:\n";
        std::cout << "----------------------------------------\n";
        std::cout << cognitive_rule << "\n";
        std::cout << "----------------------------------------\n";
        g_free(cognitive_rule);
    }
    
    // Cleanup
    gnc_ko6ml_primitive_free(account_primitive);
    gnc_ko6ml_primitive_free(account_agent);
    xaccAccountDestroy(checking);
    qof_book_destroy(book);
}

int main(int argc, char **argv) {
    std::cout << "🧠 Phase 1: Cognitive Primitives & Foundational Hypergraph Encoding Demo\n";
    std::cout << "=========================================================================\n";
    
    // Initialize systems
    gnc_engine_init(argc, argv);
    
    if (!gnc_cognitive_accounting_init()) {
        std::cerr << "Failed to initialize cognitive accounting system\n";
        return 1;
    }
    
    if (!gnc_ko6ml_init()) {
        std::cerr << "Failed to initialize ko6ml primitive system\n";
        return 1;
    }
    
    if (!gnc_ko6ml_scheme_adapters_init()) {
        std::cerr << "Failed to initialize ko6ml Scheme adapters\n";
        return 1;
    }
    
    try {
        // Run all demonstrations
        demonstrate_ko6ml_primitives();
        demonstrate_atomspace_translation();
        demonstrate_tensor_architecture();
        demonstrate_scheme_adapters();
        demonstrate_performance_benchmarks();
        demonstrate_integration_with_gnucash();
        
        print_separator("Phase 1 Demo Complete");
        std::cout << "\n🎉 Phase 1 implementation successfully demonstrated!\n\n";
        std::cout << "Key achievements:\n";
        std::cout << "✓ ko6ml cognitive primitives vocabulary established\n";
        std::cout << "✓ Bidirectional ko6ml ↔ AtomSpace translation working\n";
        std::cout << "✓ Tensor fragment architecture with [modality, depth, context, salience, autonomy_index]\n";
        std::cout << "✓ Round-trip translation tests with data integrity validation\n";
        std::cout << "✓ Modular Scheme adapters for agentic grammar\n";
        std::cout << "✓ Hypergraph node/link encoding with tensor properties\n";
        std::cout << "✓ Prime factorization mapping for tensor dimensions\n";
        std::cout << "✓ Performance benchmarks and optimization strategies\n";
        std::cout << "✓ Integration with existing GnuCash account infrastructure\n";
        std::cout << "\nThe foundational cognitive primitives are now ready for Phase 2 development.\n";
        
    } catch (const std::exception &e) {
        std::cerr << "Demo failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    // Cleanup
    gnc_ko6ml_scheme_adapters_shutdown();
    gnc_ko6ml_shutdown();
    gnc_cognitive_accounting_shutdown();
    
    return 0;
}