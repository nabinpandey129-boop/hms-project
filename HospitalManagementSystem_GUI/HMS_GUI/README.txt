Hospital Management System - GUI edition
========================================
RUN (Windows):  .\hms.exe   (from inside this folder) -> your browser opens the app.
                Keep the black window open while using it. Close it to stop.
                Data is saved in the data\ folder next to where you run it.

BUILD:  double-click build.bat
        or:  g++ -std=c++17 -O2 -o hms.exe main.cpp -lws2_32
        Linux/Mac:  g++ -std=c++17 -o hms main.cpp -lpthread && ./hms

EDITING THE INTERFACE:  web\index.html, web\style.css, web\js\*.js
        After any change run:  python web\make_page.py   then rebuild
        (build.bat does both if Python is installed).

TEAM SPLIT: see TEAM_OWNERSHIP.txt
CODE GUIDE (where to find login, appointments, menus ... in the C++ code): see CODE_GUIDE.txt

Logins:  admin01 / admin123   recep01 / recep123   dr.sharma / doc123   dr.rai / doc123
Reset all data: close the program and delete the data\ folder.
