#include <gtest/gtest.h>

#include "database/sqlidentifier.h"

TEST(SqlIdentifierTest, DelimitsAPlainName) {
    EXPECT_EQ(quotedIdentifier("products"), "\"products\"");
}

TEST(SqlIdentifierTest, DoublesAnEmbeddedQuote) {
    EXPECT_EQ(quotedIdentifier("we\"ird"), "\"we\"\"ird\"");
}

TEST(SqlIdentifierTest, DoublesEveryEmbeddedQuote) {
    EXPECT_EQ(quotedIdentifier("\"a\"b\""), "\"\"\"a\"\"b\"\"\"");
}

TEST(SqlIdentifierTest, DelimitsANameHoldingSpaces) {
    EXPECT_EQ(quotedIdentifier("order items"), "\"order items\"");
}

TEST(SqlIdentifierTest, DelimitsAReservedWord) {
    EXPECT_EQ(quotedIdentifier("order"), "\"order\"");
}

// Documents the total contract: callers guard their own lookup rather than
// reading a sentinel back out of this.
TEST(SqlIdentifierTest, DelimitsAnEmptyNameWithoutGuarding) {
    EXPECT_EQ(quotedIdentifier(""), "\"\"");
}
