TODO:
1. handle inf, denor etc... done
2. random test case... done
3. double precision.
4. internal checker


Points:
Subnormal Numbers: When all the exponent bits are 0 and the leading hidden bit of the siginificand is 0, then the floating point number is called a subnormal number. Thus, one logical representation of a subnormal number is

(–1)s × 0.f × 2–127 (all 0s for the exponent) ,

where f has at least one 1 (otherwise the number will be taken as 0). However, the standard uses –126, i.e., bias +1 for the exponent rather than –127 which is the bias for some not so obvious reason, possibly because by using –126 instead of –127, the gap between the largest subnormal number and the smallest normalized number is smaller.

The largest subnormal number is 0.999999988×2–126. It is close to the smallest normalized number 2–126.

// First 1 bit for characterstic & last 2 bits are guard bit and round bit
