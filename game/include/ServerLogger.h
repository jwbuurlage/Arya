#include "Arya.h"

#ifdef LOG_WARNING
#undef LOG_WARNING
#undef LOG_ERROR
#undef LOG_CRITICALERROR
#undef LOG_INFO
#undef LOG_DEBUG
#endif

#define LOG_WARNING(MSG)		*ServerLogger << Arya::Logger::L_WARNING		<< MSG << Arya::endLog
#define LOG_ERROR(MSG)			*ServerLogger << Arya::Logger::L_ERROR		<< MSG << Arya::endLog
#define LOG_CRITICALERROR(MSG)	*ServerLogger << Arya::Logger::L_CRITICALERROR	<< MSG << Arya::endLog
#define LOG_INFO(MSG)			*ServerLogger << Arya::Logger::L_INFO		<< MSG << Arya::endLog
#define LOG_DEBUG(MSG)			*ServerLogger << Arya::Logger::L_DEBUG		<< MSG << Arya::endLog

extern Arya::Logger* ServerLogger;

