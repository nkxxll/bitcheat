#pragma comment(lib, "Gdi32")
#pragma comment(lib, "user32")

#include <Windows.h>
#include <iostream>
#include <string>

void SaveHBITMAPToFile(HBITMAP hBitmap, const std::string &filename);
void CaptureMiddleOfScreen(const std::string &filename, int width, int height);

void CaptureMiddleOfScreen(const std::string &filename, int width, int height) {
    // Get the screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Calculate the coordinates for the top-left corner of the capture region
    int startX = (screenWidth / 2) - (width / 2);
    int startY = (screenHeight / 2) - (height / 2);

    // Get the screen device context
    HDC hdcScreen = GetDC(NULL);

    // Create a compatible device context
    HDC hdcCompatible = CreateCompatibleDC(hdcScreen);

    // Create a compatible bitmap
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);

    // Select the bitmap into the compatible device context
    SelectObject(hdcCompatible, hBitmap);

    // Copy the specified region from the screen to the compatible device context
    BitBlt(hdcCompatible, 0, 0, width, height, hdcScreen, startX * 1.5, startY * 1.5, SRCCOPY);

    // Save the bitmap to a file (you can use other formats like BMP, JPEG, etc.)
    SaveHBITMAPToFile(hBitmap, filename);

    // Cleanup
    DeleteObject(hBitmap);
    DeleteDC(hdcCompatible);
    ReleaseDC(NULL, hdcScreen);
}

void SaveHBITMAPToFile(HBITMAP hBitmap, const std::string &filename) {
    PBITMAPINFO pbmi;
    BITMAPINFOHEADER bi;

    // Initialize the bitmap information header
    ZeroMemory(&bi, sizeof(BITMAPINFOHEADER));
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = 100;
    bi.biHeight = -100;  // Negative height to ensure a top-down DIB
    bi.biPlanes = 1;
    bi.biBitCount = 24;  // 24 bits per pixel
    bi.biCompression = BI_RGB;

    // Allocate memory for the bitmap information
    pbmi = (PBITMAPINFO)malloc(sizeof(BITMAPINFOHEADER));
    memcpy(pbmi, &bi, sizeof(BITMAPINFOHEADER));

    // Create a file and write the bitmap data
    HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        // Write the bitmap file header
        BITMAPFILEHEADER bmfHeader;
        bmfHeader.bfType = 0x4D42;  // 'BM'
        bmfHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
                           100 * 100 * 3;  // 3 bytes per pixel
        bmfHeader.bfReserved1 = 0;
        bmfHeader.bfReserved2 = 0;
        bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

        DWORD dwBytesWritten;
        WriteFile(hFile, &bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);

        // Write the bitmap data
        WriteFile(hFile, pbmi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
        GetDIBits(GetDC(NULL), hBitmap, 0, 100, NULL, pbmi, DIB_RGB_COLORS);
        BYTE *pBits = (BYTE *)malloc(100 * 100 * 3);
        GetDIBits(GetDC(NULL), hBitmap, 0, 100, pBits, pbmi, DIB_RGB_COLORS);
        WriteFile(hFile, pBits, 100 * 100 * 3, &dwBytesWritten, NULL);

        // Cleanup
        CloseHandle(hFile);
        free(pbmi);
        free(pBits);
    }
}

int main() {
    // Specify the filename for the captured screenshot
    std::string filename = "middle_screen.bmp";

    // Specify the dimensions of the capture region
    int captureWidth = 100;
    int captureHeight = 100;

    // Capture the middle of the screen and save the screenshot
    CaptureMiddleOfScreen(filename, captureWidth, captureHeight);

    std::cout << "Screenshot captured and saved as " << filename << std::endl;

    return 0;
}
