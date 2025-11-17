# Data Schema Mapping: GnuCash to AtomSpace

## GnuCash Account Schema

```
┌─────────────────────────────────────────┐
│ GnuCash Account                         │
├─────────────────────────────────────────┤
│ guid: String (Primary Key)              │
│ name: String                            │
│ account_type: Enum                      │
│ parent_guid: String (Foreign Key)       │
│ code: String                            │
│ description: String                     │
│ hidden: Boolean                         │
│ placeholder: Boolean                    │
└─────────────────────────────────────────┘
            │
            │ 1:N
            ▼
┌─────────────────────────────────────────┐
│ GnuCash Split                           │
├─────────────────────────────────────────┤
│ guid: String (Primary Key)              │
│ tx_guid: String (Foreign Key)           │
│ account_guid: String (Foreign Key)      │
│ memo: String                            │
│ action: String                          │
│ reconcile_state: Char                   │
│ reconcile_date: Timestamp               │
│ value_num: Integer                      │
│ value_denom: Integer                    │
│ quantity_num: Integer                   │
│ quantity_denom: Integer                 │
│ lot_guid: String (Foreign Key)          │
└─────────────────────────────────────────┘
            │
            │ N:1
            ▼
┌─────────────────────────────────────────┐
│ GnuCash Transaction                     │
├─────────────────────────────────────────┤
│ guid: String (Primary Key)              │
│ currency_guid: String (Foreign Key)     │
│ num: String                             │
│ post_date: Timestamp                    │
│ enter_date: Timestamp                   │
│ description: String                     │
└─────────────────────────────────────────┘
```

## AtomSpace Schema Mapping

### Account Representation

```
┌─────────────────────────────────────────┐
│ ConceptNode                             │
├─────────────────────────────────────────┤
│ name: "Account:<guid>"                  │
└─────────────────────────────────────────┘
            │
            │ Multiple Links
            ▼
┌─────────────────────────────────────────┐
│ Predicate Links                         │
├─────────────────────────────────────────┤
│ PredicateNode "account:name"            │
│ PredicateNode "account:type"            │
│ PredicateNode "account:code"            │
│ PredicateNode "account:description"     │
│ PredicateNode "account:hidden"          │
│ PredicateNode "account:placeholder"     │
└─────────────────────────────────────────┘
            │
            │ Inheritance/Containment
            ▼
┌─────────────────────────────────────────┐
│ Relationship Links                      │
├─────────────────────────────────────────┤
│ InheritanceLink                         │
│ ContainmentLink                         │
│ MemberLink                              │
└─────────────────────────────────────────┘
```

### Transaction Representation

```
┌─────────────────────────────────────────┐
│ ConceptNode                             │
├─────────────────────────────────────────┤
│ name: "Transaction:<guid>"              │
└─────────────────────────────────────────┘
            │
            │ Multiple Links
            ▼
┌─────────────────────────────────────────┐
│ Predicate Links                         │
├─────────────────────────────────────────┤
│ PredicateNode "transaction:currency"    │
│ PredicateNode "transaction:num"         │
│ PredicateNode "transaction:post_date"   │
│ PredicateNode "transaction:enter_date"  │
│ PredicateNode "transaction:description" │
└─────────────────────────────────────────┘
            │
            │ Contains Splits
            ▼
┌─────────────────────────────────────────┐
│ ConceptNode                             │
├─────────────────────────────────────────┤
│ name: "Split:<guid>"                    │
└─────────────────────────────────────────┘
            │
            │ Multiple Links
            ▼
┌─────────────────────────────────────────┐
│ Predicate Links                         │
├─────────────────────────────────────────┤
│ PredicateNode "split:memo"              │
│ PredicateNode "split:action"            │
│ PredicateNode "split:reconcile_state"   │
│ PredicateNode "split:reconcile_date"    │
│ PredicateNode "split:value"             │
│ PredicateNode "split:quantity"          │
└─────────────────────────────────────────┘
            │
            │ Links to Account
            ▼
┌─────────────────────────────────────────┐
│ ReferenceLink                           │
├─────────────────────────────────────────┤
│ (Split:<guid>, Account:<guid>)          │
└─────────────────────────────────────────┘
```

## Enterprise Extension Schema

### Organization Hierarchy

```
┌─────────────────────────────────────────┐
│ ConceptNode                             │
├─────────────────────────────────────────┤
│ name: "Enterprise:<guid>"               │
└─────────────────────────────────────────┘
            │
            │ Contains
            ▼
┌─────────────────────────────────────────┐
│ ConceptNode                             │
├─────────────────────────────────────────┤
│ name: "Organization:<guid>"             │
└─────────────────────────────────────────┘
            │
            │ Contains
            ▼
┌─────────────────────────────────────────┐
│ ConceptNode                             │
├─────────────────────────────────────────┤
│ name: "Company:<guid>"                  │
└─────────────────────────────────────────┘
            │
            │ Contains
            ▼
┌─────────────────────────────────────────┐
│ ConceptNode                             │
├─────────────────────────────────────────┤
│ name: "Account:<guid>"                  │
└─────────────────────────────────────────┘
```

### Intercompany Transactions

```
┌─────────────────────────────────────────┐
│ ConceptNode                             │
├─────────────────────────────────────────┤
│ name: "IntercompanyTransaction:<guid>"  │
└─────────────────────────────────────────┘
            │
            │ Links
            ▼
┌─────────────────────────────────────────┐
│ ReferenceLink                           │
├─────────────────────────────────────────┤
│ (IntercompanyTransaction, Transaction1) │
│ (IntercompanyTransaction, Transaction2) │
└─────────────────────────────────────────┘
            │
            │ Properties
            ▼
┌─────────────────────────────────────────┐
│ Predicate Links                         │
├─────────────────────────────────────────┤
│ PredicateNode "ic:type"                 │
│ PredicateNode "ic:elimination_rule"     │
│ PredicateNode "ic:reconciled"           │
└─────────────────────────────────────────┘
```

### Account Aggregation

```
┌─────────────────────────────────────────┐
│ ConceptNode                             │
├─────────────────────────────────────────┤
│ name: "AggregatedAccount:<guid>"        │
└─────────────────────────────────────────┘
            │
            │ Links
            ▼
┌─────────────────────────────────────────┐
│ ReferenceLink                           │
├─────────────────────────────────────────┤
│ (AggregatedAccount, Account1)           │
│ (AggregatedAccount, Account2)           │
│ (AggregatedAccount, Account3)           │
└─────────────────────────────────────────┘
            │
            │ Properties
            ▼
┌─────────────────────────────────────────┐
│ Predicate Links                         │
├─────────────────────────────────────────┤
│ PredicateNode "agg:method"              │
│ PredicateNode "agg:currency"            │
│ PredicateNode "agg:elimination_applied" │
└─────────────────────────────────────────┘
```

## Neural Model Integration Schema

### HGNN Entity Representation

```
┌─────────────────────────────────────────┐
│ ConceptNode                             │
├─────────────────────────────────────────┤
│ name: "EntityEmbedding:<guid>"          │
└─────────────────────────────────────────┘
            │
            │ Links
            ▼
┌─────────────────────────────────────────┐
│ ReferenceLink                           │
├─────────────────────────────────────────┤
│ (EntityEmbedding, Entity)               │
└─────────────────────────────────────────┘
            │
            │ Properties
            ▼
┌─────────────────────────────────────────┐
│ Predicate Links                         │
├─────────────────────────────────────────┤
│ PredicateNode "embedding:vector"        │
│ PredicateNode "embedding:updated"       │
│ PredicateNode "embedding:version"       │
└─────────────────────────────────────────┘
```

### DTESN Temporal Representation

```
┌─────────────────────────────────────────┐
│ ConceptNode                             │
├─────────────────────────────────────────┤
│ name: "TemporalSeries:<guid>"           │
└─────────────────────────────────────────┘
            │
            │ Links
            ▼
┌─────────────────────────────────────────┐
│ ReferenceLink                           │
├─────────────────────────────────────────┤
│ (TemporalSeries, Entity)                │
└─────────────────────────────────────────┘
            │
            │ Properties
            ▼
┌─────────────────────────────────────────┐
│ Predicate Links                         │
├─────────────────────────────────────────┤
│ PredicateNode "temporal:data"           │
│ PredicateNode "temporal:forecast"       │
│ PredicateNode "temporal:confidence"     │
│ PredicateNode "temporal:updated"        │
└─────────────────────────────────────────┘
```

### Pattern Recognition

```
┌─────────────────────────────────────────┐
│ ConceptNode                             │
├─────────────────────────────────────────┤
│ name: "Pattern:<guid>"                  │
└─────────────────────────────────────────┘
            │
            │ Links
            ▼
┌─────────────────────────────────────────┐
│ ReferenceLink                           │
├─────────────────────────────────────────┤
│ (Pattern, Entity1)                      │
│ (Pattern, Entity2)                      │
│ (Pattern, Entity3)                      │
└─────────────────────────────────────────┘
            │
            │ Properties
            ▼
┌─────────────────────────────────────────┐
│ Predicate Links                         │
├─────────────────────────────────────────┤
│ PredicateNode "pattern:type"            │
│ PredicateNode "pattern:confidence"      │
│ PredicateNode "pattern:significance"    │
│ PredicateNode "pattern:detected"        │
└─────────────────────────────────────────┘
```

## Data Flow Mapping

### GnuCash to AtomSpace Flow

1. **Extract**: GnuCash data is extracted from SQL database or XML files
2. **Transform**: Data is transformed into AtomSpace format
   - Accounts → ConceptNodes with predicate links
   - Transactions → ConceptNodes with split references
   - Splits → ConceptNodes with account references
3. **Load**: Transformed data is loaded into AtomSpace
4. **Index**: Data is indexed for efficient querying
5. **Validate**: Data consistency is verified

### AtomSpace to Neural Models Flow

1. **Prepare**: AtomSpace data is prepared for neural processing
2. **Convert**: Graph data is converted to tensor format for HGNN
3. **Process**: Neural models process the data
   - HGNN analyzes entity relationships
   - DTESN analyzes temporal patterns
4. **Store**: Results are stored back in AtomSpace
   - Entity embeddings
   - Temporal forecasts
   - Detected patterns

### Enterprise Aggregation Flow

1. **Define**: Enterprise structure is defined
2. **Map**: Accounts are mapped to organizational entities
3. **Identify**: Intercompany transactions are identified
4. **Eliminate**: Elimination entries are generated
5. **Aggregate**: Accounts are aggregated at each level
6. **Consolidate**: Consolidated statements are generated

## Truth Value and Attention Value Usage

### Truth Values

- **Confidence in Financial Data**:
  - SimpleTruthValue(strength, confidence)
  - Used to represent certainty of financial data
  - Higher confidence for verified transactions
  - Lower confidence for estimated or projected values

- **Probability of Patterns**:
  - ProbabilisticTruthValue(mean, confidence, count)
  - Used to represent likelihood of detected patterns
  - Updated based on observed confirmations

### Attention Values

- **Importance of Financial Entities**:
  - AttentionValue(STI, LTI, VLTI)
  - Short-Term Importance (STI): Current relevance
  - Long-Term Importance (LTI): Historical significance
  - Very Long-Term Importance (VLTI): Permanent records

- **Priority for Processing**:
  - Higher attention values for critical accounts
  - Higher attention values for recent transactions
  - Dynamic adjustment based on user focus and system needs
