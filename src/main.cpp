#include <iostream>
#include <limits>

#include "array_sequence.hpp"
#include "list_sequence.hpp"
#include "bit_sequence.hpp"
#include "sequence.hpp"
#include "utils.hpp"   // zip, unzip, split — free functions

// ============================================================
// Tags of different types of sequences
// ============================================================
enum class SeqKind  { MutArray, ImmArray, MutList, ImmList };

const char* seq_kind_name(SeqKind k) {
    switch (k) {
        case SeqKind::MutArray: return "Mutable   ArraySequence<int>";
        case SeqKind::ImmArray: return "Immutable ArraySequence<int>";
        case SeqKind::MutList:  return "Mutable   ListSequence<int>";
        case SeqKind::ImmList:  return "Immutable ListSequence<int>";
    }
    return "?";
}

bool is_mutable(SeqKind k) { 
    return k == SeqKind::MutArray || k == SeqKind::MutList; 
}

// ============================================================
// Regs
// ============================================================
struct SeqEntry {
    SeqKind        kind;
    Sequence<int> *ptr;
    ~SeqEntry() { delete ptr; }
};

struct BitEntry {
    // no kind anymore. only immutable
    BitSequence *ptr;
    ~BitEntry() { delete ptr; }
};

// sequence<Bit> != sequence<int>, so need 2 independed regs
MutableArraySequence<SeqEntry*> seq_reg;   // int-sequences
MutableArraySequence<BitEntry*> bit_reg;   // bit-sequences

// ============================================================
// Input-Output
// ============================================================
void clear_screen() {
    // \x1B[2J  — clear
    // \x1B[H   — set cursor at (0,0)
    std::cout << "\x1B[2J\x1B[H" << std::flush;
}

void scroll_up() {
    std::cout << "\x1B[3J\x1B[H" << std::flush;
}

void wait_for_enter() {
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.get();
}

int read_int(const char* prompt = "") {
    int v;
    while (true) {
        if (*prompt) std::cout << prompt;
        if (std::cin >> v) { std::cin.ignore(); return v; }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  [!] Введите целое число.\n";
    }
}

int read_bit(const char* prompt = "") {
    while (true) {
        int v = read_int(prompt);
        if (v == 0 || v == 1) return v;
        std::cout << "  [!] Введите 0 или 1.\n";
    }
}

void print_seq_registry() {
    if (seq_reg.get_size() == 0) {
        std::cout << "  (пусто)\n";
        return;
    }
    for (int i = 0; i < seq_reg.get_size(); i++)
        std::cout << "  " << (i+1) << ".  "
                  << *seq_reg[i]->ptr
                  << "  [" << seq_kind_name(seq_reg[i]->kind) << "]\n";
}

void print_bit_registry() {
    if (bit_reg.get_size() == 0) {
        std::cout << "  (пусто)\n";
        return;
    }
    for (int i = 0; i < bit_reg.get_size(); i++)
        std::cout << "  " << (i+1) << ".  " << *bit_reg[i]->ptr << "\n";
}

// Выбор индекса из реестра int-последовательностей (1-based → 0-based)
int pick_seq(const char* prompt = "  Номер: ") {
    while (true) {
        print_seq_registry();
        int n = read_int(prompt);
        if (n >= 1 && n <= seq_reg.get_size()) return n - 1;
        std::cout << "  [!] Номер вне диапазона.\n";
    }
}

// Выбор индекса из реестра BitSequence (1-based → 0-based)
int pick_bit(const char* prompt = "  Номер: ") {
    while (true) {
        print_bit_registry();
        int n = read_int(prompt);
        if (n >= 1 && n <= bit_reg.get_size()) return n - 1;
        std::cout << "  [!] Номер вне диапазона.\n";
    }
}

void add_seq(SeqKind kind, Sequence<int>* ptr) {
    seq_reg.append(new SeqEntry{kind, ptr});
    std::cout << "  → Добавлено в реестр int-последовательностей под номером "
              << seq_reg.get_size() << ".\n";
}

void add_bit(BitSequence *ptr) {
    bit_reg.append(new BitEntry{ptr});
    std::cout << "  → Добавлено в реестр BitSequence под номером "
              << bit_reg.get_size() << ".\n";
}

// if operation returns a new object (immutable) — add to reg
// for mutable inst == ptr, object has already been in reg
void maybe_add_seq(SeqKind kind, Sequence<int>* result, Sequence<int>* original) {
    if (result != original) add_seq(kind, result);
    else { std::cout << "  → Объект изменён на месте.\n"; print_seq_registry(); }
}

// always add new bit_sequence
void maybe_add_bit(BitSequence* result) {
    add_bit(result);
}

// ============================================================
// Reading int array
// ============================================================
MutableArraySequence<int>* read_int_array() {
    int n;
    do {
        n = read_int(" Количество элементов: ");
        if (n < 0) std::cout << "  [!] Количество не может быть отрицательным.\n";
    } while (n < 0);

    auto* s = new MutableArraySequence<int>();
    for (int i = 0; i < n; i++) {
        std::cout << "  [" << i << "]: ";
        s->append(read_int());
    }
    return s;
}

// ============================================================
// map / where / reduce
// ============================================================
int map_double(const int& x) { return x * 2; }
int map_negate(const int& x) { return -x; }
int map_square(const int& x) { return x * x; }
int map_inc   (const int& x) { return x + 1; }

bool where_positive(const int& x) { return x > 0; }
bool where_even    (const int& x) { return x % 2 == 0; }
bool where_odd     (const int& x) { return x % 2 != 0; }

int reduce_sum(const int& a, const int& b) { return a + b; }
int reduce_mul(const int& a, const int& b) { return a * b; }
int reduce_max(const int& a, const int& b) { return a > b ? a : b; }
int reduce_min(const int& a, const int& b) { return a < b ? a : b; }

// bits
bool where_bit_one (const Bit& x) { return  x.get(); }
bool where_bit_zero(const Bit& x) { return !x.get(); }

int (*pick_map_func())(const int&) {
    std::cout << "  1. x*2  2. -x  3. x²  4. x+1\n";
    switch (read_int("  > ")) {
        case 1: return map_double; case 2: return map_negate;
        case 3: return map_square; default: return map_inc;
    }
}
bool (*pick_where_func())(const int&) {
    std::cout << "  1. x>0  2. чётные  3. нечётные\n";
    switch (read_int("  > ")) {
        case 1: return where_positive; case 2: return where_even;
        default: return where_odd;
    }
}
int (*pick_reduce_func())(const int&, const int&) {
    std::cout << "  1. сумма  2. произведение  3. max  4. min\n";
    switch (read_int("  > ")) {
        case 1: return reduce_sum; case 2: return reduce_mul;
        case 3: return reduce_max; default: return reduce_min;
    }
}

// ============================================================
// Creating new sequence
// ============================================================
void cmd_create_seq() {
    std::cout << "  1. Mutable ArraySequence<int>\n"
              << "  2. Immutable ArraySequence<int>\n"
              << "  3. Mutable ListSequence<int>\n"
              << "  4. Immutable ListSequence<int>\n";
    int choice;
    do {
        choice = read_int("  > ");
        if (choice < 1 || choice > 4)
            std::cout << "  [!] Введите число от 1 до 4.\n";
    } while (choice < 1 || choice > 4);
    auto* tmp = read_int_array();
    int   sz  = tmp->get_size();
    int*  raw = new int[sz];
    for (int i = 0; i < sz; i++) raw[i] = (*tmp)[i];
    delete tmp;

    switch (choice) {
        case 1: add_seq(SeqKind::MutArray, new MutableArraySequence<int>(raw, sz));   break;
        case 2: add_seq(SeqKind::ImmArray, new ImmutableArraySequence<int>(raw, sz)); break;
        case 3: add_seq(SeqKind::MutList,  new MutableListSequence<int>(raw, sz));    break;
        case 4: add_seq(SeqKind::ImmList,  new ImmutableListSequence<int>(raw, sz));  break;
    }
    delete[] raw;
}


void cmd_create_bit() {
    int n;
    do {
        n = read_int("  Количество бит: ");
        if (n < 0) std::cout << "  [!] Количество не может быть отрицательным.\n";
    } while (n < 0);
    bool* bits = new bool[n];
    for (int i = 0; i < n; i++) {
        std::cout << "  бит[" << i << "] (0/1): ";
        bits[i] = read_bit();
    }
    add_bit(new BitSequence(bits, n));
    delete[] bits;
}

// ============================================================
// Operation int sequences
// ============================================================
void cmd_seq_ops(int idx) {
    SeqEntry*      e = seq_reg[idx];
    Sequence<int>* s = e->ptr;
    SeqKind        k = e->kind;

    std::cout << "\n  1.  append          2.  prepend\n"
              << "  3.  insert_at       4.  set\n"
              << "  5.  remove_at       6.  get\n"
              << "  7.  get_first       8.  get_last\n"
              << "  9.  get_size        10.  get_subsequence\n"
              << "  11.  concat         12. map\n"
              << "  13. where           14. reduce\n"
              << "  15. zip             16. split\n"
              << "  17. slice           18. печать\n";
    int op = read_int("  > ");

    switch (op) {
        case 1: {
            int v = read_int("  Значение: ");
            maybe_add_seq(k, s->append(v), s);
            break;
        }
        case 2: {
            int v = read_int("  Значение: ");
            maybe_add_seq(k, s->prepend(v), s);
            break;
        }
        case 3: {
            int v   = read_int("  Значение: ");
            int pos = read_int("  Позиция: ");
            try { maybe_add_seq(k, s->insert_at(v, pos), s); }
            catch (const std::exception& ex) { std::cout << "  [!] " << ex.what() << "\n"; }
            break;
        }
        case 4: {
            int v   = read_int("  Значение: ");
            int pos = read_int("  Позиция: ");
            try { maybe_add_seq(k, s->set(v, pos), s); }
            catch (const std::exception& ex) { std::cout << "  [!] " << ex.what() << "\n"; }
            break;
        }
        case 5: {
            int pos = read_int("  Позиция: ");
            try { maybe_add_seq(k, s->remove_at(pos), s); }
            catch (const std::exception& ex) { std::cout << "  [!] " << ex.what() << "\n"; }
            break;
        }
        case 6: {
            int i = read_int("  Индекс: ");
            try { std::cout << "  [" << i << "] = " << s->get(i) << "\n"; }
            catch (const std::exception& ex) { std::cout << "  [!] " << ex.what() << "\n"; }
            break;
        }
        case 7:
            try { std::cout << "  first = " << s->get_first() << "\n"; }
            catch (const std::exception& ex) { std::cout << "  [!] " << ex.what() << "\n"; }
            break;
        case 8:
            try { std::cout << "  last = " << s->get_last() << "\n"; }
            catch (const std::exception& ex) { std::cout << "  [!] " << ex.what() << "\n"; }
            break;
        case 9:
            std::cout << "  size = " << s->get_size() << "\n";
            break;
        case 10: {
            int from = read_int("  От: ");
            int to   = read_int("  До: ");
            try {
                Sequence<int>* sub = s->get_subsequence(from, to);
                // get_subsequence creates new object (same-kind)
                add_seq(k, sub);
            }
            catch (const std::exception& ex) { std::cout << "  [!] " << ex.what() << "\n"; }
            break;
        }
        case 11: {
            std::cout << "  1. Из реестра  2. Ввести новую временную\n";
            Sequence<int>* other = nullptr;
            bool owns = false;

            if (read_int("  > ") == 1) {
                int other_idx = pick_seq("  Номер второй: ");
                other = seq_reg[other_idx]->ptr;

                // aliasing: mutable concat with this - create a copy
                if (other == s && is_mutable(k)) {
                    other = s->get_subsequence(0, s->get_size() - 1);
                    owns = true;
                }
            } else {
                other = read_int_array();
                owns = true;
            }

            Sequence<int>* res = s->concat(*other);
            if (owns) delete other;
            add_seq(k, res);
            break;
        }
        case 12:
            maybe_add_seq(k, s->map(pick_map_func()), s);
            break;
        case 13:
            maybe_add_seq(k, s->where(pick_where_func()), s);
            break;
        case 14: {
            auto  f   = pick_reduce_func();
            int start = read_int("  Начальное значение: ");
            std::cout << "  Результат: " << s->reduce(f, start) << "\n";
            break;
        }
        case 15: {
            // zip returns MutableArraySequence<Pair<int,int>>*
            // Impossible to store him in the reg: type doesn't match
            // just print, don't add to reg
            std::cout << "  Вторая последовательность:\n";
            Sequence<int>* other = seq_reg[pick_seq()]->ptr;
            auto* res = zip<int, int>(*s, *other);
            std::cout << "  zip-результат: [";
            for (int i = 0; i < res->get_size(); i++) {
                std::cout << "(" << (*res)[i].first << "," << (*res)[i].second << ")";
                if (i < res->get_size() - 1) std::cout << ", ";
            }
            std::cout << "]\n";
            std::cout << "  Выполнить unzip? (1 — да): ";
            if (read_int() == 1) {
                auto pair = unzip<int, int>(*res);
                std::cout << "  first:  " << *pair.first << "\n";
                std::cout << "  second: " << *pair.second << "\n";
                std::cout << "  Добавить оба в реестр? (1 — да): ";
                if (read_int() == 1) {
                    add_seq(SeqKind::MutArray, pair.first);
                    add_seq(SeqKind::MutArray, pair.second);
                } else {
                    delete pair.first;
                    delete pair.second;
                }
            }
            delete res;
            break;
        }
        case 16: {
            int split_val = read_int("  Элемент-разделитель: ");
            auto* res = split<int>(*s, split_val);
            std::cout << "  Фрагментов: " << res->get_size() << "\n";
            for (int i = 0; i < res->get_size(); i++) {
                std::cout << "  [" << (i+1) << "] " << *(*res)[i] << "\n";
                // фрагменты переходят во владение реестра — не удаляем здесь
                seq_reg.append(new SeqEntry{SeqKind::MutArray, (*res)[i]});
                std::cout << "      → добавлен под номером " << seq_reg.get_size() << "\n";
            }
    
            delete res;
            break;
        }
        case 17: {
            int index = read_int("  Индекс (может быть отрицательным): ");
            int count;
            do {
                count = read_int("  Количество удаляемых (>= 0): ");
                if (count < 0) std::cout << "  [!] Количество не может быть отрицательным.\n";
            } while (count < 0);

            auto* ins = read_int_array();
            try {
                Sequence<int>* res = s->slice(index, count, *ins);
                delete ins;
                maybe_add_seq(k, res, s);
            } catch (const std::exception& ex) {
                delete ins;
                std::cout << "  [!] " << ex.what() << "\n";
            }
            break;
        }
        case 18:
            std::cout << *s << "\n";
            break;
        default:
            std::cout << "  [!] Неверная операция.\n";
    }
}

// ============================================================
// BitSequence Operations
// ============================================================
void cmd_bit_ops(int idx) {
    BitSequence* b = bit_reg[idx]->ptr;

    std::cout << "\n  — Базовые операции —\n"
              << "  1. append      2. prepend\n"
              << "  3. insert_at   4. set\n"
              << "  5. remove_at   6. get\n"
              << "  7. get_first   8. get_last\n"
              << "  9. get_size   10. get_subsequence\n"
              << " 11. concat     12. slice\n"
              << "  — map / where / reduce —\n"
              << " 13. map        14. where\n"
              << " 15. reduce\n"
              << "  — Битовые операции —\n"
              << " 16. bit_and    17. bit_or\n"
              << " 18. bit_xor    19. bit_not\n"
              << " 20. печать\n";
    int op = read_int(" > ");

    auto pick_other_bit = [&]() -> BitSequence* {
        return bit_reg[pick_bit(" Номер второго BitSequence: ")]->ptr;
    };

    switch (op) {
    case 1: {
        int v = read_int(" Бит (0/1): ");
        maybe_add_bit(b->append(Bit(v != 0)));
        break;
    }
    case 2: {
        int v = read_int(" Бит (0/1): ");
        maybe_add_bit(b->prepend(Bit(v != 0)));
        break;
    }
    case 3: {
        int v   = read_int(" Бит (0/1): ");
        int pos = read_int(" Позиция: ");
        try { maybe_add_bit(b->insert_at(Bit(v != 0), pos)); }
        catch (const std::exception& ex) { std::cout << " [!] " << ex.what() << "\n"; }
        break;
    }
    case 4: {
        int v   = read_int(" Бит (0/1): ");
        int pos = read_int(" Позиция: ");
        try { maybe_add_bit(b->set(Bit(v != 0), pos)); }
        catch (const std::exception& ex) { std::cout << " [!] " << ex.what() << "\n"; }
        break;
    }
    case 5: {
        int pos = read_int(" Позиция: ");
        try { maybe_add_bit(b->remove_at(pos)); }
        catch (const std::exception& ex) { std::cout << " [!] " << ex.what() << "\n"; }
        break;
    }
    case 6: {
        int i = read_bit(" Индекс: ");
        try { std::cout << " [" << i << "] = " << (b->get(i).get() ? "1" : "0") << "\n"; }
        catch (const std::exception& ex) { std::cout << " [!] " << ex.what() << "\n"; }
        break;
    }
    case 7:
        try { std::cout << " first = " << (b->get_first().get() ? "1" : "0") << "\n"; }
        catch (const std::exception& ex) { std::cout << " [!] " << ex.what() << "\n"; }
        break;
    case 8:
        try { std::cout << " last = " << (b->get_last().get() ? "1" : "0") << "\n"; }
        catch (const std::exception& ex) { std::cout << " [!] " << ex.what() << "\n"; }
        break;
    case 9:
        std::cout << " size = " << b->get_size() << "\n";
        break;
    case 10: {
        int from = read_int(" От: ");
        int to   = read_int(" До: ");
        try { maybe_add_bit(b->get_subsequence(from, to)); }
        catch (const std::exception& ex) { std::cout << " [!] " << ex.what() << "\n"; }
        break;
    }
    case 11: {
        maybe_add_bit(b->concat(*pick_other_bit()));
        break;
    }
    case 12: {
        int index = read_int(" Индекс (может быть отрицательным): ");
        int count = read_int(" Количество удаляемых: ");
        BitSequence* ins = pick_other_bit();
        try { maybe_add_bit(b->slice(index, count, *ins)); }
        catch (const std::exception& ex) { std::cout << " [!] " << ex.what() << "\n"; }
        break;
    }
    case 13:
        maybe_add_bit(b->map([](const Bit& x) { return ~x; }));
        break;
    case 14: {
        std::cout << " 1. только 1-биты  2. только 0-биты\n";
        bool target = (read_int(" > ") == 1);
        maybe_add_bit(b->where(target ? where_bit_one : where_bit_zero));
        break;
    }
    case 15: {
        std::cout << " 1. AND  2. OR  3. XOR\n";
        int choice = read_int(" > ");
        Bit res;
        switch (choice) {
            case 1:  res = b->reduce([](const Bit& a, const Bit& b){ return a & b; }, Bit{true});  break;
        case 2:  res = b->reduce([](const Bit& a, const Bit& b){ return a | b; }, Bit{false}); break;
        default: res = b->reduce([](const Bit& a, const Bit& b){ return a ^ b; }, Bit{false}); break;
        }
        std::cout << " Результат: " << (res.get() ? "1" : "0") << "\n";
        break;
    }
    case 16: { maybe_add_bit(b->bit_and (*pick_other_bit())); break; }
    case 17: { maybe_add_bit(b->bit_or  (*pick_other_bit())); break; }
    case 18: { maybe_add_bit(b->bit_xor (*pick_other_bit())); break; }
    case 19: { maybe_add_bit(b->bit_not());                   break; }
    case 20: std::cout << *b << "\n"; break;
    default: std::cout << " [!] Неверная операция.\n";
    }
}

// ============================================================
// delete from reg
// ============================================================
template <typename Entry>
void remove_from(MutableArraySequence<Entry*>& reg, int idx) {
    delete reg[idx];
    reg.remove_at(idx);
    std::cout << "  → Удалено.\n";
}

// ============================================================
// main menu
// ============================================================
int main() {
    while (true) {
        clear_screen();

        std::cout
            << "  ════════════════════════════════\n"
            << "   INT-последовательности\n"
            << "  ────────────────────────────────\n";
        print_seq_registry();
        std::cout
            << "\n"
            << "   BitSequence\n"
            << "  ────────────────────────────────\n";
        print_bit_registry();
        std::cout
            << "  ════════════════════════════════\n"
            << "   0. Выход\n"
            << "   1. Создать int-последовательность\n"
            << "   2. Создать BitSequence\n"
            << "   3. Операции с int-последовательностью\n"
            << "   4. Операции с BitSequence\n"
            << "   5. Удалить int-последовательность\n"
            << "   6. Удалить BitSequence\n"
            << "  ════════════════════════════════\n";

        switch (read_int("  > ")) {
        case 0:
            for (int i = 0; i < seq_reg.get_size(); i++) delete seq_reg[i];
            for (int i = 0; i < bit_reg.get_size(); i++) delete bit_reg[i];
            clear_screen();
            return 0;
        case 1: clear_screen(); cmd_create_seq();   wait_for_enter(); break;
        case 2: clear_screen(); cmd_create_bit();   wait_for_enter(); break;
        case 3:
            if (seq_reg.get_size() == 0) { std::cout << "  [!] Реестр пуст.\n"; wait_for_enter(); break; }
            clear_screen();
            cmd_seq_ops(pick_seq("  Выберите последовательность: "));
            wait_for_enter();
            break;
        case 4:
            if (bit_reg.get_size() == 0) { std::cout << "  [!] Реестр пуст.\n"; wait_for_enter(); break; }
            clear_screen();
            cmd_bit_ops(pick_bit("  Выберите BitSequence: "));
            wait_for_enter();
            break;
        case 5:
            if (seq_reg.get_size() == 0) { std::cout << "  [!] Реестр пуст.\n"; wait_for_enter(); break; }
            clear_screen();
            remove_from(seq_reg, pick_seq("  Номер для удаления: "));
            wait_for_enter();
            break;
        case 6:
            if (bit_reg.get_size() == 0) { std::cout << "  [!] Реестр пуст.\n"; wait_for_enter(); break; }
            clear_screen();
            remove_from(bit_reg, pick_bit("  Номер для удаления: "));
            wait_for_enter();
            break;
        default:
            std::cout << "  [!] Неверная команда.\n";
            wait_for_enter();
        }
    }
}
