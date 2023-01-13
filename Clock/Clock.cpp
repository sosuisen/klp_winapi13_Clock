#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include <string>
#include <format>

INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    HWND hDlg = CreateDialog(
        hInstance,
        MAKEINTRESOURCE(IDD_DIALOG1),
        NULL, // 親ウィンドウはなし
        DialogProc // ダイアログボックスプロシージャ
    );
    ShowWindow(hDlg, nCmdShow);
    InvalidateRect(hDlg, NULL, true);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        // ダイアログボックス上でのメッセージだけを仕分け
        if (!IsDialogMessage(hDlg, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    static HFONT hFont;
    static wchar_t strTime[128];

    static HBRUSH bkColorBrush = CreateSolidBrush(RGB(240, 255, 240)); //背景色のブラシを用意

    int minFontSize = 60;
    int maxFontSize = 120;
    int fontSizeDelta = 10;

    //ダイアログプロシージャ
    switch (uMsg)
    {
    case WM_CTLCOLORDLG:
        // WM_CTLCOLORDLG はシステムがダイアログ ボックスを描画する前に、ダイアログ ボックスに送信
        // ダイアログの背景色を設定
        return (LRESULT)bkColorBrush;
    case WM_CTLCOLORSTATIC: // STATICの背景色
        SetBkMode(((HDC)wParam), TRANSPARENT);
        SetTextColor(((HDC)wParam), RGB(200, 0, 0));//文字の色
        return (LRESULT)bkColorBrush;
    case WM_INITDIALOG: {
        // 1秒ごとにタイマー呼び出し
        SetTimer(hDlg, 1, 1000, NULL);

        for (int size = minFontSize; size <= maxFontSize; size += fontSizeDelta) {
            // 指定のダイアログボックス内のコントロールへメッセージを送る
            SendDlgItemMessage(hDlg,
                IDC_COMBO1,
                CB_ADDSTRING,
                0,
                (LPARAM)std::format(L"{:d}", size).c_str());
        }

        // maxFontSizeを選択
        SendDlgItemMessage(hDlg,
            IDC_COMBO1,
            CB_SETCURSEL,
            (maxFontSize - minFontSize)/fontSizeDelta,
            0);

        // フォントを作成する
        hFont = CreateFont(
            maxFontSize,          // フォントの高さ(大きさ)。
            0,                    // フォントの幅。0でデフォルト。
            0, 0,                 // 角度
            FW_DONTCARE,          // 文字の太さ
            FALSE,                // フォントがイタリックならTRUE
            FALSE,                // 下線を引くならTRUE
            FALSE,                // 取り消し線を引くならTRUE
            DEFAULT_CHARSET,      // フォントの文字セット
            OUT_DEFAULT_PRECIS,   // 出力精度の設定
            CLIP_DEFAULT_PRECIS,  // クリッピング精度
            DRAFT_QUALITY,        // フォントの出力品質
            DEFAULT_PITCH,        // フォントのピッチとファミリを指定
            L"ＭＳ Ｐゴシック" // フォント名
        );
        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_COMBO1:
            if (HIWORD(wParam) == CBN_SELCHANGE) {
                int index = (int)SendDlgItemMessage(hDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0);
                DeleteObject(hFont);
                hFont = CreateFont(
                    minFontSize + fontSizeDelta * index, // フォントの高さ(大きさ)。
                    0,
                    0, 0,
                    FW_DONTCARE,
                    FALSE,
                    FALSE,
                    FALSE,
                    DEFAULT_CHARSET,
                    OUT_DEFAULT_PRECIS,
                    CLIP_DEFAULT_PRECIS,
                    DRAFT_QUALITY,
                    DEFAULT_PITCH,
                    L"ＭＳ Ｐゴシック"
                );
            }
            return TRUE;
        }
        return TRUE;
    case WM_TIMER:
        InvalidateRect(hDlg, NULL, true);
        return TRUE;
    case WM_PAINT: {
        SYSTEMTIME stTime;
        GetLocalTime(&stTime);
        std::wstring strClock = std::format(L"{:02}:{:02}:{:02}", stTime.wHour, stTime.wMinute, stTime.wSecond);
        
        hdc = BeginPaint(hDlg, &ps);       // 描画開始
        SelectObject(hdc, hFont);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(180, 220, 180));
        RECT rc = { 10, 50, 450, 300 };

        DrawText(hdc, strClock.c_str(), -1, &rc, DT_CENTER | DT_VCENTER);
            EndPaint(hDlg, &ps);               // 描画終了
        return TRUE;
    }
    case WM_CLOSE:
        DestroyWindow(hDlg);
        return TRUE;
    case WM_DESTROY:
        DeleteObject(hFont);
        PostQuitMessage(0);
        return TRUE;
    }
    return FALSE;
}