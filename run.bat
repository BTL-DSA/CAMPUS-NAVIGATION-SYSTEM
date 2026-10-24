@echo off
chcp 65001 >nul
title UTC2 Campus Navigation System - Pure C++ Web Server
echo ================================================================================
echo        HE THONG DAN DUONG KHUON VIEN UTC2 - CAMPUS NAVIGATION SYSTEM
echo        Truong Dai hoc Giao thong Van tai - Phan hieu tai TP.HCM
echo        Pure C++ Web Server (cpp-httplib ^& nlohmann/json)
echo ================================================================================
echo.

cd /d "%~dp0"

echo [1/3] Kiem tra trinh bien dich C++ (MinGW g++)...
where g++ >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [!] Khong tim thay g++ trong bien moi truong PATH.
    echo Vui long cai dat MinGW / MSYS2 de bien dich ma nguon C++.
    pause
    exit /b 1
)

echo.
echo [2/3] Dang bien dich C++ Web Server (campus_nav.exe)...
g++ -std=c++17 -D_WIN32_WINNT=0x0A00 -DCAMPUS_NAV_SERVER -O2 core-cpp/*.cpp core-cpp/src/*.cpp -I core-cpp/include -o campus_nav.exe -lws2_32
if %ERRORLEVEL% neq 0 (
    echo.
    echo [!] Bien dich that bai! Vui long kiem tra lai thong bao loi o tren.
    pause
    exit /b 1
)
echo [OK] Bien dich thanh cong C++ Backend Server: campus_nav.exe

echo.
echo [3/3] Dang khoi dong C++ Web Server tai cong 8080 va mo trinh duyet...
start "UTC2 Navigation C++ Server" cmd /k "chcp 65001 >nul && campus_nav.exe 8080"

timeout /t 2 /nobreak >nul
start http://localhost:8080/

echo.
echo ================================================================================
echo  [OK] HE THONG DA KHOI DONG THANH CONG!
echo  - Giao dien ban do tuong tac: http://localhost:8080/
echo  - Backend C++ REST API:       http://localhost:8080/api/health
echo  - Hoac mo truc tiep:          web-ui\index.html
echo ================================================================================
echo.
pause

