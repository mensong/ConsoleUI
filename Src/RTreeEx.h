//---------------------------------------------------
// 名称:	RTreeEx
// 功能:	R*树
// ------------------------------------------------------------
// 版本历史			注释                           日期
// ------------------------------------------------------------
// v1.0				初始版本                    2009-10-28
// ------------------------------------------------------------
// Note:	1.RTreeEx根据Douglas(道格拉斯)先生编写的RTree源码改写。增加了以下功能:
//			   1.1 内存数据库时采用独立堆栈。
//			2.数据模型采用RTree的变种R*树模型。与R树一样允许矩形的重叠，但在构造算法R*树时不仅考虑了索引空间的
//			  “面积”，而且还考虑了索引空间的重叠。该方法对结点的插入、分裂算法进行了改进，并采用了“强制重新
//			  插入”的方法使树的结构得到了优化。
//			2.1 R*树算法仍然不能有效降低空间的重叠程度，尤其是在数据量较大、空间维数较大时。实践证明，当维数达
//			  到5时，重叠率几乎达到90%。此时可考虑采用SS树(用最小边界圆代替最小边界矩形表示区域形状)。
//			2.2 R*树无法支持20维以上的情况。
//	
//			3.下一步改进计划:
//			  3.1 当数据量很大时支持使用外置数据库文件或XML保存数据
//			  3.2 内部结点组织方式不采用自定义指针链表，而用std::vector

#pragma once
#include <math.h>
#include <assert.h>
#include <set>		//采用set保存数据

#ifndef ASSERT
#define ASSERT assert
#endif

#ifndef _min
#define _min(a,b)    (((a) < (b)) ? (a) : (b))
#endif // !_min

#ifndef _max
#define _max(a,b)    (((a) > (b)) ? (a) : (b))
#endif // !_max


namespace RT
{
#define RTREE_EX_TEMPLATE template<class DATATYPE,class KEYTYPE,int NUMDIMS,class KEYTYPEREAL,int TMAXNODES,int TMINNODES>
#define RTREE_EX_PREFIX RTreeEx<DATATYPE,KEYTYPE,NUMDIMS,KEYTYPEREAL,TMAXNODES,TMINNODES>

#define RTREE_EX_DONT_USE_MEMPOOLS		//未使用自定义的堆栈技术。下一版本我们可以采用这一技术，可以提升性能，同时也优化了内存的管理。
#define RTREE_EX_USE_SPHERICAL_VOLUME	//在进行结点分裂计算时，采用求体积的算法。对数据插入计算的时间值稍有增加，但却减少了空间的重叠。查询速度提升

	//模板类参数说明:
	//	  DATATYPE - 对象指针类型，例如CObject*, void*,Plane*,等，不能够使用简单对象，也不应使用对象实体类型。在X86系统中此值不能够32位,
	//			否则将使得中间节点与叶节点大小不一致。指针类型刚好能够满足要求。
	//	  KEYTYPE - 边界矩形各维的元素类型，可以为int,float,double,long long,byte,char,UINT8,UINT16,UINT32,UINT64等
	//	  NUMDIMS  - 空间数据维数，一般为2，3。超过5以上建议换用SS或QR模型
	//	  KEYTYPEREAL - 程序内部在进行分裂算法时需要计算面积或体积，此时保存所用到的数据类型。可以采用与KEYTYPE一致，或者精度更高一些
	//	  TMAXNODES - 阶数，即每一个节点最多可以保存的数据量，可为3，4，...当此值较大时可以减少树的深度，但对每一节点查询及分裂运算时间有影响。建议3~20之间
	//	  TMINNODES - 下溢值，即每一节点最小存放数据量。低于此值时应删除此节点，将此节点上的数据插入到同层其它节点上。一般取TMAXNODES/2
	//使用范例: 
	//	  范例1:RTreeEx<CObject*,double,3> mRTree;
	//	  范例2:RTreeEx<PKPMVECTOR2*,float,2,double,6,3> myTree;
	template <typename DATATYPE, typename KEYTYPE, int NUMDIMS,
		typename KEYTYPEREAL = KEYTYPE, int TMAXNODES = 8, int TMINNODES = TMAXNODES / 2 >
	class RTreeEx
	{
	public:
		RTreeEx(DATATYPE invalidData = NULL);
		virtual ~RTreeEx();

	protected:
		struct Node;

	public:
		enum
		{
			MAXNODES = TMAXNODES,                         // 最大结点数量
			MINNODES = TMINNODES,                         // 最少结点数量
		};

	public://公开函数及其它
		/// 插入一个对象
		/// a_min - 最小边界矩形的n维最小值
		/// a_max - 最小边界矩形的n维最大值
		/// a_dataId 实体指针。可为空，但不能够为非法内存值
		// Notice: Every a_min[i] shout not bigger than a_max[i].
		bool Insert(const KEYTYPE a_min[NUMDIMS], const KEYTYPE a_max[NUMDIMS], const DATATYPE& a_dataId);

		/// 移除一个对象。如果边界矩形未完全包住实体，有可能无法删除
		/// a_min - 最小边界矩形的n维最小值
		/// a_max - 最小边界矩形的n维最大值
		/// a_dataId 实体指针。可为空，但不能够为非法内存值
		bool Remove(const KEYTYPE a_min[NUMDIMS], const KEYTYPE a_max[NUMDIMS], const DATATYPE& a_dataId);

		/// 通过最小边界矩形搜索对象
		/// a_min - 最小边界矩形的n维最小值
		/// a_max - 最小边界矩形的n维最大值
		/// setResult  - 返回查询到的结果集
		/// 返回搜索到的数量
		// Notice: Every a_min[i] shout not bigger than a_max[i].
		int Search(const KEYTYPE a_min[NUMDIMS], const KEYTYPE a_max[NUMDIMS], std::set<DATATYPE>* setResult = NULL)  const;

		/// 清除R*树
		void RemoveAll();

		/// 计数
		int Count() const;

		DATATYPE INVALID_DATA;							 //定义无效的数据

	protected://各种结构体
		/// 最小边界矩形(n维,n=NUMDIMS)
		struct BoundRect
		{
			KEYTYPE m_min[NUMDIMS];                      ///< 最小值(n维)
			KEYTYPE m_max[NUMDIMS];                      ///< 最大值(n维)
		};
		// 结点元数,内含值及边界矩形: 对于叶节点(level=0)指向数据, 对于中间节点(level!=0)指向子树的指针
		struct Branch
		{
			BoundRect m_BoundRect;                                  ///< Bounds
			union
			{
				Node* m_child;                              ///< Child node
				DATATYPE m_data;                            ///< Data Id or Ptr
			};
		};

		/// 节点:中间节点或叶节点
		struct Node
		{
			bool IsInternalNode()                         { return (m_level > 0); } // Not a leaf, but a internal node
			bool IsLeaf()                                 { return (m_level == 0); } // A leaf, contains data

			int m_count;                                  ///< 元数数量
			int m_level;                                  ///< 0 - 叶节点  >0 - 中间节点
			Branch m_branch[MAXNODES];                    ///< 元数
		};

		/// 节点链表(安插及删除节点时需要用到)
		struct ListNode
		{
			ListNode* m_next;                             ///< 下一节点
			Node* m_node;                                 ///< 节点
		};

		/// 为查找分裂位置所需要的结构体
		struct PartitionVars
		{
			int m_partition[MAXNODES + 1];
			int m_total;
			int m_minFill;
			int m_taken[MAXNODES + 1];
			int m_count[2];
			BoundRect m_cover[2];
			KEYTYPEREAL m_area[2];

			Branch m_branchBuf[MAXNODES + 1];
			int m_branchCount;
			BoundRect m_coverSplit;
			KEYTYPEREAL m_coverSplitArea;
		};

	protected://辅助函数
		Node* AllocNode();
		void FreeNode(Node* a_node);
		void InitNode(Node* a_node);
		void Reset();
		void RemoveAllRec(Node* a_node);
		void InitBoundRect(BoundRect* a_BoundRect);
		bool InsertBoundRect(BoundRect* a_BoundRect, const DATATYPE& a_id, Node** a_root, int a_level);
		bool InsertBoundRectRec(BoundRect* a_BoundRect, const DATATYPE& a_id, Node* a_node, Node** a_newNode, int a_level);
		BoundRect NodeCover(Node* a_node);
		bool AddBranch(Branch* a_branch, Node* a_node, Node** a_newNode);
		void DisconnectBranch(Node* a_node, int a_index);
		int PickBranch(BoundRect* a_BoundRect, Node* a_node);
		BoundRect CombineBoundRect(BoundRect* a_BoundRectA, BoundRect* a_BoundRectB);
		void SplitNode(Node* a_node, Branch* a_branch, Node** a_newNode);
		KEYTYPEREAL BoundRectSphericalVolume(BoundRect* a_BoundRect);
		KEYTYPEREAL BoundRectVolume(BoundRect* a_BoundRect);
		KEYTYPEREAL CalcBoundRectVolume(BoundRect* a_BoundRect);
		void GetBranches(Node* a_node, Branch* a_branch, PartitionVars* a_parVars);
		void ChoosePartition(PartitionVars* a_parVars, int a_minFill);
		void LoadNodes(Node* a_nodeA, Node* a_nodeB, PartitionVars* a_parVars);
		void InitParVars(PartitionVars* a_parVars, int a_maxBoundRects, int a_minFill);
		void PickSeeds(PartitionVars* a_parVars);
		void Classify(int a_index, int a_group, PartitionVars* a_parVars);
		int RemoveBoundRect(BoundRect* a_BoundRect, const DATATYPE& a_id, Node** a_root);
		int RemoveBoundRectRec(BoundRect* a_BoundRect, const DATATYPE& a_id, Node* a_node, ListNode** a_listNode);
		ListNode* AllocListNode();
		void FreeListNode(ListNode* a_listNode);
		bool Overlap(BoundRect* a_BoundRectA, BoundRect* a_BoundRectB)  const;
		void ReInsert(Node* a_node, ListNode** a_listNode);
		/*bool Search(Node* a_node, BoundRect* a_BoundRect, int& a_foundCount, bool __cdecl a_resultCallback(DATATYPE a_data, void* a_context), void* a_context);*/
		bool Search(Node* a_node, BoundRect* a_BoundRect, int& a_foundCount, std::set<DATATYPE>* setResult = NULL)  const;
		void CountRec(Node* a_node, int& a_count);

	private://成员变量
		Node* m_root;                                    ///< 根节点
		KEYTYPEREAL m_unitSphereVolume;                  ///< n维单位球体体积常数
#ifndef RTREE_EX_DONT_USE_MEMPOOLS
		HANDLE m_pHeap;									 //独立堆
#endif // !RTREE_EX_DONT_USE_MEMPOOLS
	};


	///////////////////////////////////////////////实现///////////////////////////////////////////////

	RTREE_EX_TEMPLATE
		RTREE_EX_PREFIX::RTreeEx(DATATYPE invalidData/* = NULL*/)
	{
			assert(MAXNODES > MINNODES);
			ASSERT(MINNODES > 0);

			// DATATYPE类型需要为32位简单数据类型或32位指针。因为叶节点与中间节点使用统一结构
			ASSERT(sizeof(DATATYPE) == sizeof(void*) || sizeof(DATATYPE) == sizeof(int));

			// 对于20维及以下的边界单位球体体积常数
			const float UNIT_SPHERE_VOLUMES[] = {
				0.000000f, 2.000000f, 3.141593f, // Dimension  0,1,2
				4.188790f, 4.934802f, 5.263789f, // Dimension  3,4,5
				5.167713f, 4.724766f, 4.058712f, // Dimension  6,7,8
				3.298509f, 2.550164f, 1.884104f, // Dimension  9,10,11
				1.335263f, 0.910629f, 0.599265f, // Dimension  12,13,14
				0.381443f, 0.235331f, 0.140981f, // Dimension  15,16,17
				0.082146f, 0.046622f, 0.025807f, // Dimension  18,19,20 
			};

			INVALID_DATA = invalidData;

#ifndef RTREE_EX_DONT_USE_MEMPOOLS
			m_pHeap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
#endif // !RTREE_EX_DONT_USE_MEMPOOLS
			m_root = AllocNode();
			m_root->m_level = 0;
			m_unitSphereVolume = (KEYTYPEREAL)UNIT_SPHERE_VOLUMES[NUMDIMS];
		}


	RTREE_EX_TEMPLATE
		RTREE_EX_PREFIX::~RTreeEx()
	{
			Reset(); // Free, or reset node memory
#ifndef RTREE_EX_DONT_USE_MEMPOOLS
			HeapDestroy(m_pHeap);
#endif // !RTREE_EX_DONT_USE_MEMPOOLS
		}


	RTREE_EX_TEMPLATE
		typename RTREE_EX_PREFIX::Node* RTREE_EX_PREFIX::AllocNode()
	{
			Node* newNode = NULL;
#ifdef RTREE_EX_DONT_USE_MEMPOOLS
			newNode = new Node;
#else // RTREE_EX_DONT_USE_MEMPOOLS
			//if(m_pHeap)
			//{
			newNode = (RTREE_EX_PREFIX::Node*)HeapAlloc(m_pHeap, HEAP_ZERO_MEMORY, sizeof(Node));
			//}
			// 采用独立堆栈
#endif // RTREE_EX_DONT_USE_MEMPOOLS
			InitNode(newNode);
			return newNode;
		}


	RTREE_EX_TEMPLATE
		void RTREE_EX_PREFIX::InitNode(Node* a_node)
	{
			a_node->m_count = 0;
			a_node->m_level = -1;
		}


	RTREE_EX_TEMPLATE
		void RTREE_EX_PREFIX::Reset()
	{
#ifdef RTREE_EX_DONT_USE_MEMPOOLS
			// Delete all existing nodes
			RemoveAllRec(m_root);
#else // RTREE_EX_DONT_USE_MEMPOOLS
			// 独立堆栈卸载
			RemoveAllRec(m_root);
#endif // RTREE_EX_DONT_USE_MEMPOOLS
		}


	RTREE_EX_TEMPLATE
		void RTREE_EX_PREFIX::RemoveAllRec(Node* a_node)
	{//采用递归模式。需要注意递归的深度，避免内存用完
			ASSERT(a_node);
			ASSERT(a_node->m_level >= 0);

			if (a_node->IsInternalNode()) // 如果是中间节点，则将下级节点都删除
			{
				for (int index = 0; index < a_node->m_count; ++index)
				{
					RemoveAllRec(a_node->m_branch[index].m_child);
				}
			}
			FreeNode(a_node);
		}


	RTREE_EX_TEMPLATE
		void RTREE_EX_PREFIX::FreeNode(Node* a_node)
	{
			ASSERT(a_node);

#ifdef RTREE_EX_DONT_USE_MEMPOOLS
			delete a_node;
#else // RTREE_EX_DONT_USE_MEMPOOLS
			// 独立堆栈卸载
			HeapFree(m_pHeap, HEAP_NO_SERIALIZE, a_node);
#endif // RTREE_EX_DONT_USE_MEMPOOLS
		}


	RTREE_EX_TEMPLATE
		bool RTREE_EX_PREFIX::Insert(const KEYTYPE a_min[NUMDIMS], const KEYTYPE a_max[NUMDIMS], const DATATYPE& a_dataId)
	{
#ifdef _DEBUG
			for (int index = 0; index<NUMDIMS; ++index)
			{
				ASSERT(a_min[index] <= a_max[index]);
			}
#endif //_DEBUG
			if (a_dataId == INVALID_DATA)
				return false;

			BoundRect boundRect;

			for (int axis = 0; axis<NUMDIMS; ++axis)
			{
				boundRect.m_min[axis] = a_min[axis];
				boundRect.m_max[axis] = a_max[axis];
			}

			InsertBoundRect(&boundRect, a_dataId, &m_root, 0);

			return true;
		}


	// 根据实体的边界矩形将实体插入到树中
	// a_root - 分裂算法的根节点
	// 当节点需要分裂时返回true,否则返回false
	// The level argument specifies the number of steps up from the leaf
	// level to insert; e.g. a data BoundRectangle goes in at level = 0.
	// InsertBoundRect2 递归调用.
	RTREE_EX_TEMPLATE
		bool RTREE_EX_PREFIX::InsertBoundRect(BoundRect* a_BoundRect, const DATATYPE& a_id, Node** a_root, int a_level)
	{
			ASSERT(a_BoundRect && a_root);
			ASSERT(a_level >= 0 && a_level <= (*a_root)->m_level);
#ifdef _DEBUG
			for (int index = 0; index < NUMDIMS; ++index)
			{
				ASSERT(a_BoundRect->m_min[index] <= a_BoundRect->m_max[index]);
			}
#endif //_DEBUG  

			Node* newRoot = NULL;
			Node* newNode = NULL;
			Branch branch;

			if (InsertBoundRectRec(a_BoundRect, a_id, *a_root, &newNode, a_level))  // Root split
			{
				newRoot = AllocNode();  // Grow tree taller and new root
				newRoot->m_level = (*a_root)->m_level + 1;
				branch.m_BoundRect = NodeCover(*a_root);
				branch.m_child = *a_root;
				AddBranch(&branch, newRoot, NULL);
				branch.m_BoundRect = NodeCover(newNode);
				branch.m_child = newNode;
				AddBranch(&branch, newRoot, NULL);
				*a_root = newRoot;
				return true;
			}

			return false;
		}


	// Inserts a new data BoundRectangle into the index structure.
	// Recursively descends tree, propagates splits back up.
	// Returns 0 if node was not split.  Old node updated.
	// If node was split, returns 1 and sets the pointer pointed to by
	// new_node to point to the new node.  Old node updated to become one of two.
	// The level argument specifies the number of steps up from the leaf
	// level to insert; e.g. a data BoundRectangle goes in at level = 0.
	RTREE_EX_TEMPLATE
		bool RTREE_EX_PREFIX::InsertBoundRectRec(BoundRect* a_BoundRect, const DATATYPE& a_id, Node* a_node, Node** a_newNode, int a_level)
	{
			ASSERT(a_BoundRect && a_node && a_newNode);
			ASSERT(a_level >= 0 && a_level <= a_node->m_level);

			int index;
			Branch branch;
			Node* otherNode;

			// Still above level for insertion, go down tree recursively
			if (a_node->m_level > a_level)
			{
				index = PickBranch(a_BoundRect, a_node);
				if (!InsertBoundRectRec(a_BoundRect, a_id, a_node->m_branch[index].m_child, &otherNode, a_level))
				{
					// Child was not split
					a_node->m_branch[index].m_BoundRect = CombineBoundRect(a_BoundRect, &(a_node->m_branch[index].m_BoundRect));
					return false;
				}
				else // Child was split
				{
					a_node->m_branch[index].m_BoundRect = NodeCover(a_node->m_branch[index].m_child);
					branch.m_child = otherNode;
					branch.m_BoundRect = NodeCover(otherNode);
					return AddBranch(&branch, a_node, a_newNode);
				}
			}
			else if (a_node->m_level == a_level) // Have reached level for insertion. Add BoundRect, split if necessary
			{
				branch.m_BoundRect = *a_BoundRect;
				branch.m_child = (Node*)a_id;
				// Child field of leaves contains id of data record
				return AddBranch(&branch, a_node, a_newNode);
			}
			else
			{
				// Should never occur
				ASSERT(0);
				return false;
			}
		}


	RTREE_EX_TEMPLATE
		bool RTREE_EX_PREFIX::Remove(const KEYTYPE a_min[NUMDIMS], const KEYTYPE a_max[NUMDIMS], const DATATYPE& a_dataId)
	{
#ifdef _DEBUG
			for (int index = 0; index<NUMDIMS; ++index)
			{
				ASSERT(a_min[index] <= a_max[index]);
			}
#endif //_DEBUG

			BoundRect boundRect;

			for (int axis = 0; axis<NUMDIMS; ++axis)
			{
				boundRect.m_min[axis] = a_min[axis];
				boundRect.m_max[axis] = a_max[axis];
			}

			if (INVALID_DATA == a_dataId)
			{//如果没有指定数据,则删除与区域相交的所有节点
				int foundCount = 0;
				std::set<DATATYPE> setRes;
				Search(m_root, &boundRect, foundCount, &setRes);
				if (0 == foundCount)
					return true;

				for (std::set<DATATYPE>::iterator it = setRes.begin(); it != setRes.end(); ++it)
				{
					RemoveBoundRect(&boundRect, (DATATYPE)(*it), &m_root);
				}

				return true;
			}

			//删除有指定数据的节点
			return (0 == RemoveBoundRect(&boundRect, a_dataId, &m_root));
		}


	RTREE_EX_TEMPLATE
		int RTREE_EX_PREFIX::Search(const KEYTYPE a_min[NUMDIMS], const KEYTYPE a_max[NUMDIMS], std::set<DATATYPE>* setResult)  const
	{
#ifdef _DEBUG
			for (int index = 0; index<NUMDIMS; ++index)
			{
				ASSERT(a_min[index] <= a_max[index]);
			}
#endif //_DEBUG

			BoundRect boundRect;

			for (int axis = 0; axis<NUMDIMS; ++axis)
			{
				boundRect.m_min[axis] = a_min[axis];
				boundRect.m_max[axis] = a_max[axis];
			}

			// NOTE: May want to return search result another way, perhaps returning the number of found elements here.

			int foundCount = 0;
			Search(m_root, &boundRect, foundCount, setResult);

			return foundCount;
		}


	RTREE_EX_TEMPLATE
		int RTREE_EX_PREFIX::Count()  const
	{
			int count = 0;
			CountRec(m_root, count);

			return count;
		}



	RTREE_EX_TEMPLATE
		void RTREE_EX_PREFIX::CountRec(Node* a_node, int& a_count)
	{
			if (a_node->IsInternalNode())  // not a leaf node
			{
				for (int index = 0; index < a_node->m_count; ++index)
				{
					CountRec(a_node->m_branch[index].m_child, a_count);
				}
			}
			else // A leaf node
			{
				a_count += a_node->m_count;
			}
		}


	RTREE_EX_TEMPLATE
		void RTREE_EX_PREFIX::RemoveAll()
	{
			// Delete all existing nodes
			Reset();

			m_root = AllocNode();
			m_root->m_level = 0;
		}


	// Allocate space for a node in the list used in DeletBoundRect to
	// store Nodes that are too empty.
	RTREE_EX_TEMPLATE
		typename RTREE_EX_PREFIX::ListNode* RTREE_EX_PREFIX::AllocListNode()
	{
#ifdef RTREE_EX_DONT_USE_MEMPOOLS
			return new ListNode;
#else // RTREE_EX_DONT_USE_MEMPOOLS
			// EXAMPLE
			ListNode* pListNode = (ListNode*)HeapAlloc(m_pHeap, HEAP_ZERO_MEMORY, sizeof(ListNode));
			return pListNode;
#endif // RTREE_EX_DONT_USE_MEMPOOLS
		}


	RTREE_EX_TEMPLATE
		void RTREE_EX_PREFIX::FreeListNode(ListNode* a_listNode)
	{
#ifdef RTREE_EX_DONT_USE_MEMPOOLS
			delete a_listNode;
#else // RTREE_EX_DONT_USE_MEMPOOLS
			bool bRet = HeapFree(m_pHeap, HEAP_NO_SERIALIZE, a_listNode);
			// EXAMPLE
#endif // RTREE_EX_DONT_USE_MEMPOOLS
		}



	RTREE_EX_TEMPLATE
		void RTREE_EX_PREFIX::InitBoundRect(BoundRect* a_BoundRect)
	{
			for (int index = 0; index < NUMDIMS; ++index)
			{
				a_BoundRect->m_min[index] = (KEYTYPE)0;
				a_BoundRect->m_max[index] = (KEYTYPE)0;
			}
		}


	// Find the smallest BoundRectangle that includes all BoundRectangles in branches of a node.
	RTREE_EX_TEMPLATE
		typename RTREE_EX_PREFIX::BoundRect RTREE_EX_PREFIX::NodeCover(Node* a_node)
	{
			ASSERT(a_node);

			int firstTime = true;
			BoundRect boundRect;
			InitBoundRect(&boundRect);

			for (int index = 0; index < a_node->m_count; ++index)
			{
				if (firstTime)
				{
					boundRect = a_node->m_branch[index].m_BoundRect;
					firstTime = false;
				}
				else
				{
					boundRect = CombineBoundRect(&boundRect, &(a_node->m_branch[index].m_BoundRect));
				}
			}

			return boundRect;
		}


	// Add a branch to a node.  Split the node if necessary.
	// Returns 0 if node not split.  Old node updated.
	// Returns 1 if node split, sets *new_node to address of new node.
	// Old node updated, becomes one of two.
	RTREE_EX_TEMPLATE
		bool RTREE_EX_PREFIX::AddBranch(Branch* a_branch, Node* a_node, Node** a_newNode)
	{
			ASSERT(a_branch);
			ASSERT(a_node);

			if (a_node->m_count < MAXNODES)  // Split won't be necessary
			{
				a_node->m_branch[a_node->m_count] = *a_branch;
				++a_node->m_count;

				return false;
			}
			else
			{
				ASSERT(a_newNode);

				SplitNode(a_node, a_branch, a_newNode);
				return true;
			}
		}


	// Disconnect a dependent node.
	// Caller must return (or stop using iteration index) after this as count has changed
	RTREE_EX_TEMPLATE
		void RTREE_EX_PREFIX::DisconnectBranch(Node* a_node, int a_index)
	{
			ASSERT(a_node && (a_index >= 0) && (a_index < MAXNODES));
			ASSERT(a_node->m_count > 0);

			// Remove element by swapping with the last element to prevent gaps in array
			a_node->m_branch[a_index] = a_node->m_branch[a_node->m_count - 1];

			--a_node->m_count;
		}


	// Pick a branch.  Pick the one that will need the smallest increase
	// in area to accomodate the new BoundRectangle.  This will result in the
	// least total area for the covering BoundRectangles in the current node.
	// In case of a tie, pick the one which was smaller before, to get
	// the best resolution when searching.
	RTREE_EX_TEMPLATE
		int RTREE_EX_PREFIX::PickBranch(BoundRect* a_BoundRect, Node* a_node)
	{
			ASSERT(a_BoundRect && a_node);

			bool firstTime = true;
			KEYTYPEREAL increase;
			KEYTYPEREAL bestIncr = (KEYTYPEREAL)-1;
			KEYTYPEREAL area;
			KEYTYPEREAL bestArea;
			int best;
			BoundRect tempBoundRect;

			for (int index = 0; index < a_node->m_count; ++index)
			{
				BoundRect* curBoundRect = &a_node->m_branch[index].m_BoundRect;
				area = CalcBoundRectVolume(curBoundRect);
				tempBoundRect = CombineBoundRect(a_BoundRect, curBoundRect);
				increase = CalcBoundRectVolume(&tempBoundRect) - area;
				if ((increase < bestIncr) || firstTime)
				{
					best = index;
					bestArea = area;
					bestIncr = increase;
					firstTime = false;
				}
				else if ((increase == bestIncr) && (area < bestArea))
				{
					best = index;
					bestArea = area;
					bestIncr = increase;
				}
			}
			return best;
		}


	// Combine two BoundRectangles into larger one containing both
	RTREE_EX_TEMPLATE
		typename RTREE_EX_PREFIX::BoundRect RTREE_EX_PREFIX::CombineBoundRect(BoundRect* a_BoundRectA, BoundRect* a_BoundRectB)
	{
			ASSERT(a_BoundRectA && a_BoundRectB);

			BoundRect newBoundRect;

			for (int index = 0; index < NUMDIMS; ++index)
			{
				newBoundRect.m_min[index] = _min(a_BoundRectA->m_min[index], a_BoundRectB->m_min[index]);
				newBoundRect.m_max[index] = _max(a_BoundRectA->m_max[index], a_BoundRectB->m_max[index]);
			}

			return newBoundRect;
		}



	// Split a node.
	// Divides the nodes branches and the extra one between two nodes.
	// Old node is one of the new ones, and one really new one is created.
	// Tries more than one method for choosing a partition, uses best result.
	RTREE_EX_TEMPLATE
		void RTREE_EX_PREFIX::SplitNode(Node* a_node, Branch* a_branch, Node** a_newNode)
	{
			ASSERT(a_node);
			ASSERT(a_branch);

			// Could just use local here, but member or external is faster since it is reused
			PartitionVars localVars;
			PartitionVars* parVars = &localVars;
			int level;

			// Load all the branches into a buffer, initialize old node
			level = a_node->m_level;
			GetBranches(a_node, a_branch, parVars);

			// Find partition
			ChoosePartition(parVars, MINNODES);

			// Put branches from buffer into 2 nodes according to chosen partition
			*a_newNode = AllocNode();
			(*a_newNode)->m_level = a_node->m_level = level;
			LoadNodes(a_node, *a_newNode, parVars);

			ASSERT((a_node->m_count + (*a_newNode)->m_count) == parVars->m_total);
		}


	// Calculate the n-dimensional volume of a BoundRectangle
	RTREE_EX_TEMPLATE
		KEYTYPEREAL RTREE_EX_PREFIX::BoundRectVolume(BoundRect* a_BoundRect)
	{
			ASSERT(a_BoundRect);

			KEYTYPEREAL volume = (KEYTYPEREAL)1;

			for (int index = 0; index<NUMDIMS; ++index)
			{
				volume *= a_BoundRect->m_max[index] - a_BoundRect->m_min[index];
			}

			ASSERT(volume >= (KEYTYPEREAL)0);

			return volume;
		}


	// The exact volume of the bounding sphere for the given BoundRect
	RTREE_EX_TEMPLATE
		KEYTYPEREAL RTREE_EX_PREFIX::BoundRectSphericalVolume(BoundRect* a_BoundRect)
	{
			ASSERT(a_BoundRect);

			KEYTYPEREAL sumOfSquares = (KEYTYPEREAL)0;
			KEYTYPEREAL radius;

			for (int index = 0; index < NUMDIMS; ++index)
			{
				KEYTYPEREAL halfExtent = ((KEYTYPEREAL)a_BoundRect->m_max[index] - (KEYTYPEREAL)a_BoundRect->m_min[index]) * 0.5f;
				sumOfSquares += halfExtent * halfExtent;
			}

			radius = (KEYTYPEREAL)sqrt(sumOfSquares);

			// Pow maybe slow, so test for common dims like 2,3 and just use x*x, x*x*x.
			if (NUMDIMS == 3)
			{
				return (radius * radius * radius * m_unitSphereVolume);
			}
			else if (NUMDIMS == 2)
			{
				return (radius * radius * m_unitSphereVolume);
			}
			else
			{
				return (KEYTYPEREAL)(pow(radius, NUMDIMS) * m_unitSphereVolume);
			}
		}


	// Use one of the methods to calculate retangle volume
	RTREE_EX_TEMPLATE
		KEYTYPEREAL RTREE_EX_PREFIX::CalcBoundRectVolume(BoundRect* a_BoundRect)
	{
#ifdef RTREE_USE_SPHERICAL_VOLUME
			return BoundRectSphericalVolume(a_BoundRect); // Slower but helps certain merge cases
#else // RTREE_USE_SPHERICAL_VOLUME
			return BoundRectVolume(a_BoundRect); // Faster but can cause poor merges
#endif // RTREE_USE_SPHERICAL_VOLUME  
		}


	// Load branch buffer with branches from full node plus the extra branch.
	RTREE_EX_TEMPLATE
		void RTREE_EX_PREFIX::GetBranches(Node* a_node, Branch* a_branch, PartitionVars* a_parVars)
	{
			ASSERT(a_node);
			ASSERT(a_branch);

			ASSERT(a_node->m_count == MAXNODES);

			// Load the branch buffer
			for (int index = 0; index < MAXNODES; ++index)
			{
				a_parVars->m_branchBuf[index] = a_node->m_branch[index];
			}
			a_parVars->m_branchBuf[MAXNODES] = *a_branch;
			a_parVars->m_branchCount = MAXNODES + 1;

			// Calculate BoundRect containing all in the set
			a_parVars->m_coverSplit = a_parVars->m_branchBuf[0].m_BoundRect;
			for (int index = 1; index < MAXNODES + 1; ++index)
			{
				a_parVars->m_coverSplit = CombineBoundRect(&a_parVars->m_coverSplit, &a_parVars->m_branchBuf[index].m_BoundRect);
			}
			a_parVars->m_coverSplitArea = CalcBoundRectVolume(&a_parVars->m_coverSplit);

			InitNode(a_node);
		}


	// Method #0 for choosing a partition:
	// As the seeds for the two groups, pick the two BoundRects that would waste the
	// most area if covered by a single BoundRectangle, i.e. evidently the worst pair
	// to have in the same group.
	// Of the remaining, one at a time is chosen to be put in one of the two groups.
	// The one chosen is the one with the greatest difference in area expansion
	// depending on which group - the BoundRect most strongly attracted to one group
	// and repelled from the other.
	// If one group gets too full (more would force other group to violate min
	// fill requirement) then other group gets the rest.
	// These last are the ones that can go in either group most easily.
	RTREE_EX_TEMPLATE
		void RTREE_EX_PREFIX::ChoosePartition(PartitionVars* a_parVars, int a_minFill)
	{
			ASSERT(a_parVars);

			KEYTYPEREAL biggestDiff;
			int group, chosen, betterGroup;

			InitParVars(a_parVars, a_parVars->m_branchCount, a_minFill);
			PickSeeds(a_parVars);

			while (((a_parVars->m_count[0] + a_parVars->m_count[1]) < a_parVars->m_total)
				&& (a_parVars->m_count[0] < (a_parVars->m_total - a_parVars->m_minFill))
				&& (a_parVars->m_count[1] < (a_parVars->m_total - a_parVars->m_minFill)))
			{
				biggestDiff = (KEYTYPEREAL)-1;
				for (int index = 0; index<a_parVars->m_total; ++index)
				{
					if (!a_parVars->m_taken[index])
					{
						BoundRect* curBoundRect = &a_parVars->m_branchBuf[index].m_BoundRect;
						BoundRect BoundRect0 = CombineBoundRect(curBoundRect, &a_parVars->m_cover[0]);
						BoundRect BoundRect1 = CombineBoundRect(curBoundRect, &a_parVars->m_cover[1]);
						KEYTYPEREAL growth0 = CalcBoundRectVolume(&BoundRect0) - a_parVars->m_area[0];
						KEYTYPEREAL growth1 = CalcBoundRectVolume(&BoundRect1) - a_parVars->m_area[1];
						KEYTYPEREAL diff = growth1 - growth0;
						if (diff >= 0)
						{
							group = 0;
						}
						else
						{
							group = 1;
							diff = -diff;
						}

						if (diff > biggestDiff)
						{
							biggestDiff = diff;
							chosen = index;
							betterGroup = group;
						}
						else if ((diff == biggestDiff) && (a_parVars->m_count[group] < a_parVars->m_count[betterGroup]))
						{
							chosen = index;
							betterGroup = group;
						}
					}
				}
				Classify(chosen, betterGroup, a_parVars);
			}

			// If one group too full, put remaining BoundRects in the other
			if ((a_parVars->m_count[0] + a_parVars->m_count[1]) < a_parVars->m_total)
			{
				if (a_parVars->m_count[0] >= a_parVars->m_total - a_parVars->m_minFill)
				{
					group = 1;
				}
				else
				{
					group = 0;
				}
				for (int index = 0; index<a_parVars->m_total; ++index)
				{
					if (!a_parVars->m_taken[index])
					{
						Classify(index, group, a_parVars);
					}
				}
			}

			ASSERT((a_parVars->m_count[0] + a_parVars->m_count[1]) == a_parVars->m_total);
			ASSERT((a_parVars->m_count[0] >= a_parVars->m_minFill) &&
				(a_parVars->m_count[1] >= a_parVars->m_minFill));
		}


	// Copy branches from the buffer into two nodes according to the partition.
	RTREE_EX_TEMPLATE
		void RTREE_EX_PREFIX::LoadNodes(Node* a_nodeA, Node* a_nodeB, PartitionVars* a_parVars)
	{
			ASSERT(a_nodeA);
			ASSERT(a_nodeB);
			ASSERT(a_parVars);

			for (int index = 0; index < a_parVars->m_total; ++index)
			{
				ASSERT(a_parVars->m_partition[index] == 0 || a_parVars->m_partition[index] == 1);

				if (a_parVars->m_partition[index] == 0)
				{
					AddBranch(&a_parVars->m_branchBuf[index], a_nodeA, NULL);
				}
				else if (a_parVars->m_partition[index] == 1)
				{
					AddBranch(&a_parVars->m_branchBuf[index], a_nodeB, NULL);
				}
			}
		}


	// Initialize a PartitionVars structure.
	RTREE_EX_TEMPLATE
		void RTREE_EX_PREFIX::InitParVars(PartitionVars* a_parVars, int a_maxBoundRects, int a_minFill)
	{
			ASSERT(a_parVars);

			a_parVars->m_count[0] = a_parVars->m_count[1] = 0;
			a_parVars->m_area[0] = a_parVars->m_area[1] = (KEYTYPEREAL)0;
			a_parVars->m_total = a_maxBoundRects;
			a_parVars->m_minFill = a_minFill;
			for (int index = 0; index < a_maxBoundRects; ++index)
			{
				a_parVars->m_taken[index] = false;
				a_parVars->m_partition[index] = -1;
			}
		}


	RTREE_EX_TEMPLATE
		void RTREE_EX_PREFIX::PickSeeds(PartitionVars* a_parVars)
	{
			int seed0 = -1, seed1 = -1;
			KEYTYPEREAL worst, waste;
			KEYTYPEREAL area[MAXNODES + 1];

			for (int index = 0; index<a_parVars->m_total; ++index)
			{
				area[index] = CalcBoundRectVolume(&a_parVars->m_branchBuf[index].m_BoundRect);
			}

			worst = -a_parVars->m_coverSplitArea - 1;
			for (int indexA = 0; indexA < a_parVars->m_total - 1; ++indexA)
			{
				for (int indexB = indexA + 1; indexB < a_parVars->m_total; ++indexB)
				{
					BoundRect oneBoundRect = CombineBoundRect(&a_parVars->m_branchBuf[indexA].m_BoundRect, &a_parVars->m_branchBuf[indexB].m_BoundRect);
					waste = CalcBoundRectVolume(&oneBoundRect) - area[indexA] - area[indexB];

					/************************************************************************/
					/*      if(waste > worst)   //原来代码是这样
					{
					worst = waste;
					seed0 = indexA;
					seed1 = indexB;
					}                                                                 */
					/************************************************************************/

					/************************************************************************/
					/* Rtree最坏点的判定并取得种子判断新增
					/************************************************************************/
					if (waste > worst || abs(waste - worst)<0.000001f)
					{
						worst = waste;
						seed0 = indexA;
						seed1 = indexB;
					}
				}
			}
			if (-1 != seed0)
				Classify(seed0, 0, a_parVars);
			if (-1 != seed1)
				Classify(seed1, 1, a_parVars);
		}


	// Put a branch in one of the groups.
	RTREE_EX_TEMPLATE
		void RTREE_EX_PREFIX::Classify(int a_index, int a_group, PartitionVars* a_parVars)
	{
			ASSERT(a_parVars);
			ASSERT(!a_parVars->m_taken[a_index]);

			a_parVars->m_partition[a_index] = a_group;
			a_parVars->m_taken[a_index] = true;

			if (a_parVars->m_count[a_group] == 0)
			{
				a_parVars->m_cover[a_group] = a_parVars->m_branchBuf[a_index].m_BoundRect;
			}
			else
			{
				a_parVars->m_cover[a_group] = CombineBoundRect(&a_parVars->m_branchBuf[a_index].m_BoundRect, &a_parVars->m_cover[a_group]);
			}
			a_parVars->m_area[a_group] = CalcBoundRectVolume(&a_parVars->m_cover[a_group]);
			++a_parVars->m_count[a_group];
		}


	// Delete a data BoundRectangle from an index structure.
	// Pass in a pointer to a BoundRect, the tid of the record, ptr to ptr to root node.
	// Returns 1 if record not found, 0 if success.
	// RemoveBoundRect provides for eliminating the root.
	RTREE_EX_TEMPLATE
		int RTREE_EX_PREFIX::RemoveBoundRect(BoundRect* a_BoundRect, const DATATYPE& a_id, Node** a_root)
	{
			ASSERT(a_BoundRect && a_root);
			ASSERT(*a_root);

			Node* tempNode;
			ListNode* reInsertList = NULL;

			if (0 == RemoveBoundRectRec(a_BoundRect, a_id, *a_root, &reInsertList))
			{
				// Found and deleted a data item
				// Reinsert any branches from eliminated nodes
				while (reInsertList)
				{
					tempNode = reInsertList->m_node;

					for (int index = 0; index < tempNode->m_count; ++index)
					{
						InsertBoundRect(&(tempNode->m_branch[index].m_BoundRect),
							tempNode->m_branch[index].m_data,
							a_root,
							tempNode->m_level);
					}

					ListNode* remLNode = reInsertList;
					reInsertList = reInsertList->m_next;

					FreeNode(remLNode->m_node);
					FreeListNode(remLNode);
				}

				// Check for redundant root (not leaf, 1 child) and eliminate
				if ((*a_root)->m_count == 1 && (*a_root)->IsInternalNode())
				{
					tempNode = (*a_root)->m_branch[0].m_child;

					ASSERT(tempNode);
					FreeNode(*a_root);
					*a_root = tempNode;
				}
				return 0;
			}
			else
			{
				return 1;
			}
		}


	// Delete a BoundRectangle from non-root part of an index structure.
	// Called by RemoveBoundRect.  Descends tree recursively,
	// merges branches on the way back up.
	// Returns 1 if record not found, 0 if success.
	RTREE_EX_TEMPLATE
		int RTREE_EX_PREFIX::RemoveBoundRectRec(BoundRect* a_BoundRect, const DATATYPE& a_id, Node* a_node, ListNode** a_listNode)
	{
			ASSERT(a_BoundRect && a_node && a_listNode);
			ASSERT(a_node->m_level >= 0);

			if (a_node->IsInternalNode())  // not a leaf node
			{
				for (int index = 0; index < a_node->m_count; ++index)
				{
					if (Overlap(a_BoundRect, &(a_node->m_branch[index].m_BoundRect)))
					{
						if (0 == RemoveBoundRectRec(a_BoundRect, a_id, a_node->m_branch[index].m_child, a_listNode))
						{
							if (a_node->m_branch[index].m_child->m_count >= MINNODES)
							{
								// child removed, just resize parent BoundRect
								a_node->m_branch[index].m_BoundRect = NodeCover(a_node->m_branch[index].m_child);
							}
							else
							{
								// child removed, not enough entries in node, eliminate node
								ReInsert(a_node->m_branch[index].m_child, a_listNode);
								DisconnectBranch(a_node, index); // Must return after this call as count has changed
							}
							return 0;
						}
					}
				}
				return 1;
			}
			else // A leaf node
			{
				for (int index = 0; index < a_node->m_count; ++index)
				{
					if (a_node->m_branch[index].m_child == (Node*)a_id)
					{
						DisconnectBranch(a_node, index); // Must return after this call as count has changed
						return 0;
					}
				}
				return 1;
			}
		}


	// Decide whether two BoundRectangles overlap.
	RTREE_EX_TEMPLATE
		bool RTREE_EX_PREFIX::Overlap(BoundRect* a_BoundRectA, BoundRect* a_BoundRectB) const
	{
			ASSERT(a_BoundRectA && a_BoundRectB);

			for (int index = 0; index < NUMDIMS; ++index)
			{
				if (a_BoundRectA->m_min[index] > a_BoundRectB->m_max[index] ||
					a_BoundRectB->m_min[index] > a_BoundRectA->m_max[index])
				{
					return false;
				}
			}
			return true;
		}


	// Add a node to the reinsertion list.  All its branches will later
	// be reinserted into the index structure.
	RTREE_EX_TEMPLATE
		void RTREE_EX_PREFIX::ReInsert(Node* a_node, ListNode** a_listNode)
	{
			ListNode* newListNode = AllocListNode();
			newListNode->m_node = a_node;
			newListNode->m_next = *a_listNode;
			*a_listNode = newListNode;
		}


	// Search in an index tree or subtree for all data retangles that overlap the argument BoundRectangle.
	RTREE_EX_TEMPLATE
		bool RTREE_EX_PREFIX::Search(Node* a_node, BoundRect* a_BoundRect, int& a_foundCount, std::set<DATATYPE>* setResult)  const
	{
			ASSERT(a_node);
			ASSERT(a_node->m_level >= 0);
			ASSERT(a_BoundRect);

			if (a_node->IsInternalNode()) // This is an internal node in the tree
			{
				for (int index = 0; index < a_node->m_count; ++index)
				{
					if (Overlap(a_BoundRect, &a_node->m_branch[index].m_BoundRect))
					{
						if (!Search(a_node->m_branch[index].m_child, a_BoundRect, a_foundCount, setResult))
						{
							return false; // Don't continue searching
						}
					}
				}
			}
			else // This is a leaf node
			{
				for (int index = 0; index < a_node->m_count; ++index)
				{
					if (Overlap(a_BoundRect, &a_node->m_branch[index].m_BoundRect))
					{
						DATATYPE& id = a_node->m_branch[index].m_data;

						++a_foundCount;

						if (NULL != setResult)
							setResult->insert(id);

						// NOTE: There are different ways to return results.  Here's where to modify
						//if(&a_resultCallback)
						//{
						//  if(!a_resultCallback(id, a_context))
						//  {
						//    return false; // Don't continue searching
						//  }
						//}
					}
				}
			}

			return true; // Continue searching
		}
}