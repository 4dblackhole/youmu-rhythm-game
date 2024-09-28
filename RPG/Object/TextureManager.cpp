#include "framework.h"
#include "TextureManager.h"

void TextureManager::AddTexture(ID3D11Device* device, ContainerType::key_type& key, const wstring& file)
{
	Texture tempTexture;
	tempTexture.CreateTexture(device, file);
	textureList.emplace(make_pair(key, std::move(tempTexture)));
}

void TextureManager::AddTexture(ContainerType::key_type& key, ContainerType::mapped_type& texture)
{
	textureList.emplace(make_pair(key, std::move(texture)));
}

void TextureManager::RemoveTexture(ContainerType::key_type& key)
{
	// image search
	ContainerType::iterator it = textureList.find(key);
	if (it != textureList.end()) textureList.erase(it);
}

void TextureManager::Clear()
{
	textureList.clear();
}

TextureManager::ContainerType::mapped_type& TextureManager::GetTexture(ContainerType::key_type& key)
{
	// image search
	ContainerType::iterator it = textureList.find(key);
	if (it != textureList.end()) return it->second;
	else return GetNullTexture();
}

TextureManager::ContainerType::mapped_type& TextureManager::GetNullTexture()
{
	static ContainerType::mapped_type nullTexture(App->GetDevice(), TextureDir + L"TextureNotFound.png");
	return nullTexture;
}

bool TextureManager::AddTextureArr(const string& keyStr, const vector<LPCWSTR>& fileList)
{
	if (fileList.empty()) return false;
	//Texture Array
	vector<ID3D11Texture2D*> tempTextureList;

	AddTextureArr_LoadTextureList(fileList, tempTextureList);
	bool result = AddTextureArr_MergeTextureList(keyStr, tempTextureList);

	for (auto& it : tempTextureList) ReleaseCOM(it);
	tempTextureList.clear();
	return result;
}

void TextureManager::AddTextureArr_LoadTextureList(const vector<LPCWSTR>& fileList, vector<ID3D11Texture2D*>& textureList)
{
	vector<LPCWSTR>::const_iterator cIter = fileList.begin();
	while (cIter != fileList.cend())
	{
		ID3D11Texture2D* tempTexture2D{};

		//get file name
		const BOOL& result = ShortCut::FileExists(*cIter);
		if (result == FALSE) continue;

		D3DX11_IMAGE_LOAD_INFO info{};
		info.MipLevels = 1;
		HR(D3DX11CreateTextureFromFile(App->GetDevice(), *cIter, &info, nullptr, (ID3D11Resource**)&tempTexture2D, nullptr));
		textureList.emplace_back(std::move(tempTexture2D));

		++cIter;
	}
}

bool TextureManager::AddTextureArr_MergeTextureList(const string& keyStr, const vector<ID3D11Texture2D*>& textureList)
{
	//no files
	if (textureList.empty()) return false;

	//create texture2DArr
	D3D11_TEXTURE2D_DESC texture2Ddesc;
	(*textureList.begin())->GetDesc(&texture2Ddesc);
	texture2Ddesc.ArraySize = (UINT)textureList.size();
	texture2Ddesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//texture2Ddesc.MipLevels = 1;
	//texture2Ddesc.SampleDesc.Count = 1;

	ID3D11Texture2D* texture2DArr;
	HRESULT hr = (App->GetDevice()->CreateTexture2D(&texture2Ddesc, nullptr, &texture2DArr));
	if (FAILED(hr)) return false;

	for (UINT i = 0; i < textureList.size(); ++i) {
		for (UINT mip = 0; mip < texture2Ddesc.MipLevels; ++mip) {
			App->GetDeviceContext()->CopySubresourceRegion(
				texture2DArr,
				D3D11CalcSubresource(mip, i, texture2Ddesc.MipLevels),
				0, 0, 0,
				textureList.at(i),
				mip,
				nullptr
			);
		}
	}

	//texture2DArr to SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = texture2Ddesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = texture2Ddesc.MipLevels;
	srvDesc.Texture2DArray.ArraySize = texture2Ddesc.ArraySize;

	Texture arrTexture;
	hr = (App->GetDevice()->CreateShaderResourceView(texture2DArr, &srvDesc, &arrTexture.srv));
	if (FAILED(hr)) return false;

	Texture::UpdateDescFromSRV(arrTexture);
	this->AddTexture(keyStr, arrTexture);
	ReleaseCOM(texture2DArr);

	return true;
}

bool TextureManager::AddTextureArrIndexed(const string& keyStr, const wstring& fileName)
{
	//in case there is only one texture
	if (ShortCut::FileExists(fileName.c_str()) != FALSE)
	{
		this->AddTexture(App->GetDevice(), keyStr, fileName);
		return true;
	}

	//Texture Array
	int textureIdx = 0;
	vector<ID3D11Texture2D*> tempTextureList;
	while (true)
	{
		ID3D11Texture2D* tempTexture2D{};

		//get file name
		const size_t& commaPos = fileName.find_last_of('.');
		const wstring_view& ext = wstring_view(fileName.c_str() + commaPos);
		const wstring_view& name = wstring_view(fileName.c_str(), commaPos);
		const wstring& idxStr = L"-" + to_wstring(textureIdx);
		const wstring& resultFileName = (wstring)name + idxStr.data() + ext.data();
		const BOOL& result = ShortCut::FileExists(resultFileName.c_str());
		if (result == FALSE) break;

		D3DX11_IMAGE_LOAD_INFO info{};
		info.MipLevels = 1;
		D3DX11CreateTextureFromFile(App->GetDevice(), resultFileName.c_str(), &info, nullptr, (ID3D11Resource**)&tempTexture2D, nullptr);
		tempTextureList.emplace_back(tempTexture2D);
		++textureIdx;
	}

	bool result = AddTextureArr_MergeTextureList(keyStr, tempTextureList);

	for (auto& it : tempTextureList) ReleaseCOM(it);
	tempTextureList.clear();

	return result;
}
