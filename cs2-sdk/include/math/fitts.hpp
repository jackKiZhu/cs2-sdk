#pragma once

#include <cmath>

// class for the fitts' law model
class CFitts {
   public:
    CFitts() = delete;
    CFitts(float width, float distance, float a = 0.1f, float b = 0.2f)
        : width(width),
          distance(distance),
          a(a), b(b) {}

    float Compute(float a, float b) { 
      return a + b * std::log2f(2 * distance / width);
    }

    float Compute() { 
      return a + b * std::log2f(2 * distance / width);
    }

   private:
    float width;     // width of the target (along the axis of movement, here it's x)
    float distance;  // distance from the starting point to the target
    float a;         // coefficient of the model
    float b;         // coefficient of the model
};
