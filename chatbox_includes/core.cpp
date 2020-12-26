#include "core.h"

namespace UI {
	bool setUpDarkTheme() {
		QFile f(":qdarkstyle/style.qss");
		if (!f.exists()) {
			return false;
		}
		else {
			f.open(QFile::ReadOnly | QFile::Text);
			QTextStream ts(&f);
			qApp->setStyleSheet(ts.readAll());
			return true;
		}
	}
}