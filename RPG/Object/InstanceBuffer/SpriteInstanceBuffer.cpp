#include "framework.h"
#include "SpriteInstanceBuffer.h"

template<typename RefDataSet>
SpriteInstanceBuffer<RefDataSet>::SpriteInstanceBuffer() :
	InstanceBuffer()
{
}

template<typename RefDataSet>
SpriteInstanceBuffer<RefDataSet>::~SpriteInstanceBuffer()
{
}

template<typename RefDataSet>
void SpriteInstanceBuffer<RefDataSet>::Update(const RefDataSet& refData)
{
}

template<typename RefDataSet>
UINT SpriteInstanceBuffer<RefDataSet>::GetBufferSizeFromCount(UINT count)
{
	return count * sizeof(InstanceData);
}
