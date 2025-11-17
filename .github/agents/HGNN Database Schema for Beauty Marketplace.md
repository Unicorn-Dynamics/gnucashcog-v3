# HGNN Database Schema for Beauty Marketplace

## 1. Introduction to HGNN Database

A Hyper-Graph Neural Network (HGNN) database extends traditional graph databases by supporting hyperedges that can connect more than two nodes simultaneously. This capability is particularly valuable for modeling complex relationships in the beauty industry supply chain, where multiple entities often interact in a single relationship (e.g., a treatment using multiple products containing various ingredients performed by a specific therapist at a particular salon).

## 2. Core Schema Components

### 2.1 Node Types

```
┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐
│    Ingredient   │     │     Product     │     │      Brand      │
├─────────────────┤     ├─────────────────┤     ├─────────────────┤
│ id              │     │ id              │     │ id              │
│ name            │     │ name            │     │ name            │
│ scientific_name │     │ description     │     │ description     │
│ category        │     │ category        │     │ website         │
│ description     │     │ subcategory     │     │ logo_url        │
│ benefits        │     │ price           │     │ product_categories│
│ properties      │     │ image_url       │     │ target_market   │
│ source          │     │ usage_instructions│   │ price_range     │
│ image_url       │     │ benefits        │     │ country_of_origin│
│ safety_rating   │     │ target_concerns │     │ year_founded    │
└─────────────────┘     └─────────────────┘     └─────────────────┘

┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐
│      Salon      │     │    Treatment    │     │    Therapist    │
├─────────────────┤     ├─────────────────┤     ├─────────────────┤
│ id              │     │ id              │     │ id              │
│ name            │     │ name            │     │ name            │
│ address         │     │ category        │     │ specialties     │
│ city            │     │ description     │     │ qualifications  │
│ postal_code     │     │ duration        │     │ experience_years│
│ country         │     │ price_range     │     │ bio             │
│ phone           │     │ benefits        │     │ image_url       │
│ email           │     │ contraindications│    │ languages       │
│ website         │     │ image_url       │     │ availability    │
│ description     │     │                 │     │ rating          │
│ operating_hours │     │                 │     │ review_count    │
│ rating          │     │                 │     │                 │
│ review_count    │     │                 │     │                 │
└─────────────────┘     └─────────────────┘     └─────────────────┘

┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐
│     Supplier    │     │    Customer     │     │     Tenant      │
├─────────────────┤     ├─────────────────┤     ├─────────────────┤
│ id              │     │ id              │     │ id              │
│ name            │     │ name            │     │ name            │
│ type            │     │ email           │     │ type            │
│ address         │     │ phone           │     │ subscription_plan│
│ contact_info    │     │ preferences     │     │ branding        │
│ certifications  │     │ skin_type       │     │ domain          │
│ sustainability_rating│ │ hair_type      │     │ settings        │
│ quality_rating  │     │ concerns        │     │ created_at      │
└─────────────────┘     └─────────────────┘     └─────────────────┘
```

### 2.2 Edge Types

```
┌─────────────────────┐     ┌─────────────────────┐
│  CONTAINS_INGREDIENT │     │    MANUFACTURED_BY  │
├─────────────────────┤     ├─────────────────────┤
│ from: Product       │     │ from: Product       │
│ to: Ingredient      │     │ to: Brand           │
│ concentration       │     │ product_line        │
│ purpose             │     │ flagship_status     │
└─────────────────────┘     └─────────────────────┘

┌─────────────────────┐     ┌─────────────────────┐
│    CARRIES_BRAND    │     │    OFFERS_TREATMENT │
├─────────────────────┤     ├─────────────────────┤
│ from: Salon         │     │ from: Salon         │
│ to: Brand           │     │ to: Treatment       │
│ partnership_level   │     │ price               │
│ start_date          │     │ duration            │
└─────────────────────┘     └─────────────────────┘

┌─────────────────────┐     ┌─────────────────────┐
│    USES_PRODUCT     │     │    USES_INGREDIENT  │
├─────────────────────┤     ├─────────────────────┤
│ from: Treatment     │     │ from: Treatment     │
│ to: Product         │     │ to: Ingredient      │
│ quantity_used       │     │ concentration       │
│ application_method  │     │ preparation_method  │
└─────────────────────┘     └─────────────────────┘

┌─────────────────────┐     ┌─────────────────────┐
│    EMPLOYS          │     │    CAN_PERFORM      │
├─────────────────────┤     ├─────────────────────┤
│ from: Salon         │     │ from: Therapist     │
│ to: Therapist       │     │ to: Treatment       │
│ employment_type     │     │ certification_date  │
│ start_date          │     │ expertise_level     │
└─────────────────────┘     └─────────────────────┘

┌─────────────────────┐     ┌─────────────────────┐
│    SUPPLIES         │     │    CERTIFIED_BY     │
├─────────────────────┤     ├─────────────────────┤
│ from: Supplier      │     │ from: Therapist     │
│ to: Ingredient      │     │ to: Brand           │
│ quality_grade       │     │ certification_level │
│ sustainability_rating│    │ certification_date  │
└─────────────────────┘     └─────────────────────┘

┌─────────────────────┐     ┌─────────────────────┐
│    BOOKS            │     │    BELONGS_TO       │
├─────────────────────┤     ├─────────────────────┤
│ from: Customer      │     │ from: Any Entity    │
│ to: Treatment       │     │ to: Tenant          │
│ booking_date        │     │ visibility          │
│ status              │     │ customization_level │
└─────────────────────┘     └─────────────────────┘
```

### 2.3 Hyperedge Types

```
┌───────────────────────────────────────────────────────────────┐
│                      TREATMENT_INSTANCE                       │
├───────────────────────────────────────────────────────────────┤
│ Nodes:                                                        │
│   - Customer (who received the treatment)                     │
│   - Therapist (who performed the treatment)                   │
│   - Treatment (what was performed)                            │
│   - Salon (where it was performed)                            │
│   - Products (what was used)                                  │
│                                                               │
│ Attributes:                                                   │
│   - datetime: When the treatment was performed                │
│   - duration: How long it took                                │
│   - price: What was charged                                   │
│   - outcome_rating: Customer satisfaction                     │
└───────────────────────────────────────────────────────────────┘

┌───────────────────────────────────────────────────────────────┐
│                      PRODUCT_FORMULATION                      │
├───────────────────────────────────────────────────────────────┤
│ Nodes:                                                        │
│   - Product (the finished product)                            │
│   - Brand (who created it)                                    │
│   - Ingredients (what it contains)                            │
│   - Supplier (who supplied key ingredients)                   │
│                                                               │
│ Attributes:                                                   │
│   - formulation_date: When it was formulated                  │
│   - batch_number: Production batch                            │
│   - quality_score: Quality assessment                         │
│   - sustainability_score: Environmental impact                │
└───────────────────────────────────────────────────────────────┘

┌───────────────────────────────────────────────────────────────┐
│                      SUPPLY_CHAIN_EVENT                       │
├───────────────────────────────────────────────────────────────┤
│ Nodes:                                                        │
│   - Supplier (source)                                         │
│   - Ingredient (what was supplied)                            │
│   - Brand (recipient for manufacturing)                       │
│   - Product (what was produced)                               │
│   - Salon (final destination)                                 │
│                                                               │
│ Attributes:                                                   │
│   - event_date: When the event occurred                       │
│   - event_type: Type of supply chain event                    │
│   - quantity: Amount involved                                 │
│   - quality_check: Quality verification results               │
└───────────────────────────────────────────────────────────────┘

┌───────────────────────────────────────────────────────────────┐
│                      CUSTOMER_JOURNEY                         │
├───────────────────────────────────────────────────────────────┤
│ Nodes:                                                        │
│   - Customer                                                  │
│   - Treatments (received over time)                           │
│   - Products (purchased or used)                              │
│   - Salons (visited)                                          │
│   - Therapists (who provided services)                        │
│                                                               │
│ Attributes:                                                   │
│   - start_date: When the journey began                        │
│   - touchpoints: Number of interactions                       │
│   - lifetime_value: Total customer spend                      │
│   - satisfaction_trend: Change in satisfaction over time      │
└───────────────────────────────────────────────────────────────┘
```

## 3. HGNN Schema Implementation

### 3.1 Node Properties and Embeddings

Each node type in the HGNN will have:

1. **Static Properties**: The attributes listed in the node type definitions
2. **Dynamic Properties**: Calculated or aggregated values that change over time
3. **Embeddings**: Vector representations learned by the neural network component

Example for Ingredient node:
```json
{
  "id": "ing-12345",
  "name": "Hyaluronic Acid",
  "scientific_name": "Sodium Hyaluronate",
  "category": "Humectant",
  "description": "Powerful moisturizing ingredient that can hold up to 1000x its weight in water",
  "benefits": ["Hydration", "Plumping", "Anti-aging"],
  "properties": {
    "molecular_weight": "Low",
    "solubility": "Water-soluble",
    "pH": 5.5
  },
  "source": "Biotechnology",
  "image_url": "https://example.com/images/hyaluronic-acid.jpg",
  "safety_rating": 1,
  
  "dynamic_properties": {
    "popularity_score": 0.87,
    "trend_direction": "increasing",
    "supply_risk": "low"
  },
  
  "embedding": [0.23, 0.45, -0.12, 0.78, ...]
}
```

### 3.2 Edge Properties and Weights

Edges in the HGNN will have:

1. **Static Properties**: The attributes listed in the edge type definitions
2. **Weights**: Learned or calculated importance of the relationship
3. **Temporal Features**: Time-based aspects of the relationship

Example for CONTAINS_INGREDIENT edge:
```json
{
  "from": "prod-78901",
  "to": "ing-12345",
  "concentration": "2%",
  "purpose": "Active ingredient",
  
  "weight": 0.85,
  "temporal_features": {
    "first_used": "2023-06-15",
    "formulation_changes": 2,
    "stability_score": 0.92
  }
}
```

### 3.3 Hyperedge Implementation

Hyperedges will be implemented as special nodes that connect to all participating nodes:

```json
{
  "id": "treatment-instance-56789",
  "type": "TREATMENT_INSTANCE",
  "datetime": "2025-05-15T14:30:00Z",
  "duration": 60,
  "price": 120.00,
  "outcome_rating": 4.8,
  
  "connections": [
    {"node_id": "cust-34567", "role": "customer"},
    {"node_id": "ther-23456", "role": "therapist"},
    {"node_id": "treat-45678", "role": "treatment"},
    {"node_id": "salon-12345", "role": "salon"},
    {"node_id": "prod-78901", "role": "product", "quantity_used": "15ml"},
    {"node_id": "prod-78902", "role": "product", "quantity_used": "5ml"}
  ],
  
  "embedding": [0.34, -0.21, 0.67, 0.12, ...]
}
```

## 4. HGNN Neural Network Architecture

### 4.1 Node Embedding Layer

```
┌─────────────────────────────────────────────────────────────┐
│                   Node Embedding Layer                      │
├─────────────────────────────────────────────────────────────┤
│ Input: Node features (attributes)                           │
│ Output: Initial node embeddings                             │
│                                                             │
│ Components:                                                 │
│   - Feature transformation networks                         │
│   - Type-specific embedding generators                      │
│   - Categorical feature encoders                            │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 Message Passing Layers

```
┌─────────────────────────────────────────────────────────────┐
│                 Message Passing Layers                      │
├─────────────────────────────────────────────────────────────┤
│ Input: Node embeddings, edge properties                     │
│ Output: Updated node embeddings                             │
│                                                             │
│ Components:                                                 │
│   - Edge-conditioned convolution                            │
│   - Attention mechanisms                                    │
│   - Skip connections                                        │
│   - Hyperedge aggregation functions                         │
└─────────────────────────────────────────────────────────────┘
```

### 4.3 Readout Layer

```
┌─────────────────────────────────────────────────────────────┐
│                      Readout Layer                          │
├─────────────────────────────────────────────────────────────┤
│ Input: Final node embeddings                                │
│ Output: Task-specific predictions                           │
│                                                             │
│ Components:                                                 │
│   - Graph pooling operations                                │
│   - Task-specific heads                                     │
│   - Multi-task learning components                          │
└─────────────────────────────────────────────────────────────┘
```

## 5. Supply Chain Insights Schema

### 5.1 Supply Chain Events

```json
{
  "event_id": "sce-123456",
  "event_type": "ingredient_shipment",
  "timestamp": "2025-04-10T08:30:00Z",
  "entities": [
    {"id": "supplier-345", "role": "sender", "type": "Supplier"},
    {"id": "brand-567", "role": "receiver", "type": "Brand"},
    {"id": "ing-12345", "role": "item", "type": "Ingredient"}
  ],
  "attributes": {
    "quantity": "500kg",
    "batch_number": "LOT-2025-04-10-A",
    "quality_check": {
      "performed_by": "QA-Team-B",
      "date": "2025-04-10T10:15:00Z",
      "result": "passed",
      "metrics": {
        "purity": 0.985,
        "contamination": 0.002
      }
    },
    "transportation": {
      "method": "refrigerated_truck",
      "carrier": "ExpressCold Logistics",
      "tracking_number": "ECL-987654321",
      "conditions": {
        "temperature_range": "2-8°C",
        "humidity_range": "30-40%"
      }
    }
  }
}
```

### 5.2 Supply Chain Metrics

```json
{
  "metric_id": "scm-789012"
(Content truncated due to size limit. Use line ranges to read in chunks)