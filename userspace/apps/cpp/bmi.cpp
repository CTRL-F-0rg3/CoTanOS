// bmi — body mass index from weight (kg) and height (cm). No floating point.
#include "apps.hpp"

extern "C" int cpp_bmi(const char* args, char* out, int cap) {
    Out o{out, cap, 0};

    const char* p = args;
    long long kg = atoll_simple(p);
    long long cm = atoll_simple(p);

    if (kg <= 0 || cm <= 0) {
        o.put("usage: bmi <weight_kg> <height_cm>"); o.nl();
        o.finish();
        return 1;
    }

    // BMI = kg / (m^2) = kg * 100000 / cm^2, scaled x10 for one decimal.
    long long bmi = kg * 100000 / (cm * cm);
    o.put("BMI = "); o.put_int(bmi / 10); o.put('.'); o.put_int(bmi % 10);
    o.put("  (");
    if (bmi < 185) o.put("underweight");
    else if (bmi < 250) o.put("normal");
    else if (bmi < 300) o.put("overweight");
    else o.put("obese");
    o.put(")"); o.nl();
    o.finish();
    return 0;
}
