#include "Renderer.h"

Renderer::Renderer() :
	m_direct3d(nullptr),
	m_camera(nullptr),
	m_model(nullptr),
	m_shader(nullptr)
{

}


Renderer::Renderer(const Renderer& other)
{

}


Renderer::~Renderer()
{

}


bool Renderer::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	m_direct3d = new Direct3D;
	result = m_direct3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Direct3D 초기화에 실패 했습니다.", L"Error", MB_OK);
		return false;
	}

	m_camera = new Camera;
	m_camera->SetPosition(0.f, 0.f, -5.f);

	m_model = new Model;
	result = m_model->Initialize(m_direct3d->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"모델 초기화에 실패 했습니다.", L"Error", MB_OK);
		return false;
	}

	m_shader = new Shader;
	result = m_shader->Initialize(m_direct3d->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"셰이더 초기화에 실패 했습니다.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void Renderer::Shutdown()
{
	if (m_shader)
	{
		m_shader->Shutdown();
		delete m_shader;
		m_shader = nullptr;
	}
 
	if (m_model)
	{
		m_model->Shutdown();
		delete m_model;
		m_model = nullptr;
	}

	if (m_camera)
	{
		delete m_camera;
		m_camera = nullptr;
	}

	if (m_direct3d)
	{
		m_direct3d->Shutdown();
		delete m_direct3d;
		m_direct3d = nullptr;
	}
}


bool Renderer::Frame()
{
	bool result;
	result = Render();

	return result;
}


bool Renderer::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	/*
	* 렌더링 전처리
	*/
	m_direct3d->BeginScene(0.f, 0.f, 0.f, 1.f);

	/*
	* 뷰 행렬 생성
	*/
	m_camera->Render();

	//XMFLOAT3 cur_pos = m_camera->GetPosition();
	//cur_pos.z -= 0.001f;
	//m_camera->SetPosition(cur_pos.x, cur_pos.y, cur_pos.z);

	/*
	* MVP 행렬 설정
	*/
	m_direct3d->GetWorldMatrix(worldMatrix);
	m_camera->GetViewMatrix(viewMatrix);
	m_direct3d->GetProjectionMatrix(projectionMatrix);

	/*
	* Render 함수를 통해 모델의 정점/인덱스 버퍼를 생성하고 IA(Input Assembler)에 설정
	*/
	m_model->Render(m_direct3d->GetDeviceContext());

	/*
	* 드로우 콜
	*/
	result = m_shader->Render(m_direct3d->GetDeviceContext(), m_model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	/*
	* 출력 요청
	*/
	m_direct3d->EndScene();

	return true;
}