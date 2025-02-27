/*  Waterfill Tile (64-bit integer)
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "Kernels/Kernels_BitScan.h"
#include "Kernels_Waterfill_Tile_Default.h"

namespace PokemonAutomation{
namespace Kernels{
namespace Waterfill{



bool find_bit(size_t& x, size_t& y, const BinaryTile_Default& tile){
    uint64_t anything = tile.vec[0];
    anything |= tile.vec[1];
    anything |= tile.vec[2];
    anything |= tile.vec[3];
    if (!anything){
        return false;
    }
    for (size_t c = 0; c < 4; c++){
        size_t pos;
        if (trailing_zeros(pos, tile.vec[c])){
            x = pos;
            y = c;
            return true;
        }
    }
    return false;
}



void boundaries(
    const BinaryTile_Default& tile,
    size_t& min_x, size_t& max_x,
    size_t& min_y, size_t& max_y
){
    uint64_t all_or = 0;
    all_or |= tile.vec[0];
    all_or |= tile.vec[1];
    all_or |= tile.vec[2];
    all_or |= tile.vec[3];
    trailing_zeros(min_x, all_or);
    max_x = bitlength(all_or);

    for (size_t c = 0; c < 4; c++){
        if (tile.vec[c] != 0){
            min_y = c;
            break;
        }
    }
    for (size_t c = 4; c > 0; c--){
        if (tile.vec[c - 1] != 0){
            max_y = c;
            break;
        }
    }
}



uint64_t popcount_indexsum(uint64_t& sum_index, uint64_t x){
    //  1 -> 2
    uint64_t sum_high;
    uint64_t pop_high = (x >> 1) & 0x5555555555555555;
    uint64_t sumxaxis = pop_high;
    uint64_t popcount = (x & 0x5555555555555555) + pop_high;

    //  2 -> 4
    sum_high = (sumxaxis >> 2) & 0x3333333333333333;
    pop_high = (popcount >> 2) & 0x3333333333333333;
    sumxaxis = (sumxaxis & 0x3333333333333333) + sum_high;
    sumxaxis += pop_high << 1;
    popcount = (popcount & 0x3333333333333333) + pop_high;

    //  4 -> 8
    sum_high = (sumxaxis >> 4) & 0x0f0f0f0f0f0f0f0f;
    pop_high = (popcount >> 4) & 0x0f0f0f0f0f0f0f0f;
    sumxaxis = (sumxaxis & 0x0f0f0f0f0f0f0f0f) + sum_high;
    sumxaxis += pop_high << 2;
    popcount = (popcount & 0x0f0f0f0f0f0f0f0f) + pop_high;

    //  8 -> 16
    sum_high = (sumxaxis >> 8) & 0x00ff00ff00ff00ff;
    pop_high = (popcount >> 8) & 0x00ff00ff00ff00ff;
    sumxaxis = (sumxaxis & 0x00ff00ff00ff00ff) + sum_high;
    sumxaxis += pop_high << 3;
    popcount = (popcount & 0x00ff00ff00ff00ff) + pop_high;

    //  16 -> 32
    sum_high = (sumxaxis >> 16) & 0x0000ffff0000ffff;
    pop_high = (popcount >> 16) & 0x0000ffff0000ffff;
    sumxaxis = (sumxaxis & 0x0000ffff0000ffff) + sum_high;
    sumxaxis += pop_high << 4;
    popcount = (popcount & 0x0000ffff0000ffff) + pop_high;

    //  32 -> 64
    sum_high = sumxaxis >> 32;
    pop_high = popcount >> 32;
    sumxaxis += sum_high;
    sumxaxis += pop_high << 5;
    popcount += pop_high;

    sum_index = (uint32_t)sumxaxis;
    return (uint32_t)popcount;
}
uint64_t popcount_sumcoord(
    uint64_t& sum_xcoord, uint64_t& sum_ycoord,
    const BinaryTile_Default& tile
){
    uint64_t sum_p = 0;
    uint64_t sum_x = 0;
    uint64_t sum_y = 0;
    {
        uint64_t sum;
        sum_p += popcount_indexsum(sum, tile.vec[0]);
        sum_x += sum;
    }
    {
        uint64_t pop, sum;
        pop = popcount_indexsum(sum, tile.vec[1]);
        sum_p += pop;
        sum_x += sum;
        sum_y += pop;
    }
    {
        uint64_t pop, sum;
        pop = popcount_indexsum(sum, tile.vec[2]);
        sum_p += pop;
        sum_x += sum;
        sum_y += pop * 2;
    }
    {
        uint64_t pop, sum;
        pop = popcount_indexsum(sum, tile.vec[3]);
        sum_p += pop;
        sum_x += sum;
        sum_y += pop * 3;
    }
    sum_xcoord = sum_x;
    sum_ycoord = sum_y;
    return sum_p;
}



PA_FORCE_INLINE uint64_t bitreverse64(uint64_t x){
    uint64_t r0, r1;

#if 0
#elif __GNUC__
    r0 = __builtin_bswap64(x);
#elif __INTEL_COMPILER
    r0 = _bswap64(x);
#elif _MSC_VER
    r0 = _byteswap_uint64(x);
#else
#error "No byte-swap for this compiler."
#endif

    r1 = r0 >> 4;
    r0 = r0 << 4;
    r1 &= 0x0f0f0f0f0f0f0f0full;
    r0 &= 0xf0f0f0f0f0f0f0f0ull;
    r0 |= r1;

    r1 = r0 >> 2;
    r0 = r0 << 2;
    r1 &= 0x3333333333333333ull;
    r0 &= 0xccccccccccccccccull;
    r0 |= r1;

    r1 = r0 >> 1;
    r0 = r0 << 1;
    r1 &= 0x5555555555555555ull;
    r0 &= 0xaaaaaaaaaaaaaaaaull;
    r0 |= r1;

    return r0;
}


struct ProcessedMask{
    uint64_t m0, m1, m2, m3; //  Copy of the masks.
    uint64_t b0, b1, b2, b3; //  Bit-reversed copy of the masks.

    PA_FORCE_INLINE ProcessedMask(
        const BinaryTile_Default& m,
        uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3
    ){
        m0 = x0 | m.vec[0];
        m1 = x1 | m.vec[1];
        m2 = x2 | m.vec[2];
        m3 = x3 | m.vec[3];

        b0 = bitreverse64(m0);
        b1 = bitreverse64(m1);
        b2 = bitreverse64(m2);
        b3 = bitreverse64(m3);
    }
};


PA_FORCE_INLINE void expand_reverse(uint64_t m, uint64_t b, uint64_t& x){
    uint64_t s = bitreverse64(bitreverse64(x) + b);
    s ^= m;
    s &= m;
    x |= s;
}


PA_FORCE_INLINE void expand_forward(
    const ProcessedMask& mask,
    uint64_t& x0, uint64_t& x1, uint64_t& x2, uint64_t& x3
){
    uint64_t s0 = x0 + mask.m0;
    uint64_t s1 = x1 + mask.m1;
    uint64_t s2 = x2 + mask.m2;
    uint64_t s3 = x3 + mask.m3;

    s0 ^= mask.m0;
    s1 ^= mask.m1;
    s2 ^= mask.m2;
    s3 ^= mask.m3;

    s0 &= mask.m0;
    s1 &= mask.m1;
    s2 &= mask.m2;
    s3 &= mask.m3;

    x0 |= s0;
    x1 |= s1;
    x2 |= s2;
    x3 |= s3;
}
PA_FORCE_INLINE void expand_reverse(
    const ProcessedMask& mask,
    uint64_t& x0, uint64_t& x1, uint64_t& x2, uint64_t& x3
){
    expand_reverse(mask.m0, mask.b0, x0);
    expand_reverse(mask.m1, mask.b1, x1);
    expand_reverse(mask.m2, mask.b2, x2);
    expand_reverse(mask.m3, mask.b3, x3);
}
PA_FORCE_INLINE void expand_vertical(
    const ProcessedMask& mask,
    uint64_t& x0, uint64_t& x1, uint64_t& x2, uint64_t& x3
){
    x1 |= x0 & mask.m1;
    x2 |= x3 & mask.m2;
    x2 |= x1 & mask.m2;
    x1 |= x2 & mask.m1;
    x3 |= x2 & mask.m3;
    x0 |= x1 & mask.m0;
}

void waterfill_expand(const BinaryTile_Default& m, BinaryTile_Default& x){
    uint64_t x0 = x.vec[0];
    uint64_t x1 = x.vec[1];
    uint64_t x2 = x.vec[2];
    uint64_t x3 = x.vec[3];

    ProcessedMask mask(m, x0, x1, x2, x3);

    uint64_t changed;
    do{
        expand_forward(mask, x0, x1, x2, x3);
        expand_vertical(mask, x0, x1, x2, x3);
        expand_reverse(mask, x0, x1, x2, x3);
        changed  = x0 ^ x.vec[0];
        changed |= x1 ^ x.vec[1];
        changed |= x2 ^ x.vec[2];
        changed |= x3 ^ x.vec[3];
        x.vec[0] = x0;
        x.vec[1] = x1;
        x.vec[2] = x2;
        x.vec[3] = x3;
//        cout << x.dump() << endl;
    }while (changed);
}



bool waterfill_touch_top(const BinaryTile_Default& mask, BinaryTile_Default& tile, const BinaryTile_Default& border){
    uint64_t available = mask.vec[0] & ~tile.vec[0];
    uint64_t new_bits = available & border.vec[3];
    if (new_bits == 0){
        return false;
    }
    tile.vec[0] |= new_bits;
    return true;
}
bool waterfill_touch_bottom(const BinaryTile_Default& mask, BinaryTile_Default& tile, const BinaryTile_Default& border){
    uint64_t available = mask.vec[3] & ~tile.vec[3];
    uint64_t new_bits = available & border.vec[0];
    if (new_bits == 0){
        return false;
    }
    tile.vec[3] |= new_bits;
    return true;
}
bool waterfill_touch_left(const BinaryTile_Default& mask, BinaryTile_Default& tile, const BinaryTile_Default& border){
    bool changed = false;
    for (size_t c = 0; c < 4; c++){
        uint64_t available = mask.vec[c] & ~tile.vec[c];
        uint64_t new_bits = available & (border.vec[c] >> 63);
        changed |= new_bits != 0;
        tile.vec[c] |= new_bits;
    }
    return changed;
}
bool waterfill_touch_right(const BinaryTile_Default& mask, BinaryTile_Default& tile, const BinaryTile_Default& border){
    bool changed = false;
    for (size_t c = 0; c < 4; c++){
        uint64_t available = mask.vec[c] & ~tile.vec[c];
        uint64_t new_bits = available & (border.vec[c] << 63);
        changed |= new_bits != 0;
        tile.vec[c] |= new_bits;
    }
    return changed;
}












}
}
}
