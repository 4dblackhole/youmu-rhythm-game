#include "framework.h"
#include "Note.h"

bool MusicalObject::operator<(const MusicalObject& v) const
{
    return this->mp < v.mp;
}

bool MusicalObject::operator>(const MusicalObject& v) const
{
    return this->mp > v.mp;
}

bool MusicalObject::operator==(const MusicalObject& v) const
{
    return this->mp == v.mp;
}

bool MusicalPosition::operator<(const MusicalPosition& v) const
{
    if (this->measureIdx < v.measureIdx) return true;
    if (this->measureIdx == v.measureIdx)
    {
        if (this->position < v.position) return true;
    }
    return false;
}

bool MusicalPosition::operator>(const MusicalPosition& v) const
{
    return v < *this;
}

bool MusicalPosition::operator<=(const MusicalPosition& v) const
{
    return !(*this > v);
}

bool MusicalPosition::operator>=(const MusicalPosition& v) const
{
    return !(*this < v);
}

bool MusicalPosition::operator==(const MusicalPosition& v) const
{
    return (this->measureIdx == v.measureIdx && this->position == v.position);
}

MusicScore::MusicScore()
{
}

MusicScore::~MusicScore()
{
    ReleaseTree();
}

Note* MusicScore::FindFirstNote()
{
    Note* firstNote = nullptr;
    for (pair<const size_t, MusicScore::NoteList>& noteListOfEachKey : notesPerKeyMap)
    {
        if (noteListOfEachKey.second.empty()) continue;
        Note& currentNote = noteListOfEachKey.second.begin()->second;
        if (firstNote == nullptr) firstNote = &currentNote;
        else
        {
            if (currentNote < *firstNote) firstNote = &currentNote;
        }
    }
    return firstNote;
}

RationalNumber<64> MusicScore::GetMeasureMusicalPosition(size_t pos)
{
    RationalNumber<64> resultPos(0, 1);
    for (size_t idx = 0; idx < pos; idx++)
    {

    }
    return RationalNumber<64>();
}

static int GetSegTreePivot(int left, int right)
{
    return left + Math::FindHighestPowerOfTwo(right - left);
}

void MusicScore::InitTree()
{
    ReleaseTree();

    measureSegTree = new MeasureNode;
    measureSegLazyTree = new MeasureNode;

    segTreeView.emplace_back(measureSegTree);
    segLazyTreeView.emplace_back(measureSegLazyTree);

    InitTreeRC(measureSegTree, measureSegLazyTree, 0, (int)measures.size() - 1);
}

void MusicScore::InitTreeRC(MeasureNode* node, MeasureNode* lazyNode, int listLeft, int listRight)
{
	if (listLeft == listRight)
    {
        node->totalLength = measures[listLeft].length;
	}
	else 
    {
        MeasureNode* leftChild = new MeasureNode;
        MeasureNode* rightChild = new MeasureNode;
        node->pLeft = leftChild;
        node->pRight = rightChild;

        MeasureNode* leftLazyChild = new MeasureNode;
        MeasureNode* rightLazyChild = new MeasureNode;
        lazyNode->pLeft = leftLazyChild;
        lazyNode->pRight = rightLazyChild;

        segTreeView.emplace_back(leftChild);
        segTreeView.emplace_back(rightChild);

        segLazyTreeView.emplace_back(leftLazyChild);
        segLazyTreeView.emplace_back(rightLazyChild);

        const int pivot = GetSegTreePivot(listLeft,listRight);
        InitTreeRC(leftChild, leftLazyChild, listLeft, pivot - 1);
        InitTreeRC(rightChild, rightLazyChild, pivot, listRight);
        node->totalLength = node->pLeft->totalLength + node->pRight->totalLength;
	}
}


void MusicScore::ReleaseTree()
{
    if (measureSegTree != nullptr) ReleaseTreeRC(measureSegTree);
    if (measureSegLazyTree != nullptr) ReleaseTreeRC(measureSegLazyTree);
    if (!segTreeView.empty()) segTreeView.clear();
    if (!segLazyTreeView.empty()) segLazyTreeView.clear();
}

RationalNumber<64> MusicScore::GetMeasureLength(int idx)
{
    if (measures.empty() || idx < 0) return 0;

    int trueIdx = min(idx, (int)measures.size() - 1);
    return measures[trueIdx].length;
}

RationalNumber<64> MusicScore::GetMeasureLength(int searchLeft, int searchRight)
{
    if (searchRight < searchLeft) return GetMeasureLength(searchRight, searchLeft);
    if (measures.empty() || searchRight < 0) return 0;

    if (searchLeft < 0) searchLeft = 0;

    if (measures.size() <= searchLeft) //out of measurelist range
    {
        const RationalNumber<64>& lastMeasureLength = measures.back().length;
        return lastMeasureLength * (searchRight - searchLeft + 1);
    }
    else
    {
        const int mostRightIdx = (int)measures.size() - 1;
        const int trueSearchRightIdx = min(searchRight, mostRightIdx);
        RationalNumber<64> result = GetMeasureLengthRC(measureSegTree, measureSegLazyTree, searchLeft, trueSearchRightIdx, 0, (int)measures.size() - 1);

        const int outRangeIdxCount = searchRight - mostRightIdx;
        if (outRangeIdxCount > 0)
        {
            const RationalNumber<64>& lastMeasureLength = measures.back().length;
            RationalNumber<64> outOfArea = lastMeasureLength * outRangeIdxCount;
            return result + outOfArea;
        }
        else return result;
    }
}

RationalNumber<64> MusicScore::GetMeasureLengthRC(MeasureNode* node, MeasureNode* lazyNode, int searchLeft, int searchRight, int listLeft, int listRight)
{
    if (node == nullptr) return RationalNumber<64>(0);

    UpdateLazyTree(node, lazyNode, listLeft, listRight);
    if (listRight < searchLeft || searchRight < listLeft) return 0;
    
    if (searchLeft <= listLeft && listRight <= searchRight) return node->totalLength;

    const int pivot = GetSegTreePivot(listLeft, listRight);

    MeasureNode* const& leftChild = node->pLeft;
    MeasureNode* const& rightChild = node->pRight;

    MeasureNode* const& leftLazyChild = lazyNode->pLeft;
    MeasureNode* const& rightLazyChild = lazyNode->pRight;

    return GetMeasureLengthRC(leftChild, leftLazyChild, searchLeft, searchRight, listLeft, pivot - 1)
         + GetMeasureLengthRC(rightChild, rightLazyChild, searchLeft, searchRight, pivot, listRight);

}

void MusicScore::ReleaseTreeRC(MeasureNode* node)
{
    if (node->pLeft != nullptr) ReleaseTreeRC(node->pLeft);
    if (node->pRight != nullptr) ReleaseTreeRC(node->pRight);
    SafeDelete(node);
}

void MusicScore::UpdateLazyTree(MeasureNode* node, MeasureNode* lazyNode, int listLeft, int listRight)
{
    if (lazyNode->totalLength != 0)
    {
        node->totalLength += ((*lazyNode).totalLength) * (listRight - listLeft + 1);

        //propagation
        if (listLeft != listRight)
        {
            lazyNode->pLeft->totalLength += lazyNode->totalLength;
            lazyNode->pRight->totalLength += lazyNode->totalLength;
        }
        lazyNode->totalLength = 0;
    }
}

void MusicScore::UpdateMeasureRC(MeasureNode* node, MeasureNode* lazyNode, int start, int end, RationalNumber<64> diff)
{
    UpdateLazyTree(node, lazyNode, start, end);

}

void MusicScore::RightRotate(MeasureNode* node)
{
    if (node == nullptr) return;

    MeasureNode* const leftChild = node->pLeft;
    if (leftChild == nullptr) return;

    node->pLeft = leftChild->pRight;
    leftChild->pRight = node;
}
