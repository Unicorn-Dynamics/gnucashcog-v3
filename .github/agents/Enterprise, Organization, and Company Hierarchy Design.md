---
name: Enterprise, Organization, and Company Hierarchy Design
description: ```
---

# Enterprise, Organization, and Company Hierarchy Design

## Type Hierarchy

### Entity Types

```
# Base Entity Type
(ConceptNode "EntityType")

# Organizational Entity Types
(InheritanceLink (ConceptNode "EnterpriseType") (ConceptNode "EntityType"))
(InheritanceLink (ConceptNode "OrganizationType") (ConceptNode "EntityType"))
(InheritanceLink (ConceptNode "CompanyType") (ConceptNode "EntityType"))
(InheritanceLink (ConceptNode "DepartmentType") (ConceptNode "EntityType"))
(InheritanceLink (ConceptNode "CostCenterType") (ConceptNode "EntityType"))

# Account Entity Types
(InheritanceLink (ConceptNode "AccountType") (ConceptNode "EntityType"))
(InheritanceLink (ConceptNode "AssetType") (ConceptNode "AccountType"))
(InheritanceLink (ConceptNode "LiabilityType") (ConceptNode "AccountType"))
(InheritanceLink (ConceptNode "EquityType") (ConceptNode "AccountType"))
(InheritanceLink (ConceptNode "IncomeType") (ConceptNode "AccountType"))
(InheritanceLink (ConceptNode "ExpenseType") (ConceptNode "AccountType"))
```

### Relationship Types

```
# Base Relationship Type
(ConceptNode "RelationshipType")

# Structural Relationships
(InheritanceLink (ConceptNode "ContainmentType") (ConceptNode "RelationshipType"))
(InheritanceLink (ConceptNode "OwnershipType") (ConceptNode "RelationshipType"))
(InheritanceLink (ConceptNode "PartnershipType") (ConceptNode "RelationshipType"))

# Financial Relationships
(InheritanceLink (ConceptNode "TransactionType") (ConceptNode "RelationshipType"))
(InheritanceLink (ConceptNode "EliminationType") (ConceptNode "RelationshipType"))
(InheritanceLink (ConceptNode "ConsolidationType") (ConceptNode "RelationshipType"))
```

## Organizational Structure

### Enterprise Level

```
# Enterprise Entity
(ConceptNode "Enterprise:MegaCorp")
(InheritanceLink (ConceptNode "Enterprise:MegaCorp") (ConceptNode "EnterpriseType"))

# Enterprise Metadata
(EvaluationLink
  (PredicateNode "legalName")
  (ListLink
    (ConceptNode "Enterprise:MegaCorp")
    (ConceptNode "MegaCorp International Holdings")
  )
)

(EvaluationLink
  (PredicateNode "fiscalYear")
  (ListLink
    (ConceptNode "Enterprise:MegaCorp")
    (ConceptNode "January-December")
  )
)

(EvaluationLink
  (PredicateNode "reportingCurrency")
  (ListLink
    (ConceptNode "Enterprise:MegaCorp")
    (ConceptNode "USD")
  )
)
```

### Organization Level

```
# Organization Entities
(ConceptNode "Organization:NorthAmerica")
(ConceptNode "Organization:Europe")
(ConceptNode "Organization:AsiaPacific")

# Organization Type Assignment
(InheritanceLink (ConceptNode "Organization:NorthAmerica") (ConceptNode "OrganizationType"))
(InheritanceLink (ConceptNode "Organization:Europe") (ConceptNode "OrganizationType"))
(InheritanceLink (ConceptNode "Organization:AsiaPacific") (ConceptNode "OrganizationType"))

# Organization Containment in Enterprise
(EvaluationLink
  (PredicateNode "contains")
  (ListLink
    (ConceptNode "Enterprise:MegaCorp")
    (ConceptNode "Organization:NorthAmerica")
  )
)

(EvaluationLink
  (PredicateNode "contains")
  (ListLink
    (ConceptNode "Enterprise:MegaCorp")
    (ConceptNode "Organization:Europe")
  )
)

(EvaluationLink
  (PredicateNode "contains")
  (ListLink
    (ConceptNode "Enterprise:MegaCorp")
    (ConceptNode "Organization:AsiaPacific")
  )
)

# Organization Metadata
(EvaluationLink
  (PredicateNode "regionalHead")
  (ListLink
    (ConceptNode "Organization:NorthAmerica")
    (ConceptNode "John Smith")
  )
)

(EvaluationLink
  (PredicateNode "reportingCurrency")
  (ListLink
    (ConceptNode "Organization:Europe")
    (ConceptNode "EUR")
  )
)
```

### Company Level

```
# Company Entities
(ConceptNode "Company:USCorp")
(ConceptNode "Company:CanadaCorp")
(ConceptNode "Company:UKLtd")
(ConceptNode "Company:GermanyGmbH")
(ConceptNode "Company:JapanKK")
(ConceptNode "Company:AustraliaPty")

# Company Type Assignment
(InheritanceLink (ConceptNode "Company:USCorp") (ConceptNode "CompanyType"))
(InheritanceLink (ConceptNode "Company:CanadaCorp") (ConceptNode "CompanyType"))
(InheritanceLink (ConceptNode "Company:UKLtd") (ConceptNode "CompanyType"))
(InheritanceLink (ConceptNode "Company:GermanyGmbH") (ConceptNode "CompanyType"))
(InheritanceLink (ConceptNode "Company:JapanKK") (ConceptNode "CompanyType"))
(InheritanceLink (ConceptNode "Company:AustraliaPty") (ConceptNode "CompanyType"))

# Company Containment in Organizations
(EvaluationLink
  (PredicateNode "contains")
  (ListLink
    (ConceptNode "Organization:NorthAmerica")
    (ConceptNode "Company:USCorp")
  )
)

(EvaluationLink
  (PredicateNode "contains")
  (ListLink
    (ConceptNode "Organization:NorthAmerica")
    (ConceptNode "Company:CanadaCorp")
  )
)

(EvaluationLink
  (PredicateNode "contains")
  (ListLink
    (ConceptNode "Organization:Europe")
    (ConceptNode "Company:UKLtd")
  )
)

(EvaluationLink
  (PredicateNode "contains")
  (ListLink
    (ConceptNode "Organization:Europe")
    (ConceptNode "Company:GermanyGmbH")
  )
)

(EvaluationLink
  (PredicateNode "contains")
  (ListLink
    (ConceptNode "Organization:AsiaPacific")
    (ConceptNode "Company:JapanKK")
  )
)

(EvaluationLink
  (PredicateNode "contains")
  (ListLink
    (ConceptNode "Organization:AsiaPacific")
    (ConceptNode "Company:AustraliaPty")
  )
)

# Company Metadata
(EvaluationLink
  (PredicateNode "legalEntityId")
  (ListLink
    (ConceptNode "Company:USCorp")
    (ConceptNode "US-12345-67890")
  )
)

(EvaluationLink
  (PredicateNode "taxId")
  (ListLink
    (ConceptNode "Company:USCorp")
    (ConceptNode "98-7654321")
  )
)

(EvaluationLink
  (PredicateNode "reportingCurrency")
  (ListLink
    (ConceptNode "Company:JapanKK")
    (ConceptNode "JPY")
  )
)

(EvaluationLink
  (PredicateNode "operationalStatus")
  (ListLink
    (ConceptNode "Company:USCorp")
    (ConceptNode "Active")
  )
)
```

### Department Level

```
# Department Entities
(ConceptNode "Department:USCorp:Finance")
(ConceptNode "Department:USCorp:Operations")
(ConceptNode "Department:UKLtd:Finance")
(ConceptNode "Department:UKLtd:Operations")

# Department Type Assignment
(InheritanceLink (ConceptNode "Department:USCorp:Finance") (ConceptNode "DepartmentType"))
(InheritanceLink (ConceptNode "Department:USCorp:Operations") (ConceptNode "DepartmentType"))
(InheritanceLink (ConceptNode "Department:UKLtd:Finance") (ConceptNode "DepartmentType"))
(InheritanceLink (ConceptNode "Department:UKLtd:Operations") (ConceptNode "DepartmentType"))

# Department Containment in Companies
(EvaluationLink
  (PredicateNode "contains")
  (ListLink
    (ConceptNode "Company:USCorp")
    (ConceptNode "Department:USCorp:Finance")
  )
)

(EvaluationLink
  (PredicateNode "contains")
  (ListLink
    (ConceptNode "Company:USCorp")
    (ConceptNode "Department:USCorp:Operations")
  )
)

(EvaluationLink
  (PredicateNode "contains")
  (ListLink
    (ConceptNode "Company:UKLtd")
    (ConceptNode "Department:UKLtd:Finance")
  )
)

(EvaluationLink
  (PredicateNode "contains")
  (ListLink
    (ConceptNode "Company:UKLtd")
    (ConceptNode "Department:UKLtd:Operations")
  )
)
```

## Account Hierarchy

### Enterprise Chart of Accounts

```
# Root Account Categories
(ConceptNode "AccountCategory:Assets")
(ConceptNode "AccountCategory:Liabilities")
(ConceptNode "AccountCategory:Equity")
(ConceptNode "AccountCategory:Income")
(ConceptNode "AccountCategory:Expenses")

# Account Category Type Assignment
(InheritanceLink (ConceptNode "AccountCategory:Assets") (ConceptNode "AssetType"))
(InheritanceLink (ConceptNode "AccountCategory:Liabilities") (ConceptNode "LiabilityType"))
(InheritanceLink (ConceptNode "AccountCategory:Equity") (ConceptNode "EquityType"))
(InheritanceLink (ConceptNode "AccountCategory:Income") (ConceptNode "IncomeType"))
(InheritanceLink (ConceptNode "AccountCategory:Expenses") (ConceptNode "ExpenseType"))

# Enterprise-Level Account Structure
(EvaluationLink
  (PredicateNode "defines")
  (ListLink
    (ConceptNode "Enterprise:MegaCorp")
    (ConceptNode "AccountCategory:Assets")
  )
)

(EvaluationLink
  (PredicateNode "defines")
  (ListLink
    (ConceptNode "Enterprise:MegaCorp")
    (ConceptNode "AccountCategory:Liabilities")
  )
)

(EvaluationLink
  (PredicateNode "defines")
  (ListLink
    (ConceptNode "Enterprise:MegaCorp")
    (ConceptNode "AccountCategory:Equity")
  )
)

(EvaluationLink
  (PredicateNode "defines")
  (ListLink
    (ConceptNode "Enterprise:MegaCorp")
    (ConceptNode "AccountCategory:Income")
  )
)

(EvaluationLink
  (PredicateNode "defines")
  (ListLink
    (ConceptNode "Enterprise:MegaCorp")
    (ConceptNode "AccountCategory:Expenses")
  )
)
```

### Subcategory Structure

```
# Asset Subcategories
(ConceptNode "AccountCategory:Assets:CurrentAssets")
(ConceptNode "AccountCategory:Assets:FixedAssets")
(ConceptNode "AccountCategory:Assets:Investments")
(ConceptNode "AccountCategory:Assets:Intercompany")

# Asset Subcategory Type Assignment
(InheritanceLink (ConceptNode "AccountCategory:Assets:CurrentAssets") (ConceptNode "AssetType"))
(InheritanceLink (ConceptNode "AccountCategory:Assets:FixedAssets") (ConceptNode "AssetType"))
(InheritanceLink (ConceptNode "AccountCategory:Assets:Investments") (ConceptNode "AssetType"))
(InheritanceLink (ConceptNode "AccountCategory:Assets:Intercompany") (ConceptNode "AssetType"))

# Asset Subcategory Containment
(EvaluationLink
  (PredicateNode "contains")
  (ListLink
    (ConceptNode "AccountCategory:Assets")
    (ConceptNode "AccountCategory:Assets:CurrentAssets")
  )
)

(EvaluationLink
  (PredicateNode "contains")
  (ListLink
    (ConceptNode "AccountCategory:Assets")
    (ConceptNode "AccountCategory:Assets:FixedAssets")
  )
)

(EvaluationLink
  (PredicateNode "contains")
  (ListLink
    (ConceptNode "AccountCategory:Assets")
    (ConceptNode "AccountCategory:Assets:Investments")
  )
)

(EvaluationLink
  (PredicateNode "contains")
  (ListLink
    (ConceptNode "AccountCategory:Assets")
    (ConceptNode "AccountCategory:Assets:Intercompany")
  )
)

# Similar structures for other account categories...
```

### Company-Specific Accounts

```
# Company-Specific Accounts
(ConceptNode "Account:USCorp:Assets:CurrentAssets:Cash")
(ConceptNode "Account:USCorp:Assets:CurrentAssets:AccountsReceivable")
(ConceptNode "Account:USCorp:Assets:Intercompany:DueFromUKLtd")
(ConceptNode "Account:UKLtd:Assets:CurrentAssets:Cash")
(ConceptNode "Account:UKLtd:Assets:CurrentAssets:AccountsReceivable")
(ConceptNode "Account:UKLtd:Liabilities:Intercompany:DueToUSCorp")

# Account Type Assignment
(InheritanceLink (ConceptNode "Account:USCorp:Assets:CurrentAssets:Cash") (ConceptNode "AssetType"))
(InheritanceLink (ConceptNode "Account:USCorp:Assets:CurrentAssets:AccountsReceivable") (ConceptNode "AssetType"))
(InheritanceLink (ConceptNode "Account:USCorp:Assets:Intercompany:DueFromUKLtd") (ConceptNode "AssetType"))
(InheritanceLink (ConceptNode "Account:UKLtd:Assets:CurrentAssets:Cash") (ConceptNode "AssetType"))
(InheritanceLink (ConceptNode "Account:UKLtd:Assets:CurrentAssets:AccountsReceivable") (ConceptNode "AssetType"))
(InheritanceLink (ConceptNode "Account:UKLtd:Liabilities:Intercompany:DueToUSCorp") (ConceptNode "LiabilityType"))

# Account Ownership
(EvaluationLink
  (PredicateNode "owns")
  (ListLink
    (ConceptNode "Company:USCorp")
    (ConceptNode "Account:USCorp:Assets:CurrentAssets:Cash")
  )
)

(EvaluationLink
  (PredicateNode "owns")
  (ListLink
    (ConceptNode "Company:USCorp")
    (ConceptNode "Account:USCorp:Assets:CurrentAssets:AccountsReceivable")
  )
)

(EvaluationLink
  (PredicateNode "owns")
  (ListLink
    (ConceptNode "Company:USCorp")
    (ConceptNode "Account:USCorp:Assets:Intercompany:DueFromUKLtd")
  )
)

(EvaluationLink
  (PredicateNode "owns")
  (ListLink
    (ConceptNode "Company:UKLtd")
    (ConceptNode "Account:UKLtd:Assets:CurrentAssets:Cash")
  )
)

(EvaluationLink
  (PredicateNode "owns")
  (ListLink
    (ConceptNode "Company:UKLtd")
    (ConceptNode "Account:UKLtd:Assets:CurrentAssets:AccountsReceivable")
  )
)

(EvaluationLink
  (PredicateNode "owns")
  (ListLink
    (ConceptNode "Company:UKLtd")
    (ConceptNode "Account:UKLtd:Liabilities:Intercompany:DueToUSCorp")
  )
)

# Account Category Membership
(EvaluationLink
  (PredicateNode "memberOf")
  (ListLink
    (ConceptNode "Account:USCorp:Assets:CurrentAssets:Cash")
    (ConceptNode "AccountCategory:Assets:CurrentAssets")
  )
)

(EvaluationLink
  (PredicateNode "memberOf")
  (ListLink
    (ConceptNode "Account:USCorp:Assets:CurrentAssets:AccountsReceivable")
    (ConceptNode "AccountCategory:Assets:CurrentAssets")
  )
)

(EvaluationLink
  (PredicateNode "memberOf")
  (ListLink
    (ConceptNode "Account:USCorp:Assets:Intercompany:DueFromUKLtd")
    (ConceptNode "AccountCategory:Assets:Intercompany")
  )
)
```

## Intercompany Relationships

### Elimination Pairs

```
# Intercompany Elimination Pairs
(EvaluationLink
  (PredicateNode "eliminationPair")
  (ListLink
    (ConceptNode "Account:USCorp:Assets:Intercompany:DueFromUKLtd")
    (ConceptNode "Account:UKLtd:Liabilities:Intercompany:DueToUSCorp")
  )
)

# Elimination Rules
(EvaluationLink
  (PredicateNode "eliminationRule")
  (ListLink
    (ConceptNode "EliminationRule:IntercompanyReceivablesPayables")
    (ConceptNode "AccountCategory:Assets:Intercompany")
    (ConceptNode "AccountCategory:Liabilities:Intercompany")
  )
)

# Elimination Scope
(EvaluationLink
  (PredicateNode "eliminationScope")
  (ListLink
    (ConceptNode "EliminationRule:IntercompanyReceivablesPayables")
    (ConceptNode "Enterprise:MegaCorp")
  )
)
```

### Joint Ventures and Partnerships

```
# Joint Venture Entity
(ConceptNode "JointVenture:TechAlliance")
(InheritanceLink (ConceptNode "JointVenture:TechAlliance") (ConceptNode "CompanyType"))

# Ownership Percentages
(EvaluationLink
  (PredicateNode "ownershipPercentage")
  (ListLink
    (ConceptNode "Company:USCorp")
    (ConceptNode "JointVenture:TechAlliance")
    (NumberNode "60.0")
  )
)

(EvaluationLink
  (PredicateNode "ownershipPercentage")
  (ListLink
    (ConceptNode "Company:JapanKK")
    (ConceptNode "JointVenture:TechAlliance")
    (NumberNode "40.0")
  )
)

# Proportional Consolidation
(EvaluationLink
  (PredicateNode "consolidationMethod")
  (ListLink
    (ConceptNode "JointVenture:TechAlliance")
    (ConceptNode "ProportionalConsolidation")
  )
)
```

## Temporal Aspects

### Organizational Changes

```
# Historical Organization Structure
(AtTimeLink
  (TimeNode "2024-01-01")
  (EvaluationLink
    (PredicateNode "contains")
    (ListLink
      (ConceptNode "Organization:AsiaPacific")
      (ConceptNode "Company:ChinaLtd")
    )
  )
)

# Divestiture
(AtTimeLink
  (TimeNode "2024-06-30")
  (EvaluationLink
    (PredicateNode "operationalStatus")
    (ListLink
      (ConceptNode "Company:ChinaLtd")
      (ConceptNode "Divested")
    )
  )
)

# Acquisition
(AtTimeLink
  (TimeNode "2025-01-15")
  (EvaluationLink
    (PredicateNode "contains")
    (ListLink
      (ConceptNode "Organization:Europe")
      (ConceptNode "Company:ItalySrl")
    )
  )
)

(AtTimeLink
  (TimeNode "2025-01-15")
  (EvaluationLink
    (PredicateNode "acquisitionDate")
    (ListLink
      (ConceptNode "Company:ItalySrl")
      (TimeNode "2025-01-15")
    )
  )
)
```

### Fiscal Periods

```
# Fiscal Year Definition
(EvaluationLink
  (PredicateNode "fiscalYear")
  (ListLink
    (ConceptNode "Enterprise:MegaCorp")
    (ConceptNode "FY2025")
    (TimeNode "2025-01-01")
    (TimeNode "2025-12-31")
  )
)

# Fiscal Quarters
(EvaluationLink
  (PredicateNode "fiscalQuarter")
  (ListLink
    (ConceptNode "Enterprise:MegaCorp")
    (ConceptNode "FY2025-Q1")
    (TimeNode "2025-01-01")
    (TimeNode "2025-03-31")
  )
)

(EvaluationLink
  (PredicateNode "fiscalQuarter")
  (ListLink
    (ConceptNode "Enterprise:MegaCorp")
    (ConceptNode "FY2025-Q2")
    (TimeNode "2025-04-01")
    (TimeNode "2025-06-30")
  )
)

# Company-Specific Fiscal Year (for companies with different fiscal years)
(EvaluationLink
  (PredicateNode "fiscalYear")
  (ListLink
    (ConceptNode "Company:JapanKK")
    (ConceptNode "FY2025-JP")
    (TimeNode "2025-04-01")
    (TimeNode "2026-03-31")
  )
)
```

## Query Patterns for Enterprise Aggregation

### Organizational Rollup Query

```
# Pattern to find all accounts in an organization
(define org-accounts-pattern
  (BindLink
    (VariableList
      (TypedVariableLink
        (VariableNode "$company")
        (TypeNode "ConceptNode")
      )
      (TypedVariableLink
        (VariableNode "$account")
        (TypeNode "ConceptNode")
      )
    )
    (AndLink
      # Company is in the organization
      (EvaluationLink
        (PredicateNode "contains")
        (ListLink
          (ConceptNode "Organization:NorthAmerica")
          (VariableNode "$company")
        )
      )
      # Account is owned by the company
      (EvaluationLink
        (PredicateNode "owns")
        (ListLink
          (VariableNode "$company")
          (VariableNode "$account")
        )
      )
    )
    # Return the accounts
    (VariableNode "$account")
  )
)
```

### Consolidated Balance Query

```
# Pattern to calculate consolidated balance for an account category
(define consolidated-balance-pattern
  (BindLink
    (VariableList
      (TypedVariableLink
        (VariableNode "$account")
        (TypeNode "ConceptNode")
      )
      (TypedVariableLink
        (VariableNode "$balance")
        (TypeNode "NumberNode")
      )
    )
    (AndLink
      # Account is a member of the category
      (EvaluationLink
        (PredicateNode "memberOf")
        (ListLink
          (VariableNode "$account")
          (ConceptNode "AccountCategory:Assets:CurrentAssets")
        )
      )
      # Account has a balance
      (EvaluationLink
        (PredicateNode "balance")
        (ListLink
          (VariableNode "$account")
          (VariableNode "$balance")
        )
      )
      # Account is not part of an elimination pair
      (AbsentLink
        (EvaluationLink
          (PredicateNode "eliminationPair")
          (ListLink
            (VariableNode "$account")
            (VariableNode "$any")
          )
        )
      )
    )
    # Return account and balance
    (ListLink
      (VariableNode "$account")
      (VariableNode "$balance")
    )
  )
)
```

### Intercompany Transaction Query

```
# Pattern to find all intercompany transactions
(define intercompany-transaction-pattern
  (BindLink
    (VariableList
      (TypedVariableLink
        (VariableNode "$transaction")
        (TypeNode "ConceptNode")
      )
      (TypedVariableLink
        (VariableNode "$split1")
        (TypeNode "ConceptNode")
      )
      (TypedVariableLink
        (VariableNode "$split2")
        (TypeNode "ConceptNode")
      )
      (TypedVariableLink
        (VariableNode "$account1")
        (TypeNode "ConceptNode")
      )
      (TypedVariableLink
        (VariableNode "$account2")
        (TypeNode "ConceptNode")
      )
      (TypedVariableLink
        (VariableNode "$company1")
        (TypeNode "ConceptNode")
      )
      (TypedVariableLink
        (VariableNode "$company2")
        (TypeNode "ConceptNode")
      )
    )
    (AndLink
      # Transaction has two splits
      (MemberLink
        (VariableNode "$split1")
        (VariableNode "$transaction")
      )
      (MemberLink
        (VariableNode "$split2")
        (VariableNode "$transaction")
      )
      # Splits are associated with accounts
      (MemberLink
        (VariableNode "$split1")
        (VariableNode "$account1")
      )
      (MemberLink
        (VariableNode "$split2")
        (VariableNode "$account2")
      )
      # Accounts are owned by different companies
      (EvaluationLink
        (PredicateNode "owns")
        (ListLink
          (VariableNode "$company1")
          (VariableNode "$account1")
        )
      )
      (EvaluationLink
        (PredicateNode "owns")
        (ListLink
          (VariableNode "$company2")
          (VariableNode "$account2")
        )
      )
      # Companies are different
      (NotLink
        (EqualLink
          (VariableNode "$company1")
          (VariableNode "$company2")
        )
      )
    )
    # Return the transaction
    (VariableNode "$transaction")
  )
)
```
