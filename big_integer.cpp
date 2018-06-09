#include "big_integer.h"

#define get_bits std::get<std::shared_ptr<std::vector<unsigned int>>>(bits)
#define get_small std::get<unsigned int>(small)
#define get_bits_o(other) std::get<std::shared_ptr<std::vector<unsigned int>>>(other.bits)
#define get_small_o(other) std::get<unsigned int>(other.small)

typedef unsigned int uint;
typedef unsigned long long Ulong;

big_integer::big_integer() {
    *this = big_integer(0);
}

big_integer::~big_integer() {}

big_integer::big_integer(big_integer const &other) : iuLast(other.iuLast), length(other.length), sign(other.sign) {
    if (iuLast == 0) {
        small = other.small;
    } else {
        bits = get_bits_o(other);
    }
}

big_integer::big_integer(unsigned int new_small, std::vector<unsigned int> new_bits, int n_iuLast, size_t n_length, bool nw_sign) {
    iuLast = n_iuLast;
    length = n_length;
    sign = nw_sign;
    if (iuLast == 0)
        small = new_small;
    else {
        std::shared_ptr<std::vector<unsigned int>> ptr;
        ptr.reset(new std::vector<unsigned int>(new_bits.size()));
        copy(new_bits.begin(), new_bits.end(), (*ptr.get()).begin());
        bits = ptr;
    }
    make_fit(*this);
}

big_integer &big_integer::operator=(big_integer const &other) {
    if (other.iuLast == 0) {
        small = other.small;
    } else {
        bits = get_bits_o(other);
    }
    iuLast = other.iuLast;
    length = other.length;
    sign = other.sign;
    return *this;
}
//constructor int
big_integer::big_integer(int value) {
    iuLast = 0;
    length = 0;
    if (value == -2147483648) {
        *this = big_integer("-2147483648");
    } else {
        small = static_cast<unsigned int>(abs(value));
        sign = static_cast<bool>(value < 0 ? 0 : 1);
    }
}

void foo(int &d, unsigned int &sign) {
    if (d < 0) {
        d = abs(d);
        sign = 0;
    }
}

int pow100(int x) {
    int res = 1;
    for (int i = 1; i <= x; i++) res *= 10;
    return res;
}

//ctor string
big_integer::big_integer(std::string const &digit) {
    big_integer res(0);
    unsigned int sign = 1;
    for (size_t i = 0; i < digit.size(); i += 9) {
        if (i + 9 >= digit.size()) {
            int d = atoi(digit.substr(i, digit.size() - i).c_str());
            foo(d, sign);
            res *= pow100(digit.size() - i);
            res += d;
            break;
        } else {
            int d = atoi(digit.substr(i, 9).c_str());
            foo(d, sign);
            res *= 1e9;
            res += d;
        }
    }
    Trim(res);
    res.sign = static_cast<bool>(sign);
    *this = big_integer(res);
}

// +=, -=, *=

int big_integer::GetReadSign() const {
    return sign;
}



big_integer &big_integer::operator+=(big_integer const &reg) {
    ///zero?
    int real_sign = this->GetReadSign();
    int real_reg_sign = reg.GetReadSign();

    if (real_sign == real_reg_sign) {
        Add(reg);
    } else {
        Sub(sign, reg);
    }
    make_fit(*this);
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &reg) {
    int real_sign = this->GetReadSign();
    int real_reg_sign = reg.GetReadSign();
    if (real_sign != real_reg_sign) {
        Add(reg);
    } else {
        Sub(sign, reg);
    }
    make_fit(*this);
    return *this;
}

big_integer &big_integer::operator*=(big_integer const &reg) {
    int real_sign = this->GetReadSign();
    int real_reg_sign = reg.GetReadSign();
    Mul(reg);
    if ((real_sign == 0 && real_reg_sign == 1) || (real_sign == 1 && real_reg_sign == 0))
        sign = 0;
    else
        sign = 1;
    make_fit(*this);
    //print(*this);
    return *this;
}

big_integer &big_integer::operator/=(big_integer const &reg) {
    bool tmp = sign;
    Div(reg);
    if ((tmp == 0 && reg.sign == 1) || (tmp == 1 && reg.sign == 0))
        sign = 0;
    else
        sign = 1;
    make_fit(*this);
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &reg) {
    bool tmp = sign;
    Mod(reg);
    if ((tmp == 0 && reg.sign == 1) || (tmp == 1 && reg.sign == 0))
        sign = 0;
    else
        sign = 1;
    make_fit(*this);
    return *this;
}

big_integer operator+(big_integer a, big_integer const &b) {
    a += b;
    make_fit(a);
    return a;
}

big_integer operator-(big_integer a, big_integer const &b) {
    a -= b;
    make_fit(a);
    return a;
}

big_integer operator*(big_integer a, big_integer const &b) {
    a *= b;
    make_fit(a);
    return a;
}

big_integer operator/(big_integer a, big_integer const &b) {
    a /= b;
    make_fit(a);
    return a;
}

big_integer operator%(big_integer a, big_integer const &b) {
    bool tmp = a.sign;
    a %= b;
    a.sign = tmp;
    return a;
}

///+++++++++++++compare++++++++++++++++

bool operator==(big_integer const &a, big_integer const &b) {
    if (a.iuLast != 0 && b.iuLast != 0) {
        if (a.sign != b.sign) {
            return a.iuLast == 0 && b.iuLast == 0 &&
                    get_small_o(a) == 0 && get_small_o(b) == 0;
        } else {
            return std::equal((*get_bits_o(a).get()).begin(), (*get_bits_o(a)).begin() + a.iuLast, (*get_bits_o(b)).begin());
        }
    } else {
        return a.small == b.small;
    }
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.sign != b.sign) {
        return a.sign == 0;
    }
    if (a.iuLast != 0 && b.iuLast != 0) {
        if (a.iuLast != b.iuLast) {
            return a.iuLast < b.iuLast;
        }
        for (int i = a.iuLast; i >= 0; i--) {
            if ((*get_bits_o(a).get())[i] < (*get_bits_o(b).get())[i])
                return true;
            if ((*get_bits_o(a).get())[i] > (*get_bits_o(b).get())[i])
                return false;
        }
        return false;
    } else {
        if (b.iuLast != 0) return true;
        if (a.iuLast != 0) return false;
        return a.small < b.small;
    }
}

bool operator>(big_integer const &a, big_integer const &b) {
    return !(a == b || a < b);
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return !(a > b);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return !(a < b);
}

///------------------compare------------------
///
///

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    if (iuLast != 0)
        return big_integer(0, (*get_bits.get()), iuLast, length, !sign);
    else
        return big_integer(get_small, std::vector<unsigned int>(0), iuLast, length, !sign);
}

big_integer big_integer::operator~() const {
    big_integer a = *this;
    return -(a + big_integer(1));
}

///+++++++++++++ bit_operations ++++++++++++++

big_integer &big_integer::operator&=(big_integer const &rhs) {
    return *this = *this & rhs;
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    return *this = *this | rhs;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    return *this = *this ^ rhs;
}

big_integer &big_integer::operator<<=(uint b) {
    return *this = *this << b;
}

big_integer &big_integer::operator>>=(uint b) {
    return *this = *this >> b;
}

void big_integer::GetBits(unsigned int *res) const{
    res = new uint[length];
    for (int i = 0; i < length; i++) {
        res[i] = (*get_bits.get())[i];
    }
}

bool GetResSign(int a, int b, int mod) {
    if (!a) a = -1;
    if (!b) b = -1;
    if (mod == 1) return static_cast<bool>((a | b) == -1 ? 0 : 1);
    if (mod == 2) return static_cast<bool>((a & b) == -1 ? 0 : 1);
    if (mod == 3) return static_cast<bool>((a * b) == -1 ? 0 : 1);
}

big_integer operator&(big_integer const &left, big_integer const &right) {
    ///zero?
    big_integer a = left.swapForm();
    big_integer b = right.swapForm();
    if (left.iuLast == 0 && right.iuLast == 0) {
        return big_integer(get_small_o(a) & get_small_o(b), std::vector<uint>(0), 0, 0, GetResSign(a.sign, b.sign, 2)).swapForm();
    }
    uint *aReg = nullptr;
    uint *bReg = nullptr;
    a.GetBits(aReg);
    b.GetBits(bReg);

    uint xE = (a.sign < 0) ? UINT32_MAX : 0;
    uint yE = (b.sign < 0) ? UINT32_MAX : 0;
    size_t len = std::max(left.length, right.length);
    std::vector<uint> res(len);
    for (size_t i = 0; i < len; i++) {
        uint xu = (i < a.length) ? (*get_bits_o(a).get())[i] : xE;
        uint yu = (i < b.length) ? (*get_bits_o(b).get())[i] : yE;
        res[i] = xu & yu;
    }
    return big_integer(res[0], res, (len - 1 < 0 ? 0 : len - 1), len, GetResSign(a.sign, b.sign, 2)).swapForm();
}



big_integer operator|(big_integer const &left, big_integer const &right) {
    ///zero?
    big_integer a = left.swapForm();
    big_integer b = right.swapForm();
    if (left.iuLast == 0 && right.iuLast == 0) {
        return big_integer(get_small_o(a) | get_small_o(b), std::vector<uint>(0), 0, 0, GetResSign(a.sign, b.sign, 1)).swapForm();
    }
    uint *aReg = nullptr;
    uint *bReg = nullptr;
    a.GetBits(aReg);
    b.GetBits(bReg);

    size_t len = std::max(left.length, right.length);
    std::vector<uint> res(len);
    uint xE = (left.sign < 0) ? UINT32_MAX : 0;
    uint yE = (right.sign < 0) ? UINT32_MAX : 0;
    for (size_t i = 0; i < len; i++) {
        uint xu = (i < a.length) ? (*get_bits_o(a).get())[i] : xE;
        uint yu = (i < b.length) ? (*get_bits_o(b).get())[i] : yE;
        res[i] = xu | yu;
    }
    return big_integer(res[0], res, (len - 1 < 0 ? 0 : len - 1), len, GetResSign(a.sign, b.sign, 1)).swapForm();
}

big_integer operator^(big_integer const &left, big_integer const &right) {
    big_integer a = left.swapForm();
    big_integer b = right.swapForm();
    if (left.iuLast == 0 && right.iuLast == 0) {
        return big_integer(get_small_o(a) ^ get_small_o(b), std::vector<uint>(0), 0, 0, GetResSign(a.sign, b.sign, 3)).swapForm();
    }
    uint *aReg = nullptr;
    uint *bReg = nullptr;
    a.GetBits(aReg);
    b.GetBits(bReg);

    size_t len = std::max(left.length, right.length);
    std::vector<uint> res(len);
    uint xE = (left.sign < 0) ? UINT32_MAX : 0;
    uint yE = (right.sign < 0) ? UINT32_MAX : 0;
    for (size_t i = 0; i < len; i++) {
        uint xu = (i < a.length) ? (*get_bits_o(a).get())[i] : xE;
        uint yu = (i < b.length) ? (*get_bits_o(b).get())[i] : yE;
        res[i] = xu ^ yu;
    }
    return big_integer(res[0], res, (len - 1 < 0 ? 0 : len - 1), len, GetResSign(a.sign, b.sign, 3)).swapForm();
}

big_integer operator<<(big_integer const &left, uint b) {
    if (b == 0) { return left; }
    big_integer a = left.swapForm();
    if (a.iuLast == 0) {
        unsigned int tmp = get_small_o(a);
        a.SetSizeLazy(2);
        (*get_bits_o(a).get())[0] = tmp;
        a.length = 1;
    }


    size_t d = b >> 5;
    size_t sz = a.length + d;
    std::vector<uint> res(sz);
    res[0] = 324;
    size_t mod = b & (31);
    res[d] = uint(Ulong(a.get_digit(0)) << mod);
    for (size_t i = d + 1; i < sz; i++) {
        Ulong x = Ulong(a.get_digit(i - d)) << mod;
        Ulong y = Ulong((*get_bits_o(a).get())[i - d - 1]) >> (32 - mod);
        res[i] = uint(x | y);
    }

    if (sz == 1) {
        int resSz = res[0] * (a.sign ? 1 : -1);
        return big_integer(resSz);
    }

    return big_integer(res[0], res, (sz - 1 < 0 ? 0 : sz - 1), sz, a.sign).swapForm();
}

big_integer operator>>(big_integer const &left, uint b) {
    if (b == 0) { return left; }
    big_integer a = left.swapForm();
    if (a.iuLast == 0) {
        unsigned int tmp = get_small_o(a);
        a.SetSizeLazy(2);
        (*get_bits_o(a).get())[0] = tmp;
        a.length = 1;
    }

    size_t d = b >> (uint) 5;
    size_t mod = b & ((uint) 31);
    size_t sz = 0;
    if (d < a.length) {
        sz = a.length - d;
    }
    std::vector<uint> res(sz);

    for (size_t i = 0; i < sz; i++) {
        Ulong x = Ulong((*get_bits_o(a).get())[i + d]) >> mod;
        Ulong y = Ulong(a.get_digit(i + d + 1)) << (32 - mod);
        res[i] = (uint) (x | y);
    }
    if (sz == 1) {
        int resSz = res[0] * (a.sign ? 1 : -1);
        return big_integer(resSz);
    }


    big_integer RES = big_integer(res[0], res, (sz - 1 < 0 ? 0 : sz - 1), sz, a.sign);
    RES = RES.swapForm();
    return RES;
}

///------------- bit_operations ---------------
///

size_t big_integer::lenArray() {
    return static_cast<size_t>(length);
}

void big_integer::SetSizeLazy(size_t len) {
    if (len <= 1) {
        iuLast = 0;
        return;
    }
    std::shared_ptr<std::vector<unsigned int>> ptr;
    ptr.reset(new std::vector<unsigned int>(len));
    bits = ptr;
    length = len;
    iuLast = len - 1;
}

void big_integer::SetSizeKeep(size_t len, size_t extra) {
    if (len <= 1) {
        if (iuLast > 0) {
            unsigned int tmp = (*get_bits.get())[0];
            get_small = tmp;
        }
        iuLast = 0;
        return;
    }
    if (!(get_bits.unique())|| lenArray() < len) {
        std::vector<uint> rgu(static_cast<unsigned int>(len + extra));
        if (iuLast == 0)
            rgu[0] = get_small;
        else
            std::copy((*get_bits.get()).begin(), (*get_bits.get()).end(), rgu.begin());
        std::shared_ptr<std::vector<unsigned int>> ptr;
        ptr.reset(new std::vector<unsigned int>(len + extra));
        std::copy(rgu.begin(), rgu.end(), (*ptr.get()).begin());
        bits = ptr;
        length = len + extra;

    } else if (iuLast + 1 < len) {
        for (int i = 0; i < len - iuLast - 1; i++)
            (*get_bits.get())[iuLast + 1 + i] = 0;
        if (iuLast == 0)
            (*get_bits.get())[0] = get_small;
    }
    iuLast = len - 1;
}

void big_integer::Load(big_integer const &reg, size_t extra) {
    if (reg.iuLast == 0) {
        small = reg.small;
        sign = reg.sign;
        iuLast = 0;
    } else {
        if (!(get_bits_o(reg).unique()) || lenArray() <= reg.iuLast) {
            std::shared_ptr<std::vector<unsigned int>> ptr;
            ptr.reset(new std::vector<unsigned int>(reg.iuLast + 1 + extra));
            bits = ptr;
            length = reg.iuLast + 1 + extra;

        }
        sign = reg.sign;
        iuLast = reg.iuLast;
        std::copy((*get_bits_o(reg).get()).begin(), (*get_bits_o(reg).get()).begin() + iuLast + 1, (*get_bits.get()).begin());
    }
}

void big_integer::ensureWritable(size_t len, size_t extra) {
    if (iuLast == 0)
        return;
    if ((get_bits.unique()) && lenArray() >= len)
        return;

    std::vector<uint> rgu(static_cast<unsigned int>(len + extra));
    std::shared_ptr<std::vector<unsigned int>> ptr;
    ptr.reset(new std::vector<unsigned int>(len + extra));
    if (iuLast > 0) {
        if (iuLast >= len)
            iuLast = len - 1;
        std::copy((*get_bits.get()).begin(), (*get_bits.get()).begin() + iuLast + 1, (*ptr.get()).begin());

    }
    bits = ptr;
    length = len + extra;
    //fw = true;
}

void big_integer::ensureWritable(size_t extra) {
    if (iuLast == 0)
        return;
    if ((get_bits.unique()))
        return;
    std::shared_ptr<std::vector<unsigned int>> ptr;
    ptr.reset(new std::vector<unsigned int>(iuLast + 1 + extra));
    std::copy((*get_bits.get()).begin(), (*get_bits.get()).begin() + iuLast + 1, (*ptr.get()).begin());
    bits = ptr;
}


///++++++++++++ operations ++++++++++++++++++

void big_integer::Add(uint value) {
    if (iuLast == 0) {
        if ((get_small += value) >= value) {
            return;
        }
        SetSizeLazy(2);

        (*get_bits.get())[0] = get_small;
        (*get_bits.get())[1] = 1;

    }
    if (value == 0)
        return;
    ensureWritable(0);
    uint nNew = (*get_bits.get())[0] + value;
    if (nNew < value) {
        ensureWritable(1);
        int lnAdd = 1;
        for (;; lnAdd++) {
            if (lnAdd > iuLast) {
                if ((size_t) iuLast + 1 == lenArray()) {
                    std::vector<uint> tmp(static_cast<unsigned int>(length + 2));
                    copy((*get_bits.get()).begin(), (*get_bits.get()).begin() + lenArray(), tmp.begin());
                    swap(tmp, (*get_bits.get()));
                    length = length + 2;
                }
                (*get_bits.get())[++iuLast] = 1;
                break;
            }
            if (++(*get_bits.get())[lnAdd] > 0)
                break;
        }
    }
    //else if (!w) enseure();
    (*get_bits.get())[0] = nNew;
}

void big_integer::Add(big_integer const &reg) {
    if (reg.iuLast == 0) {
        Add(get_small_o(reg));
        return;
    }
    if (iuLast == 0) {
        uint u = get_small;
        if (u == 0) {
            *this = big_integer(reg);
        } else {
            Load(reg, 1);
            Add(u);
        }
        return;
    }

    ensureWritable((size_t)std::max(iuLast, reg.iuLast) + 1, 1);

    int lnAdd = reg.iuLast + 1;
    if (iuLast < reg.iuLast) {
        lnAdd = iuLast + 1;
        std::copy((*get_bits_o(reg).get()).begin() + iuLast + 1, (*get_bits_o(reg).get()).begin() + reg.length, (*get_bits.get()).begin() + iuLast + 1);
        iuLast = reg.iuLast;
    }
    uint Carry = 0;
    for (int i = 0; i < lnAdd; i++) {
        Carry = AddCarry((*get_bits.get())[i], (*get_bits_o(reg).get())[i], Carry);
    }
    if (Carry != 0) {
        for (;; lnAdd++) {
            if (lnAdd > iuLast) {
                if ((size_t) iuLast + 1 == lenArray()) {
                    std::vector<uint> tmp(static_cast<unsigned int>(length + 2));
                    copy((*get_bits.get()).begin(), (*get_bits.get()).begin() + lenArray(), tmp.begin());
                    swap((*get_bits.get()), tmp);
                    length = length + 2;
                }
                (*get_bits.get())[++iuLast] = 1;
                break;
            }
            if (++(*get_bits.get())[lnAdd] > 0)
                break;
        }
    }
    make_fit(*this);
}


void big_integer::Sub(bool &sign, unsigned int u) {
    if (iuLast == 0) {
        if (u <= get_small) {
            get_small -= u;
        } else {
            get_small = u - get_small;
            sign = !sign;
        }
        return;
    }

    if (u == 0)
        return;
    ensureWritable(0);

    uint tmp = (*get_bits.get())[0];
    (*get_bits.get())[0] = tmp - u;
    if (tmp < u) {
        for (int iu = 1; iu <= iuLast; iu++) {
            uint res = (*get_bits.get())[iu]--;
            if (res > 0)
                return;
        }
        Trim(*this);
    }
}

void big_integer::Sub(bool &sign, big_integer const &reg) {
    if (reg.iuLast == 0) {
        Sub(sign, get_small_o(reg));
        return;
    }
    if (iuLast == 0) {
        uint u = get_small;
        if (u == 0) {
            *this = big_integer(reg);
        } else {
            Load(reg, 0);
            Sub(sign, u);
        }
        sign = !sign;
        return;
    }

    if (iuLast < reg.iuLast) {
        SubRev(reg);
        sign = !sign;
        make_fit(*this);
        return;
    }

    int lnSub = reg.iuLast + 1;
    if (iuLast == reg.iuLast) {
        iuLast = getDiffLength((*get_bits.get()), (*get_bits_o(reg).get()), iuLast + 1) - 1;
        if (iuLast < 0) {
            iuLast = 0;
            get_small = 0;
            return;
        }
        uint u1 = (*get_bits.get())[iuLast];
        uint u2 = (*get_bits_o(reg).get())[iuLast];
        if (iuLast == 0) {
            if (u1 < u2) {
                get_small = u2 - u1;
                sign = !sign;
            } else {
                get_small = u1 - u2;
            }
            return;
        }

        if (u1 < u2) {
            SubRev(reg);
            sign = !sign;
            make_fit(*this);
            return;
        }
        lnSub = iuLast + 1;
    }

    ensureWritable(0);
    uint Borrow = 0;
    for (int i = 0; i < lnSub; i++) {
        Borrow = SubBorrow((*get_bits.get())[i], (*get_bits_o(reg).get())[i], Borrow);
    }
    if (Borrow != 0) {
        for (int i = lnSub; i <= iuLast; i++) {
            uint u = (*get_bits.get())[i]--;
            if (u > 0) {
                break;
            }
        }
    }
    Trim(*this);
}

void big_integer::SubRev(big_integer const &reg) {

    ensureWritable((size_t)reg.iuLast + 1, 0);

    int lnSub = iuLast + 1;
    if (iuLast < reg.iuLast) {
        std::copy((*get_bits_o(reg).get()).begin() + iuLast + 1, (*get_bits_o(reg).get()).begin() + reg.length, (*get_bits.get()).begin() + iuLast + 1);
        iuLast = reg.iuLast;
    }

    uint Borrow = 0;
    for (int i = 0; i < lnSub; i++) {
        Borrow = SubRevBorrow((*get_bits.get())[i], (*get_bits_o(reg).get())[i], Borrow);
    }
    if (Borrow != 0) {
        for (int i = lnSub; i <= iuLast; i++) {
            uint u = (*get_bits.get())[i]--;
            if (u > 0) {
                break;
            }
        }
    }
    Trim(*this);
}

void big_integer::Set(unsigned long long value) {
    uint uHi = (uint)(value >> bitUint);
    if (uHi == 0) {
        small = (uint)value;
        iuLast = 0;
    } else {
        SetSizeLazy(2);
        (*get_bits.get())[0] = (uint)value;
        (*get_bits.get())[1] = uHi;
    }
}

void big_integer::Mul(unsigned int u) {
    if (u == 0) {
        iuLast = 0;
        small = 0;
        return;
    }
    if (u == 1) {
        return;
    }
    if (iuLast == 0) {
        Set((unsigned long long)get_small * u);
        return;
    }
    ensureWritable(1);
    uint carry = 0;
    for (int iu = 0; iu <= iuLast; iu++) {
        carry = MulCarry((*get_bits.get())[iu], u, carry);
    }
    if (carry != 0) {
        SetSizeKeep((size_t)iuLast + 2, 0);
        (*get_bits.get())[iuLast] = carry;
    }
}

void big_integer::Mul(big_integer const &reg) {

    if (reg.iuLast == 0) {
        Mul(get_small_o(reg));
    } else if (iuLast == 0) {
        uint u = get_small;
        if (u == 1)
            *this = big_integer(reg);
        else if (u != 0) {
            Load(reg, 1);
            Mul(u);
        }
    } else {
        int lnBase = iuLast + 1;
        SetSizeKeep((size_t)lnBase + reg.iuLast, 1);

        for (int i = lnBase; --i >= 0;) {
            uint uMul = (*get_bits.get())[i];
            (*get_bits.get())[i] = 0;
            uint Carry = 0;
            for (int src = 0; src <= reg.iuLast; src++)
                Carry = AddMulCarry((*get_bits.get())[i + src], (*get_bits_o(reg).get())[src], uMul, Carry);
            if (Carry != 0) {
                for (int dst = i + reg.iuLast + 1; Carry != 0 && dst <= iuLast; dst++)
                    Carry = AddCarry((*get_bits.get())[dst], 0, Carry);
                if (Carry != 0) {
                    SetSizeKeep((size_t)iuLast + 2, 0);
                    (*get_bits.get())[iuLast] = Carry;
                }
            }
        }
    }
}

void Trim(big_integer &reg) {
    if (reg.iuLast > 0 && (*get_bits_o(reg).get())[reg.iuLast] == 0) {
        get_small_o(reg) = (*get_bits_o(reg).get())[0];
        while (--reg.iuLast > 0 && (*get_bits_o(reg).get())[reg.iuLast] == 0);
    }
}

void big_integer::ModDiv(big_integer &regF, big_integer const &regS, big_integer &regAns, bool MODE) {
    regAns.Set(0);
    if (regF.iuLast < regS.iuLast)
        return;

    int Den = regS.iuLast + 1;
    int Diff = regF.iuLast - regS.iuLast;

    int Ans = Diff;
    for (int i = regF.iuLast;; i--) {
        if (i < Diff) {
            Ans++;
            break;
        }
        if ((*get_bits_o(regS).get())[i - Diff] != (*get_bits_o(regF).get())[i]) {
            if ((*get_bits_o(regS).get())[i - Diff] < (*get_bits_o(regF).get())[i])
                Ans++;
            break;
        }
    }

    if (Ans == 0)
        return;
    if (MODE) {
        regAns.SetSizeLazy((size_t)Ans);
    }

    uint uDen = (*get_bits_o(regS).get())[Den - 1];
    uint uDenNext = (*get_bits_o(regS).get())[Den - 2];
    uint bitLeft = static_cast<uint>(getHighZero(uDen));
    uint bitRight = bitUint - bitLeft;
    if (bitLeft > 0) {
        uDen = (uDen << bitLeft) | (uDenNext >> bitRight);
        uDenNext <<= bitLeft;
        if (Den > 2) {
            uDenNext |= (*get_bits_o(regS).get())[Den - 3] >> bitRight;
        }
    }

    regF.ensureWritable(0);

    for (int i = Ans; --i >= 0;) {
        uint uNumHi = (i + Den <= regF.iuLast) ? (*get_bits_o(regF).get())[i + Den] : 0;

        Ulong Num = MakeULong(uNumHi, (*get_bits_o(regF).get())[i + Den - 1]);
        uint Next = (*get_bits_o(regF).get())[i + Den - 2];
        if (bitLeft > 0 ) {
            Num = (Num << bitLeft) | (Next >> bitRight);
            Next <<= bitLeft;
            if (i + Den >= 3) {
                Next |= (*get_bits_o(regF).get())[i + Den - 3] >> bitRight;
            }
        }

        Ulong left = Num / uDen;
        Ulong right = (uint) (Num % uDen);

        const uint UI_MAX = 4294967295;

        if (left > UI_MAX) {
            right += uDen * (left - UI_MAX);
            left = UI_MAX;
        }
        while ((right <= UI_MAX) && (left * uDenNext > MakeULong((uint) right, Next))) {
            left--;
            right += uDen;
        }

        if (left > 0) {
            Ulong Borrow = 0;
            for (int j = 0; j < Den; j++) {
                Borrow += (*get_bits_o(regS).get())[j] * left;
                uint Sub = (uint) Borrow;
                Borrow >>= bitUint;
                if ((*get_bits_o(regF).get())[i + j] < Sub)
                    Borrow++;
                (*get_bits_o(regF).get())[i + j] -= Sub;
            }

            if (uNumHi < Borrow) {
                uint Carry = 0;
                for (int j = 0; j < Den; j++) {
                    Carry = AddCarry((*get_bits_o(regF).get())[i + j], (*get_bits_o(regS).get())[j], Carry);
                }
                left--;
            }
            regF.iuLast = i + Den - 1;
        }
        if (MODE) {
            if (Ans == 1) {
                get_small_o(regAns) = (uint) left;
            }
            else {
                (*get_bits_o(regAns).get())[i] = (uint) left;
            }
        }
    }
    regF.iuLast = Den - 1;
    Trim(regF);
}

void big_integer::Div(big_integer const &regS) {
    if (regS.iuLast == 0) {
        div(get_small_o(regS));
        return;
    }
    if (iuLast == 0) {
        small = 0;
        return;
    }
    big_integer regAns(0);
    ModDiv(*this, regS, regAns, true);
    mswap(*this, regAns);
}

void big_integer::Mod(big_integer const &regS) {
    if (regS.iuLast == 0) {
        Set(mod(*this, get_small_o(regS)));
        return;
    }
    if (iuLast == 0)
        return;
    big_integer regAns(0);
    ModDiv(*this, regS, regAns, false);
}

///---------------- operations ----------------------
///
///
///

void make_fit(big_integer &a) {
    Trim(a);
}

void print(big_integer const &a) {
    if (a.iuLast == 0) {
        std::cout << 0 << " " << 0 << std::endl;
        std::cout << get_small_o(a) << std::endl;
        return;
    }
    std::cout << a.iuLast << " " << a.length << " " << (*get_bits_o(a).get()).size() << " " << get_bits_o(a).use_count() << "\n";
    for (size_t i = 0; i < a.length; i++) {
        std::cout << (*get_bits_o(a).get())[i] << " ";
    }
    std::cout << std::endl;
}

std::string to_string(big_integer tmp) {
    big_integer a = tmp;
    a.ensureWritable(0);
    if (a.iuLast == 0) {
        std::string ans = "";
        if (!a.sign)
            ans = "-";
        return ans + std::to_string(get_small_o(a));
    }
    if (a.length == 1 && (*get_bits_o(a).get())[0] == 0) { return "0"; }
    std::string ans;
    big_integer b = a;
    big_integer base((int) 1e9);
    while (!(a.iuLast == 0 && get_small_o(a) == 0)) {
        b %= base;
        uint tmp_digit = (b.iuLast == 0 ? get_small_o(b) : (*get_bits_o(b).get())[0]);
        for (size_t i = 0; i < 9; i++) {
            ans.push_back(static_cast<char>('0' + tmp_digit % 10));
            tmp_digit /= 10;
        }

        a /= base;
        b = a;
    }
    while (!ans.empty() && ans.back() == '0') {
        ans.pop_back();
    }
    if (a.GetReadSign() == 0) {
        ans.push_back('-');
    }
    reverse(ans.begin(), ans.end());
    return ans;
}

///++++++++++++other_function++++++++++++++++++

big_integer big_integer::swapForm() const {
    if (sign) {
        return *this;
    }
    big_integer res = *this;
    if (iuLast == 0) {
        get_small_o(res) = ~get_small_o(res);
        res -= 1;
        return res;
    }
    for (size_t i = 0; i < res.length; i++) {
        (*get_bits_o(res).get())[i] = ~(*get_bits_o(res).get())[i];
    }
    res -= 1;
    return res;
}

uint big_integer::AddCarry(uint &u1, uint u2, uint carry) {
    Ulong uu = (Ulong) u1 + u2 + carry;
    u1 = (uint) uu;
    return (uint) (uu >> bitUint);
}

uint big_integer::SubBorrow(uint &u1, uint u2, uint borrow) {
    Ulong uu = (Ulong) u1 - u2 - borrow;
    u1 = (uint) uu;
    return (uint) -(int) (uu >> bitUint);
}

uint big_integer::SubRevBorrow(uint &u1, uint u2, uint uBorrow) {
    Ulong uu = (Ulong) u2 - u1 - uBorrow;
    u1 = (uint) uu;
    return (uint) -(int) (uu >> bitUint);
}

uint big_integer::AddMulCarry(uint &add, uint mul1, uint mul2, uint carry) {
    Ulong res = (Ulong) mul1 * mul2 + add + carry;
    add = (uint) res;
    return (uint) (res >> bitUint);
}

unsigned int big_integer::MulCarry(unsigned int &u1, unsigned int u2, unsigned int carry) {
    unsigned long long res = (Ulong)u1 * u2 + carry;
    u1 = (uint)res;
    return (uint)(res >> bitUint);
}


uint big_integer::get_digit(size_t it) const {
    if (it < length) {
        return (*get_bits.get())[it];
    } else if (!sign) {
        return UINT32_MAX;
    } else {
        return 0;
    }
}

int big_integer::getDiffLength(std::vector<uint> const &reg1, std::vector<uint> const &reg2, int cu) {
    for (int i = cu; --i >= 0;) {
        if (reg1[i] != reg2[i])
            return i + 1;
    }
    return 0;
}

int big_integer::getHighZero(uint u) {
    if (u == 0)
        return 32;

    int bit = 0;
    if ((u & 0xFFFF0000) == 0) bit += 16, u <<= 16;
    if ((u & 0xFF000000) == 0) bit += 8, u <<= 8;
    if ((u & 0xF0000000) == 0) bit += 4, u <<= 4;
    if ((u & 0xC0000000) == 0) bit += 2, u <<= 2;
    if ((u & 0x80000000) == 0) bit += 1;
    return bit;
}

Ulong big_integer::MakeULong(uint H, uint L) {
    return ((Ulong) H << bitUint) | L;
}

void big_integer::mswap(big_integer &a, big_integer &b) {
   big_integer tmp = a;
   a = b;
   b = tmp;
}

uint big_integer::mod(big_integer &regF, uint uDen) {
    if (uDen == 1)
        return 0;
    if (regF.iuLast == 0)
        return get_small_o(regF) % uDen;

    Ulong uu = 0;
    for (int i = regF.iuLast; i >= 0; i--) {
        uu = MakeULong((uint) uu, (*get_bits_o(regF).get())[i]);
        uu %= uDen;
    }
    return (uint) uu;
}

uint big_integer::div(uint uDen) {
    if (uDen == 1)
        return 0;
    if (iuLast == 0) {
        if (uDen == 0) {
            get_small = 0;
            return 0;
        }
        uint tmp = get_small;
        get_small = tmp / uDen;
        iuLast = 0;
        return tmp % uDen;
    }

    ensureWritable(0);

    Ulong uu = 0;
    for (int i = iuLast; i >= 0; i--) {
        uu = MakeULong((uint) uu, (*get_bits.get())[i]);
        (*get_bits.get())[i] = (uint) (uu / uDen);
        uu %= uDen;
    }
    Trim(*this);
    return (uint) uu;
}

///-----------other_function-------------------

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
    s << to_string(a);
    return s;
}