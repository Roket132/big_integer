#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <cstring>
#include <string>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <variant>
#include <vector>

struct big_integer {
        big_integer();
        big_integer(big_integer const &other);
        big_integer(int a);
        explicit big_integer(unsigned int new_sign,  std::vector<unsigned int> new_bits, int a, size_t b, bool sign);
        explicit big_integer(std::string const &str);

        ~big_integer();

        big_integer& operator=(big_integer const& other);

        big_integer& operator+=(big_integer const& rhs);
        big_integer& operator-=(big_integer const& rhs);
        big_integer& operator*=(big_integer const& rhs);
        big_integer& operator/=(big_integer const& rhs);
        big_integer& operator%=(big_integer const& rhs);

        friend big_integer operator+(big_integer a, big_integer const& b);
        friend big_integer operator-(big_integer a, big_integer const& b);
        friend big_integer operator*(big_integer a, big_integer const& b);
        friend big_integer operator/(big_integer a, big_integer const& b);
        friend big_integer operator%(big_integer a, big_integer const& b);

        friend bool operator==(big_integer const& a, big_integer const& b);
        friend bool operator<(big_integer const& a, big_integer const& b);
        friend bool operator>(big_integer const& a, big_integer const& b);
        friend bool operator!=(big_integer const& a, big_integer const& b);
        friend bool operator<=(big_integer const& a, big_integer const& b);
        friend bool operator>=(big_integer const& a, big_integer const& b);

        big_integer operator+() const;
        big_integer operator-() const;
        big_integer operator~() const;

        big_integer& operator&=(big_integer const& rhs);
        big_integer& operator|=(big_integer const& rhs);
        big_integer& operator^=(big_integer const& rhs);
        big_integer& operator<<=(unsigned int rhs);
        big_integer& operator>>=(unsigned int rhs);

        friend big_integer operator&(big_integer const &a, big_integer const& b);
        friend big_integer operator|(big_integer const &a, big_integer const& b);
        friend big_integer operator^(big_integer const &a, big_integer const& b);
        friend big_integer operator<<(big_integer const &a, unsigned int b);
        friend big_integer operator>>(big_integer const &a, unsigned int b);

        friend void print(big_integer const &a);
        friend std::string to_string(big_integer a);

        friend std::ostream& operator<<(std::ostream& os, big_integer const &a);

    private:
        std::variant<unsigned int, std::shared_ptr<std::vector<unsigned int>>> small, bits;
        bool sign;
        int iuLast;
        size_t length;
        const int bitUint = 32;

        void Add(big_integer const &reg);
        void Add(unsigned int value);
        void Sub(bool &sign, big_integer const &reg);
        void Sub(bool &sign, unsigned int u);
        void SubRev(big_integer const &reg);
        void Mul(big_integer const &reg);
        void Mul(unsigned int u);
        void ModDiv(big_integer &a, big_integer const &b, big_integer &ans, bool MODE);
        void Div(big_integer const &reg);
        void Mod(big_integer const &reg);

        void Set(unsigned long long value);
        void SetSizeLazy(size_t len);
        void SetSizeKeep(size_t len, size_t extra);
        void Load(big_integer const &reg, size_t extra);
        int GetReadSign() const;
        void GetBits(unsigned int *res) const;
        void ensureWritable(size_t len, size_t extra);
        void ensureWritable(size_t extra);

        friend void make_fit(big_integer &reg);
        friend void Trim(big_integer &reg);
        unsigned int AddCarry(unsigned int &u1, unsigned int u2, unsigned int Carry);
        big_integer swapForm() const;
        unsigned int SubBorrow(unsigned int &u1, unsigned int u2, unsigned int Borrow);
        unsigned int SubRevBorrow(unsigned int &u1, unsigned int u2, unsigned int uBorrow);
        unsigned int AddMulCarry(unsigned int &Add, unsigned int Mul1, unsigned int Mul2, unsigned int Carry);
        unsigned int MulCarry(unsigned int &u1, unsigned int u2, unsigned int carry);
        unsigned int get_digit(size_t it) const;

        int getDiffLength(std::vector<unsigned int> const &reg1, std::vector<unsigned int> const &reg2, int cu);
        int getHighZero (unsigned int u);
        unsigned long long MakeULong(unsigned int h, unsigned int l);
        void mswap(big_integer &a, big_integer &b);
        unsigned int mod(big_integer &regF, unsigned int uDen);
        unsigned int div (unsigned int uDen);

        size_t lenArray();

};

#endif // BIG_INTEGER_H
