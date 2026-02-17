#include "metric_impl/cyclomatic_complexity.hpp"
#include "file.hpp"
#include "function.hpp"
#include <gtest/gtest.h>

namespace analyzer::metric::metric_impl {

class CyclomaticComplexityTest : public ::testing::Test {
protected:
    static inline constexpr char const *test_files_dir = TEST_FILES_DIR;
    analyzer::metric::metric_impl::CyclomaticComplexityMetric m;

    struct TestCase {
        std::string filename;
        std::string func_name;
        int expected_complexity;
    };

    void TestFile(const TestCase &tc) {
        analyzer::file::File f(std::string(test_files_dir) + "/" + tc.filename);
        analyzer::function::FunctionExtractor e;
        auto allFunctions = e.Get(f);
        ASSERT_EQ(allFunctions.size(), 1);
        const auto &func = allFunctions.front();
        ASSERT_EQ(func.name, tc.func_name);
        EXPECT_EQ(m.Calculate(func).value, tc.expected_complexity);
    }
};

TEST_F(CyclomaticComplexityTest, CommentsPyFile) { TestFile({"comments.py", "Func_comments", 1}); }
TEST_F(CyclomaticComplexityTest, ExceptionsPyFile) { TestFile({"exceptions.py", "Try_Exceptions", 4}); }
TEST_F(CyclomaticComplexityTest, IfPyFile) { TestFile({"if.py", "testIf", 2}); }
TEST_F(CyclomaticComplexityTest, LoopsPyFile) { TestFile({"loops.py", "TestLoops", 4}); }
TEST_F(CyclomaticComplexityTest, ManyLinesPyFile) { TestFile({"many_lines.py", "testmultiline", 2}); }
TEST_F(CyclomaticComplexityTest, ManyParametersPyFile) {
    TestFile({"many_parameters.py", "__test_multiparameters__", 2});
}
TEST_F(CyclomaticComplexityTest, MatchCasePyFile) { TestFile({"match_case.py", "test_Match_case", 4}); }
TEST_F(CyclomaticComplexityTest, NestedIfPyFile) { TestFile({"nested_if.py", "Testnestedif", 4}); }
TEST_F(CyclomaticComplexityTest, SimplePyFile) { TestFile({"simple.py", "test_simple", 2}); }
TEST_F(CyclomaticComplexityTest, TernaryPyFile) { TestFile({"ternary.py", "teSt_ternary", 3}); }

}  // namespace analyzer::metric::metric_impl
