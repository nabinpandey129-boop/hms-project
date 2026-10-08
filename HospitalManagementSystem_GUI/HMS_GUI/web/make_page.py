# Builds web/Page.h from index.html + style.css + js/*.js
# (the browser page is embedded inside hms.exe). Run after editing any file in web/.
import re, os
base = os.path.dirname(os.path.abspath(__file__))
read = lambda p: open(os.path.join(base, p), encoding="utf-8").read()

html = read("index.html")
html = re.sub(r'<link rel="stylesheet" href="([^"]+)">',
              lambda m: "<style>\n" + read(m.group(1)) + "</style>", html)
html = re.sub(r'<script src="([^"]+)"></script>',
              lambda m: "<script>\n" + read(m.group(1)) + "</script>", html)

with open(os.path.join(base, "Page.h"), "w", encoding="utf-8") as f:
    f.write('#pragma once\nstatic const char* kPage = R"HMSPAGE(' + html + ')HMSPAGE";\n')
print("web/Page.h updated")
