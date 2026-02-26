#include "metric_impl/code_lines_count.hpp"
#include "file.hpp"
#include "function.hpp"
#include <gtest/gtest.h>
#include <iostream>

class CodeLinesCountTest : public ::testing::Test {
protected:
    static inline constexpr char const *test_files_dir = TEST_FILES_DIR;
    analyzer::metric::metric_impl::CodeLinesCountMetric m;

    struct TestCase {
        std::string filename;
        std::string func_name;
        int expected_lines;
    };

    void TestFile(const TestCase &tc) {
        // TODO: reduce duplication with other tests
        analyzer::file::File f(std::string(test_files_dir) + "/" + tc.filename);
        analyzer::function::FunctionExtractor e;
        auto allFunctions = e.Get(f);
        ASSERT_EQ(allFunctions.size(), 1);
        const auto &func = allFunctions.front();
        ASSERT_EQ(func.name, tc.func_name);
        EXPECT_EQ(m.Calculate(func).value, tc.expected_lines);
    }
};

TEST_F(CodeLinesCountTest, CommentsPyFile) { TestFile({"comments.py", "Func_comments", 3}); }
TEST_F(CodeLinesCountTest, ExceptionsPyFile) { TestFile({"exceptions.py", "Try_Exceptions", 7}); }
TEST_F(CodeLinesCountTest, IfPyFile) { TestFile({"if.py", "testIf", 3}); }
TEST_F(CodeLinesCountTest, LoopsPyFile) { TestFile({"loops.py", "TestLoops", 6}); }
TEST_F(CodeLinesCountTest, ManyLinesPyFile) { TestFile({"many_lines.py", "testmultiline", 11}); }
TEST_F(CodeLinesCountTest, ManyParametersPyFile) { TestFile({"many_parameters.py", "__test_multiparameters__", 1}); }
TEST_F(CodeLinesCountTest, MatchCasePyFile) { TestFile({"match_case.py", "test_Match_case", 7}); }
TEST_F(CodeLinesCountTest, NestedIfPyFile) { TestFile({"nested_if.py", "Testnestedif", 8}); }
TEST_F(CodeLinesCountTest, SimplePyFile) { TestFile({"simple.py", "test_simple", 5}); }
TEST_F(CodeLinesCountTest, TernaryPyFile) { TestFile({"ternary.py", "teSt_ternary", 1}); }