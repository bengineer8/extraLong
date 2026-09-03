#include <vector>
#include <iostream>
#include <cstdint>

const char hexchars[] = "0123456789ABCDEF";
class extraLong{
    public:
        std::vector<uint64_t> val;

        extraLong (int64_t n){ val = {n}; }

        extraLong(){}

        bool isNeg(const extraLong& N){ return isNeg(N.val); }

        bool isNeg(const std::vector<uint64_t>& v){ return v.size() > 0 && (v.back()>>63) == 1; }

        uint64_t ulongat(const std::vector<uint64_t>& v, int n){
           if(n > -1){
                if(v.size() > n) return v[n];
                if(isNeg(v)) return -1;
            }
            return 0;
        }

        uint64_t ulongat(const int64_t& N, int n){
            if(n > 0){
                if(N < 0) return -1;
                return 0;
            }
            return N;
        }

        void prune(std::vector<uint64_t>& v){
            for(int n = v.size()-1; n > 0 && ( (v[n] == 0 && v[n-1]>>63 == 0) || (v[n] == -1 && v[n-1]>>63 == 1) ); n--) v.pop_back();
        }

        extraLong addosub(const std::vector<uint64_t> &a, const std::vector<uint64_t> &b, bool sub){
            uint64_t mask;
            if(sub) mask = -1;
            else mask = 0;
            extraLong c;
            int maxleng = std::max(a.size(),b.size()) + 1;
            c.val.resize(maxleng);
            int n = 0;
            bool carry = sub;
            while(n < maxleng){
                uint64_t A = ulongat(a,n), B = mask^ulongat(b,n);
                bool nextCarry = (A > -1 - B);
                c.val[n] = A + B;
                if(carry){
                    c.val[n]++;
                    nextCarry |= c.val[n] == 0;
                }
                carry = nextCarry;
                n++;
            }
            prune(c.val);
            return c;
        }

        extraLong addosub(const std::vector<uint64_t> &a, int64_t b, bool sub){
            uint64_t mask;
            if(sub) mask = -1;
            else mask = 0;
            extraLong c;
            c.val.resize(a.size() + 1);
            int n = 0;
            bool carry = sub;
            while(n < c.val.size()){
                uint64_t A = ulongat(a,n), B = mask^ulongat(b,n);
                bool nextCarry = (A > -1 - B);
                c.val[n] = A + B;
                if(carry){
                    c.val[n]++;
                    nextCarry |= c.val[n] == 0;
                }
                carry = nextCarry;
                n++;
            }
            prune(c.val);
            return c;
        }

        void operator= (int64_t n){//this feels weird
            val.resize(1);
            val[0] = n;
        }

        extraLong operator+ (const extraLong &b){ return addosub(val,b.val,false); }

        extraLong operator+ (const int64_t b){ return addosub(val,b,false); }

        extraLong operator- (const extraLong &b){ return addosub(val,b.val,true); }

        extraLong operator- (const int64_t &b){ return addosub(val,b,true); }

        extraLong operator- (){ return addosub({0},val,true); }

        extraLong smallshiftlr(std::vector<uint64_t> v, int shift){//TODO make this more effecient
            extraLong N;
            if(shift > 0){//shift left
                N.val.resize(v.size() + 1);
                uint64_t ul1 = 0, ul2;
                for(int n = 0; n < N.val.size(); n++){
                    ul2 = ulongat(v,n)<<shift;
                    N.val[n] = ul2 | ul1;
                    ul1 = ulongat(v,n)>>(64 - shift);
                }
            } else if(shift < 0) {//shift right
                N.val.resize(v.size());
                for(int n = 0; n < N.val.size(); N.val[n++] = (ulongat(v,n)>>-shift) | (ulongat(v,n+1)<<(64 + shift)) );
            } else {
                N.val.resize(v.size());
                for(int n = 0; n < N.val.size();  N.val[n++] = v[n]);
            }
            prune(N.val);
            return N;
        }

        extraLong greatshiftlr(std::vector<uint64_t> v, int shift){//TODO make this more effecient
            int gshift;
            if(shift < 0){
                gshift = -((-shift)>>6);
            } else {
                gshift = shift>>6;
            }
            shift = shift%64;//for once, the default behavior with negatives is wanted
            extraLong N;
            if(gshift + int(v.size()) > 0){
                N.val.resize(v.size() + gshift);
                for(int n = 0; n < N.val.size(); N.val[n++] = ulongat(v,n - gshift));
            } else {
                if(isNeg(v)) N.val.push_back(-1);
                else N.val.push_back(0);
            }
            return smallshiftlr(N.val,shift);
        }

        extraLong operator<< (int shift){ return greatshiftlr(val,shift); }

        extraLong operator>> (int shift){ return greatshiftlr(val,-shift); }

        bool operator== (extraLong B){
            if(val.size() != B.val.size() || isNeg(val)^isNeg(B)) return false;
            bool equal = true;
            for(int n = 0; n < val.size() && equal; equal = val[n] == B.val[n++]);
            return equal;
        }

        bool operator== (int64_t B){ return val.size() == 1 && val[0] == B; }
};

bool isNeg(extraLong N){ return N.val.size() > 0 && (N.val.back()>>63) == 1; }


int sign(const extraLong& N){
    if(isNeg(N)) return -1;
    return 1;
}


uint64_t ulongat(const extraLong& N, int n){
    if(n > -1){
        if(N.val.size() > n) return N.val[n];
        if(isNeg(N)) return -1;
    }
    return 0;
}

void printel(extraLong N){//prints the raw 2s compliment data in hex
    for(int n = N.val.size() - 1; n >= 0; n--){
        for(int x = 60; x >= 0; x-=4){
            printf("%c",hexchars[ (N.val[n]>>x)&15 ]);
        }
    }
    printf("\n");
}
