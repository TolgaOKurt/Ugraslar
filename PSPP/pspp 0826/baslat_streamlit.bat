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

python -m streamlit run app_streamlit.py --server.headless=false

echo.
echo [BILGI] Streamlit oturumu sonlandi.
pause
