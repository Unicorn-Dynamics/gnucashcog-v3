/********************************************************************\
 * gtest-multi-entity-edge-cases.cpp -- Edge case tests            *
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
\********************************************************************/

#include <config.h>
#include <glib.h>
#include <vector>
#include <algorithm>
#include <string>
#include "../qof.h"
#include "../qofid.h"
#include "../qofbook.h"
#include "../qofinstance.h"
#include "../gncOrganization.h"
#include "../guid.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcpp"
#include <gtest/gtest.h>
#pragma GCC diagnostic pop

// ============================================================
// Edge Cases: Invalid GUIDs and Entity States
// ============================================================

class MultiEntityEdgeCaseTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        qof_init();
        qof_book_register();
        gncOrganizationRegister();
        
        book = qof_book_new();
        ASSERT_NE(nullptr, book);
    }
    
    void TearDown() override
    {
        for (auto* entity : all_entities)
        {
            g_object_unref(entity);
        }
        
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
    std::vector<QofInstance*> all_entities;
};

TEST_F(MultiEntityEdgeCaseTest, EntityWithNullGUID)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    QofInstance* entity = createEntity("Account");
    // Entity has null GUID by default from g_object_new
    
    // Should fail to add entity with null GUID
    gboolean result = qof_multi_entity_collection_add_entity(multi_coll, entity);
    EXPECT_FALSE(result);
    EXPECT_EQ(0U, qof_multi_entity_collection_count(multi_coll));
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(MultiEntityEdgeCaseTest, EntityGUIDConsistency)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    QofInstance* entity = createEntity("Account");
    
    // Set a valid GUID
    GncGUID guid;
    guid_replace(&guid);
    qof_instance_set_guid(entity, &guid);
    
    EXPECT_TRUE(qof_multi_entity_collection_add_entity(multi_coll, entity));
    EXPECT_TRUE(qof_multi_entity_collection_contains(multi_coll, entity));
    
    const GncGUID* retrieved_guid = qof_instance_get_guid(entity);
    EXPECT_TRUE(guid_equal(&guid, retrieved_guid));
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(MultiEntityEdgeCaseTest, MultipleEntitiesSameType)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    // Add 100 entities of the same type
    for (int i = 0; i < 100; i++)
    {
        QofInstance* entity = createEntity("Account");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        qof_multi_entity_collection_add_entity(multi_coll, entity);
    }
    
    EXPECT_EQ(100U, qof_multi_entity_collection_count(multi_coll));
    
    GList* types = qof_multi_entity_collection_get_types(multi_coll);
    EXPECT_EQ(1U, g_list_length(types));
    EXPECT_STREQ("Account", (const char*)types->data);
    
    g_list_free(types);
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(MultiEntityEdgeCaseTest, AlternatingAddRemove)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    std::vector<QofInstance*> entities;
    for (int i = 0; i < 10; i++)
    {
        QofInstance* entity = createEntity("Account");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        entities.push_back(entity);
    }
    
    // Alternately add and remove entities
    for (int cycle = 0; cycle < 5; cycle++)
    {
        // Add all
        for (auto* entity : entities)
        {
            qof_multi_entity_collection_add_entity(multi_coll, entity);
        }
        EXPECT_EQ(10U, qof_multi_entity_collection_count(multi_coll));
        
        // Remove all
        for (auto* entity : entities)
        {
            qof_multi_entity_collection_remove_entity(multi_coll, entity);
        }
        EXPECT_EQ(0U, qof_multi_entity_collection_count(multi_coll));
    }
    
    qof_multi_entity_collection_destroy(multi_coll);
}

// ============================================================
// Organization Edge Cases
// ============================================================

TEST_F(MultiEntityEdgeCaseTest, OrganizationWithNoEntities)
{
    GncOrganization* org = gncOrganizationCreate(book);
    
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_from_organization(org);
    ASSERT_NE(nullptr, multi_coll);
    EXPECT_EQ(0U, qof_multi_entity_collection_count(multi_coll));
    
    qof_multi_entity_collection_destroy(multi_coll);
    gncOrganizationDestroy(org);
}

TEST_F(MultiEntityEdgeCaseTest, OrganizationWithSingleEntity)
{
    GncOrganization* org = gncOrganizationCreate(book);
    QofInstance* entity = createEntity("Account");
    
    GncGUID guid;
    guid_replace(&guid);
    qof_instance_set_guid(entity, &guid);
    
    gncOrganizationAddEntity(org, entity);
    
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_from_organization(org);
    EXPECT_EQ(1U, qof_multi_entity_collection_count(multi_coll));
    EXPECT_TRUE(qof_multi_entity_collection_contains(multi_coll, entity));
    
    qof_multi_entity_collection_destroy(multi_coll);
    gncOrganizationDestroy(org);
}

TEST_F(MultiEntityEdgeCaseTest, MultipleOrganizationsSharingEntities)
{
    GncOrganization* org1 = gncOrganizationCreate(book);
    GncOrganization* org2 = gncOrganizationCreate(book);
    
    QofInstance* shared_entity = createEntity("Account");
    GncGUID guid;
    guid_replace(&guid);
    qof_instance_set_guid(shared_entity, &guid);
    
    // Add same entity to both organizations
    gncOrganizationAddEntity(org1, shared_entity);
    gncOrganizationAddEntity(org2, shared_entity);
    
    EXPECT_EQ(1U, gncOrganizationGetEntityCount(org1));
    EXPECT_EQ(1U, gncOrganizationGetEntityCount(org2));
    
    // Create collections from both
    QofMultiEntityCollection* coll1 = qof_multi_entity_collection_from_organization(org1);
    QofMultiEntityCollection* coll2 = qof_multi_entity_collection_from_organization(org2);
    
    EXPECT_TRUE(qof_multi_entity_collection_contains(coll1, shared_entity));
    EXPECT_TRUE(qof_multi_entity_collection_contains(coll2, shared_entity));
    
    // Merge should not duplicate
    QofMultiEntityCollection* merged = qof_multi_entity_collection_merge(coll1, coll2);
    EXPECT_EQ(1U, qof_multi_entity_collection_count(merged));
    
    qof_multi_entity_collection_destroy(coll1);
    qof_multi_entity_collection_destroy(coll2);
    qof_multi_entity_collection_destroy(merged);
    gncOrganizationDestroy(org1);
    gncOrganizationDestroy(org2);
}

TEST_F(MultiEntityEdgeCaseTest, RemoveEntityFromOrganizationInMultiCollection)
{
    GncOrganization* org = gncOrganizationCreate(book);
    
    QofInstance* entity = createEntity("Account");
    GncGUID guid;
    guid_replace(&guid);
    qof_instance_set_guid(entity, &guid);
    
    gncOrganizationAddEntity(org, entity);
    
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_from_organization(org);
    EXPECT_EQ(1U, qof_multi_entity_collection_count(multi_coll));
    
    // Remove entity from organization
    gncOrganizationRemoveEntity(org, entity);
    EXPECT_EQ(0U, gncOrganizationGetEntityCount(org));
    
    // Original multi-collection should still have the entity (snapshot)
    EXPECT_TRUE(qof_multi_entity_collection_contains(multi_coll, entity));
    
    // New collection from organization should be empty
    QofMultiEntityCollection* new_coll = qof_multi_entity_collection_from_organization(org);
    EXPECT_EQ(0U, qof_multi_entity_collection_count(new_coll));
    
    qof_multi_entity_collection_destroy(multi_coll);
    qof_multi_entity_collection_destroy(new_coll);
    gncOrganizationDestroy(org);
}

// ============================================================
// Filter Edge Cases
// ============================================================

TEST_F(MultiEntityEdgeCaseTest, FilterReturnsAllEntities)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    for (int i = 0; i < 10; i++)
    {
        QofInstance* entity = createEntity("Account");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        qof_multi_entity_collection_add_entity(multi_coll, entity);
    }
    
    auto always_true = [](QofInstance*, gpointer) -> gboolean {
        return TRUE;
    };
    
    QofMultiEntityCollection* filtered = 
        qof_multi_entity_collection_filter(multi_coll, always_true, nullptr);
    
    EXPECT_EQ(qof_multi_entity_collection_count(multi_coll),
              qof_multi_entity_collection_count(filtered));
    
    qof_multi_entity_collection_destroy(multi_coll);
    qof_multi_entity_collection_destroy(filtered);
}

TEST_F(MultiEntityEdgeCaseTest, FilterReturnsNoEntities)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    for (int i = 0; i < 10; i++)
    {
        QofInstance* entity = createEntity("Account");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        qof_multi_entity_collection_add_entity(multi_coll, entity);
    }
    
    auto always_false = [](QofInstance*, gpointer) -> gboolean {
        return FALSE;
    };
    
    QofMultiEntityCollection* filtered = 
        qof_multi_entity_collection_filter(multi_coll, always_false, nullptr);
    
    EXPECT_EQ(0U, qof_multi_entity_collection_count(filtered));
    
    qof_multi_entity_collection_destroy(multi_coll);
    qof_multi_entity_collection_destroy(filtered);
}

TEST_F(MultiEntityEdgeCaseTest, FilterWithComplexPredicate)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    // Add entities with different types
    for (int i = 0; i < 20; i++)
    {
        const char* type = (i < 5) ? "Account" : 
                          (i < 10) ? "Transaction" : 
                          (i < 15) ? "Customer" : "Vendor";
        QofInstance* entity = createEntity(type);
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        qof_multi_entity_collection_add_entity(multi_coll, entity);
    }
    
    // Filter: Accept only Account and Transaction
    auto filter_account_or_transaction = [](QofInstance* entity, gpointer) -> gboolean {
        return (g_strcmp0(entity->e_type, "Account") == 0 ||
                g_strcmp0(entity->e_type, "Transaction") == 0);
    };
    
    QofMultiEntityCollection* filtered = 
        qof_multi_entity_collection_filter(multi_coll, filter_account_or_transaction, nullptr);
    
    EXPECT_EQ(10U, qof_multi_entity_collection_count(filtered));
    
    GList* types = qof_multi_entity_collection_get_types(filtered);
    EXPECT_EQ(2U, g_list_length(types));
    
    g_list_free(types);
    qof_multi_entity_collection_destroy(multi_coll);
    qof_multi_entity_collection_destroy(filtered);
}

TEST_F(MultiEntityEdgeCaseTest, FilterWithStatefulPredicate)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    for (int i = 0; i < 20; i++)
    {
        QofInstance* entity = createEntity("Account");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        qof_multi_entity_collection_add_entity(multi_coll, entity);
    }
    
    // Filter that accepts first N entities
    struct LimitState {
        int limit;
        int count;
    } state = {7, 0};
    
    auto limited_filter = [](QofInstance*, gpointer user_data) -> gboolean {
        LimitState* s = static_cast<LimitState*>(user_data);
        if (s->count < s->limit)
        {
            s->count++;
            return TRUE;
        }
        return FALSE;
    };
    
    QofMultiEntityCollection* filtered = 
        qof_multi_entity_collection_filter(multi_coll, limited_filter, &state);
    
    EXPECT_EQ(7U, qof_multi_entity_collection_count(filtered));
    EXPECT_EQ(7, state.count);
    
    qof_multi_entity_collection_destroy(multi_coll);
    qof_multi_entity_collection_destroy(filtered);
}

// ============================================================
// Merge Edge Cases
// ============================================================

TEST_F(MultiEntityEdgeCaseTest, MergeSelfWithSelf)
{
    QofMultiEntityCollection* coll = qof_multi_entity_collection_new();
    
    for (int i = 0; i < 5; i++)
    {
        QofInstance* entity = createEntity("Account");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        qof_multi_entity_collection_add_entity(coll, entity);
    }
    
    QofMultiEntityCollection* merged = qof_multi_entity_collection_merge(coll, coll);
    
    // Should have same entities (no duplicates)
    EXPECT_EQ(5U, qof_multi_entity_collection_count(merged));
    
    qof_multi_entity_collection_destroy(coll);
    qof_multi_entity_collection_destroy(merged);
}

TEST_F(MultiEntityEdgeCaseTest, MergeEmptyWithNonEmpty)
{
    QofMultiEntityCollection* empty = qof_multi_entity_collection_new();
    QofMultiEntityCollection* non_empty = qof_multi_entity_collection_new();
    
    for (int i = 0; i < 5; i++)
    {
        QofInstance* entity = createEntity("Account");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        qof_multi_entity_collection_add_entity(non_empty, entity);
    }
    
    QofMultiEntityCollection* merged1 = qof_multi_entity_collection_merge(empty, non_empty);
    EXPECT_EQ(5U, qof_multi_entity_collection_count(merged1));
    
    QofMultiEntityCollection* merged2 = qof_multi_entity_collection_merge(non_empty, empty);
    EXPECT_EQ(5U, qof_multi_entity_collection_count(merged2));
    
    qof_multi_entity_collection_destroy(empty);
    qof_multi_entity_collection_destroy(non_empty);
    qof_multi_entity_collection_destroy(merged1);
    qof_multi_entity_collection_destroy(merged2);
}

TEST_F(MultiEntityEdgeCaseTest, ChainedMerges)
{
    QofMultiEntityCollection* coll1 = qof_multi_entity_collection_new();
    QofMultiEntityCollection* coll2 = qof_multi_entity_collection_new();
    QofMultiEntityCollection* coll3 = qof_multi_entity_collection_new();
    
    // Add entities to each collection
    for (int i = 0; i < 3; i++)
    {
        QofInstance* entity = createEntity("Account");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        qof_multi_entity_collection_add_entity(coll1, entity);
    }
    
    for (int i = 0; i < 4; i++)
    {
        QofInstance* entity = createEntity("Transaction");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        qof_multi_entity_collection_add_entity(coll2, entity);
    }
    
    for (int i = 0; i < 2; i++)
    {
        QofInstance* entity = createEntity("Customer");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        qof_multi_entity_collection_add_entity(coll3, entity);
    }
    
    // Chain merges: (coll1 + coll2) + coll3
    QofMultiEntityCollection* temp = qof_multi_entity_collection_merge(coll1, coll2);
    QofMultiEntityCollection* final = qof_multi_entity_collection_merge(temp, coll3);
    
    EXPECT_EQ(9U, qof_multi_entity_collection_count(final));
    
    GList* types = qof_multi_entity_collection_get_types(final);
    EXPECT_EQ(3U, g_list_length(types));
    
    g_list_free(types);
    qof_multi_entity_collection_destroy(coll1);
    qof_multi_entity_collection_destroy(coll2);
    qof_multi_entity_collection_destroy(coll3);
    qof_multi_entity_collection_destroy(temp);
    qof_multi_entity_collection_destroy(final);
}

// ============================================================
// Iteration Edge Cases
// ============================================================

TEST_F(MultiEntityEdgeCaseTest, IterateEmptyCollection)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    int callback_count = 0;
    auto callback = [](QofInstance*, gpointer user_data) {
        int* count = static_cast<int*>(user_data);
        (*count)++;
    };
    
    qof_multi_entity_collection_foreach(multi_coll, callback, &callback_count);
    EXPECT_EQ(0, callback_count);
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(MultiEntityEdgeCaseTest, IterationWithModification)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    std::vector<QofInstance*> entities;
    for (int i = 0; i < 10; i++)
    {
        QofInstance* entity = createEntity("Account");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        entities.push_back(entity);
        qof_multi_entity_collection_add_entity(multi_coll, entity);
    }
    
    // Note: Modifying collection during iteration would be undefined behavior
    // This test just verifies iteration works properly
    int iteration_count = 0;
    auto counter = [](QofInstance*, gpointer user_data) {
        int* count = static_cast<int*>(user_data);
        (*count)++;
    };
    
    qof_multi_entity_collection_foreach(multi_coll, counter, &iteration_count);
    EXPECT_EQ(10, iteration_count);
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(MultiEntityEdgeCaseTest, SortedIterationWithNullComparator)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    for (int i = 0; i < 5; i++)
    {
        QofInstance* entity = createEntity("Account");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        qof_multi_entity_collection_add_entity(multi_coll, entity);
    }
    
    int count = 0;
    auto callback = [](QofInstance*, gpointer user_data) {
        int* c = static_cast<int*>(user_data);
        (*c)++;
    };
    
    // Null comparator should still iterate (unsorted)
    qof_multi_entity_collection_foreach_sorted(multi_coll, callback, &count, nullptr);
    EXPECT_EQ(5, count);
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(MultiEntityEdgeCaseTest, SortedIterationWithComparator)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    std::vector<QofInstance*> entities;
    for (int i = 0; i < 5; i++)
    {
        QofInstance* entity = createEntity("Account");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        entities.push_back(entity);
        qof_multi_entity_collection_add_entity(multi_coll, entity);
    }
    
    auto guid_comparator = [](gconstpointer a, gconstpointer b) -> gint {
        const QofInstance* inst_a = static_cast<const QofInstance*>(a);
        const QofInstance* inst_b = static_cast<const QofInstance*>(b);
        const GncGUID* guid_a = qof_instance_get_guid(inst_a);
        const GncGUID* guid_b = qof_instance_get_guid(inst_b);
        return guid_compare(guid_a, guid_b);
    };
    
    std::vector<QofInstance*> sorted_entities;
    auto collector = [](QofInstance* entity, gpointer user_data) {
        auto* vec = static_cast<std::vector<QofInstance*>*>(user_data);
        vec->push_back(entity);
    };
    
    qof_multi_entity_collection_foreach_sorted(multi_coll, collector, 
                                                &sorted_entities, guid_comparator);
    
    EXPECT_EQ(5U, sorted_entities.size());
    
    // Verify entities are sorted by GUID
    for (size_t i = 1; i < sorted_entities.size(); i++)
    {
        const GncGUID* guid_prev = qof_instance_get_guid(sorted_entities[i-1]);
        const GncGUID* guid_curr = qof_instance_get_guid(sorted_entities[i]);
        EXPECT_LE(guid_compare(guid_prev, guid_curr), 0);
    }
    
    qof_multi_entity_collection_destroy(multi_coll);
}

// ============================================================
// Collection Type Tracking Edge Cases
// ============================================================

TEST_F(MultiEntityEdgeCaseTest, TypeTrackingWithAddRemove)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    QofInstance* account1 = createEntity("Account");
    QofInstance* account2 = createEntity("Account");
    QofInstance* transaction = createEntity("Transaction");
    
    GncGUID guid1, guid2, guid3;
    guid_replace(&guid1);
    guid_replace(&guid2);
    guid_replace(&guid3);
    qof_instance_set_guid(account1, &guid1);
    qof_instance_set_guid(account2, &guid2);
    qof_instance_set_guid(transaction, &guid3);
    
    qof_multi_entity_collection_add_entity(multi_coll, account1);
    qof_multi_entity_collection_add_entity(multi_coll, account2);
    qof_multi_entity_collection_add_entity(multi_coll, transaction);
    
    GList* types = qof_multi_entity_collection_get_types(multi_coll);
    EXPECT_EQ(2U, g_list_length(types));
    g_list_free(types);
    
    // Remove one account - Account type should still be present
    qof_multi_entity_collection_remove_entity(multi_coll, account1);
    types = qof_multi_entity_collection_get_types(multi_coll);
    EXPECT_EQ(2U, g_list_length(types));
    g_list_free(types);
    
    // Remove last account - Account type should disappear
    qof_multi_entity_collection_remove_entity(multi_coll, account2);
    types = qof_multi_entity_collection_get_types(multi_coll);
    EXPECT_EQ(1U, g_list_length(types));
    EXPECT_STREQ("Transaction", (const char*)types->data);
    g_list_free(types);
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(MultiEntityEdgeCaseTest, AddCollectionWithDuplicates)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    QofCollection* qof_coll = qof_collection_new("Account");
    
    // Create entity and add to QofCollection
    QofInstance* entity = createEntity("Account");
    GncGUID guid;
    guid_replace(&guid);
    qof_instance_set_guid(entity, &guid);
    qof_collection_insert_entity(qof_coll, entity);
    
    // Add entity directly to multi-collection
    qof_multi_entity_collection_add_entity(multi_coll, entity);
    EXPECT_EQ(1U, qof_multi_entity_collection_count(multi_coll));
    
    // Try to add the QofCollection containing the same entity
    qof_multi_entity_collection_add_collection(multi_coll, qof_coll);
    
    // Should still be 1 (no duplicate)
    EXPECT_EQ(1U, qof_multi_entity_collection_count(multi_coll));
    
    qof_collection_destroy(qof_coll);
    qof_multi_entity_collection_destroy(multi_coll);
}

// ============================================================
// Organization-Collection Lifecycle Tests
// ============================================================

TEST_F(MultiEntityEdgeCaseTest, OrganizationEntityAddedAfterCollectionCreation)
{
    GncOrganization* org = gncOrganizationCreate(book);
    
    QofInstance* entity1 = createEntity("Account");
    GncGUID guid1;
    guid_replace(&guid1);
    qof_instance_set_guid(entity1, &guid1);
    gncOrganizationAddEntity(org, entity1);
    
    // Create collection - should have 1 entity
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_from_organization(org);
    EXPECT_EQ(1U, qof_multi_entity_collection_count(multi_coll));
    
    // Add another entity to organization
    QofInstance* entity2 = createEntity("Transaction");
    GncGUID guid2;
    guid_replace(&guid2);
    qof_instance_set_guid(entity2, &guid2);
    gncOrganizationAddEntity(org, entity2);
    
    // Original collection should not be updated (snapshot behavior)
    EXPECT_EQ(1U, qof_multi_entity_collection_count(multi_coll));
    
    // New collection should have both
    QofMultiEntityCollection* new_coll = qof_multi_entity_collection_from_organization(org);
    EXPECT_EQ(2U, qof_multi_entity_collection_count(new_coll));
    
    qof_multi_entity_collection_destroy(multi_coll);
    qof_multi_entity_collection_destroy(new_coll);
    gncOrganizationDestroy(org);
}

TEST_F(MultiEntityEdgeCaseTest, FilterByOrganizationAfterEntityRemoval)
{
    GncOrganization* org = gncOrganizationCreate(book);
    
    std::vector<QofInstance*> org_entities;
    for (int i = 0; i < 5; i++)
    {
        QofInstance* entity = createEntity("Account");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        org_entities.push_back(entity);
        gncOrganizationAddEntity(org, entity);
    }
    
    // Create multi-entity collection with all entities
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    for (auto* entity : org_entities)
    {
        qof_multi_entity_collection_add_entity(multi_coll, entity);
    }
    
    // Remove some entities from organization
    gncOrganizationRemoveEntity(org, org_entities[0]);
    gncOrganizationRemoveEntity(org, org_entities[2]);
    
    // Filter should only include entities still in organization
    QofMultiEntityCollection* filtered = 
        qof_multi_entity_collection_filter_by_organization(multi_coll, org);
    
    EXPECT_EQ(3U, qof_multi_entity_collection_count(filtered));
    
    EXPECT_FALSE(qof_multi_entity_collection_contains(filtered, org_entities[0]));
    EXPECT_TRUE(qof_multi_entity_collection_contains(filtered, org_entities[1]));
    EXPECT_FALSE(qof_multi_entity_collection_contains(filtered, org_entities[2]));
    EXPECT_TRUE(qof_multi_entity_collection_contains(filtered, org_entities[3]));
    EXPECT_TRUE(qof_multi_entity_collection_contains(filtered, org_entities[4]));
    
    qof_multi_entity_collection_destroy(multi_coll);
    qof_multi_entity_collection_destroy(filtered);
    gncOrganizationDestroy(org);
}

// ============================================================
// Stress and Performance Tests
// ============================================================

TEST_F(MultiEntityEdgeCaseTest, LargeScaleOrganization)
{
    GncOrganization* org = gncOrganizationCreate(book);
    gncOrganizationSetName(org, "Mega Corp");
    
    const int entity_count = 10000;
    
    for (int i = 0; i < entity_count; i++)
    {
        QofInstance* entity = createEntity((i % 2 == 0) ? "Account" : "Transaction");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        gncOrganizationAddEntity(org, entity);
    }
    
    EXPECT_EQ(entity_count, gncOrganizationGetEntityCount(org));
    
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_from_organization(org);
    EXPECT_EQ(entity_count, qof_multi_entity_collection_count(multi_coll));
    
    GList* types = qof_multi_entity_collection_get_types(multi_coll);
    EXPECT_EQ(2U, g_list_length(types));
    
    g_list_free(types);
    qof_multi_entity_collection_destroy(multi_coll);
    gncOrganizationDestroy(org);
}

TEST_F(MultiEntityEdgeCaseTest, DeepFilterNesting)
{
    QofMultiEntityCollection* base = qof_multi_entity_collection_new();
    
    // Add 100 entities
    for (int i = 0; i < 100; i++)
    {
        QofInstance* entity = createEntity((i % 4 == 0) ? "Account" : 
                                          (i % 4 == 1) ? "Transaction" :
                                          (i % 4 == 2) ? "Customer" : "Vendor");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        qof_multi_entity_collection_add_entity(base, entity);
    }
    
    // Filter 1: Get only Account and Transaction
    auto filter1 = [](QofInstance* entity, gpointer) -> gboolean {
        return (g_strcmp0(entity->e_type, "Account") == 0 ||
                g_strcmp0(entity->e_type, "Transaction") == 0);
    };
    QofMultiEntityCollection* level1 = qof_multi_entity_collection_filter(base, filter1, nullptr);
    
    // Filter 2: From level1, get only Accounts
    auto filter2 = [](QofInstance* entity, gpointer) -> gboolean {
        return g_strcmp0(entity->e_type, "Account") == 0;
    };
    QofMultiEntityCollection* level2 = qof_multi_entity_collection_filter(level1, filter2, nullptr);
    
    EXPECT_EQ(100U, qof_multi_entity_collection_count(base));
    EXPECT_EQ(50U, qof_multi_entity_collection_count(level1));
    EXPECT_EQ(25U, qof_multi_entity_collection_count(level2));
    
    qof_multi_entity_collection_destroy(base);
    qof_multi_entity_collection_destroy(level1);
    qof_multi_entity_collection_destroy(level2);
}

TEST_F(MultiEntityEdgeCaseTest, ComplexMergeFilterChain)
{
    // Create 3 organizations with different entity types
    GncOrganization* org1 = gncOrganizationCreate(book);
    GncOrganization* org2 = gncOrganizationCreate(book);
    GncOrganization* org3 = gncOrganizationCreate(book);
    
    // Populate org1 with Accounts
    for (int i = 0; i < 10; i++)
    {
        QofInstance* entity = createEntity("Account");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        gncOrganizationAddEntity(org1, entity);
    }
    
    // Populate org2 with Transactions
    for (int i = 0; i < 15; i++)
    {
        QofInstance* entity = createEntity("Transaction");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        gncOrganizationAddEntity(org2, entity);
    }
    
    // Populate org3 with Customers
    for (int i = 0; i < 5; i++)
    {
        QofInstance* entity = createEntity("Customer");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        gncOrganizationAddEntity(org3, entity);
    }
    
    // Create collections from organizations
    QofMultiEntityCollection* coll1 = qof_multi_entity_collection_from_organization(org1);
    QofMultiEntityCollection* coll2 = qof_multi_entity_collection_from_organization(org2);
    QofMultiEntityCollection* coll3 = qof_multi_entity_collection_from_organization(org3);
    
    // Merge all
    QofMultiEntityCollection* temp = qof_multi_entity_collection_merge(coll1, coll2);
    QofMultiEntityCollection* merged = qof_multi_entity_collection_merge(temp, coll3);
    
    EXPECT_EQ(30U, qof_multi_entity_collection_count(merged));
    
    // Filter to get only Accounts and Transactions
    auto filter_no_customers = [](QofInstance* entity, gpointer) -> gboolean {
        return g_strcmp0(entity->e_type, "Customer") != 0;
    };
    
    QofMultiEntityCollection* filtered = 
        qof_multi_entity_collection_filter(merged, filter_no_customers, nullptr);
    
    EXPECT_EQ(25U, qof_multi_entity_collection_count(filtered));
    
    GList* types = qof_multi_entity_collection_get_types(filtered);
    EXPECT_EQ(2U, g_list_length(types));
    
    g_list_free(types);
    qof_multi_entity_collection_destroy(coll1);
    qof_multi_entity_collection_destroy(coll2);
    qof_multi_entity_collection_destroy(coll3);
    qof_multi_entity_collection_destroy(temp);
    qof_multi_entity_collection_destroy(merged);
    qof_multi_entity_collection_destroy(filtered);
    gncOrganizationDestroy(org1);
    gncOrganizationDestroy(org2);
    gncOrganizationDestroy(org3);
}

// ============================================================
// Memory and Resource Management Tests
// ============================================================

TEST_F(MultiEntityEdgeCaseTest, RepeatedCreateDestroy)
{
    for (int i = 0; i < 1000; i++)
    {
        QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
        
        QofInstance* entity = createEntity("Account");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        
        qof_multi_entity_collection_add_entity(multi_coll, entity);
        qof_multi_entity_collection_destroy(multi_coll);
    }
    
    // Should not crash or leak memory
}

TEST_F(MultiEntityEdgeCaseTest, RepeatedOrganizationCreateDestroy)
{
    for (int i = 0; i < 100; i++)
    {
        GncOrganization* org = gncOrganizationCreate(book);
        gncOrganizationSetName(org, "Temp Org");
        
        for (int j = 0; j < 10; j++)
        {
            QofInstance* entity = createEntity("Account");
            GncGUID guid;
            guid_replace(&guid);
            qof_instance_set_guid(entity, &guid);
            gncOrganizationAddEntity(org, entity);
        }
        
        gncOrganizationDestroy(org);
    }
    
    // Should not crash or leak memory
}

// ============================================================
// Boundary Value Tests
// ============================================================

TEST_F(MultiEntityEdgeCaseTest, MaxIntEntityCount)
{
    // Test with a very large but reasonable number
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    const int large_count = 50000;
    
    for (int i = 0; i < large_count; i++)
    {
        QofInstance* entity = createEntity("Account");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        qof_multi_entity_collection_add_entity(multi_coll, entity);
    }
    
    EXPECT_EQ(large_count, qof_multi_entity_collection_count(multi_coll));
    
    // Count via iteration
    int iteration_count = 0;
    auto counter = [](QofInstance*, gpointer user_data) {
        int* c = static_cast<int*>(user_data);
        (*c)++;
    };
    
    qof_multi_entity_collection_foreach(multi_coll, counter, &iteration_count);
    EXPECT_EQ(large_count, iteration_count);
    
    qof_multi_entity_collection_destroy(multi_coll);
}

TEST_F(MultiEntityEdgeCaseTest, EmptyOrganizationOperations)
{
    GncOrganization* org = gncOrganizationCreate(book);
    gncOrganizationSetName(org, "Empty Org");
    
    // Various operations on empty organization
    EXPECT_EQ(0U, gncOrganizationGetEntityCount(org));
    EXPECT_EQ(nullptr, gncOrganizationGetEntities(org));
    
    // Create collection from empty org
    QofMultiEntityCollection* coll1 = qof_multi_entity_collection_from_organization(org);
    EXPECT_EQ(0U, qof_multi_entity_collection_count(coll1));
    
    // Add organization entities (none) to collection
    QofMultiEntityCollection* coll2 = qof_multi_entity_collection_new();
    qof_multi_entity_collection_add_organization_entities(coll2, org);
    EXPECT_EQ(0U, qof_multi_entity_collection_count(coll2));
    
    // Filter by empty organization
    QofMultiEntityCollection* coll3 = qof_multi_entity_collection_new();
    QofMultiEntityCollection* filtered = 
        qof_multi_entity_collection_filter_by_organization(coll3, org);
    EXPECT_EQ(0U, qof_multi_entity_collection_count(filtered));
    
    qof_multi_entity_collection_destroy(coll1);
    qof_multi_entity_collection_destroy(coll2);
    qof_multi_entity_collection_destroy(coll3);
    qof_multi_entity_collection_destroy(filtered);
    gncOrganizationDestroy(org);
}

// ============================================================
// Data Consistency Tests
// ============================================================

TEST_F(MultiEntityEdgeCaseTest, CollectionSnapshotBehavior)
{
    GncOrganization* org = gncOrganizationCreate(book);
    
    QofInstance* entity1 = createEntity("Account");
    QofInstance* entity2 = createEntity("Transaction");
    
    GncGUID guid1, guid2;
    guid_replace(&guid1);
    guid_replace(&guid2);
    qof_instance_set_guid(entity1, &guid1);
    qof_instance_set_guid(entity2, &guid2);
    
    gncOrganizationAddEntity(org, entity1);
    
    // Create snapshot
    QofMultiEntityCollection* snapshot = qof_multi_entity_collection_from_organization(org);
    EXPECT_EQ(1U, qof_multi_entity_collection_count(snapshot));
    
    // Modify organization
    gncOrganizationAddEntity(org, entity2);
    EXPECT_EQ(2U, gncOrganizationGetEntityCount(org));
    
    // Snapshot should be unchanged
    EXPECT_EQ(1U, qof_multi_entity_collection_count(snapshot));
    EXPECT_TRUE(qof_multi_entity_collection_contains(snapshot, entity1));
    EXPECT_FALSE(qof_multi_entity_collection_contains(snapshot, entity2));
    
    qof_multi_entity_collection_destroy(snapshot);
    gncOrganizationDestroy(org);
}

TEST_F(MultiEntityEdgeCaseTest, TypeListConsistency)
{
    QofMultiEntityCollection* multi_coll = qof_multi_entity_collection_new();
    
    // Add entities of 3 types
    for (int i = 0; i < 5; i++)
    {
        QofInstance* entity = createEntity((i % 3 == 0) ? "Account" : 
                                          (i % 3 == 1) ? "Transaction" : "Customer");
        GncGUID guid;
        guid_replace(&guid);
        qof_instance_set_guid(entity, &guid);
        qof_multi_entity_collection_add_entity(multi_coll, entity);
    }
    
    // Get types multiple times - should be consistent
    GList* types1 = qof_multi_entity_collection_get_types(multi_coll);
    GList* types2 = qof_multi_entity_collection_get_types(multi_coll);
    GList* types3 = qof_multi_entity_collection_get_types(multi_coll);
    
    EXPECT_EQ(g_list_length(types1), g_list_length(types2));
    EXPECT_EQ(g_list_length(types2), g_list_length(types3));
    
    g_list_free(types1);
    g_list_free(types2);
    g_list_free(types3);
    
    qof_multi_entity_collection_destroy(multi_coll);
}
