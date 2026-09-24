@echo off
chcp 65001 > nul
title PSPP Canli Dashboard (Streamlit)
cd /d "%~dp0"

echo ============================================================
echo         PSPP CANLI STREAMLIT DASHBOARD BASLATILIYOR
echo ============================================================
echo.
echo Tarayici otomatik olarak acilacaktir: http://localhost:8501
echo.
echo Sunucuyu durdurmak icin pencereyi kapatabilir veya Ctrl+C yapabilirsiniz.
echo ============================================================
echo.

:: 1. py launcher ile baslatmayı dene
where py >nul 2>nul
if %errorlevel% equ 0 (
    py -m streamlit run app_streamlit.py --server.headless=false
    goto :end
)

:: 2. Dogrudan python yolu ile dene
if exist "%LOCALAPPDATA%\Python\pythoncore-3.14-64\python.exe" (
    "%LOCALAPPDATA%\Python\pythoncore-3.14-64\python.exe" -m streamlit run app_streamlit.py --server.headless=false
    goto :end
)

:: 3. Varsayilan python ile dene
python -m streamlit run app_streamlit.py --server.headless=false

:end
echo.
echo [BILGI] Streamlit oturumu sonlandi.
pause

