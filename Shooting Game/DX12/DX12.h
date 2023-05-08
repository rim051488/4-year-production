#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <directXMath.h>
#include <wrl/client.h>
#include <DirectXTex.h>
#include <d3dx12.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"DirectXTex.lib")

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

struct TexRGBA
{
	unsigned char R, G, B, A;
};

struct Wave
{
	XMFLOAT2 dir;
	float amplitude;
	float waveLenght;
};

struct cbuff
{
	XMMATRIX worldMat;
	XMMATRIX viewMat;
	Wave waves[100];
};

// PMD�w�b�_�\����
struct PMDHeader {
	float version;
	// ���f���̖��O
	char model_name[20];
	// ���f���̃R�����g
	char comment[256];
};

// ���_�f�[�^�\����
struct PMD_VERTEX
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 uv;
	uint16_t bone_no[2];
	uint8_t  weight;
	uint8_t  EdgeFlag;
	uint16_t dummy;
};


class DX12
{
public:
	DX12(HWND hwnd);
	~DX12(void);
	// ���C�����[�v�ł̏���
	void Render(void);
	// ���_�ݒ�
	void CreateVertices(void);
	
private:
	const unsigned int window_width = 1280;
	const unsigned int window_height = 720;
	// �t���[����
	unsigned int frame;
	// ���f���̂P���_������̃T�C�Y
	const size_t pmdvertexSize_ = 38;

	HWND hwnd_;

	// �f�o�C�X
	//ID3D12Device* dev_;
	ComPtr<ID3D12Device> dev_;
	// �t�@�N�g���[
	ComPtr<IDXGIFactory6> dxgiFactory_;
	// �X���b�v�`�F�[��
	ComPtr<IDXGISwapChain4> swapchain_;
	// �R�}���h�A���P�[�^�[
	ComPtr<ID3D12CommandAllocator> cmdAllcator_;
	// �R�}���h���X�g
	ComPtr<ID3D12GraphicsCommandList> cmdList_;
	// �R�}���h�L���[
	ComPtr<ID3D12CommandQueue> cmdQueue_;

	ComPtr<ID3D12DescriptorHeap> rtvHeaps_;
	vector<ComPtr<ID3D12Resource>> backBuffers_;

	// �t�F���X�ݒ�p�ϐ�
	ComPtr<ID3D12Fence> fence_;
	UINT64 fenceVal_;

	// ���_�ݒ莞�p�̕ϐ�
	ComPtr<ID3D12Resource> vertBuff_;
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};

	// �p�C�v���C���X�e�[�g�p�ϐ�
	ComPtr<ID3D12PipelineState> pipelinestate_;
	// ���[�g�V�O�l�C�`���p�̕ϐ�
	ComPtr<ID3D12RootSignature> rootSig_;
	ComPtr<ID3DBlob> rootSigBlob_;

	// �e�N�X�`���p�ϐ�
	ComPtr<ID3D12Resource> texbuff_;
	// �e�N�X�`���p�̃f�B�X�N���v�^�q�[�v�ϐ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	ID3D12DescriptorHeap* texDescHeap = nullptr;

	// �V�F�[�_�[�I�u�W�F�N�g�ێ��p�ϐ�
	ComPtr<ID3DBlob> vsBlob_;
	ComPtr<ID3DBlob> psBlob_;
	// �G���[���ɕێ�����ϐ�
	ComPtr<ID3DBlob> errorBlob_;

	// �萔�o�b�t�@�̕ϐ�
	ComPtr<ID3D12Resource> constBuff_;
	// �s��ϐ�
	// �}�b�v��������|�C���^�[
	XMMATRIX* mapMatrix_;
	cbuff* mapBuff_;
	ID3D12DescriptorHeap* basicDescHeap_ = nullptr;
	// �������A�j���[�V����
	float angle;
	XMMATRIX worldMat;
	XMMATRIX viewMat;
	XMMATRIX projMat;

	// �r���[�|�[�g�p�ϐ�
	D3D12_VIEWPORT viewport_ = {};
	// �V�U�[��`�p�ϐ�
	D3D12_RECT scissorrect_ = {};
	// �C���f�b�N�X�̗p�ϐ�
	D3D12_INDEX_BUFFER_VIEW ibVIew_ = {};

	// ���f���̓ǂݍ��ݗp�ϐ�
	// �V�O�l�`��
	char signature_[3] = {};
	PMDHeader pmdHeader = {};
	// ���_��
	unsigned int vertNum_;
	vector<PMD_VERTEX> vertices_;
	// �C���f�b�N�X��
	unsigned int indicesNum;
	vector<unsigned short> indices_;
	ComPtr<ID3D12Resource> idxBuff_;

	// �f�o�C�X�̏�����
	void CreateDvice(void);
	// �R�}���h�n�̏�����
	void CreateCommand(void);
	// �R�}���h�L���[�̏�����
	void CreateCommandQueue(void);
	// �X���b�v�`�F�[���̏�����
	void CreateSwapChain(void);
	// �f�B�X�N���v�^�q�[�v�̏�����
	void CreateDescriptor(void);
	// �t�F���X�̐ݒ�
	void CreateFence(void);
	// �V�F�[�_�[�ǂݍ��ݎ��Ɏ��s�����Ƃ��̏���
	void FailedShader(HRESULT result);
	// �r���[�|�[�g�̐ݒ�
	void CreateView(void);
	// �V�U�[��`
	void CreateScissor(void);
	// ���f���̃��[�h
	void LoadModel(void);
};

