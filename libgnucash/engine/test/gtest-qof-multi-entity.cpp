/********************************************************************\
 * gtest-qof-multi-entity.cpp -- Unit tests for multi-entity       *
 *                               aggregation functionality          *
 *                                                                  *
 * Copyright 2024 GnuCash Contributors                              *
 *                                                                  *
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free Software Foundation; either version 2 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU General Public License for more details.                     *
 *                                                                  *
 * You should have received a copy of the GNU General Public License*
 * along with this program; if not, contact:                        *
 *                                                                  *
 * Free Software Foundation           Voice:  +1-617-542-5942       *
 * 51 Franklin Street, Fifth Floor    Fax:    +1-617-542-2652       *
 * Boston, MA  02110-1301,  USA       gnu@gnu.org                   *
 *                                                                  *
 *********************************************************************/

#include <config.h>
#include <glib.h>
#include "../test-core/test-engine-stuff.h"
#include "../qofid.h"
#include "../qofinstance.h"
#include "../guid.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcpp"
#include <gtest/gtest.h>
#pragma GCC diagnostic pop

class QofMultiEntityTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Create test collections
        account_coll = qof_collection_new("Account");
        transaction_coll = qof_collection_new("Transaction");
        
        // Create test entities using proper GObject creation
        for (int i = 0; i < 3; i++)
        {
            QofInstance* account = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
            QofInstance* transaction = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
            
            // Set entity types
            account->e_type = "Account";
            transaction->e_type = "Transaction";
            
            qof_collection_insert_entity(account_coll, account);
            qof_collection_insert_entity(transaction_coll, transaction);
            
            accounts.push_back(account);
            transactions.push_back(transaction);
        }
    }
    
    void TearDown() override
    {
        // Clean up entities
        for (auto* account : accounts)
        {
            g_object_unref(account);
        }
        for (auto* transaction : transactions)
        {
            g_object_unref(transaction);
        }
        
        // Clean up collections
        qof_collection_destroy(account_coll);
        qof_collection_destroy(transaction_coll);
    }
    
    QofCollection* account_coll;
    QofCollection* transaction_coll;
    std::vector<QofInstance*> accounts;
    std::vector<QofInstance*> transactions;
};

TEST_F(QofMultiEntityTest, CreateAndDestroy)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    ASSERT_NE(nullptr, multi_coll);
    
    EXPECT_EQ(0U, qof_multi_entity_collection_count(multi_coll));
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(QofMultiEntityTest, AddSingleEntity)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    // Add a single account
    EXPECT_TRUE(qof_multi_entity_collection_add_entity(multi_coll, accounts[0]));
    EXPECT_EQ(1U, qof_multi_entity_collection_count(multi_coll));
    EXPECT_TRUE(qof_multi_entity_collection_contains(multi_coll, accounts[0]));
    
    // Try to add the same entity again (should fail)
    EXPECT_FALSE(qof_multi_entity_collection_add_entity(multi_coll, accounts[0]));
    EXPECT_EQ(1U, qof_multi_entity_collection_count(multi_coll));
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(QofMultiEntityTest, AddEntireCollection)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    // Add all accounts
    qof_multi_entity_collection_add_collection(multi_coll, account_coll);
    EXPECT_EQ(3U, qof_multi_entity_collection_count(multi_coll));
    
    // Verify all accounts are present
    for (const auto* account : accounts)
    {
        EXPECT_TRUE(qof_multi_entity_collection_contains(multi_coll, account));
    }
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(QofMultiEntityTest, AddMultipleCollections)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    // Add both collections
    qof_multi_entity_collection_add_collection(multi_coll, account_coll);
    qof_multi_entity_collection_add_collection(multi_coll, transaction_coll);
    
    EXPECT_EQ(6U, qof_multi_entity_collection_count(multi_coll));
    
    // Verify all entities are present
    for (const auto* account : accounts)
    {
        EXPECT_TRUE(qof_multi_entity_collection_contains(multi_coll, account));
    }
    for (const auto* transaction : transactions)
    {
        EXPECT_TRUE(qof_multi_entity_collection_contains(multi_coll, transaction));
    }
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(QofMultiEntityTest, RemoveEntity)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    // Add all accounts
    qof_multi_entity_collection_add_collection(multi_coll, account_coll);
    EXPECT_EQ(3U, qof_multi_entity_collection_count(multi_coll));
    
    // Remove one account
    EXPECT_TRUE(qof_multi_entity_collection_remove_entity(multi_coll, accounts[0]));
    EXPECT_EQ(2U, qof_multi_entity_collection_count(multi_coll));
    EXPECT_FALSE(qof_multi_entity_collection_contains(multi_coll, accounts[0]));
    
    // Try to remove the same entity again (should fail)
    EXPECT_FALSE(qof_multi_entity_collection_remove_entity(multi_coll, accounts[0]));
    EXPECT_EQ(2U, qof_multi_entity_collection_count(multi_coll));
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(QofMultiEntityTest, GetTypes)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    // Add both collections
    qof_multi_entity_collection_add_collection(multi_coll, account_coll);
    qof_multi_entity_collection_add_collection(multi_coll, transaction_coll);
    
    GList* types = qof_multi_entity_collection_get_types(multi_coll);
    EXPECT_EQ(2U, g_list_length(types));
    
    // Check that both types are present
    gboolean found_account = FALSE;
    gboolean found_transaction = FALSE;
    
    for (GList* node = types; node; node = node->next)
    {
        const char* type = (const char*)node->data;
        if (g_strcmp0(type, "Account") == 0)
            found_account = TRUE;
        else if (g_strcmp0(type, "Transaction") == 0)
            found_transaction = TRUE;
    }
    
    EXPECT_TRUE(found_account);
    EXPECT_TRUE(found_transaction);
    
    g_list_free(types);
    qof_multi_entity_collection_destroy(multi_coll);
}

// Helper function for filtering tests
static gboolean
filter_accounts_only(QofInstance* entity, gpointer user_data)
{
    return g_strcmp0(entity->e_type, "Account") == 0;
}

TEST_F(QofMultiEntityTest, FilteredAdd)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    // Add only accounts from both collections using filter
    qof_multi_entity_collection_add_collection_filtered(multi_coll, account_coll, 
                                                         filter_accounts_only, nullptr);
    qof_multi_entity_collection_add_collection_filtered(multi_coll, transaction_coll, 
                                                         filter_accounts_only, nullptr);
    
    // Should only have accounts (3), no transactions
    EXPECT_EQ(3U, qof_multi_entity_collection_count(multi_coll));
    
    for (const auto* account : accounts)
    {
        EXPECT_TRUE(qof_multi_entity_collection_contains(multi_coll, account));
    }
    for (const auto* transaction : transactions)
    {
        EXPECT_FALSE(qof_multi_entity_collection_contains(multi_coll, transaction));
    }
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(QofMultiEntityTest, FilterCollection)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    // Add both collections
    qof_multi_entity_collection_add_collection(multi_coll, account_coll);
    qof_multi_entity_collection_add_collection(multi_coll, transaction_coll);
    
    // Filter to get only accounts
    QofMultiEntityCollection* filtered_coll = 
        qof_multi_entity_collection_filter(multi_coll, filter_accounts_only, nullptr);
    
    EXPECT_EQ(3U, qof_multi_entity_collection_count(filtered_coll));
    
    for (const auto* account : accounts)
    {
        EXPECT_TRUE(qof_multi_entity_collection_contains(filtered_coll, account));
    }
    for (const auto* transaction : transactions)
    {
        EXPECT_FALSE(qof_multi_entity_collection_contains(filtered_coll, transaction));
    }
    
    qof_multi_entity_collection_destroy(multi_coll);
    qof_multi_entity_collection_destroy(filtered_coll);
}

TEST_F(QofMultiEntityTest, MergeCollections)
{
    QofMultiEntityCollection* coll1 = qof_multi_entity_collection_new();
    QofMultiEntityCollection* coll2 = qof_multi_entity_collection_new();
    
    // Add accounts to coll1, transactions to coll2
    qof_multi_entity_collection_add_collection(coll1, account_coll);
    qof_multi_entity_collection_add_collection(coll2, transaction_coll);
    
    // Merge collections
    QofMultiEntityCollection* merged_coll = 
        qof_multi_entity_collection_merge(coll1, coll2);
    
    EXPECT_EQ(6U, qof_multi_entity_collection_count(merged_coll));
    
    // Verify all entities are present
    for (const auto* account : accounts)
    {
        EXPECT_TRUE(qof_multi_entity_collection_contains(merged_coll, account));
    }
    for (const auto* transaction : transactions)
    {
        EXPECT_TRUE(qof_multi_entity_collection_contains(merged_coll, transaction));
    }
    
    qof_multi_entity_collection_destroy(coll1);
    qof_multi_entity_collection_destroy(coll2);
    qof_multi_entity_collection_destroy(merged_coll);
}

// Helper for iteration test
struct IterationCounter
{
    int count;
    std::vector<QofInstance*> entities;
};

static void
count_entities_cb(QofInstance* entity, gpointer user_data)
{
    IterationCounter* counter = (IterationCounter*)user_data;
    counter->count++;
    counter->entities.push_back(entity);
}

TEST_F(QofMultiEntityTest, ForEach)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    // Add both collections
    qof_multi_entity_collection_add_collection(multi_coll, account_coll);
    qof_multi_entity_collection_add_collection(multi_coll, transaction_coll);
    
    // Test foreach iteration
    IterationCounter counter = {0, {}};
    qof_multi_entity_collection_foreach(multi_coll, count_entities_cb, &counter);
    
    EXPECT_EQ(6, counter.count);
    EXPECT_EQ(6U, counter.entities.size());
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(QofMultiEntityTest, NullPointerHandling)
{
    // Test null pointer handling - these should not crash
    qof_multi_entity_collection_destroy(nullptr);
    qof_multi_entity_collection_add_collection(nullptr, account_coll);
    qof_multi_entity_collection_add_collection_filtered(nullptr, account_coll, 
                                                         filter_accounts_only, nullptr);
    EXPECT_FALSE(qof_multi_entity_collection_add_entity(nullptr, accounts[0]));
    EXPECT_FALSE(qof_multi_entity_collection_remove_entity(nullptr, accounts[0]));
    EXPECT_EQ(0U, qof_multi_entity_collection_count(nullptr));
    EXPECT_FALSE(qof_multi_entity_collection_contains(nullptr, accounts[0]));
    qof_multi_entity_collection_foreach(nullptr, count_entities_cb, nullptr);
    EXPECT_EQ(nullptr, qof_multi_entity_collection_get_types(nullptr));
    EXPECT_EQ(nullptr, qof_multi_entity_collection_filter(nullptr, filter_accounts_only, nullptr));
    EXPECT_EQ(nullptr, qof_multi_entity_collection_merge(nullptr, nullptr));
}

// ============================================================
// Organization Integration Tests
// ============================================================

#include "../gncOrganization.h"
#include "../qofbook.h"

class QofMultiEntityOrgTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        qof_init();
        qof_book_register();
        gncOrganizationRegister();
        
        book = qof_book_new();
        ASSERT_NE(nullptr, book);
        
        // Create test organization
        org = gncOrganizationCreate(book);
        ASSERT_NE(nullptr, org);
        gncOrganizationSetName(org, "Test Organization");
        
        // Create test entities
        for (int i = 0; i < 5; i++)
        {
            QofInstance* entity = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
            entity->e_type = (i < 3) ? "Account" : "Transaction";
            entities.push_back(entity);
        }
    }
    
    void TearDown() override
    {
        for (auto* entity : entities)
        {
            g_object_unref(entity);
        }
        
        gncOrganizationDestroy(org);
        qof_book_destroy(book);
        qof_close();
    }
    
    QofBook* book;
    GncOrganization* org;
    std::vector<QofInstance*> entities;
};

TEST_F(QofMultiEntityOrgTest, CreateFromOrganization)
{
    // Add entities to organization
    for (auto* entity : entities)
    {
        gncOrganizationAddEntity(org, entity);
    }
    
    EXPECT_EQ(5U, gncOrganizationGetEntityCount(org));
    
    // Create multi-entity collection from organization
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_from_organization(org);
    ASSERT_NE(nullptr, multi_coll);
    
    EXPECT_EQ(5U, qof_multi_entity_collection_count(multi_coll));
    
    // Verify all entities are present
    for (auto* entity : entities)
    {
        EXPECT_TRUE(qof_multi_entity_collection_contains(multi_coll, entity));
    }
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(QofMultiEntityOrgTest, CreateFromEmptyOrganization)
{
    // Don't add any entities to organization
    EXPECT_EQ(0U, gncOrganizationGetEntityCount(org));
    
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_from_organization(org);
    ASSERT_NE(nullptr, multi_coll);
    
    EXPECT_EQ(0U, qof_multi_entity_collection_count(multi_coll));
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(QofMultiEntityOrgTest, CreateFromNullOrganization)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_from_organization(nullptr);
    EXPECT_EQ(nullptr, multi_coll);
}

TEST_F(QofMultiEntityOrgTest, AddOrganizationEntities)
{
    // Add entities to organization
    for (auto* entity : entities)
    {
        gncOrganizationAddEntity(org, entity);
    }
    
    // Create empty multi-entity collection
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    ASSERT_NE(nullptr, multi_coll);
    EXPECT_EQ(0U, qof_multi_entity_collection_count(multi_coll));
    
    // Add organization entities to collection
    qof_multi_entity_collection_add_organization_entities(multi_coll, org);
    
    EXPECT_EQ(5U, qof_multi_entity_collection_count(multi_coll));
    
    for (auto* entity : entities)
    {
        EXPECT_TRUE(qof_multi_entity_collection_contains(multi_coll, entity));
    }
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(QofMultiEntityOrgTest, AddOrganizationEntitiesToExisting)
{
    // Add entities to organization
    for (int i = 0; i < 3; i++)
    {
        gncOrganizationAddEntity(org, entities[i]);
    }
    
    // Create multi-entity collection with some other entities
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    qof_multi_entity_collection_add_entity(multi_coll, entities[3]);
    qof_multi_entity_collection_add_entity(multi_coll, entities[4]);
    
    EXPECT_EQ(2U, qof_multi_entity_collection_count(multi_coll));
    
    // Add organization entities
    qof_multi_entity_collection_add_organization_entities(multi_coll, org);
    
    // Should now have 5 entities (no duplicates if entities were shared)
    EXPECT_EQ(5U, qof_multi_entity_collection_count(multi_coll));
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(QofMultiEntityOrgTest, AddOrganizationEntitiesNullHandling)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    // Should not crash
    qof_multi_entity_collection_add_organization_entities(nullptr, org);
    qof_multi_entity_collection_add_organization_entities(multi_coll, nullptr);
    qof_multi_entity_collection_add_organization_entities(nullptr, nullptr);
    
    EXPECT_EQ(0U, qof_multi_entity_collection_count(multi_coll));
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(QofMultiEntityOrgTest, FilterByOrganization)
{
    // Add entities to organization
    for (int i = 0; i < 3; i++)
    {
        gncOrganizationAddEntity(org, entities[i]);
    }
    
    // Create multi-entity collection with ALL entities
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    for (auto* entity : entities)
    {
        qof_multi_entity_collection_add_entity(multi_coll, entity);
    }
    
    EXPECT_EQ(5U, qof_multi_entity_collection_count(multi_coll));
    
    // Filter by organization - should only get first 3 entities
    QofMultiEntityCollection* filtered = 
        qof_multi_entity_collection_filter_by_organization(multi_coll, org);
    
    ASSERT_NE(nullptr, filtered);
    EXPECT_EQ(3U, qof_multi_entity_collection_count(filtered));
    
    // Verify correct entities are in filtered collection
    for (int i = 0; i < 3; i++)
    {
        EXPECT_TRUE(qof_multi_entity_collection_contains(filtered, entities[i]));
    }
    for (int i = 3; i < 5; i++)
    {
        EXPECT_FALSE(qof_multi_entity_collection_contains(filtered, entities[i]));
    }
    
    qof_multi_entity_collection_destroy(multi_coll);
    qof_multi_entity_collection_destroy(filtered);
}

TEST_F(QofMultiEntityOrgTest, FilterByOrganizationNullHandling)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    QofMultiEntityCollection* result1 = 
        qof_multi_entity_collection_filter_by_organization(nullptr, org);
    EXPECT_EQ(nullptr, result1);
    
    QofMultiEntityCollection* result2 = 
        qof_multi_entity_collection_filter_by_organization(multi_coll, nullptr);
    EXPECT_EQ(nullptr, result2);
    
    QofMultiEntityCollection* result3 = 
        qof_multi_entity_collection_filter_by_organization(nullptr, nullptr);
    EXPECT_EQ(nullptr, result3);
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(QofMultiEntityOrgTest, FilterByOrganizationNoMatches)
{
    // Create organization without any entities
    EXPECT_EQ(0U, gncOrganizationGetEntityCount(org));
    
    // Create multi-entity collection with entities not in organization
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    for (auto* entity : entities)
    {
        qof_multi_entity_collection_add_entity(multi_coll, entity);
    }
    
    EXPECT_EQ(5U, qof_multi_entity_collection_count(multi_coll));
    
    // Filter should return empty collection
    QofMultiEntityCollection* filtered = 
        qof_multi_entity_collection_filter_by_organization(multi_coll, org);
    
    ASSERT_NE(nullptr, filtered);
    EXPECT_EQ(0U, qof_multi_entity_collection_count(filtered));
    
    qof_multi_entity_collection_destroy(multi_coll);
    qof_multi_entity_collection_destroy(filtered);
}

TEST_F(QofMultiEntityOrgTest, MultipleOrganizationsFiltering)
{
    // Create second organization
    GncOrganization* org2 = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org2);
    gncOrganizationSetName(org2, "Second Organization");
    
    // Add first 3 entities to org1
    for (int i = 0; i < 3; i++)
    {
        gncOrganizationAddEntity(org, entities[i]);
    }
    
    // Add last 2 entities to org2
    for (int i = 3; i < 5; i++)
    {
        gncOrganizationAddEntity(org2, entities[i]);
    }
    
    // Create multi-entity collection with all entities
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    for (auto* entity : entities)
    {
        qof_multi_entity_collection_add_entity(multi_coll, entity);
    }
    
    // Filter by first organization
    QofMultiEntityCollection* filtered1 = 
        qof_multi_entity_collection_filter_by_organization(multi_coll, org);
    EXPECT_EQ(3U, qof_multi_entity_collection_count(filtered1));
    
    // Filter by second organization
    QofMultiEntityCollection* filtered2 = 
        qof_multi_entity_collection_filter_by_organization(multi_coll, org2);
    EXPECT_EQ(2U, qof_multi_entity_collection_count(filtered2));
    
    qof_multi_entity_collection_destroy(multi_coll);
    qof_multi_entity_collection_destroy(filtered1);
    qof_multi_entity_collection_destroy(filtered2);
    gncOrganizationDestroy(org2);
}

TEST_F(QofMultiEntityOrgTest, GetTypesFromOrganization)
{
    // Add mixed entity types to organization
    for (auto* entity : entities)
    {
        gncOrganizationAddEntity(org, entity);
    }
    
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_from_organization(org);
    ASSERT_NE(nullptr, multi_coll);
    
    GList* types = qof_multi_entity_collection_get_types(multi_coll);
    EXPECT_EQ(2U, g_list_length(types));  // Should have "Account" and "Transaction"
    
    gboolean found_account = FALSE;
    gboolean found_transaction = FALSE;
    
    for (GList* node = types; node; node = node->next)
    {
        const char* type = (const char*)node->data;
        if (g_strcmp0(type, "Account") == 0)
            found_account = TRUE;
        else if (g_strcmp0(type, "Transaction") == 0)
            found_transaction = TRUE;
    }
    
    EXPECT_TRUE(found_account);
    EXPECT_TRUE(found_transaction);
    
    g_list_free(types);
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(QofMultiEntityOrgTest, MergeOrganizationCollections)
{
    // Create two organizations
    GncOrganization* org2 = gncOrganizationCreate(book);
    gncOrganizationSetName(org2, "Second Org");
    
    // Add first 2 entities to org1
    for (int i = 0; i < 2; i++)
    {
        gncOrganizationAddEntity(org, entities[i]);
    }
    
    // Add last 3 entities to org2
    for (int i = 2; i < 5; i++)
    {
        gncOrganizationAddEntity(org2, entities[i]);
    }
    
    // Create collections from both organizations
    QofMultiEntityCollection* coll1 = qof_multi_entity_collection_from_organization(org);
    QofMultiEntityCollection* coll2 = qof_multi_entity_collection_from_organization(org2);
    
    EXPECT_EQ(2U, qof_multi_entity_collection_count(coll1));
    EXPECT_EQ(3U, qof_multi_entity_collection_count(coll2));
    
    // Merge collections
    QofMultiEntityCollection* merged = qof_multi_entity_collection_merge(coll1, coll2);
    ASSERT_NE(nullptr, merged);
    
    EXPECT_EQ(5U, qof_multi_entity_collection_count(merged));
    
    for (auto* entity : entities)
    {
        EXPECT_TRUE(qof_multi_entity_collection_contains(merged, entity));
    }
    
    qof_multi_entity_collection_destroy(coll1);
    qof_multi_entity_collection_destroy(coll2);
    qof_multi_entity_collection_destroy(merged);
    gncOrganizationDestroy(org2);
}

TEST_F(QofMultiEntityOrgTest, ComplexOrganizationScenario)
{
    // Create 3 organizations
    GncOrganization* parent_org = org;  // Reuse existing org
    GncOrganization* subsidiary1 = gncOrganizationCreate(book);
    GncOrganization* subsidiary2 = gncOrganizationCreate(book);
    
    gncOrganizationSetName(parent_org, "Parent Corp");
    gncOrganizationSetName(subsidiary1, "Subsidiary Alpha");
    gncOrganizationSetName(subsidiary2, "Subsidiary Beta");
    
    // Create additional entities
    std::vector<QofInstance*> all_entities;
    for (int i = 0; i < 15; i++)
    {
        QofInstance* entity = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
        entity->e_type = (i % 3 == 0) ? "Account" : (i % 3 == 1) ? "Transaction" : "Customer";
        all_entities.push_back(entity);
    }
    
    // Distribute entities across organizations
    for (int i = 0; i < 5; i++)
        gncOrganizationAddEntity(parent_org, all_entities[i]);
    for (int i = 5; i < 10; i++)
        gncOrganizationAddEntity(subsidiary1, all_entities[i]);
    for (int i = 10; i < 15; i++)
        gncOrganizationAddEntity(subsidiary2, all_entities[i]);
    
    // Create collection for parent
    QofMultiEntityCollection* parent_coll = qof_multi_entity_collection_from_organization(parent_org);
    EXPECT_EQ(5U, qof_multi_entity_collection_count(parent_coll));
    
    // Create collection for subsidiary1
    QofMultiEntityCollection* sub1_coll = qof_multi_entity_collection_from_organization(subsidiary1);
    EXPECT_EQ(5U, qof_multi_entity_collection_count(sub1_coll));
    
    // Create consolidated collection
    QofMultiEntityCollection* consolidated = qof_multi_entity_collection_new();
    qof_multi_entity_collection_add_organization_entities(consolidated, parent_org);
    qof_multi_entity_collection_add_organization_entities(consolidated, subsidiary1);
    qof_multi_entity_collection_add_organization_entities(consolidated, subsidiary2);
    
    EXPECT_EQ(15U, qof_multi_entity_collection_count(consolidated));
    
    // Get types - should have Account, Transaction, Customer
    GList* types = qof_multi_entity_collection_get_types(consolidated);
    EXPECT_EQ(3U, g_list_length(types));
    
    g_list_free(types);
    qof_multi_entity_collection_destroy(parent_coll);
    qof_multi_entity_collection_destroy(sub1_coll);
    qof_multi_entity_collection_destroy(consolidated);
    gncOrganizationDestroy(subsidiary1);
    gncOrganizationDestroy(subsidiary2);
    
    for (auto* entity : all_entities)
    {
        g_object_unref(entity);
    }
}

TEST_F(QofMultiEntityOrgTest, OrganizationEntityIterationWithCallback)
{
    // Add entities to organization
    for (auto* entity : entities)
    {
        gncOrganizationAddEntity(org, entity);
    }
    
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_from_organization(org);
    
    // Count entities using foreach
    struct CounterData {
        int count;
        std::vector<QofInstance*> seen;
    } counter = {0, {}};
    
    auto callback = [](QofInstance* entity, gpointer user_data) {
        CounterData* data = static_cast<CounterData*>(user_data);
        data->count++;
        data->seen.push_back(entity);
    };
    
    qof_multi_entity_collection_foreach(multi_coll, callback, &counter);
    
    EXPECT_EQ(5, counter.count);
    EXPECT_EQ(5U, counter.seen.size());
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(QofMultiEntityOrgTest, DynamicOrganizationUpdates)
{
    // Start with empty organization
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_from_organization(org);
    EXPECT_EQ(0U, qof_multi_entity_collection_count(multi_coll));
    qof_multi_entity_collection_destroy(multi_coll);
    
    // Add some entities
    for (int i = 0; i < 3; i++)
    {
        gncOrganizationAddEntity(org, entities[i]);
    }
    
    multi_coll = qof_multi_entity_collection_from_organization(org);
    EXPECT_EQ(3U, qof_multi_entity_collection_count(multi_coll));
    qof_multi_entity_collection_destroy(multi_coll);
    
    // Add more entities
    for (int i = 3; i < 5; i++)
    {
        gncOrganizationAddEntity(org, entities[i]);
    }
    
    multi_coll = qof_multi_entity_collection_from_organization(org);
    EXPECT_EQ(5U, qof_multi_entity_collection_count(multi_coll));
    qof_multi_entity_collection_destroy(multi_coll);
    
    // Remove some entities
    gncOrganizationRemoveEntity(org, entities[0]);
    gncOrganizationRemoveEntity(org, entities[2]);
    
    multi_coll = qof_multi_entity_collection_from_organization(org);
    EXPECT_EQ(3U, qof_multi_entity_collection_count(multi_coll));
    
    qof_multi_entity_collection_destroy(multi_coll);
}

// ============================================================
// Stress Tests
// ============================================================

TEST_F(QofMultiEntityOrgTest, LargeOrganizationCollection)
{
    const int num_entities = 10000;
    std::vector<QofInstance*> large_entities;
    
    // Create large organization
    for (int i = 0; i < num_entities; i++)
    {
        QofInstance* entity = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
        entity->e_type = (i % 2 == 0) ? "Account" : "Transaction";
        large_entities.push_back(entity);
        gncOrganizationAddEntity(org, entity);
    }
    
    EXPECT_EQ(num_entities, gncOrganizationGetEntityCount(org));
    
    // Create multi-entity collection
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_from_organization(org);
    EXPECT_EQ(num_entities, qof_multi_entity_collection_count(multi_coll));
    
    // Verify types
    GList* types = qof_multi_entity_collection_get_types(multi_coll);
    EXPECT_EQ(2U, g_list_length(types));
    g_list_free(types);
    
    qof_multi_entity_collection_destroy(multi_coll);
    
    for (auto* entity : large_entities)
    {
        g_object_unref(entity);
    }
}

TEST_F(QofMultiEntityOrgTest, MultipleOrganizationMergeStress)
{
    const int num_orgs = 10;
    const int entities_per_org = 100;
    
    std::vector<GncOrganization*> orgs;
    std::vector<QofInstance*> all_entities;
    
    // Create multiple organizations with entities
    for (int org_idx = 0; org_idx < num_orgs; org_idx++)
    {
        GncOrganization* test_org = gncOrganizationCreate(book);
        orgs.push_back(test_org);
        
        for (int i = 0; i < entities_per_org; i++)
        {
            QofInstance* entity = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
            entity->e_type = "TestEntity";
            all_entities.push_back(entity);
            gncOrganizationAddEntity(test_org, entity);
        }
    }
    
    // Create merged collection
    QofMultiEntityCollection* merged = qof_multi_entity_collection_new();
    for (auto* test_org : orgs)
    {
        qof_multi_entity_collection_add_organization_entities(merged, test_org);
    }
    
    EXPECT_EQ(num_orgs * entities_per_org, qof_multi_entity_collection_count(merged));
    
    qof_multi_entity_collection_destroy(merged);
    
    for (auto* test_org : orgs)
    {
        gncOrganizationDestroy(test_org);
    }
    
    for (auto* entity : all_entities)
    {
        g_object_unref(entity);
    }
}
// ============================================================
// Advanced Edge Case Tests
// ============================================================

class QofMultiEntityAdvancedTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        qof_init();
        qof_book_register();
        
        book = qof_book_new();
        ASSERT_NE(nullptr, book);
        
        account_coll = qof_collection_new("Account");
        transaction_coll = qof_collection_new("Transaction");
        customer_coll = qof_collection_new("Customer");
    }
    
    void TearDown() override
    {
        for (auto* entity : all_entities)
        {
            g_object_unref(entity);
        }
        
        qof_collection_destroy(account_coll);
        qof_collection_destroy(transaction_coll);
        qof_collection_destroy(customer_coll);
        qof_book_destroy(book);
        qof_close();
    }
    
    QofInstance* createEntity(const char* type)
    {
        QofInstance* entity = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
        entity->e_type = type;
        all_entities.push_back(entity);
        return entity;
    }
    
    QofBook* book;
    QofCollection* account_coll;
    QofCollection* transaction_coll;
    QofCollection* customer_coll;
    std::vector<QofInstance*> all_entities;
};

TEST_F(QofMultiEntityAdvancedTest, EmptyCollectionOperations)
{
    QofMultiEntityCollection* empty = qof_multi_entity_collection_new();
    
    EXPECT_EQ(0U, qof_multi_entity_collection_count(empty));
    EXPECT_EQ(nullptr, qof_multi_entity_collection_get_types(empty));
    
    int count = 0;
    auto callback = [](QofInstance*, gpointer user_data) {
        int* counter = static_cast<int*>(user_data);
        (*counter)++;
    };
    qof_multi_entity_collection_foreach(empty, callback, &count);
    EXPECT_EQ(0, count);
    
    QofMultiEntityCollection* merged = qof_multi_entity_collection_merge(empty, empty);
    EXPECT_EQ(0U, qof_multi_entity_collection_count(merged));
    
    qof_multi_entity_collection_destroy(empty);
    qof_multi_entity_collection_destroy(merged);
}

TEST_F(QofMultiEntityAdvancedTest, SingleEntityCollection)
{
    QofMultiEntityCollection* single = qof_multi_entity_collection_new();
    QofInstance* entity = createEntity("Account");
    
    qof_multi_entity_collection_add_entity(single, entity);
    
    EXPECT_EQ(1U, qof_multi_entity_collection_count(single));
    EXPECT_TRUE(qof_multi_entity_collection_contains(single, entity));
    
    GList* types = qof_multi_entity_collection_get_types(single);
    EXPECT_EQ(1U, g_list_length(types));
    EXPECT_STREQ("Account", (const char*)types->data);
    
    g_list_free(types);
    qof_multi_entity_collection_destroy(single);
}

TEST_F(QofMultiEntityAdvancedTest, RemoveFromEmptyCollection)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    QofInstance* entity = createEntity("Account");
    
    EXPECT_FALSE(qof_multi_entity_collection_remove_entity(multi_coll, entity));
    EXPECT_EQ(0U, qof_multi_entity_collection_count(multi_coll));
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(QofMultiEntityAdvancedTest, AddRemoveAddSameEntity)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    QofInstance* entity = createEntity("Account");
    
    EXPECT_TRUE(qof_multi_entity_collection_add_entity(multi_coll, entity));
    EXPECT_EQ(1U, qof_multi_entity_collection_count(multi_coll));
    
    EXPECT_TRUE(qof_multi_entity_collection_remove_entity(multi_coll, entity));
    EXPECT_EQ(0U, qof_multi_entity_collection_count(multi_coll));
    
    EXPECT_TRUE(qof_multi_entity_collection_add_entity(multi_coll, entity));
    EXPECT_EQ(1U, qof_multi_entity_collection_count(multi_coll));
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(QofMultiEntityAdvancedTest, TypeCountAfterRemoval)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    QofInstance* account = createEntity("Account");
    QofInstance* transaction = createEntity("Transaction");
    
    qof_multi_entity_collection_add_entity(multi_coll, account);
    qof_multi_entity_collection_add_entity(multi_coll, transaction);
    
    GList* types = qof_multi_entity_collection_get_types(multi_coll);
    EXPECT_EQ(2U, g_list_length(types));
    g_list_free(types);
    
    qof_multi_entity_collection_remove_entity(multi_coll, account);
    
    types = qof_multi_entity_collection_get_types(multi_coll);
    EXPECT_EQ(1U, g_list_length(types));
    EXPECT_STREQ("Transaction", (const char*)types->data);
    
    g_list_free(types);
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(QofMultiEntityAdvancedTest, FilterPreservesOriginal)
{
    QofMultiEntityCollection* original = qof_multi_entity_collection_new();
    
    for (int i = 0; i < 5; i++)
    {
        qof_multi_entity_collection_add_entity(original, 
            createEntity((i % 2 == 0) ? "Account" : "Transaction"));
    }
    
    EXPECT_EQ(5U, qof_multi_entity_collection_count(original));
    
    auto filter_accounts = [](QofInstance* entity, gpointer) -> gboolean {
        return g_strcmp0(entity->e_type, "Account") == 0;
    };
    
    QofMultiEntityCollection* filtered = 
        qof_multi_entity_collection_filter(original, filter_accounts, nullptr);
    
    EXPECT_EQ(3U, qof_multi_entity_collection_count(filtered));
    EXPECT_EQ(5U, qof_multi_entity_collection_count(original));
    
    qof_multi_entity_collection_destroy(original);
    qof_multi_entity_collection_destroy(filtered);
}

TEST_F(QofMultiEntityAdvancedTest, MergeWithOverlap)
{
    QofMultiEntityCollection* coll1 = qof_multi_entity_collection_new();
    QofMultiEntityCollection* coll2 = qof_multi_entity_collection_new();
    
    QofInstance* shared = createEntity("Account");
    QofInstance* unique1 = createEntity("Transaction");
    QofInstance* unique2 = createEntity("Customer");
    
    qof_multi_entity_collection_add_entity(coll1, shared);
    qof_multi_entity_collection_add_entity(coll1, unique1);
    
    qof_multi_entity_collection_add_entity(coll2, shared);
    qof_multi_entity_collection_add_entity(coll2, unique2);
    
    QofMultiEntityCollection* merged = qof_multi_entity_collection_merge(coll1, coll2);
    
    EXPECT_EQ(3U, qof_multi_entity_collection_count(merged));
    
    EXPECT_TRUE(qof_multi_entity_collection_contains(merged, shared));
    EXPECT_TRUE(qof_multi_entity_collection_contains(merged, unique1));
    EXPECT_TRUE(qof_multi_entity_collection_contains(merged, unique2));
    
    qof_multi_entity_collection_destroy(coll1);
    qof_multi_entity_collection_destroy(coll2);
    qof_multi_entity_collection_destroy(merged);
}

TEST_F(QofMultiEntityAdvancedTest, LargeScaleMerge)
{
    const int collections_count = 10;
    const int entities_per_collection = 100;
    
    std::vector<QofMultiEntityCollection*> collections;
    
    for (int c = 0; c < collections_count; c++)
    {
        QofMultiEntityCollection* coll = qof_multi_entity_collection_new();
        collections.push_back(coll);
        
        for (int i = 0; i < entities_per_collection; i++)
        {
            qof_multi_entity_collection_add_entity(coll, createEntity("Account"));
        }
        
        EXPECT_EQ(entities_per_collection, qof_multi_entity_collection_count(coll));
    }
    
    QofMultiEntityCollection* final_merged = qof_multi_entity_collection_new();
    
    for (auto* coll : collections)
    {
        QofMultiEntityCollection* temp = qof_multi_entity_collection_merge(final_merged, coll);
        qof_multi_entity_collection_destroy(final_merged);
        final_merged = temp;
    }
    
    EXPECT_EQ(collections_count * entities_per_collection, 
              qof_multi_entity_collection_count(final_merged));
    
    for (auto* coll : collections)
    {
        qof_multi_entity_collection_destroy(coll);
    }
    qof_multi_entity_collection_destroy(final_merged);
}

TEST_F(QofMultiEntityAdvancedTest, IterationConsistency)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    std::vector<QofInstance*> added_entities;
    for (int i = 0; i < 10; i++)
    {
        QofInstance* entity = createEntity("Account");
        added_entities.push_back(entity);
        qof_multi_entity_collection_add_entity(multi_coll, entity);
    }
    
    for (int iteration = 0; iteration < 3; iteration++)
    {
        std::vector<QofInstance*> seen_entities;
        auto collector = [](QofInstance* entity, gpointer user_data) {
            auto* vec = static_cast<std::vector<QofInstance*>*>(user_data);
            vec->push_back(entity);
        };
        
        qof_multi_entity_collection_foreach(multi_coll, collector, &seen_entities);
        
        EXPECT_EQ(10U, seen_entities.size());
        
        for (auto* added : added_entities)
        {
            EXPECT_TRUE(std::find(seen_entities.begin(), seen_entities.end(), added) != seen_entities.end());
        }
    }
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(QofMultiEntityAdvancedTest, CollectionIndependence)
{
    QofInstance* entity = createEntity("Account");
    
    QofMultiEntityCollection* coll1 = qof_multi_entity_collection_new();
    QofMultiEntityCollection* coll2 = qof_multi_entity_collection_new();
    
    qof_multi_entity_collection_add_entity(coll1, entity);
    EXPECT_EQ(1U, qof_multi_entity_collection_count(coll1));
    EXPECT_EQ(0U, qof_multi_entity_collection_count(coll2));
    
    qof_multi_entity_collection_add_entity(coll2, entity);
    EXPECT_EQ(1U, qof_multi_entity_collection_count(coll1));
    EXPECT_EQ(1U, qof_multi_entity_collection_count(coll2));
    
    qof_multi_entity_collection_remove_entity(coll1, entity);
    EXPECT_EQ(0U, qof_multi_entity_collection_count(coll1));
    EXPECT_EQ(1U, qof_multi_entity_collection_count(coll2));
    
    qof_multi_entity_collection_destroy(coll1);
    qof_multi_entity_collection_destroy(coll2);
}
