#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "file.hpp"
#include "function.hpp"
#include "metric.hpp"
#include "metric_accumulator.hpp"

namespace analyzer {

namespace rv = std::ranges::views;
namespace rs = std::ranges;
/**
 * @brief Анализирует список Python-файлов и извлекает метрики для всех функций и методов.
 *
 * Эта функция — центральный "конвейер" обработки:
 * 1. Принимает имена файлов.
 * 2. Для каждого файла создаёт объект `File`, который автоматически парсит его через tree-sitter
 *    и строит AST.
 * 3. Извлекает из AST все функции и методы с помощью `FunctionExtractor`.
 * 4. Объединяет все функции из всех файлов в один плоский список (`join`).
 * 5. Для каждой функции вычисляет набор метрик через переданный `metric_extractor`.
 * 6. Возвращает вектор пар: (функция, результаты её метрик).
 */
using AnalysisResult = std::vector<std::pair<analyzer::function::Function, analyzer::metric::MetricResults>>;

auto AnalyseFunctions(const std::vector<std::string> &filenames,
                      const analyzer::metric::MetricExtractor &metric_extractor) {
    // здесь ваш код
    analyzer::function::FunctionExtractor extractor;
    // clang-format off
    return filenames 
    | rv::transform([&](const auto &filename) { return analyzer::file::File(filename); }) 
    | rv::transform([&](const auto &f) { return extractor.Get(f); }) 
    | rv::join 
    | rv::transform([&](const auto &func) { return std::make_pair(func, metric_extractor.Get(func)); }) 
    | rs::to<AnalysisResult>();
    // clang-format on
}

/**
 *
 * @brief Группирует результаты анализа по классам.
 *
 * Эта функция:
 * 1. Отфильтровывает только те функции, которые являются **методами классов**
 *    (у них `class_name.has_value()` == true).
 * 2. Группирует последовательные элементы с одинаковым именем класса с помощью `chunk_by`.
 *
 * Важно:
 * - `chunk_by` работает только с **последовательными** одинаковыми элементами!
 *   Поэтому предполагается, что входной диапазон уже упорядочен по классам
 *   (например, порядок методов в AST сохраняется как в исходном файле).
 * - Если порядок нарушен, один и тот же класс может быть разбит на несколько групп.
 *
 *  Чтобы убедиться, что фильтрация работает, проверьте, что свободные функции (без class_name)
 * действительно исчезают из результата.
 */
auto SplitByClasses(const auto &analysis) {
    // clang-format off
    return analysis 
    | rv::filter([](const auto &p) {  
               return p.first.class_name.has_value();
           })
    | rv::chunk_by([](const auto &lhs, const auto &rhs) {  
               return lhs.first.class_name == rhs.first.class_name;
           })
    | rs::to<std::vector<AnalysisResult>>();

    // clang-format on
}

/**
 * @brief Группирует результаты анализа по исходным файлам.
 *
 * Эта функция:
 * - Разбивает весь список функций на группы, где каждая группа содержит
 *   только функции из одного и того же файла (`filename`).
 * - Использует `chunk_by`, поэтому **порядок функций в `analysis` должен быть по файлам**.
 */
auto SplitByFiles(const auto &analysis) {
    // clang-format off
    return analysis 
    | rv::filter([](const auto &p) {  
               return p.first.class_name.has_value();
           })
    | rv::chunk_by([](const auto &lhs, const auto &rhs) {  
               return lhs.first.filename == rhs.first.filename;
           })
    | rs::to<std::vector<AnalysisResult>>();
    // clang-format on
}

/**
 * @brief Агрегирует метрики всех функций с помощью аккумулятора.
 *
 * Эта функция:
 * - Проходит по каждому элементу результата `AnalyseFunctions`
 *   (то есть по каждой функции и её метрикам).
 * - Передаёт результаты метрик (`elem.second`) в аккумулятор через `AccumulateNextFunctionResults`.
 */
void AccumulateFunctionAnalysis(const AnalysisResult &analysis,
                                const analyzer::metric_accumulator::MetricsAccumulator &accumulator) {
    std::ranges::for_each(
        analysis, [&](const auto &results) { accumulator.AccumulateNextFunctionResults(results); },
        [](const auto &pair) { return pair.second; });
}

}  // namespace analyzer
