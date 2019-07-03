//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Logging macros
//

#if LOG_ENABLED
#define logRaw(msg) logBuffer.print(msg);Serial.print(msg);
#else
#define logRaw(msg)
#endif

#define logLine() logRaw('\n')
#define logHeader() logRaw('#');logRaw(millis());logRaw(" | ");
#define logInfo(msg) logHeader();logRaw(F(msg));logLine()
#define logValue(msg,val) logHeader();logRaw(F(msg));logRaw(val);logLine()
