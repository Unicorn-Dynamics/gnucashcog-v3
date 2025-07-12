# ko6ml Cognitive Primitives API Documentation

## Overview

The ko6ml (Knowledge-Oriented 6-Dimensional Modeling Language) primitives provide the atomic vocabulary and bidirectional translation mechanisms between cognitive concepts and AtomSpace hypergraph patterns. This implementation establishes the foundational cognitive primitives for Phase 1 of the Distributed Agentic Cognitive Grammar Network.

## Core Concepts

### Cognitive Primitive Types

ko6ml defines 10 fundamental primitive types that serve as the building blocks for cognitive representation:

- **KO6ML_CONCEPT**: Basic conceptual primitives for representing entities and ideas
- **KO6ML_RELATION**: Relational primitives for representing connections and relationships
- **KO6ML_AGENT**: Agentic primitives for autonomous cognitive entities
- **KO6ML_STATE**: State primitives for representing system states and conditions
- **KO6ML_PROCESS**: Process primitives for representing workflows and procedures
- **KO6ML_CONTEXT**: Contextual primitives for situational awareness
- **KO6ML_MODALITY**: Modal primitives for representing different modes of existence
- **KO6ML_TEMPORAL**: Temporal primitives for time-dependent representations
- **KO6ML_SPATIAL**: Spatial primitives for location and spatial relationships
- **KO6ML_COMPOSITE**: Composite primitives for complex multi-faceted entities

### Tensor Fragment Architecture

Each primitive is encoded with a 5-dimensional tensor shape representing:

```
[modality, depth, context, salience, autonomy_index]
```

- **modality**: Dimension representing the mode or type of cognitive processing
- **depth**: Cognitive depth level (complexity of reasoning required)
- **context**: Contextual dimension size (breadth of context needed)
- **salience**: Attention weight [0.0, 1.0] indicating importance
- **autonomy_index**: Autonomy level [0.0, 1.0] indicating independent operation capability

## API Reference

### Core Primitive Management

#### `gnc_ko6ml_init()`
```c
gboolean gnc_ko6ml_init(void);
```
Initialize the ko6ml primitive system. Must be called before using any other ko6ml functions.

**Returns**: TRUE on success, FALSE on failure

#### `gnc_ko6ml_primitive_new()`
```c
Ko6mlPrimitive* gnc_ko6ml_primitive_new(Ko6mlPrimitiveType type, 
                                        const gchar *name, 
                                        const gchar *description);
```
Create a new ko6ml primitive with specified type, name and description.

**Parameters**:
- `type`: The primitive type from Ko6mlPrimitiveType enum
- `name`: Unique identifier for the primitive
- `description`: Human-readable description

**Returns**: Newly created primitive (caller must free with `gnc_ko6ml_primitive_free()`)

#### `gnc_ko6ml_primitive_set_property()`
```c
void gnc_ko6ml_primitive_set_property(Ko6mlPrimitive *primitive, 
                                      const gchar *key, 
                                      const gchar *value);
```
Set a property on a ko6ml primitive.

**Parameters**:
- `primitive`: Target primitive
- `key`: Property key
- `value`: Property value

### AtomSpace Translation

#### `gnc_ko6ml_to_atomspace()`
```c
Ko6mlTranslationResult* gnc_ko6ml_to_atomspace(Ko6mlPrimitive *primitive);
```
Translate a ko6ml primitive to AtomSpace hypergraph representation.

**Parameters**:
- `primitive`: ko6ml primitive to translate

**Returns**: Translation result containing AtomSpace handle, tensor shape, and success status

#### `gnc_ko6ml_from_atomspace()`
```c
Ko6mlPrimitive* gnc_ko6ml_from_atomspace(GncAtomHandle atom_handle);
```
Translate an AtomSpace handle back to ko6ml primitive representation.

**Parameters**:
- `atom_handle`: AtomSpace handle to translate

**Returns**: ko6ml primitive (caller must free) or NULL if failed

#### `gnc_ko6ml_round_trip_test()`
```c
gboolean gnc_ko6ml_round_trip_test(Ko6mlPrimitive *original);
```
Test round-trip translation integrity: ko6ml → AtomSpace → ko6ml.

**Parameters**:
- `original`: Original ko6ml primitive

**Returns**: TRUE if round-trip preserves data integrity

### Tensor Architecture

#### `gnc_ko6ml_create_tensor_shape()`
```c
Ko6mlTensorShape gnc_ko6ml_create_tensor_shape(const gchar *agent_id,
                                               gpointer state_data,
                                               gsize modality,
                                               gsize depth, 
                                               gsize context);
```
Create tensor shape from agent/state encoding with specified dimensions.

#### `gnc_ko6ml_encode_hypergraph_node()`
```c
GncAtomHandle gnc_ko6ml_encode_hypergraph_node(Ko6mlPrimitive *primitive,
                                               Ko6mlTensorShape *shape);
```
Encode a hypergraph node with tensor shape properties.

#### `gnc_ko6ml_encode_hypergraph_link()`
```c
GncAtomHandle gnc_ko6ml_encode_hypergraph_link(Ko6mlPrimitive *source_primitive,
                                               Ko6mlPrimitive *target_primitive,
                                               const gchar *relation_type,
                                               Ko6mlTensorShape *shape);
```
Encode a hypergraph link between two primitives with specified relation type.

### Scheme Adapter System

#### `gnc_ko6ml_scheme_adapters_init()`
```c
gboolean gnc_ko6ml_scheme_adapters_init(void);
```
Initialize the modular Scheme adapter system for agentic grammar translation.

#### `gnc_ko6ml_primitive_to_scheme()`
```c
Ko6mlSchemeTranslationResult* gnc_ko6ml_primitive_to_scheme(Ko6mlPrimitive *primitive);
```
Translate ko6ml primitive to Scheme representation using registered adapters.

#### `gnc_ko6ml_generate_agentic_grammar()`
```c
gchar* gnc_ko6ml_generate_agentic_grammar(Ko6mlPrimitive *primitive);
```
Generate agentic grammar pattern for primitive in Scheme format.

#### `gnc_ko6ml_create_agentic_bindlink()`
```c
gchar* gnc_ko6ml_create_agentic_bindlink(Ko6mlPrimitive *agent_primitive,
                                         Ko6mlPrimitive *context_primitive);
```
Create AtomSpace BindLink pattern for agentic behavior between agent and context.

### Performance and Optimization

#### `gnc_ko6ml_benchmark_tensor_operations()`
```c
gdouble gnc_ko6ml_benchmark_tensor_operations(Ko6mlPrimitive *primitive, gint n_iterations);
```
Benchmark tensor operation performance with specified number of iterations.

**Returns**: Average time per operation in microseconds

#### `gnc_ko6ml_optimize_tensor_shape()`
```c
Ko6mlTensorShape gnc_ko6ml_optimize_tensor_shape(Ko6mlTensorShape *shape);
```
Optimize tensor shape for better cache efficiency using power-of-2 dimensions.

### Prime Factorization Mapping

#### `gnc_ko6ml_tensor_to_prime_factors()`
```c
guint* gnc_ko6ml_tensor_to_prime_factors(Ko6mlTensorShape *shape, gsize *n_factors);
```
Map tensor dimensions to prime factorization for efficient encoding.

#### `gnc_ko6ml_prime_factors_to_tensor()`
```c
Ko6mlTensorShape gnc_ko6ml_prime_factors_to_tensor(guint *prime_factors, gsize n_factors);
```
Reconstruct tensor shape from prime factorization.

## Usage Examples

### Basic Primitive Creation and Translation

```c
// Initialize systems
gnc_ko6ml_init();
gnc_ko6ml_scheme_adapters_init();

// Create a cognitive agent primitive
Ko6mlPrimitive *agent = gnc_ko6ml_primitive_new(
    KO6ML_AGENT, "TradingBot", "Autonomous trading agent");
gnc_ko6ml_primitive_set_property(agent, "strategy", "momentum");
agent->salience = 0.9;
agent->autonomy_index = 0.8;

// Translate to AtomSpace
Ko6mlTranslationResult *result = gnc_ko6ml_to_atomspace(agent);
if (result->success) {
    printf("AtomSpace handle: %u\n", result->atom_handle);
    printf("Tensor shape: [%zu,%zu,%zu,%.3f,%.3f]\n",
           result->tensor_shape.modality, result->tensor_shape.depth,
           result->tensor_shape.context, result->tensor_shape.salience,
           result->tensor_shape.autonomy_index);
}

// Test round-trip translation
gboolean integrity = gnc_ko6ml_round_trip_test(agent);
printf("Round-trip integrity: %s\n", integrity ? "PASSED" : "FAILED");

// Generate Scheme representation
Ko6mlSchemeTranslationResult *scheme = gnc_ko6ml_primitive_to_scheme(agent);
if (scheme->success) {
    printf("Scheme code:\n%s\n", scheme->scheme_code);
}

// Cleanup
gnc_ko6ml_scheme_translation_result_free(scheme);
gnc_ko6ml_translation_result_free(result);
gnc_ko6ml_primitive_free(agent);
```

### Tensor-Based Hypergraph Encoding

```c
// Create agent and state primitives
Ko6mlPrimitive *agent = gnc_ko6ml_primitive_new(
    KO6ML_AGENT, "RiskAgent", "Risk analysis agent");
Ko6mlPrimitive *state = gnc_ko6ml_primitive_new(
    KO6ML_STATE, "PortfolioState", "Current portfolio state");

// Create tensor shapes
Ko6mlTensorShape agent_shape = gnc_ko6ml_create_tensor_shape(
    "RiskAgent", agent, 5, 3, 7);
agent_shape.salience = 0.85;
agent_shape.autonomy_index = 0.75;

// Encode hypergraph nodes
GncAtomHandle agent_node = gnc_ko6ml_encode_hypergraph_node(agent, &agent_shape);
GncAtomHandle state_node = gnc_ko6ml_encode_hypergraph_node(state, &agent_shape);

// Create relationship link
GncAtomHandle analyzes_link = gnc_ko6ml_encode_hypergraph_link(
    agent, state, "analyzes", &agent_shape);

printf("Created hypergraph: Agent(%u) -analyzes-> State(%u) via Link(%u)\n",
       agent_node, state_node, analyzes_link);
```

### Agentic Grammar Generation

```c
Ko6mlPrimitive *agent = gnc_ko6ml_primitive_new(
    KO6ML_AGENT, "FinancialAdvisor", "AI financial advisor");
Ko6mlPrimitive *context = gnc_ko6ml_primitive_new(
    KO6ML_CONTEXT, "InvestmentContext", "Investment planning context");

// Generate agentic grammar
gchar *grammar = gnc_ko6ml_generate_agentic_grammar(agent);
printf("Agentic Grammar:\n%s\n", grammar);

// Generate BindLink pattern
gchar *bindlink = gnc_ko6ml_create_agentic_bindlink(agent, context);
printf("BindLink Pattern:\n%s\n", bindlink);

g_free(grammar);
g_free(bindlink);
```

## Performance Considerations

### Optimization Strategies

1. **Tensor Shape Optimization**: Use `gnc_ko6ml_optimize_tensor_shape()` to align dimensions with power-of-2 boundaries for better cache efficiency.

2. **Prime Factorization Caching**: The prime factorization mapping provides efficient encoding/decoding for tensor dimensions.

3. **Adapter Performance Tuning**: Monitor adapter performance with `gnc_ko6ml_benchmark_adapter()` and update scores accordingly.

4. **Memory Management**: Always free allocated structures using the appropriate free functions.

### Benchmarking

```c
Ko6mlPrimitive *test = gnc_ko6ml_primitive_new(KO6ML_PROCESS, "TestProcess", "Test");

// Benchmark tensor operations
gdouble tensor_time = gnc_ko6ml_benchmark_tensor_operations(test, 1000);
printf("Tensor ops: %.2f μs per operation\n", tensor_time);

// Benchmark adapter performance
Ko6mlSchemeAdapter *adapter = gnc_ko6ml_get_best_adapter(KO6ML_PROCESS);
gdouble adapter_time = gnc_ko6ml_benchmark_adapter(adapter, test, 500);
printf("Adapter: %.2f μs per translation\n", adapter_time);
```

## Integration with GnuCash

The ko6ml primitives integrate seamlessly with existing GnuCash infrastructure:

```c
// Create GnuCash account
QofBook *book = qof_book_new();
Account *account = xaccMallocAccount(book);
xaccAccountSetName(account, "SmartAccount");

// Create corresponding ko6ml primitive
Ko6mlPrimitive *account_primitive = gnc_ko6ml_primitive_new(
    KO6ML_CONCEPT, "SmartAccount", "Cognitive-enhanced account");

// Create tensor encoding
Ko6mlTensorShape shape = gnc_ko6ml_create_tensor_shape(
    "SmartAccount", account, 4, 3, 6);
GncAtomHandle node = gnc_ko6ml_encode_hypergraph_node(account_primitive, &shape);
```

## Error Handling

All ko6ml functions include comprehensive error handling:

- Functions return appropriate error codes (FALSE, NULL, etc.) on failure
- Translation results include error messages for debugging
- Validation functions check parameter correctness
- Round-trip tests verify data integrity

Always check return values and handle errors appropriately in production code.

## Thread Safety

The ko6ml primitive system is designed for single-threaded use. For multi-threaded applications, implement appropriate locking mechanisms around ko6ml function calls.

## Memory Management

- All created primitives must be freed with `gnc_ko6ml_primitive_free()`
- Translation results must be freed with appropriate free functions
- Generated strings must be freed with `g_free()`
- The system automatically manages internal registries and mappings