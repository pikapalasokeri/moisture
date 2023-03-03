#ifndef MODE_CHECKER_H
#define MODE_CHECKER_H

enum class Mode
{
  MoistureReading = 0,
  Pinging
};

Mode getMode();

#endif  // MODE_CHECKER_H
