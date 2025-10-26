// File: johnson-trotter-lex.cpp
#include <algorithm>
#include <cctype>
#include <iostream>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace permtool {

struct Options {
    enum class Algo { JT, Lex, Both };
    Algo algo = Algo::Both;
    bool quiet = false;
    bool test = false;
    bool arrows = true; // show stacked arrows over chars for JT
};

static inline std::string trim_copy(std::string s) {
    auto issp = [](unsigned char c){ return std::isspace(c) != 0; };
    while (!s.empty() && issp(static_cast<unsigned char>(s.back()))) s.pop_back();
    while (!s.empty() && issp(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
    return s;
}

static inline bool hasDistinctChars(const std::string& s) {
    std::unordered_set<char> seen;
    seen.reserve(s.size());
    for (char c : s) if (!seen.insert(c).second) return false;
    return true;
}

static inline std::vector<char> toVector(const std::string& s) { return std::vector<char>(s.begin(), s.end()); }
static inline std::string toString(const std::vector<char>& v) { return std::string(v.begin(), v.end()); }

static inline size_t factorial_clamped(size_t n) {
    const size_t LIM = static_cast<size_t>(-1);
    size_t r = 1;
    for (size_t i = 2; i <= n; ++i) { if (r > LIM / i) return LIM; r *= i; }
    return r;
}

static inline void printPermutation(const std::string& s, size_t step, bool quiet) {
    if (quiet) return;
    std::cout << "[" << step << "] " << s << "\n";
}

/* ---------- JT printer (stacked): arrows line ABOVE characters line ---------- */
static inline void printJTStateStacked(const std::vector<char>& a,
                                       const std::vector<int>& dir,
                                       size_t step,
                                       bool quiet,
                                       bool show_arrows)
{
    if (quiet) return;

    // Header and indent to align the second line under it
    std::ostringstream hdr;
    hdr << "[" << step + 1 << "] ";
    const std::string header = hdr.str();
    const std::string indent(header.size(), ' ');

    if (show_arrows && a.size() == dir.size()) {
        // Arrows (top)
        std::cout << indent;
        for (size_t i = 0; i < dir.size(); ++i) {
            if (i) std::cout << " ";
            std::cout << (dir[i] < 0 ? "<-" : "->");
        }
        std::cout << "\n";

        // Characters (below)
        std::cout << header;
        for (size_t i = 0; i < a.size(); ++i) {
            if (i) std::cout << " ";
            std::cout << a[i] << " ";
        }
        std::cout << "\n";
    } else {
        // Fallback: single elements line
        std::cout << header;
        for (size_t i = 0; i < a.size(); ++i) {
            if (i) std::cout << " ";
            std::cout << a[i] << " ";
        }
        std::cout << "\n";
    }
}

/* ---------- Johnson–Trotter (distinct symbols only) ---------- */
std::vector<std::string> johnsonTrotter(const std::string& input,
                                        bool quiet = false,
                                        bool show_arrows = false)
{
    if (!hasDistinctChars(input)) {
        throw std::invalid_argument("Johnson-Trotter requires all distinct symbols");
    }

    // Start from sorted order to match typical/legacy presentation
    std::string s = input;
    std::sort(s.begin(), s.end());

    const size_t n = s.size();
    std::vector<char> a = toVector(s);
    std::vector<int> dir(n, -1); // -1: left ("<-"), +1: right ("->")

    auto is_mobile = [&](size_t i) -> bool {
        int d = dir[i];
        if (d == -1) { if (i == 0) return false; return a[i] > a[i - 1]; }
        else { if (i + 1 >= n) return false; return a[i] > a[i + 1]; }
    };

    auto find_largest_mobile_index = [&]() -> std::optional<size_t> {
        std::optional<size_t> k;
        for (size_t i = 0; i < n; ++i) {
            if (!is_mobile(i)) continue;
            if (!k.has_value() || a[i] > a[*k]) k = i;
        }
        return k;
    };

    std::vector<std::string> out;
    if (n <= 10) { try { out.reserve(factorial_clamped(n)); } catch (...) {} }

    size_t step = 0;
    out.push_back(toString(a));
    printJTStateStacked(a, dir, step++, quiet, show_arrows); // arrows ABOVE chars

    while (true) {
        auto k = find_largest_mobile_index();
        if (!k.has_value()) break;

        size_t i = *k;
        int d = dir[i];
        size_t j = static_cast<size_t>(static_cast<long long>(i) + d);

        std::swap(a[i], a[j]);
        std::swap(dir[i], dir[j]);

        // Reverse direction of all elements greater than the moved one (now at j)
        for (size_t t = 0; t < n; ++t) if (a[t] > a[j]) dir[t] = -dir[t];

        out.push_back(toString(a));
        printJTStateStacked(a, dir, step++, quiet, show_arrows);
    }
    return out;
}

/* ---------- Lexicographic (supports duplicates) ---------- */
std::vector<std::string> lexicographicPermutations(std::string s, bool quiet = false) {
    std::sort(s.begin(), s.end());
    std::vector<std::string> out;
    if (s.size() <= 10) { try { out.reserve(factorial_clamped(s.size())); } catch (...) {} }
    size_t step = 1;
    out.push_back(s);
    printPermutation(s, step++, quiet);
    while (std::next_permutation(s.begin(), s.end())) {
        out.push_back(s);
        printPermutation(s, step++, quiet);
    }
    return out;
}

/* ---------- CLI parsing ---------- */
struct OptionsParseError : std::invalid_argument { using std::invalid_argument::invalid_argument; };

Options parseOptions(int argc, char** argv) {
    Options opt;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i] ? std::string(argv[i]) : std::string();
        if (a.empty()) continue;

        if (a == "--algo" && i + 1 < argc) {
            std::string v = argv[++i] ? std::string(argv[i]) : std::string();
            if      (v == "jt")   opt.algo = Options::Algo::JT;
            else if (v == "lex")  opt.algo = Options::Algo::Lex;
            else if (v == "both") opt.algo = Options::Algo::Both;
            else throw OptionsParseError("Unknown --algo value. Use jt|lex|both.");
        } else if (a == "--quiet") {
            opt.quiet = true;
        } else if (a == "--test") {
            opt.test = true;
        } else if (a == "--arrowsoff") {
            opt.arrows = false;
        } else if (a == "--help" || a == "-h") {
            std::cout <<
R"(Usage: johnson-trotter-lex [--algo jt|lex|both] [--quiet] [--arrowsoff] [--test]
  stdin: input string (e.g., "ABC")
  --algo   jt   : Johnson-Trotter (distinct symbols only)
           lex  : Lexicographic (std::next_permutation, supports duplicates)
           both : Run both (default)
  --arrowsoff   : Hide arrows printed ABOVE characters for JT
  --quiet  : Suppress per-step prints (still prints summary)
  --test   : Run built-in tests and exit
)";
            std::exit(0);
        } else {
            throw OptionsParseError("Unknown option: " + a);
        }
    }
    return opt;
}

/* ---------- Tests ---------- */
static inline void require(bool cond, const char* msg) {
    if (!cond) throw std::runtime_error(std::string("Test failed: ") + msg);
}
template <typename T>
static inline void require_eq(const T& a, const T& b, const char* msg) {
    if (!(a == b)) throw std::runtime_error(std::string("Test failed: ") + msg);
}
static inline bool isSortedNonDecreasing(const std::vector<std::string>& v) {
    for (size_t i = 1; i < v.size(); ++i) if (v[i - 1] > v[i]) return false;
    return true;
}

void runTests() {
    {
        auto v = johnsonTrotter("ABC", /*quiet=*/true, /*arrows=*/true);
        require_eq(v.size(), static_cast<size_t>(6), "JT ABC count == 6");
        auto adj_swap = [](const std::string& x, const std::string& y){
            if (x.size() != y.size()) return false;
            int diff = 0, p1 = -1, p2 = -1;
            for (size_t i = 0; i < x.size(); ++i) if (x[i] != y[i]) { ++diff; if (p1==-1) p1=int(i); else p2=int(i); }
            if (diff != 2) return false;
            if (p2 - p1 != 1) return false;
            return x[p1] == y[p2] && x[p2] == y[p1];
        };
        for (size_t i = 1; i < v.size(); ++i) require(adj_swap(v[i-1], v[i]), "JT adjacent swap property");
    }
    {
        bool threw = false;
        try { (void)johnsonTrotter("AAB", true, true); } catch (const std::invalid_argument&) { threw = true; }
        require(threw, "JT should reject duplicates");
    }
    {
        auto v = lexicographicPermutations("ABC", true);
        require_eq(v.size(), static_cast<size_t>(6), "lex ABC count == 6");
        require(v.front() == "ABC" && v.back() == "CBA", "lex ABC bounds");
        require(isSortedNonDecreasing(v), "lex results sorted");
        std::set<std::string> uniq(v.begin(), v.end());
        require_eq(uniq.size(), v.size(), "lex ABC unique");
    }
    {
        auto v = lexicographicPermutations("AAB", true);
        require_eq(v.size(), static_cast<size_t>(3), "lex AAB count == 3");
        std::vector<std::string> expected = {"AAB","ABA","BAA"};
        require_eq(v, expected, "lex AAB ordering");
    }
    {
        auto v0 = lexicographicPermutations("", true);
        require_eq(v0.size(), static_cast<size_t>(1), "empty string has 1 perm (empty)");
        require_eq(v0[0], std::string(), "empty is empty");
        auto v1 = lexicographicPermutations("A", true);
        require_eq(v1.size(), static_cast<size_t>(1), "single char has 1 perm");
        require_eq(v1[0], std::string("A"), "single char identity");
    }
    std::cout << "All tests passed.\n";
}

} // namespace permtool

int main(int argc, char** argv) {
    using namespace permtool;
    try {
        Options opt = parseOptions(argc, argv);
        if (opt.test) { runTests(); return 0; }

        std::string input;
        {
            std::string line;
            if (!std::getline(std::cin, line)) { std::cerr << "No input received on stdin.\n"; return 1; }
            input = trim_copy(line);
        }

        if (opt.algo == Options::Algo::JT || opt.algo == Options::Algo::Both) {
            if (!hasDistinctChars(input)) {
                std::cerr << "Warning: Johnson-Trotter requires distinct symbols; skipping JT for this input.\n";
            } else {
                if (!opt.quiet) {
                    std::cout << "== Johnson-Trotter (" << input.size() << "! up to "
                              << factorial_clamped(input.size()) << ") ==\n";
                }
                auto jt = johnsonTrotter(input, opt.quiet, opt.arrows);
                if (!opt.quiet) std::cout << "-- Count: " << jt.size() << "\n\n";
            }
        }

        if (opt.algo == Options::Algo::Lex || opt.algo == Options::Algo::Both) {
            if (!opt.quiet) std::cout << "== Lexicographic (std::next_permutation) ==\n";
            auto lex = lexicographicPermutations(input, opt.quiet);
            if (!opt.quiet) std::cout << "-- Count: " << lex.size() << "\n";
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }
}