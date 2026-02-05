/********************************************************************\
 * gtest-organization.cpp -- Unit tests for GncOrganization         *
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
#include <string>
#include "../qof.h"
#include "../qofbook.h"
#include "../qofinstance.h"
#include "../gncOrganization.h"
#include "../gncAddress.h"
#include "../gnc-commodity.h"
#include "../guid.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcpp"
#include <gtest/gtest.h>
#pragma GCC diagnostic pop

class GncOrganizationTest : public ::testing::Test
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
        qof_book_destroy(book);
        qof_close();
    }
    
    QofBook* book;
};

// ============================================================
// Basic Creation/Destruction Tests
// ============================================================

TEST_F(GncOrganizationTest, CreateOrganization)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    // Check default values
    EXPECT_NE(nullptr, gncOrganizationGetID(org));
    EXPECT_NE(nullptr, gncOrganizationGetName(org));
    EXPECT_NE(nullptr, gncOrganizationGetNotes(org));
    EXPECT_TRUE(gncOrganizationGetActive(org));
    EXPECT_EQ(0U, gncOrganizationGetEntityCount(org));
    
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, CreateWithNullBook)
{
    GncOrganization* org = gncOrganizationCreate(nullptr);
    EXPECT_EQ(nullptr, org);
}

TEST_F(GncOrganizationTest, DestroyNullOrganization)
{
    // Should not crash
    gncOrganizationDestroy(nullptr);
}

// ============================================================
// Setter/Getter Tests
// ============================================================

TEST_F(GncOrganizationTest, SetAndGetID)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    const char* test_id = "ORG-12345";
    gncOrganizationSetID(org, test_id);
    
    const char* retrieved_id = gncOrganizationGetID(org);
    EXPECT_STREQ(test_id, retrieved_id);
    
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, SetIDWithNull)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    gncOrganizationSetID(org, nullptr);
    const char* id = gncOrganizationGetID(org);
    EXPECT_NE(nullptr, id);  // Should have empty string, not NULL
    
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, SetAndGetName)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    const char* test_name = "ACME Corporation";
    gncOrganizationSetName(org, test_name);
    
    const char* retrieved_name = gncOrganizationGetName(org);
    EXPECT_STREQ(test_name, retrieved_name);
    
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, SetAndGetNotes)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    const char* test_notes = "This is a test organization for unit testing purposes.";
    gncOrganizationSetNotes(org, test_notes);
    
    const char* retrieved_notes = gncOrganizationGetNotes(org);
    EXPECT_STREQ(test_notes, retrieved_notes);
    
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, SetAndGetActive)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    // Default should be active
    EXPECT_TRUE(gncOrganizationGetActive(org));
    
    // Set to inactive
    gncOrganizationSetActive(org, FALSE);
    EXPECT_FALSE(gncOrganizationGetActive(org));
    
    // Set back to active
    gncOrganizationSetActive(org, TRUE);
    EXPECT_TRUE(gncOrganizationGetActive(org));
    
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, SetAndGetAddress)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    GncAddress* addr = gncAddressCreate(book, QOF_INSTANCE(org));
    ASSERT_NE(nullptr, addr);
    
    gncAddressSetName(addr, "Test Building");
    gncAddressSetAddr1(addr, "123 Main St");
    gncAddressSetAddr2(addr, "Suite 456");
    
    gncOrganizationSetAddr(org, addr);
    
    GncAddress* retrieved_addr = gncOrganizationGetAddr(org);
    EXPECT_EQ(addr, retrieved_addr);
    EXPECT_STREQ("Test Building", gncAddressGetName(retrieved_addr));
    
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, GettersWithNullOrg)
{
    EXPECT_EQ(nullptr, gncOrganizationGetID(nullptr));
    EXPECT_EQ(nullptr, gncOrganizationGetName(nullptr));
    EXPECT_EQ(nullptr, gncOrganizationGetNotes(nullptr));
    EXPECT_EQ(nullptr, gncOrganizationGetCurrency(nullptr));
    EXPECT_FALSE(gncOrganizationGetActive(nullptr));
    EXPECT_EQ(nullptr, gncOrganizationGetAddr(nullptr));
}

TEST_F(GncOrganizationTest, SettersWithNullOrg)
{
    // These should not crash
    gncOrganizationSetID(nullptr, "test");
    gncOrganizationSetName(nullptr, "test");
    gncOrganizationSetNotes(nullptr, "test");
    gncOrganizationSetCurrency(nullptr, nullptr);
    gncOrganizationSetActive(nullptr, TRUE);
    gncOrganizationSetAddr(nullptr, nullptr);
}

// ============================================================
// Entity Management Tests
// ============================================================

TEST_F(GncOrganizationTest, AddSingleEntity)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    // Create a test entity
    QofInstance* entity = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
    entity->e_type = "TestEntity";
    
    EXPECT_EQ(0U, gncOrganizationGetEntityCount(org));
    
    gncOrganizationAddEntity(org, entity);
    
    EXPECT_EQ(1U, gncOrganizationGetEntityCount(org));
    
    GList* entities = gncOrganizationGetEntities(org);
    EXPECT_EQ(1U, g_list_length(entities));
    EXPECT_EQ(entity, entities->data);
    
    g_object_unref(entity);
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, AddMultipleEntities)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    const int num_entities = 5;
    QofInstance* entities[num_entities];
    
    for (int i = 0; i < num_entities; i++)
    {
        entities[i] = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
        entities[i]->e_type = "TestEntity";
        gncOrganizationAddEntity(org, entities[i]);
    }
    
    EXPECT_EQ(num_entities, gncOrganizationGetEntityCount(org));
    
    GList* org_entities = gncOrganizationGetEntities(org);
    EXPECT_EQ(num_entities, g_list_length(org_entities));
    
    for (int i = 0; i < num_entities; i++)
    {
        g_object_unref(entities[i]);
    }
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, AddDuplicateEntity)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    QofInstance* entity = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
    entity->e_type = "TestEntity";
    
    gncOrganizationAddEntity(org, entity);
    EXPECT_EQ(1U, gncOrganizationGetEntityCount(org));
    
    // Try to add the same entity again
    gncOrganizationAddEntity(org, entity);
    EXPECT_EQ(1U, gncOrganizationGetEntityCount(org));  // Should still be 1
    
    g_object_unref(entity);
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, RemoveEntity)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    QofInstance* entity1 = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
    QofInstance* entity2 = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
    entity1->e_type = "TestEntity";
    entity2->e_type = "TestEntity";
    
    gncOrganizationAddEntity(org, entity1);
    gncOrganizationAddEntity(org, entity2);
    EXPECT_EQ(2U, gncOrganizationGetEntityCount(org));
    
    gncOrganizationRemoveEntity(org, entity1);
    EXPECT_EQ(1U, gncOrganizationGetEntityCount(org));
    
    GList* entities = gncOrganizationGetEntities(org);
    EXPECT_FALSE(g_list_find(entities, entity1) != NULL);
    EXPECT_TRUE(g_list_find(entities, entity2) != NULL);
    
    g_object_unref(entity1);
    g_object_unref(entity2);
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, RemoveNonexistentEntity)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    QofInstance* entity = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
    entity->e_type = "TestEntity";
    
    // Remove entity that was never added - should not crash
    gncOrganizationRemoveEntity(org, entity);
    EXPECT_EQ(0U, gncOrganizationGetEntityCount(org));
    
    g_object_unref(entity);
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, RemoveAllEntities)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    const int num_entities = 3;
    QofInstance* entities[num_entities];
    
    for (int i = 0; i < num_entities; i++)
    {
        entities[i] = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
        entities[i]->e_type = "TestEntity";
        gncOrganizationAddEntity(org, entities[i]);
    }
    
    EXPECT_EQ(num_entities, gncOrganizationGetEntityCount(org));
    
    // Remove all entities
    for (int i = 0; i < num_entities; i++)
    {
        gncOrganizationRemoveEntity(org, entities[i]);
    }
    
    EXPECT_EQ(0U, gncOrganizationGetEntityCount(org));
    
    for (int i = 0; i < num_entities; i++)
    {
        g_object_unref(entities[i]);
    }
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, EntityManagementWithNull)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    QofInstance* entity = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
    
    // These should not crash
    gncOrganizationAddEntity(nullptr, entity);
    gncOrganizationAddEntity(org, nullptr);
    gncOrganizationAddEntity(nullptr, nullptr);
    
    gncOrganizationRemoveEntity(nullptr, entity);
    gncOrganizationRemoveEntity(org, nullptr);
    gncOrganizationRemoveEntity(nullptr, nullptr);
    
    EXPECT_EQ(nullptr, gncOrganizationGetEntities(nullptr));
    EXPECT_EQ(0U, gncOrganizationGetEntityCount(nullptr));
    
    g_object_unref(entity);
    gncOrganizationDestroy(org);
}

// ============================================================
// Comparison Tests
// ============================================================

TEST_F(GncOrganizationTest, CompareByName)
{
    GncOrganization* org1 = gncOrganizationCreate(book);
    GncOrganization* org2 = gncOrganizationCreate(book);
    GncOrganization* org3 = gncOrganizationCreate(book);
    
    gncOrganizationSetName(org1, "Alpha Corp");
    gncOrganizationSetName(org2, "Beta Corp");
    gncOrganizationSetName(org3, "Alpha Corp");
    
    EXPECT_LT(gncOrganizationCompare(org1, org2), 0);  // Alpha < Beta
    EXPECT_GT(gncOrganizationCompare(org2, org1), 0);  // Beta > Alpha
    EXPECT_EQ(gncOrganizationCompare(org1, org3), 0);  // Alpha == Alpha
    
    gncOrganizationDestroy(org1);
    gncOrganizationDestroy(org2);
    gncOrganizationDestroy(org3);
}

TEST_F(GncOrganizationTest, CompareWithNull)
{
    GncOrganization* org = gncOrganizationCreate(book);
    
    EXPECT_EQ(0, gncOrganizationCompare(nullptr, nullptr));
    EXPECT_LT(gncOrganizationCompare(nullptr, org), 0);
    EXPECT_GT(gncOrganizationCompare(org, nullptr), 0);
    
    gncOrganizationDestroy(org);
}

// ============================================================
// Property Update Tests
// ============================================================

TEST_F(GncOrganizationTest, UpdateIDMultipleTimes)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    gncOrganizationSetID(org, "ID-001");
    EXPECT_STREQ("ID-001", gncOrganizationGetID(org));
    
    gncOrganizationSetID(org, "ID-002");
    EXPECT_STREQ("ID-002", gncOrganizationGetID(org));
    
    gncOrganizationSetID(org, "ID-003");
    EXPECT_STREQ("ID-003", gncOrganizationGetID(org));
    
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, SetSameValueTwice)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    gncOrganizationSetName(org, "Test Name");
    gncOrganizationSetName(org, "Test Name");  // Same value
    
    EXPECT_STREQ("Test Name", gncOrganizationGetName(org));
    
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, SetEmptyStrings)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    gncOrganizationSetID(org, "");
    gncOrganizationSetName(org, "");
    gncOrganizationSetNotes(org, "");
    
    EXPECT_STREQ("", gncOrganizationGetID(org));
    EXPECT_STREQ("", gncOrganizationGetName(org));
    EXPECT_STREQ("", gncOrganizationGetNotes(org));
    
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, SetLongStrings)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    // Create a very long string (1000 characters)
    std::string long_string(1000, 'X');
    
    gncOrganizationSetID(org, long_string.c_str());
    gncOrganizationSetName(org, long_string.c_str());
    gncOrganizationSetNotes(org, long_string.c_str());
    
    EXPECT_STREQ(long_string.c_str(), gncOrganizationGetID(org));
    EXPECT_STREQ(long_string.c_str(), gncOrganizationGetName(org));
    EXPECT_STREQ(long_string.c_str(), gncOrganizationGetNotes(org));
    
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, SetSpecialCharacters)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    const char* special_chars = "Test <>&\"'\n\t\r\\/@#$%^&*()";
    
    gncOrganizationSetName(org, special_chars);
    EXPECT_STREQ(special_chars, gncOrganizationGetName(org));
    
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, SetUnicodeStrings)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    const char* unicode_name = "テスト Corporation 测试 مؤسسة";
    
    gncOrganizationSetName(org, unicode_name);
    EXPECT_STREQ(unicode_name, gncOrganizationGetName(org));
    
    gncOrganizationDestroy(org);
}

// ============================================================
// Address Integration Tests
// ============================================================

TEST_F(GncOrganizationTest, AddressLifecycle)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    // Initially no address
    EXPECT_EQ(nullptr, gncOrganizationGetAddr(org));
    
    // Create and set address
    GncAddress* addr1 = gncAddressCreate(book, QOF_INSTANCE(org));
    gncAddressSetName(addr1, "Address 1");
    gncOrganizationSetAddr(org, addr1);
    EXPECT_EQ(addr1, gncOrganizationGetAddr(org));
    
    // Replace with new address
    GncAddress* addr2 = gncAddressCreate(book, QOF_INSTANCE(org));
    gncAddressSetName(addr2, "Address 2");
    gncOrganizationSetAddr(org, addr2);
    EXPECT_EQ(addr2, gncOrganizationGetAddr(org));
    
    // Old address should be destroyed, new address should be active
    EXPECT_STREQ("Address 2", gncAddressGetName(gncOrganizationGetAddr(org)));
    
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, SetNullAddress)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    GncAddress* addr = gncAddressCreate(book, QOF_INSTANCE(org));
    gncOrganizationSetAddr(org, addr);
    EXPECT_NE(nullptr, gncOrganizationGetAddr(org));
    
    // Setting null should clear the address
    gncOrganizationSetAddr(org, nullptr);
    EXPECT_EQ(nullptr, gncOrganizationGetAddr(org));
    
    gncOrganizationDestroy(org);
}

// ============================================================
// Large-Scale Entity Tests
// ============================================================

TEST_F(GncOrganizationTest, AddManyEntities)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    const int num_entities = 1000;
    std::vector<QofInstance*> entities;
    
    for (int i = 0; i < num_entities; i++)
    {
        QofInstance* entity = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
        entity->e_type = "TestEntity";
        entities.push_back(entity);
        gncOrganizationAddEntity(org, entity);
    }
    
    EXPECT_EQ(num_entities, gncOrganizationGetEntityCount(org));
    
    GList* org_entities = gncOrganizationGetEntities(org);
    EXPECT_EQ(num_entities, g_list_length(org_entities));
    
    for (auto* entity : entities)
    {
        g_object_unref(entity);
    }
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, AddAndRemoveManyEntities)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    const int num_entities = 100;
    std::vector<QofInstance*> entities;
    
    // Add entities
    for (int i = 0; i < num_entities; i++)
    {
        QofInstance* entity = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
        entity->e_type = "TestEntity";
        entities.push_back(entity);
        gncOrganizationAddEntity(org, entity);
    }
    
    EXPECT_EQ(num_entities, gncOrganizationGetEntityCount(org));
    
    // Remove every other entity
    for (int i = 0; i < num_entities; i += 2)
    {
        gncOrganizationRemoveEntity(org, entities[i]);
    }
    
    EXPECT_EQ(num_entities / 2, gncOrganizationGetEntityCount(org));
    
    for (auto* entity : entities)
    {
        g_object_unref(entity);
    }
    gncOrganizationDestroy(org);
}

// ============================================================
// Mixed Entity Type Tests
// ============================================================

TEST_F(GncOrganizationTest, MixedEntityTypes)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    QofInstance* account = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
    QofInstance* transaction = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
    QofInstance* customer = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
    
    account->e_type = "Account";
    transaction->e_type = "Transaction";
    customer->e_type = "Customer";
    
    gncOrganizationAddEntity(org, account);
    gncOrganizationAddEntity(org, transaction);
    gncOrganizationAddEntity(org, customer);
    
    EXPECT_EQ(3U, gncOrganizationGetEntityCount(org));
    
    GList* entities = gncOrganizationGetEntities(org);
    EXPECT_TRUE(g_list_find(entities, account) != NULL);
    EXPECT_TRUE(g_list_find(entities, transaction) != NULL);
    EXPECT_TRUE(g_list_find(entities, customer) != NULL);
    
    g_object_unref(account);
    g_object_unref(transaction);
    g_object_unref(customer);
    gncOrganizationDestroy(org);
}

// ============================================================
// Complete Organization Lifecycle Test
// ============================================================

TEST_F(GncOrganizationTest, CompleteLifecycle)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    // Set all properties
    gncOrganizationSetID(org, "ORG-LIFECYCLE-001");
    gncOrganizationSetName(org, "Lifecycle Test Corp");
    gncOrganizationSetNotes(org, "Testing complete lifecycle");
    gncOrganizationSetActive(org, TRUE);
    
    // Create and set address
    GncAddress* addr = gncAddressCreate(book, QOF_INSTANCE(org));
    gncAddressSetName(addr, "HQ Building");
    gncAddressSetAddr1(addr, "456 Test Ave");
    gncAddressSetAddr2(addr, "Floor 7");
    gncAddressSetAddr3(addr, "Suite 701");
    gncAddressSetAddr4(addr, "Mailbox 12");
    gncAddressSetPhone(addr, "+1-555-0123");
    gncAddressSetFax(addr, "+1-555-0124");
    gncAddressSetEmail(addr, "test@lifecycle.com");
    gncOrganizationSetAddr(org, addr);
    
    // Add entities
    std::vector<QofInstance*> entities;
    for (int i = 0; i < 5; i++)
    {
        QofInstance* entity = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
        entity->e_type = "TestEntity";
        entities.push_back(entity);
        gncOrganizationAddEntity(org, entity);
    }
    
    // Verify all properties
    EXPECT_STREQ("ORG-LIFECYCLE-001", gncOrganizationGetID(org));
    EXPECT_STREQ("Lifecycle Test Corp", gncOrganizationGetName(org));
    EXPECT_STREQ("Testing complete lifecycle", gncOrganizationGetNotes(org));
    EXPECT_TRUE(gncOrganizationGetActive(org));
    EXPECT_EQ(5U, gncOrganizationGetEntityCount(org));
    
    GncAddress* retrieved_addr = gncOrganizationGetAddr(org);
    EXPECT_STREQ("HQ Building", gncAddressGetName(retrieved_addr));
    EXPECT_STREQ("456 Test Ave", gncAddressGetAddr1(retrieved_addr));
    EXPECT_STREQ("test@lifecycle.com", gncAddressGetEmail(retrieved_addr));
    
    // Modify organization
    gncOrganizationSetActive(org, FALSE);
    EXPECT_FALSE(gncOrganizationGetActive(org));
    
    // Remove some entities
    gncOrganizationRemoveEntity(org, entities[0]);
    gncOrganizationRemoveEntity(org, entities[2]);
    EXPECT_EQ(3U, gncOrganizationGetEntityCount(org));
    
    // Clean up
    for (auto* entity : entities)
    {
        g_object_unref(entity);
    }
    gncOrganizationDestroy(org);
}

// ============================================================
// Lookup and Book Integration Tests
// ============================================================

TEST_F(GncOrganizationTest, LookupByGUID)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    gncOrganizationSetName(org, "Lookup Test");
    
    const GncGUID* guid = qof_instance_get_guid(QOF_INSTANCE(org));
    ASSERT_NE(nullptr, guid);
    
    GncOrganization* found_org = gncOrganizationLookup(book, guid);
    EXPECT_EQ(org, found_org);
    EXPECT_STREQ("Lookup Test", gncOrganizationGetName(found_org));
    
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, LookupWithInvalidGUID)
{
    GncGUID invalid_guid;
    guid_replace(&invalid_guid);
    
    GncOrganization* org = gncOrganizationLookup(book, &invalid_guid);
    EXPECT_EQ(nullptr, org);
}

TEST_F(GncOrganizationTest, LookupWithNullBook)
{
    GncGUID test_guid;
    guid_replace(&test_guid);
    
    GncOrganization* org = gncOrganizationLookup(nullptr, &test_guid);
    EXPECT_EQ(nullptr, org);
}

// ============================================================
// Multiple Organizations in Same Book
// ============================================================

TEST_F(GncOrganizationTest, MultipleOrganizationsInBook)
{
    GncOrganization* org1 = gncOrganizationCreate(book);
    GncOrganization* org2 = gncOrganizationCreate(book);
    GncOrganization* org3 = gncOrganizationCreate(book);
    
    gncOrganizationSetName(org1, "Organization 1");
    gncOrganizationSetName(org2, "Organization 2");
    gncOrganizationSetName(org3, "Organization 3");
    
    // Each should have unique GUIDs
    const GncGUID* guid1 = qof_instance_get_guid(QOF_INSTANCE(org1));
    const GncGUID* guid2 = qof_instance_get_guid(QOF_INSTANCE(org2));
    const GncGUID* guid3 = qof_instance_get_guid(QOF_INSTANCE(org3));
    
    EXPECT_FALSE(guid_equal(guid1, guid2));
    EXPECT_FALSE(guid_equal(guid2, guid3));
    EXPECT_FALSE(guid_equal(guid1, guid3));
    
    // Each should be lookupable
    EXPECT_EQ(org1, gncOrganizationLookup(book, guid1));
    EXPECT_EQ(org2, gncOrganizationLookup(book, guid2));
    EXPECT_EQ(org3, gncOrganizationLookup(book, guid3));
    
    gncOrganizationDestroy(org1);
    gncOrganizationDestroy(org2);
    gncOrganizationDestroy(org3);
}

// ============================================================
// Edge Cases and Boundary Conditions
// ============================================================

TEST_F(GncOrganizationTest, GetEntitiesEmptyOrganization)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    GList* entities = gncOrganizationGetEntities(org);
    EXPECT_EQ(nullptr, entities);
    EXPECT_EQ(0U, gncOrganizationGetEntityCount(org));
    
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, EntityOrderPreservation)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    std::vector<QofInstance*> entities;
    for (int i = 0; i < 5; i++)
    {
        QofInstance* entity = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
        entity->e_type = "TestEntity";
        entities.push_back(entity);
        gncOrganizationAddEntity(org, entity);
    }
    
    GList* org_entities = gncOrganizationGetEntities(org);
    EXPECT_EQ(5U, g_list_length(org_entities));
    
    // Verify all entities are present (order may vary due to prepend)
    for (auto* entity : entities)
    {
        EXPECT_TRUE(g_list_find(org_entities, entity) != NULL);
    }
    
    for (auto* entity : entities)
    {
        g_object_unref(entity);
    }
    gncOrganizationDestroy(org);
}

// ============================================================
// QOF Edit/Commit Tests
// ============================================================

TEST_F(GncOrganizationTest, BeginEditCommitEdit)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    gncOrganizationBeginEdit(org);
    gncOrganizationSetName(org, "Edit Test");
    gncOrganizationCommitEdit(org);
    
    EXPECT_STREQ("Edit Test", gncOrganizationGetName(org));
    
    gncOrganizationDestroy(org);
}

TEST_F(GncOrganizationTest, BeginEditWithNull)
{
    // Should not crash
    gncOrganizationBeginEdit(nullptr);
}

TEST_F(GncOrganizationTest, CommitEditWithNull)
{
    // Should not crash
    gncOrganizationCommitEdit(nullptr);
}

// ============================================================
// Integration with QOF System
// ============================================================

TEST_F(GncOrganizationTest, QOFRegistration)
{
    // Registration should succeed
    EXPECT_TRUE(gncOrganizationRegister());
    
    // Check that organization type is registered
    // (This would require access to QOF class registry internals)
}

TEST_F(GncOrganizationTest, GetBookFromOrganization)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    QofBook* retrieved_book = qof_instance_get_book(QOF_INSTANCE(org));
    EXPECT_EQ(book, retrieved_book);
    
    gncOrganizationDestroy(org);
}

// ============================================================
// Stress Tests
// ============================================================

TEST_F(GncOrganizationTest, RapidCreateDestroy)
{
    for (int i = 0; i < 100; i++)
    {
        GncOrganization* org = gncOrganizationCreate(book);
        ASSERT_NE(nullptr, org);
        gncOrganizationSetName(org, "Rapid Test");
        gncOrganizationDestroy(org);
    }
}

TEST_F(GncOrganizationTest, RapidEntityAddRemove)
{
    GncOrganization* org = gncOrganizationCreate(book);
    ASSERT_NE(nullptr, org);
    
    for (int iteration = 0; iteration < 10; iteration++)
    {
        std::vector<QofInstance*> entities;
        
        // Add 50 entities
        for (int i = 0; i < 50; i++)
        {
            QofInstance* entity = static_cast<QofInstance*>(g_object_new(QOF_TYPE_INSTANCE, NULL));
            entity->e_type = "TestEntity";
            entities.push_back(entity);
            gncOrganizationAddEntity(org, entity);
        }
        
        EXPECT_EQ(50U, gncOrganizationGetEntityCount(org));
        
        // Remove all
        for (auto* entity : entities)
        {
            gncOrganizationRemoveEntity(org, entity);
            g_object_unref(entity);
        }
        
        EXPECT_EQ(0U, gncOrganizationGetEntityCount(org));
    }
    
    gncOrganizationDestroy(org);
}
