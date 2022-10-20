#include <Windows.h>
#include <chrono>
#include <string>
#include "glad/glad.h"

#include "Shader.h"
#include "Camera.h"
#include "stb/stb_image.h"

const char* c_cVertexFilePath = "vertex.vs";
const char* c_cFragmentFilePath = "fragment.fs";

HDC DC;
HGLRC RC;
Shader tSquare;

unsigned uScreenWidth = GetSystemMetrics(SM_CXSCREEN);
unsigned uiScreenHeight = GetSystemMetrics(SM_CYSCREEN);

uint16_t uMapSize = 12; //12 max
const uint16_t uVrtexCount = 4;
const uint16_t uCoorCount = 3;
float iRougness = 0.2f;

auto tProgStart = std::chrono::high_resolution_clock::now();

LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

float* GetVertIndx(float* pfMap, const uint64_t uRealMapSize, const uint64_t X, const uint64_t Y);
void InitMap(float* pfMap, const uint64_t uRealMapSize);
void DiamondStep(float* pfMap, const uint64_t uRealMapSize, const uint64_t uUpLeft, const uint64_t uSideLength);
void SquareStep(float* pfMap, const uint64_t uRealMapSize, const uint64_t uUpLeft, const uint64_t uSideLength);

float GetRandHeight(float length)
{
	const uint64_t accuracy = 10000;
	return length * iRougness - 2 * float(rand() % accuracy) / accuracy * length * iRougness;
}

struct Exception
{
	Exception(const std::wstring& message, const int excNumber) : msg(message), eNum(excNumber) {}
	std::wstring msg;
	int eNum;
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	MSG msg = {};
	WNDCLASSEX WndC =
	{
		sizeof(WNDCLASSEX),
		CS_OWNDC,
		reinterpret_cast<WNDPROC>(WinProc),
		0, 0,
		hInstance,
		LoadIcon(NULL, IDI_APPLICATION),
		LoadCursor(NULL, IDC_ARROW),
		reinterpret_cast<HBRUSH>(COLOR_WINDOW + 8),
		nullptr,
		L"Render window",
		nullptr
	};

	RegisterClassEx(&WndC);

	HWND hwnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		L"Render window",
		L"There is nothing to see...",
		WS_OVERLAPPEDWINDOW,
		0, 0,
		uScreenWidth, uiScreenHeight,
		NULL, NULL, hInstance, NULL);
		


	if (!hwnd)
	{
		MessageBox(NULL, L"Failed to create Handel!", L"ERROR!", MB_ICONERROR | MB_OK);
		return 1;
	}
	

	
	float faVertecies[] =
	{
		1.0f, 1.0f,  1.0f,
	   -1.0f, 1.0f,  1.0f,
	   -1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
	};


	
	uint64_t uRealMapSize = pow(2.0f, uMapSize) + 1;

	tSquare.init(c_cVertexFilePath, c_cFragmentFilePath);

	unsigned VBO, VAO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	float* Map = new float[uRealMapSize * uRealMapSize * uCoorCount];
	InitMap(Map, uRealMapSize);

	unsigned uaIndecies[] =
	{
		0, 1, uRealMapSize,
		uRealMapSize, 1, uRealMapSize + 1
	};


	srand(time(NULL));
	GetVertIndx(Map, uRealMapSize, 0, 0)[1] = GetRandHeight(uRealMapSize);
	GetVertIndx(Map, uRealMapSize, uRealMapSize - 1, 0)[1] = GetRandHeight(uRealMapSize);
	GetVertIndx(Map, uRealMapSize, uRealMapSize - 1, uRealMapSize -1)[1] = GetRandHeight(uRealMapSize);
	GetVertIndx(Map, uRealMapSize, 0, uRealMapSize - 1)[1] = GetRandHeight(uRealMapSize);

	uint64_t uSideLength = uRealMapSize;
	uint64_t uSquareCount = 1;
	while (ceil(static_cast<float>(uSideLength) / 2) > 1)
	{
		uint64_t uUpLeftIndex = 0;
		uint64_t uRowIndex = 0;
		int i = 0;
		while (i < uSquareCount)
		{
			DiamondStep(Map, uRealMapSize, uUpLeftIndex, uSideLength);
			SquareStep(Map, uRealMapSize, uUpLeftIndex, uSideLength);
			uUpLeftIndex += uSideLength;
			if (uUpLeftIndex < uRowIndex + uRealMapSize)
			{
				uUpLeftIndex--;
			}
			else
			{
				uUpLeftIndex -= uRealMapSize;
				uRowIndex += uRealMapSize * (uSideLength - 1);
				uUpLeftIndex += uRealMapSize * (uSideLength - 1);
			}
			i++;
		}
		uSideLength = ceil(static_cast<float>(uSideLength) / 2);
		uSquareCount *= 4;
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uRealMapSize * uRealMapSize * uCoorCount, Map, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
	glEnableVertexAttribArray(0);

	delete[] Map;


	uint64_t uNumOfSqrs = (uRealMapSize - 1) * (uRealMapSize - 1);
	unsigned* uaFullIndxs = new unsigned[uNumOfSqrs * 6];


	for (size_t i = 0; i < (uRealMapSize - 1); i++)
	{
		for (size_t j = 0; j < (uRealMapSize - 1); j++)
		{
			for (size_t k = 0; k < 6; k++)
			{
				uint32_t indx = k + j * 6 + i * (uRealMapSize - 1) * 6;
				uaFullIndxs[indx] = uaIndecies[k] + j + i * uRealMapSize;
			}
		}
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * uNumOfSqrs * 6, uaFullIndxs, GL_STATIC_DRAW);

	delete[] uaFullIndxs;


	tSquare.use();
	Camera cam(glm::vec3(0.0f, 1.0f, 0.0f), uScreenWidth, uiScreenHeight, tSquare.GetProgramID());
	ShowWindow(hwnd, SW_SHOW);
	std::chrono::duration<float> tFrameTime = std::chrono::high_resolution_clock::now() - tProgStart;
	bool bQuit = false;
	while (!bQuit)
	{
		auto tFrameBegin = std::chrono::high_resolution_clock::now();
		glm::mat4 model = glm::mat4(1.0f);
		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bQuit = true;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		cam.MouseControl();
		cam.KeyboardControl(tFrameTime);

		if (GetKeyState(VK_ESCAPE) < 0)
			SendMessage(hwnd, WM_DESTROY, NULL, NULL);

		if (GetKeyState('R') < 0)
		{
			float* Map = new float[uRealMapSize * uRealMapSize * uCoorCount];
			InitMap(Map, uRealMapSize);

			srand(time(NULL));
			GetVertIndx(Map, uRealMapSize, 0, 0)[1] = GetRandHeight(uRealMapSize);
			GetVertIndx(Map, uRealMapSize, uRealMapSize - 1, 0)[1] = GetRandHeight(uRealMapSize);
			GetVertIndx(Map, uRealMapSize, uRealMapSize - 1, uRealMapSize - 1)[1] = GetRandHeight(uRealMapSize);
			GetVertIndx(Map, uRealMapSize, 0, uRealMapSize - 1)[1] = GetRandHeight(uRealMapSize);

			uSideLength = uRealMapSize;
			uSquareCount = 1;
			while (ceil(static_cast<float>(uSideLength) / 2) > 1)
			{
				uint64_t uUpLeftIndex = 0;
				uint64_t uRowIndex = 0;
				int i = 0;
				while (i < uSquareCount)
				{
					DiamondStep(Map, uRealMapSize, uUpLeftIndex, uSideLength);
					SquareStep(Map, uRealMapSize, uUpLeftIndex, uSideLength);
					uUpLeftIndex += uSideLength;
					if (uUpLeftIndex < uRowIndex + uRealMapSize)
					{
						uUpLeftIndex--;
					}
					else
					{
						uUpLeftIndex -= uRealMapSize;
						uRowIndex += uRealMapSize * (uSideLength - 1);
						uUpLeftIndex += uRealMapSize * (uSideLength - 1);
					}
					i++;
				}
				uSideLength = ceil(static_cast<float>(uSideLength) / 2);
				uSquareCount *= 4;
			}
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)* uRealMapSize* uRealMapSize* uCoorCount, Map, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
			glEnableVertexAttribArray(0);

			delete[] Map;
		}

		

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniformMatrix4fv(glGetUniformLocation(tSquare.GetProgramID(), "model"), 1, GL_FALSE, glm::value_ptr(model));
		
		/*for (int i = 0; i < uRealMapSize - 1; ++i)
		{
			int arr[6];
			for (size_t j = 0; j < uRealMapSize - 1; j++)
			{
				for (size_t k = 0; k < 6; k++)
					arr[k] = uaIndecies[k] + j + i * uRealMapSize;
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(arr), arr, GL_DYNAMIC_DRAW);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
			}
		}*/

		glDrawElements(GL_TRIANGLES, uNumOfSqrs * 6, GL_UNSIGNED_INT, 0);
		

		SwapBuffers(DC);
		auto tFrameEnd = std::chrono::high_resolution_clock::now();
		tFrameTime = tFrameEnd - tFrameBegin;
		SetWindowText(hwnd, std::to_wstring(1 / tFrameTime.count()).c_str());
	}

}

LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
		PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
		32,                   // Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,                   // Number of bits for the depthbuffer
		8,                    // Number of bits for the stencilbuffer
		0,                    // Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	int pfn;


	switch (msg)
	{
	case WM_CREATE:
		{
			DC = GetDC(hwnd);
			pfn = ChoosePixelFormat(DC, &pfd);
			SetPixelFormat(DC, pfn, &pfd);
			RC = wglCreateContext(DC);
			wglMakeCurrent(DC, RC);
			if (!gladLoadGL())
			{
				int err = GetLastError();
				MessageBox(hwnd, L"Failed to initialize glut!", L"ERROR!", MB_ICONERROR | MB_OK);
				return -1;
			}
			return 0;
		}

	case WM_DESTROY:
		{
			wglMakeCurrent(DC, NULL);
			wglDeleteContext(RC);
			PostQuitMessage(0);
			return 0;
		}

	case WM_SIZE:
			glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
			uScreenWidth = LOWORD(lParam);
			uiScreenHeight = HIWORD(lParam);

	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


float* GetVertIndx(float* pfMap, const uint64_t uRealMapSize, const uint64_t X, const uint64_t Y)
{
	uint64_t uLineSize = uCoorCount * uRealMapSize;
	return pfMap + uCoorCount * X + Y * uLineSize;
}

void InitMap(float* pfMap, const uint64_t uRealMapSize)
{
	for (size_t i = 0; i < uRealMapSize; i++)
	{
		for (size_t j = 0; j < uRealMapSize; j++)
		{
			float* fVertCoord = GetVertIndx(pfMap, uRealMapSize, j, i);
			fVertCoord[0] = j;
			fVertCoord[1] = 0.0f;
			fVertCoord[2] = i;
		}
	}
}

void DiamondStep(float* pfMap, const uint64_t uRealMapSize, const uint64_t uUpLeft, const uint64_t uSideLength)
{
	//uint16_t uCenterIndex = uUpLeft + uSideLength / 2;
	//float* fCnterVertex = GetVertIndx(pfMap, uRealMapSize, uCenterIndex, uCenterIndex);
	//float fHeightVal = GetVertIndx(pfMap, uRealMapSize, uUpLeft, uUpLeft)[1];
	//fHeightVal += GetVertIndx(pfMap, uRealMapSize, uUpLeft, uUpLeft + uSideLength - 1)[1];
	//fHeightVal += GetVertIndx(pfMap, uRealMapSize, uUpLeft + uSideLength - 1, uUpLeft)[1];
	//fHeightVal += GetVertIndx(pfMap, uRealMapSize, uUpLeft + uSideLength - 1, uUpLeft + uSideLength - 1)[1];
	//fHeightVal /= 4;
	//fHeightVal += float(rand() % 1000) / iRougness; //Roughness

	//fCnterVertex[1] = fHeightVal;

	uint64_t uCenterIndex = uUpLeft + uSideLength / 2 + uRealMapSize * (uSideLength / 2);
	uCenterIndex *= uCoorCount;

	float fHeightVal = pfMap[uUpLeft * uCoorCount + 1];
	fHeightVal += pfMap[(uUpLeft + uSideLength - 1) * uCoorCount + 1];
	uint64_t indx = (uUpLeft + (uSideLength - 1) * uRealMapSize) * uCoorCount;
	fHeightVal += pfMap[(uUpLeft + (uSideLength - 1) * uRealMapSize) * uCoorCount + 1];
	fHeightVal += pfMap[(uUpLeft + uSideLength - 1 + (uSideLength - 1) * uRealMapSize) * uCoorCount + 1];
	fHeightVal /= 4;
	fHeightVal += GetRandHeight(uSideLength / 2);
	
	pfMap[uCenterIndex + 1] = fHeightVal;
}

float GetHeight(float* pfMap, const uint64_t uRealMapSize, const uint64_t uSideLength, const uint64_t uFillIndex)
{
	bool bEdge = false;
	const uint64_t uMaxIndex = uRealMapSize * uRealMapSize;
	
	float fHeightVal = 0.0f;
	if (uFillIndex % uRealMapSize != 0)
		fHeightVal += pfMap[(uFillIndex - uSideLength / 2) * uCoorCount + 1];
	else
		bEdge = true;

	if (uFillIndex % uRealMapSize != uRealMapSize - 1)
		fHeightVal += pfMap[(uFillIndex + uSideLength / 2) * uCoorCount + 1];
	else
		bEdge = true;

	if (uFillIndex + (uSideLength / 2) * uRealMapSize < uMaxIndex)
		fHeightVal += pfMap[(uFillIndex + (uSideLength / 2) * uRealMapSize) * uCoorCount + 1];
	else
		bEdge = true;

	if (uFillIndex - (uSideLength / 2) * uRealMapSize < uMaxIndex)
		fHeightVal += pfMap[(uFillIndex - (uSideLength / 2) * uRealMapSize) * uCoorCount + 1];
	else
		bEdge = true;
	
	if (bEdge)
		fHeightVal /= 3;
	else
		fHeightVal /= 4;
	fHeightVal += GetRandHeight(uSideLength);

	return fHeightVal;
}

void SquareStep(float* pfMap, const uint64_t uRealMapSize, const uint64_t uUpLeft, const uint64_t uSideLength)
{
	/*uint16_t uCenterIndex = uUpLeft + uSideLength / 2 + uRealMapSize * (uSideLength / 2);
	uCenterIndex *= uCoorCount;

	float fHeightVal = pfMap[uUpLeft * uCoorCount + 1];
	fHeightVal += pfMap[uCenterIndex + 1];
	fHeightVal += pfMap[(uUpLeft + uSideLength - 1) * uCoorCount + 1];
	fHeightVal /= 3;
	fHeightVal += GetRandHeight();

	pfMap[(uUpLeft + uSideLength / 2) * uCoorCount + 1] = fHeightVal;

	fHeightVal = pfMap[uCenterIndex + 1];
	fHeightVal += pfMap[(uUpLeft + uSideLength - 1) * uCoorCount + 1];
	fHeightVal += pfMap[(uUpLeft + uSideLength - 1 + (uSideLength - 1) * uRealMapSize) * uCoorCount + 1];
	fHeightVal /= 3;
	fHeightVal += GetRandHeight();

	pfMap[((uUpLeft + uSideLength - 1) + (uSideLength / 2) * uRealMapSize) * uCoorCount + 1] = fHeightVal;

	fHeightVal = pfMap[uCenterIndex + 1];
	fHeightVal += pfMap[(uUpLeft + uSideLength - 1 + (uSideLength - 1) * uRealMapSize) * uCoorCount + 1];
	fHeightVal += pfMap[(uUpLeft + (uSideLength - 1) * uRealMapSize) * uCoorCount + 1];
	fHeightVal /= 3;
	fHeightVal += GetRandHeight();

	pfMap[((uUpLeft + uSideLength / 2) + (uSideLength - 1) * uRealMapSize) * uCoorCount + 1] = fHeightVal;

	fHeightVal = pfMap[uCenterIndex + 1];
	fHeightVal += pfMap[(uUpLeft + (uSideLength - 1) * uRealMapSize) * uCoorCount + 1];
	fHeightVal += pfMap[uUpLeft * uCoorCount + 1];
	fHeightVal /= 3;
	fHeightVal += GetRandHeight();

	pfMap[(uUpLeft + (uSideLength / 2) * uRealMapSize) * uCoorCount + 1] = fHeightVal;*/


	uint64_t uFillIndex = (uUpLeft + uSideLength / 2);
	pfMap[uFillIndex * uCoorCount + 1] = GetHeight(pfMap, uRealMapSize, uSideLength, uFillIndex);
	
	uFillIndex = ((uUpLeft + uSideLength - 1) + (uSideLength / 2) * uRealMapSize);
	pfMap[uFillIndex * uCoorCount + 1] = GetHeight(pfMap, uRealMapSize, uSideLength, uFillIndex);

	uFillIndex = ((uUpLeft + uSideLength / 2) + (uSideLength - 1) * uRealMapSize);
	pfMap[uFillIndex * uCoorCount + 1] = GetHeight(pfMap, uRealMapSize, uSideLength, uFillIndex);

	uFillIndex = (uUpLeft + (uSideLength / 2) * uRealMapSize);
	pfMap[uFillIndex * uCoorCount + 1] = GetHeight(pfMap, uRealMapSize, uSideLength, uFillIndex);
	

}
