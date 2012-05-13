/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#ifndef WI_CSS_H
#define	WI_CSS_H

#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

	typedef enum cssTheme_e {
		CSS_THEME_MIN,
		CSS_THEME_0 = CSS_THEME_MIN,
		CSS_THEME_1,
		CSS_THEME_MAX = CSS_THEME_1
	} cssTheme_t;

	extern const char * const cssTheme[];

	struct wcontext_s;

	void printApplyThemeJavaScript(struct wcontext_s *wctx, const char * fieldId, bool cookie);

	void printCssThemeSelector(struct wcontext_s *wctx, const char * id);

#ifdef	__cplusplus
}
#endif

#endif	/* CSS_H */

