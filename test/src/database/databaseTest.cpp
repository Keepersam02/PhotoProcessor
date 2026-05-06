#include <gtest/gtest.h>
#include "database.h"

// ─────────────────────────────────────────────────────────────────────────────
//  Fixture
//  Every TEST_F gets a fresh in-memory database with the users table ready.
// ─────────────────────────────────────────────────────────────────────────────

class DatabaseTest : public ::testing::Test {
protected:
    Database db{":memory:"};

    void SetUp() override {
        db.execute(R"(
            CREATE TABLE users (
                id    INTEGER PRIMARY KEY AUTOINCREMENT,
                name  TEXT    NOT NULL,
                email TEXT    UNIQUE,
                age   INTEGER
            );
        )");
    }

    // Convenience: insert a user and return the new rowid
    int64_t insertUser(const std::string& name,
                       const std::string& email = "",
                       int age = 0) {
        auto stmt = db.prepare(
            "INSERT INTO users (name, email, age) VALUES (?, ?, ?);"
        );
        stmt.bindText(1, name);
        email.empty() ? stmt.bindNull(2) : stmt.bindText(2, email);
        age == 0      ? stmt.bindNull(3) : stmt.bindInt (3, age);
        stmt.step();
        return db.lastInsertRowId();
    }
};

// ─────────────────────────────────────────────────────────────────────────────
//  Construction
// ─────────────────────────────────────────────────────────────────────────────

TEST(DatabaseConstructionTest, OpensInMemoryDatabase) {
    EXPECT_NO_THROW(Database(":memory:"));
}

TEST(DatabaseConstructionTest, ThrowsOnInvalidPath) {
    EXPECT_THROW(Database("/no/such/directory/db.sqlite"), std::runtime_error);
}

TEST(DatabaseConstructionTest, MovedDatabaseIsStillUsable) {
    Database a(":memory:");
    a.execute("CREATE TABLE t (x INTEGER);");

    Database b = std::move(a);
    EXPECT_NO_THROW(b.execute("INSERT INTO t VALUES (1);"));
    EXPECT_EQ(b.queryAll("SELECT * FROM t;").size(), 1u);
}

// ─────────────────────────────────────────────────────────────────────────────
//  DDL / execute
// ─────────────────────────────────────────────────────────────────────────────

TEST_F(DatabaseTest, TableExistsAfterCreate) {
    EXPECT_TRUE(db.tableExists("users"));
}

TEST_F(DatabaseTest, TableExistsReturnsFalseForMissingTable) {
    EXPECT_FALSE(db.tableExists("nonexistent_table"));
}

TEST_F(DatabaseTest, ExecuteThrowsOnInvalidSQL) {
    EXPECT_THROW(db.execute("THIS IS NOT VALID SQL;"), std::runtime_error);
}

TEST_F(DatabaseTest, CanDropAndRecreateTable) {
    EXPECT_NO_THROW(db.execute("DROP TABLE users;"));
    EXPECT_FALSE(db.tableExists("users"));
    EXPECT_NO_THROW(db.execute("CREATE TABLE users (id INTEGER PRIMARY KEY);"));
    EXPECT_TRUE(db.tableExists("users"));
}

// ─────────────────────────────────────────────────────────────────────────────
//  Insert / lastInsertRowId / changedRows
// ─────────────────────────────────────────────────────────────────────────────

TEST_F(DatabaseTest, InsertReturnsCorrectRowId) {
    int64_t id = insertUser("Alice", "alice@test.com", 30);
    EXPECT_EQ(id, 1);
}

TEST_F(DatabaseTest, SequentialInsertsIncrementRowId) {
    EXPECT_EQ(insertUser("Alice"), 1);
    EXPECT_EQ(insertUser("Bob"),   2);
    EXPECT_EQ(insertUser("Carol"), 3);
}

TEST_F(DatabaseTest, ChangedRowsReflectsUpdateCount) {
    insertUser("Alice");
    insertUser("Bob");
    db.execute("UPDATE users SET age = 99;");
    EXPECT_EQ(db.changedRows(), 2);
}

TEST_F(DatabaseTest, ChangedRowsIsZeroWhenNothingMatches) {
    insertUser("Alice");
    db.execute("UPDATE users SET age = 1 WHERE name = 'Nobody';");
    EXPECT_EQ(db.changedRows(), 0);
}

TEST_F(DatabaseTest, DuplicateUniqueKeyThrows) {
    insertUser("Alice", "alice@test.com");
    EXPECT_THROW(insertUser("Alice2", "alice@test.com"), std::runtime_error);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Prepared Statement — bindings
// ─────────────────────────────────────────────────────────────────────────────

TEST_F(DatabaseTest, BindTextRoundTrips) {
    insertUser("Alice");
    auto rows = db.queryAll("SELECT name FROM users;");
    ASSERT_EQ(rows.size(), 1u);
    EXPECT_EQ(rows[0].at("name"), "Alice");
}

TEST_F(DatabaseTest, BindIntRoundTrips) {
    insertUser("Alice", "", 42);
    auto rows = db.queryAll("SELECT age FROM users;");
    ASSERT_EQ(rows.size(), 1u);
    EXPECT_EQ(rows[0].at("age"), "42");
}

TEST_F(DatabaseTest, BindInt64RoundTrips) {
    db.execute("CREATE TABLE big (val INTEGER);");
    int64_t big = 9'000'000'000LL;
    auto ins = db.prepare("INSERT INTO big VALUES (?);");
    ins.bindInt64(1, big);
    ins.step();

    auto sel = db.prepare("SELECT val FROM big;");
    ASSERT_TRUE(sel.step());
    EXPECT_EQ(sel.colInt64(0), big);
}

TEST_F(DatabaseTest, BindDoubleRoundTrips) {
    db.execute("CREATE TABLE floats (val REAL);");
    auto ins = db.prepare("INSERT INTO floats VALUES (?);");
    ins.bindDouble(1, 3.14159);
    ins.step();

    auto sel = db.prepare("SELECT val FROM floats;");
    ASSERT_TRUE(sel.step());
    EXPECT_DOUBLE_EQ(sel.colDouble(0), 3.14159);
}

TEST_F(DatabaseTest, BindNullStoredAndReadAsEmptyString) {
    auto stmt = db.prepare("INSERT INTO users (name, age) VALUES (?, ?);");
    stmt.bindText(1, "NullAge");
    stmt.bindNull(2);
    stmt.step();

    auto sel = db.prepare("SELECT age FROM users;");
    ASSERT_TRUE(sel.step());
    EXPECT_EQ(sel.colText(0), "");
}

TEST_F(DatabaseTest, StatementReuseWithReset) {
    auto stmt = db.prepare("INSERT INTO users (name) VALUES (?);");
    for (auto& name : {"A", "B", "C"}) {
        stmt.bindText(1, name);
        stmt.step();
        stmt.reset();
    }
    EXPECT_EQ(db.queryAll("SELECT * FROM users;").size(), 3u);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Query — queryAll
// ─────────────────────────────────────────────────────────────────────────────

TEST_F(DatabaseTest, QueryAllOnEmptyTableReturnsEmptyVector) {
    auto rows = db.queryAll("SELECT * FROM users;");
    EXPECT_TRUE(rows.empty());
}

TEST_F(DatabaseTest, QueryAllReturnsCorrectRowCount) {
    insertUser("A");
    insertUser("B");
    insertUser("C");
    auto rows = db.queryAll("SELECT * FROM users;");
    EXPECT_EQ(rows.size(), 3u);
}

TEST_F(DatabaseTest, QueryAllRowContainsCorrectColumnValues) {
    insertUser("Alice", "alice@test.com", 30);
    auto rows = db.queryAll("SELECT name, email, age FROM users;");
    ASSERT_EQ(rows.size(), 1u);
    EXPECT_EQ(rows[0].at("name"),  "Alice");
    EXPECT_EQ(rows[0].at("email"), "alice@test.com");
    EXPECT_EQ(rows[0].at("age"),   "30");
}

TEST_F(DatabaseTest, QueryAllRespectsWhereClause) {
    insertUser("Alice", "", 30);
    insertUser("Bob",   "", 17);
    insertUser("Carol", "", 25);
    auto rows = db.queryAll("SELECT name FROM users WHERE age >= 25;");
    EXPECT_EQ(rows.size(), 2u);
}

TEST_F(DatabaseTest, QueryAllRespectsOrderBy) {
    insertUser("Charlie");
    insertUser("Alice");
    insertUser("Bob");
    auto rows = db.queryAll("SELECT name FROM users ORDER BY name ASC;");
    ASSERT_EQ(rows.size(), 3u);
    EXPECT_EQ(rows[0].at("name"), "Alice");
    EXPECT_EQ(rows[1].at("name"), "Bob");
    EXPECT_EQ(rows[2].at("name"), "Charlie");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Query — callback style
// ─────────────────────────────────────────────────────────────────────────────

TEST_F(DatabaseTest, QueryCallbackInvokedOncePerRow) {
    insertUser("A");
    insertUser("B");
    insertUser("C");

    int count = 0;
    db.query("SELECT * FROM users;", [&](const Row&) { ++count; });
    EXPECT_EQ(count, 3);
}

TEST_F(DatabaseTest, QueryCallbackNotInvokedOnEmptyResult) {
    bool called = false;
    db.query("SELECT * FROM users;", [&](const Row&) { called = true; });
    EXPECT_FALSE(called);
}

TEST_F(DatabaseTest, QueryCallbackReceivesCorrectData) {
    insertUser("Alice", "alice@test.com", 30);

    db.query("SELECT name, email, age FROM users;", [](const Row& row) {
        EXPECT_EQ(row.at("name"),  "Alice");
        EXPECT_EQ(row.at("email"), "alice@test.com");
        EXPECT_EQ(row.at("age"),   "30");
    });
}

// ─────────────────────────────────────────────────────────────────────────────
//  Transactions
// ─────────────────────────────────────────────────────────────────────────────

TEST_F(DatabaseTest, CommittedTransactionPersistsRows) {
    db.beginTransaction();
    insertUser("Alice");
    insertUser("Bob");
    db.commit();

    EXPECT_EQ(db.queryAll("SELECT * FROM users;").size(), 2u);
}

TEST_F(DatabaseTest, RolledBackTransactionLeavesTableEmpty) {
    db.beginTransaction();
    insertUser("Alice");
    insertUser("Bob");
    db.rollback();

    EXPECT_TRUE(db.queryAll("SELECT * FROM users;").empty());
}

TEST_F(DatabaseTest, BulkInsertWithTransactionIsConsistent) {
    db.beginTransaction();
    auto stmt = db.prepare("INSERT INTO users (name, age) VALUES (?, ?);");
    for (int i = 0; i < 100; ++i) {
        stmt.bindText(1, "User" + std::to_string(i));
        stmt.bindInt (2, i);
        stmt.step();
        stmt.reset();
    }
    db.commit();

    EXPECT_EQ(db.queryAll("SELECT * FROM users;").size(), 100u);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Column metadata
// ─────────────────────────────────────────────────────────────────────────────

TEST_F(DatabaseTest, ColCountMatchesSelectedColumns) {
    insertUser("Alice");
    auto stmt = db.prepare("SELECT id, name, age FROM users;");
    EXPECT_EQ(stmt.colCount(), 3);
}

TEST_F(DatabaseTest, ColNameMatchesSelectAlias) {
    insertUser("Alice");
    auto stmt = db.prepare("SELECT name AS user_name FROM users;");
    EXPECT_EQ(stmt.colName(0), "user_name");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Edge cases
// ─────────────────────────────────────────────────────────────────────────────

TEST_F(DatabaseTest, EmptyStringNameIsAllowed) {
    EXPECT_NO_THROW(insertUser(""));
    auto rows = db.queryAll("SELECT name FROM users;");
    EXPECT_EQ(rows[0].at("name"), "");
}

TEST_F(DatabaseTest, SpecialCharactersInTextAreHandledSafely) {
    std::string tricky = "O'Brien; DROP TABLE users;--";
    insertUser(tricky);
    auto rows = db.queryAll("SELECT name FROM users;");
    ASSERT_EQ(rows.size(), 1u);
    EXPECT_EQ(rows[0].at("name"), tricky);
    // Confirm the table was NOT dropped
    EXPECT_TRUE(db.tableExists("users"));
}

TEST_F(DatabaseTest, UnicodeTextRoundTrips) {
    insertUser("日本語テスト");
    auto rows = db.queryAll("SELECT name FROM users;");
    EXPECT_EQ(rows[0].at("name"), "日本語テスト");
}