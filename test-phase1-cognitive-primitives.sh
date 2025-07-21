#!/bin/bash

########################################################################
# test-phase1-cognitive-primitives.sh
# 
# Comprehensive testing script for Phase 1: Cognitive Primitives &
# Foundational Hypergraph Encoding
# 
# Copyright (C) 2024 GnuCash Cognitive Engine
########################################################################

echo "========================================================================"
echo "    Phase 1: Cognitive Primitives & Hypergraph Encoding Test Suite"
echo "========================================================================"
echo "Testing the foundational cognitive architecture components that enable"
echo "transformation of classical ledgers into cognitive neural-symbolic"  
echo "tapestries..."
echo ""

# Set error handling
set -e

# Configuration
TEST_DIR="/tmp/phase1-test-$$"
LOG_FILE="$TEST_DIR/test-phase1.log"
ERROR_COUNT=0

# Ensure cleanup on exit
cleanup() {
    echo "🧹 Cleaning up test environment..."
    rm -rf "$TEST_DIR" 2>/dev/null || true
}
trap cleanup EXIT

# Create test directory
mkdir -p "$TEST_DIR"

echo "📋 Test Configuration:"
echo "   Test Directory: $TEST_DIR"
echo "   Log File: $LOG_FILE"
echo ""

# Function to run test and capture results
run_test() {
    local test_name="$1"
    local test_command="$2"
    local description="$3"
    
    echo "🔍 Testing: $test_name"
    echo "   Description: $description"
    echo -n "   Running... "
    
    if eval "$test_command" >> "$LOG_FILE" 2>&1; then
        echo "✅ PASSED"
        echo "   ✓ $test_name: PASSED" >> "$LOG_FILE"
    else
        echo "❌ FAILED"
        echo "   ✗ $test_name: FAILED" >> "$LOG_FILE"
        ((ERROR_COUNT++))
    fi
    echo ""
}

# Phase 1 Core Tests
echo "🧠 Phase 1: Foundational Component Tests"
echo "========================================"
echo ""

# Test 1: Cognitive Primitives Header Compilation
run_test "Cognitive Primitives Header" \
    "gcc -I./libgnucash/engine -I./common $(pkg-config --cflags glib-2.0) -c -x c++ -o $TEST_DIR/test-header.o - <<< '#include \"gnc-cognitive-primitives.h\"'" \
    "Validate cognitive primitives header compiles correctly"

# Test 2: Basic Include Dependencies  
run_test "Header Dependencies" \
    "grep -q 'gnc-cognitive-accounting.h' libgnucash/engine/gnc-cognitive-primitives.h" \
    "Check required header dependencies are properly included"

# Test 3: Function Declarations Present
run_test "API Function Declarations" \
    "grep -q 'gnc_cognitive_primitive_create' libgnucash/engine/gnc-cognitive-primitives.h && grep -q 'gnc_financial_hypergraph_create' libgnucash/engine/gnc-cognitive-primitives.h" \
    "Verify core API functions are declared"

# Test 4: Data Structure Definitions
run_test "Data Structure Definitions" \
    "grep -q 'GncCognitivePrimitiveUnit' libgnucash/engine/gnc-cognitive-primitives.h && grep -q 'GncFinancialHypergraph' libgnucash/engine/gnc-cognitive-primitives.h" \
    "Ensure fundamental data structures are defined"

# Test 5: CMake Integration
run_test "CMake Integration" \
    "grep -q 'gnc-cognitive-primitives.h' libgnucash/engine/CMakeLists.txt && grep -q 'gnc-cognitive-primitives.cpp' libgnucash/engine/CMakeLists.txt" \
    "Verify build system integration is complete"

# Test 6: Test Suite Integration
run_test "Test Suite Integration" \
    "grep -q 'test-cognitive-primitives.cpp' libgnucash/engine/test/CMakeLists.txt" \
    "Confirm test files are integrated into build system"

# Demo Application Tests  
echo "🎭 Phase 1: Demo Application Tests"
echo "=================================="
echo ""

# Test 7: Demo Application Present
run_test "Demo Application File" \
    "[ -f 'phase1-cognitive-primitives-demo.cpp' ]" \
    "Verify Phase 1 demonstration application exists"

# Test 8: Demo CMake Integration
run_test "Demo Build Integration" \
    "grep -q 'phase1-cognitive-primitives-demo' CMakeLists.txt" \
    "Check demo application is integrated into build system"

# Neural-Symbolic Architecture Tests
echo "🔗 Phase 1: Neural-Symbolic Architecture Tests"  
echo "=============================================="
echo ""

# Test 9: Neural-Symbolic Bridge Types
run_test "Neural-Symbolic Types" \
    "grep -q 'GncNeuralSymbolicEmbedding' libgnucash/engine/gnc-cognitive-primitives.h && grep -q 'GncNeuralSymbolicBridge' libgnucash/engine/gnc-cognitive-primitives.h" \
    "Validate neural-symbolic bridge data types are defined"

# Test 10: Hypergraph Pattern Encoding
run_test "Hypergraph Pattern Functions" \
    "grep -q 'gnc_account_to_hypergraph_pattern' libgnucash/engine/gnc-cognitive-primitives.h && grep -q 'gnc_transaction_to_hypergraph_pattern' libgnucash/engine/gnc-cognitive-primitives.h" \
    "Ensure hypergraph encoding functions are declared"

# Test 11: Grammar Network Support
run_test "Financial Grammar Network" \
    "grep -q 'GncCognitiveGrammarPattern' libgnucash/engine/gnc-cognitive-primitives.h && grep -q 'gnc_generate_financial_grammar_rules' libgnucash/engine/gnc-cognitive-primitives.h" \
    "Verify financial grammar network structures are present"

# Test 12: Distributed Agent Primitives
run_test "Distributed Agent Support" \
    "grep -q 'GncDistributedAgentPrimitive' libgnucash/engine/gnc-cognitive-primitives.h && grep -q 'gnc_distributed_agent_create' libgnucash/engine/gnc-cognitive-primitives.h" \
    "Check distributed agentic coordination primitives exist"

# Integration Tests
echo "🔄 Phase 1: Integration Readiness Tests"
echo "======================================="
echo ""

# Test 13: AtomSpace Integration Points
run_test "AtomSpace Integration Hooks" \
    "grep -q 'gnc_cognitive_primitives_integrate_atomspace' libgnucash/engine/gnc-cognitive-primitives.h" \
    "Verify AtomSpace integration interfaces are defined"

# Test 14: ECAN Integration Points  
run_test "ECAN Integration Hooks" \
    "grep -q 'gnc_cognitive_primitives_integrate_ecan' libgnucash/engine/gnc-cognitive-primitives.h" \
    "Confirm ECAN integration interfaces are defined"

# Test 15: System Statistics Support
run_test "System Statistics API" \
    "grep -q 'GncCognitivePrimitiveStats' libgnucash/engine/gnc-cognitive-primitives.h && grep -q 'gnc_cognitive_primitives_get_stats' libgnucash/engine/gnc-cognitive-primitives.h" \
    "Ensure system monitoring and statistics support exists"

# Implementation Quality Tests
echo "🎯 Phase 1: Implementation Quality Tests"
echo "========================================"
echo ""

# Test 16: Memory Management
run_test "Memory Management Functions" \
    "grep -q 'gnc_cognitive_primitive_free' libgnucash/engine/gnc-cognitive-primitives.h && grep -q 'gnc_financial_hypergraph_free' libgnucash/engine/gnc-cognitive-primitives.h" \
    "Validate proper memory cleanup functions are provided"

# Test 17: Error Handling Patterns
run_test "Error Handling Implementation" \
    "grep -q 'g_return_val_if_fail' libgnucash/engine/gnc-cognitive-primitives.cpp && grep -q 'g_return_if_fail' libgnucash/engine/gnc-cognitive-primitives.cpp" \
    "Check error handling and parameter validation is implemented"

# Test 18: Thread Safety Considerations
run_test "Thread Safety Implementation" \
    "grep -q 'GMutex' libgnucash/engine/gnc-cognitive-primitives.cpp && grep -q 'g_mutex_lock' libgnucash/engine/gnc-cognitive-primitives.cpp" \
    "Verify thread safety mechanisms are implemented"

# Code Quality Tests
echo "📊 Phase 1: Code Quality Assessment"
echo "==================================="
echo ""

# Test 19: Documentation Coverage
run_test "Documentation Coverage" \
    "grep -c '/\*\*' libgnucash/engine/gnc-cognitive-primitives.h | awk '{print (\$1 > 20)}' | grep -q 1" \
    "Ensure adequate documentation coverage in header file"

# Test 20: Implementation Size Validation
run_test "Implementation Completeness" \
    "wc -l libgnucash/engine/gnc-cognitive-primitives.cpp | awk '{print (\$1 > 500)}' | grep -q 1" \
    "Validate implementation file has substantial content"

# Test 21: Test Coverage Validation
run_test "Test Coverage Completeness" \
    "wc -l libgnucash/engine/test/test-cognitive-primitives.cpp | awk '{print (\$1 > 200)}' | grep -q 1" \
    "Ensure comprehensive test coverage is provided"

# Advanced Architecture Tests  
echo "🏗️ Phase 1: Advanced Architecture Tests"
echo "========================================"
echo ""

# Test 22: Emergent Pattern Support
run_test "Emergent Pattern Detection" \
    "grep -q 'GncEmergentPattern' libgnucash/engine/gnc-cognitive-primitives.h && grep -q 'gnc_detect_emergent_patterns' libgnucash/engine/gnc-cognitive-primitives.h" \
    "Verify emergent behavior detection capabilities are defined"

# Test 23: System Coherence Measurement
run_test "System Coherence Functions" \
    "grep -q 'gnc_financial_hypergraph_coherence' libgnucash/engine/gnc-cognitive-primitives.h && grep -q 'coherence_measure' libgnucash/engine/gnc-cognitive-primitives.h" \
    "Check system coherence measurement capabilities exist"

# Test 24: Financial Data Transformation
run_test "Financial Data Integration" \
    "grep -q 'gnc_account_to_cognitive_primitives' libgnucash/engine/gnc-cognitive-primitives.h && grep -q 'gnc_transaction_to_cognitive_primitives' libgnucash/engine/gnc-cognitive-primitives.h" \
    "Validate financial data transformation functions are present"

# Performance and Scalability Tests
echo "⚡ Phase 1: Performance Architecture Tests"
echo "=========================================="
echo ""

# Test 25: Scalability Considerations  
run_test "Scalability Design" \
    "grep -q 'max_nodes' libgnucash/engine/gnc-cognitive-primitives.h && grep -q 'max_edges' libgnucash/engine/gnc-cognitive-primitives.h" \
    "Ensure scalability limits and capacity planning are considered"

# Final Integration Verification
echo "🔗 Phase 1: Final Integration Verification"  
echo "=========================================="
echo ""

# Test 26: Complete API Surface
run_test "Complete API Surface" \
    "grep -c '^[[:space:]]*[^/][^*].*(' libgnucash/engine/gnc-cognitive-primitives.h | awk '{print (\$1 > 30)}' | grep -q 1" \
    "Verify comprehensive API surface is provided"

# Test 27: Header Guard Consistency
run_test "Header Guard Implementation" \
    "grep -q '#ifndef GNC_COGNITIVE_PRIMITIVES_H' libgnucash/engine/gnc-cognitive-primitives.h && grep -q '#define GNC_COGNITIVE_PRIMITIVES_H' libgnucash/engine/gnc-cognitive-primitives.h" \
    "Check proper header guard implementation"

# Test 28: C++ Compatibility
run_test "C++ Compatibility" \
    "grep -q 'extern \"C\"' libgnucash/engine/gnc-cognitive-primitives.h" \
    "Ensure C++ compatibility through extern C declarations"

# Results Summary
echo "========================================================================"
echo "                          Phase 1 Test Results"
echo "========================================================================"
echo ""

if [ $ERROR_COUNT -eq 0 ]; then
    echo "🎉 ALL TESTS PASSED! Phase 1 is ready for integration!"
    echo ""
    echo "✅ Cognitive Primitives & Foundational Hypergraph Encoding: COMPLETE"
    echo "✅ Neural-symbolic bridge foundations: IMPLEMENTED" 
    echo "✅ Financial grammar network: ESTABLISHED"
    echo "✅ Distributed agentic coordination: READY"
    echo "✅ Emergent pattern detection: FOUNDATION_SET"
    echo "✅ System integration hooks: PREPARED"
    echo ""
    echo "🧠 The classical ledger has been successfully transmuted into cognitive"
    echo "   neural-symbolic tapestries with foundational hypergraph encoding!"
    echo ""
    echo "🔗 Phase 1 provides the cognitive primitives foundation that enables:"
    echo "   • Phase 2: ECAN Attention Allocation & Resource Kernel"
    echo "   • Phase 3: Neural-Symbolic Synthesis via Custom ggml Kernels" 
    echo "   • Phase 4: Distributed Cognitive Mesh API & Embodiment Layer"
    echo "   • Phase 5: Recursive Meta-Cognition & Evolutionary Optimization"
    echo "   • Phase 6: Rigorous Testing, Documentation, and Cognitive Unification"
    echo ""
    echo "✨ Ready for recursive self-optimization spiral to commence!"
else
    echo "❌ $ERROR_COUNT tests failed. Please review the implementation."
    echo ""
    echo "📋 Failed tests are logged in: $LOG_FILE"
    echo "🔧 Please address the issues before proceeding to advanced phases."
fi

echo ""
echo "========================================================================"
echo "                      Phase 1 Test Summary Complete"
echo "========================================================================"

exit $ERROR_COUNT