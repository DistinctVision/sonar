#ifndef SONAR_MACROS_H
#define SONAR_MACROS_H

#define SONAR_UNUSED(x) static_cast<void>(x)
#define SONAR_PTR2STR(data) ((data != nullptr) ? "pointer" : "null")

#endif // SONAR_MACROS_H
