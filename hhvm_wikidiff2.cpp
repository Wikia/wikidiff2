/* $Id$ */

#include "hphp/util/lock.h"
#include "hphp/runtime/ext/extension.h"
#include "hphp/util/compatibility.h"
#include "hphp/util/alloc.h"
#include "Wikidiff2.h"
#include "TableDiff.h"
#include "InlineDiff.h"

#include <string>

namespace HPHP {

/* {{{ proto string wikidiff2_do_diff(string text1, string text2, int numContextLines, int maxMovedLines = 25)
 *
 * Warning: the input text must be valid UTF-8! Do not pass user input directly
 * to this function.
 */
static String HHVM_FUNCTION(wikidiff2_do_diff,
	const String& text1,
	const String& text2,
	int64_t numContextLines,
	int64_t maxMovedLines)
{
    String result;
	try {
		TableDiff wikidiff2;
		Wikidiff2::String text1String(text1.c_str());
		Wikidiff2::String text2String(text2.c_str());
		result = wikidiff2.execute(text1String, text2String, numContextLines, maxMovedLines);
	} catch (OutOfMemoryException &e) {
		raise_error("Out of memory in wikidiff2_do_diff().");
	} catch (...) {
		raise_error("Unknown exception in wikidiff2_do_diff().");
	}
	return result;
}

/* {{{ proto string wikidiff2_inline_diff(string text1, string text2, int numContextLines, int maxMovedLines)
 *
 * Warning: the input text must be valid UTF-8! Do not pass user input directly
 * to this function.
 */
static String HHVM_FUNCTION(wikidiff2_inline_diff,
	const String& text1,
	const String& text2,
	int64_t numContextLines,
	int64_t maxMovedLines)
{
    String result;
	try {
		InlineDiff wikidiff2;
		Wikidiff2::String text1String(text1.c_str());
		Wikidiff2::String text2String(text2.c_str());
		result = wikidiff2.execute(text1String, text2String, numContextLines, 0 /*inlinediff todo*/);
	} catch (OutOfMemoryException &e) {
		raise_error("Out of memory in wikidiff2_do_diff().");
	} catch (...) {
		raise_error("Unknown exception in wikidiff2_do_diff().");
	}
	return result;
}

/* {{{ proto string wikidiff2_version()
 */
static String HHVM_FUNCTION(wikidiff2_version)
{
    String version = WIKIDIFF2_VERSION_STRING;
	return version;
}

// ini settings settable anywhere (PHP_INI_ALL)
thread_local struct {
	double changeThreshold;
	double movedLineThreshold;
} s_ini;

static class Wikidiff2Extension : public Extension {
	public:
		Wikidiff2Extension() : Extension("wikidiff2", WIKIDIFF2_VERSION_STRING) {}
		virtual void moduleInit() {
			HHVM_FE(wikidiff2_do_diff);
			HHVM_FE(wikidiff2_inline_diff);
			HHVM_FE(wikidiff2_version);
			loadSystemlib();
		}
		void threadInit() override {
			IniSetting::Bind(this, IniSetting::PHP_INI_ALL, "wikidiff2.change_threshold", WIKIDIFF2_CHANGE_THRESHOLD_DEFAULT, &s_ini.changeThreshold);
			IniSetting::Bind(this, IniSetting::PHP_INI_ALL, "wikidiff2.moved_line_threshold", WIKIDIFF2_MOVED_LINE_THRESHOLD_DEFAULT, &s_ini.movedLineThreshold);
		}
} s_wikidiff2_extension;

HHVM_GET_MODULE(wikidiff2)

} // namespace HPHP
