
#include "utility.hpp"
#include <functional>
#include <cstddef>
#include "exception.hpp"
#include <map>
#include <fstream>
const int MAX_BUFFER_SIZE = 4096;
using namespace std;

namespace sjtu
{
	template <class Key, class Value, class Compare = std::less<Key>>
	class BTree
	{
	private:
		// Your private members go here
	public:
		typedef std::pair<Key, Value> value_type;

		class const_iterator;
		class iterator
		{
		private:
			// Your private members go here
		public:
			bool modify(const Value& value)
			{
			}
			iterator()
			{
				// TODO Default Constructor
			}
			iterator(const iterator& other)
			{
				// TODO Copy Constructor
			}
			// Return a new iterator which points to the n-next elements
			iterator operator++(int)
			{
				// Todo iterator++
			}
			iterator& operator++()
			{
				// Todo ++iterator
			}
			iterator operator--(int)
			{
				// Todo iterator--
			}
			iterator& operator--()
			{
				// Todo --iterator
			}
			// Overloaded of operator '==' and '!='
			// Check whether the iterators are same
			bool operator==(const iterator& rhs) const
			{
				// Todo operator ==
			}
			bool operator==(const const_iterator& rhs) const
			{
				// Todo operator ==
			}
			bool operator!=(const iterator& rhs) const
			{
				// Todo operator !=
			}
			bool operator!=(const const_iterator& rhs) const
			{
				// Todo operator !=
			}
		};
		class const_iterator
		{
			// it should has similar member method as iterator.
			//  and it should be able to construct from an iterator.
		private:
			// Your private members go here
		public:
			const_iterator()
			{
				// TODO
			}
			const_iterator(const const_iterator& other)
			{
				// TODO
			}
			const_iterator(const iterator& other)
			{
				// TODO
			}
			// And other methods in iterator, please fill by yourself.
		};

		static const int idxSize = 500;
		static const int dataBlkSize = 200;
		struct idxNode
		{
			size_t offset;
			bool isEmpty;
			int type;
			Key key[idxSize - 1];
			size_t idx[idxSize];
			int len;
			idxNode() : len(1), isEmpty(false) {}
		};

		struct dataNode
		{
			size_t offset;
			bool isEmpty;
			int len;
			value_type record[dataBlkSize];
			dataNode() : len(1), isEmpty(false) {}
		};
		//idxNode* root;
	private:
		fstream fidx, fdat;
		char Buffer[MAX_BUFFER_SIZE];
	public:
		// Default Constructor and Copy Constructor
		BTree()
		{
			//cout << sizeof(Buffer) << endl;
			//cout << sizeof(idxNode) << endl;
			//cout << sizeof(dataNode) << endl;
			// Todo Default
			//root = NULL;
			openFile();
		}
		BTree(const BTree& other)
		{
			// Todo Copy
		}
		BTree& operator=(const BTree& other)
		{
			// Todo Assignment
		}
		~BTree()
		{
			closeFile();
			// Todo Destructor
		}
		void openFile()
		{
			fidx.open("idx.dat", ios::in);
			if (!fidx)
			{
				fidx.close();
				init();
			}
			fidx.open("idx.dat", ios::out | ios::in | ios::binary);
			//fidx << "test" << endl;
		}
		void closeFile()
		{
			fidx.flush();
			fidx.close();
		}
		void init()
		{
			fidx.open("idx.dat", ios::out);
			idxNode root;
			root.offset = 0;
			root.isEmpty = true;
			root.len = 1;
			writeFile(&root, root.offset, sizeof(idxNode));
			fidx.close();
		}
		inline void writeFile(void* place, size_t offset, size_t size)
		{
			fidx.clear();
			fidx.seekp(offset, ios::beg);
			fidx.write(reinterpret_cast<char*>(place), size);
			fidx.flush();
		}
		inline void readFile(void* place, size_t offset, size_t size)
		{
			fidx.clear();
			fidx.seekg(offset, ios::beg);
			//streampos len = fidx.tellg();
			//cout << len << endl;
			fidx.read(Buffer, size);
			memcpy(place, Buffer, size);
		}
		inline size_t getFileLen()
		{
			fidx.clear();
			fidx.seekg(0, ios::end);
			return fidx.tellg();
		}
		inline void getRoot(idxNode& root)
		{
			readFile(&root, 0, sizeof(idxNode));
		}
		// Insert: Insert certain Key-Value into the database
		// Return a pair, the first of the pair is the iterator point to the new
		// element, the second of the pair is Success if it is successfully inserted
		pair<iterator, OperationResult> insert(const Key& key, const Value& value)
		{
			iterator x;
			pair<iterator, OperationResult> r(x, Fail);

			idxNode root;
			getRoot(root);
			//readFile(&root, 0, sizeof(idxNode));//need modify
			if (root.isEmpty)
			{
				root.isEmpty = false;
				//root = new idxNode;
				//root->type = 1;
				root.type = 1;
				//root->idx[0] = new dataNode;
				dataNode son;
				root.idx[0] = son.offset = getFileLen();
				son.record[0] = make_pair(key, value);

				writeFile(&root, root.offset, sizeof(idxNode));
				writeFile(&son, son.offset, sizeof(dataNode));
				//dataNode* p = (dataNode*)root->idx[0];
				//p->record[0] = make_pair(key, value);
				return r;
			}

			idxNode p = insert(key, value, 0);//need modify
			//idxNode* p = insert(key, value, root);
			if (!p.isEmpty)
			{
				/*idxNode* t = root;
				root = new idxNode;
				root->type = 0;
				root->len = 2;
				root->idx[0] = t;
				root->idx[1] = p;
				while (p->type == 0) p = (idxNode*)p->idx[0];
				dataNode* d = (dataNode*)p->idx[0];
				root->key[0] = d->record[0].first;*/
				idxNode t = root;
				root.offset = getFileLen();
				root.type = 0;
				root.len = 2;
				root.idx[0] = t.offset;//need modify
				root.idx[1] = p.offset;
				while (p.type == 0) readFile(&p, p.idx[0], sizeof(idxNode));
				dataNode d;
				readFile(&d, p.idx[0], sizeof(dataNode));
				root.key[0] = d.record[0].first;

				writeFile(&root, root.offset, sizeof(idxNode));
			}

			return r;
		}
		idxNode insert(const Key& key, const Value& value, size_t offset_t)
		{
			//void* newNode;
			idxNode emptyNode;
			emptyNode.isEmpty = true;
			idxNode t;
			readFile(&t, offset_t, sizeof(idxNode));

			int i;
			for (i = 0; i < t.len - 1; ++i)
				if (key < t.key[i]) break;

			if (t.type == 0)
			{
				idxNode newNode;
				newNode = insert(key, value, t.idx[i]);
				if (newNode.isEmpty) return emptyNode;
				else return addIdxBlk(newNode, t);
			}
			else
			{
				dataNode newNode;
				newNode = insertData(key, value, t.idx[i]);
				if (newNode.isEmpty) return emptyNode;
				else return addDataBlk(newNode, t);
			}
		}
		dataNode insertData(const Key& key, const Value& value, size_t offset_t)
		{
			dataNode emptyNode;
			emptyNode.isEmpty = true;
			dataNode t;
			readFile(&t, offset_t, sizeof(dataNode));

			for (int i = 0; i < t.len; ++i)
				if (t.record[i].first == key) return emptyNode;

			if (t.len < dataBlkSize)
			{
				int i;
				for (i = t.len; i > 0 && key < t.record[i - 1].first; --i)
					t.record[i] = t.record[i - 1];
				t.record[i] = make_pair(key, value);
				++(t.len);

				writeFile(&t, t.offset, sizeof(dataNode));
				return emptyNode;
			}

			//dataNode* newNode = new dataNode;
			dataNode newNode;
			newNode.offset = getFileLen();
			int i, j;
			int max = dataBlkSize / 2;
			newNode.len = max + 1;
			for (i = max, j = dataBlkSize - 1; i >= 0 && t.record[j].first > key; --i)
				newNode.record[i] = t.record[j--];
			if (i >= 0)
			{
				newNode.record[i] = make_pair(key, value);
				--i;
			}
			for (; i >= 0; --i) newNode.record[i] = t.record[j--];

			t.len = dataBlkSize - max;
			if (j < t.len - 1)
			{
				for (; j >= 0 && key < t.record[j].first; --j)
					t.record[j + 1] = t.record[j];
				t.record[j + 1] = make_pair(key, value);
			}

			writeFile(&newNode, newNode.offset, sizeof(dataNode));
			writeFile(&t, t.offset, sizeof(dataNode));
			return newNode;
		}
		idxNode addIdxBlk(idxNode newNode, idxNode t)
		{
			idxNode emptyNode;
			emptyNode.isEmpty = true;

			//idxNode* p = newNode;
			idxNode p = newNode;

			while (p.type == 0) readFile(&p, p.idx[0], sizeof(idxNode));
			//dataNode* d = (dataNode*)p->idx[0];
			dataNode d;
			readFile(&d, p.idx[0], sizeof(dataNode));
			Key min = d.record[0].first;

			if (t.len < idxSize)
			{
				int i;
				for (i = t.len - 1; i > 0 && min < t.key[i - 1]; --i)
				{
					t.key[i] = t.key[i - 1];
					t.idx[i + 1] = t.idx[i];
				}
				t.idx[i + 1] = newNode.offset;
				t.key[i] = min;
				++(t.len);

				writeFile(&t, t.offset, sizeof(idxNode));
				return emptyNode;
			}

			//idxNode* newIdx = new idxNode;
			idxNode newIdx;
			newIdx.offset = getFileLen();
			newIdx.type = 0;
			int max = idxSize / 2;
			newIdx.len = max + 1;
			int i, j;

			if (min > t.key[idxSize - 2])
			{
				newIdx.key[max - 1] = min;
				newIdx.idx[max] = newNode.offset;
			}
			else
			{
				newIdx.key[max - 1] = t.key[idxSize - 2];
				newIdx.idx[max] = t.idx[idxSize - 1];
				int i;
				for (i = t.len - 2; i > 0 && min < t.key[i - 1]; --i)
				{
					t.key[i] = t.key[i - 1];
					t.idx[i + 1] = t.idx[i];
				}
				t.key[i] = min;
				t.idx[i + 1] = newNode.offset;
			}

			for (i = max - 1, j = idxSize - 1; i > 0; --i, --j)
			{
				newIdx.idx[i] = t.idx[j];
				newIdx.key[i - 1] = t.key[j - 1];
			}
			newIdx.idx[0] = t.idx[j];

			t.len = idxSize - max;

			writeFile(&newIdx, newIdx.offset, sizeof(idxNode));
			writeFile(&t, t.offset, sizeof(idxNode));

			return newIdx;
		}
		idxNode addDataBlk(dataNode newNode, idxNode t)
		{
			idxNode emptyNode;
			emptyNode.isEmpty = true;

			if (t.len < idxSize)
			{
				int i;
				for (i = t.len - 1; i > 0 && newNode.record[0].first < t.key[i - 1]; --i)
				{
					t.key[i] = t.key[i - 1];
					t.idx[i + 1] = t.idx[i];
				}
				t.key[i] = newNode.record[0].first;
				t.idx[i + 1] = newNode.offset;
				++(t.len);

				writeFile(&t, t.offset, sizeof(idxNode));
				return emptyNode;
			}

			//idxNode* newIdx = new idxNode;
			idxNode newIdx;
			newIdx.offset = getFileLen();
			newIdx.type = 1;
			int max = idxSize / 2;
			newIdx.len = max + 1;
			int i, j;

			if (newNode.record[0].first > t.key[idxSize - 2])
			{
				newIdx.key[max - 1] = newNode.record[0].first;
				newIdx.idx[max] = newNode.offset;
			}
			else
			{
				newIdx.key[max - 1] = t.key[idxSize - 2];
				newIdx.idx[max] = t.idx[idxSize - 1];
				int i;
				for (i = t.len - 2; i > 0 && newNode.record[0].first < t.key[i - 1]; --i)
				{
					t.key[i] = t.key[i - 1];
					t.idx[i + 1] = t.idx[i];
				}
				t.key[i] = newNode.record[0].first;
				t.idx[i + 1] = newNode.offset;
			}

			for (i = max - 1, j = idxSize - 1; i > 0; --i, --j)
			{
				newIdx.idx[i] = t.idx[j];
				newIdx.key[i - 1] = t.key[j - 1];
			}
			newIdx.idx[0] = t.idx[j];

			t.len = idxSize - max;

			writeFile(&newIdx, newIdx.offset, sizeof(idxNode));
			writeFile(&t, t.offset, sizeof(idxNode));
			return newIdx;
		}

		// Erase: Erase the Key-Value
		// Return Success if it is successfully erased
		// Return Fail if the key doesn't exist in the database
		OperationResult erase(const Key& key)
		{
			idxNode root;
			getRoot(root);
			if (root.isEmpty) return Fail;
			return remove(key, 0);//need modify

			//return Fail; // If you can't finish erase part, just remaining here.
		}
		OperationResult remove(const Key& key, size_t offset_t)
		{
			idxNode t;
			readFile(&t, offset_t, sizeof(idxNode));

			int i;
			for (i = 0; i < t.len - 1; ++i)
				if (key < t.key[i]) break;

			if (t.type == 0) return remove(key, t.idx[i]);
			else return removeData(key, t.idx[i]);
		}
		OperationResult removeData(const Key& key, size_t offset_t)
		{
			dataNode t;
			readFile(&t, offset_t, sizeof(dataNode));
			for (int i = 0; i < t.len; ++i)
				if (key == t.record[i].first)
				{
					for (int j = i; j < t.len; ++j)
						t.record[j] = t.record[j + 1];
					--(t.len);
					writeFile(&t, t.offset, sizeof(dataNode));
					return Success;
				}
			return Fail;
		}
		// Return a iterator to the beginning
		iterator begin() {}
		const_iterator cbegin() const {}
		// Return a iterator to the end(the next element after the last)
		iterator end() {}
		const_iterator cend() const {}
		// Check whether this BTree is empty
		bool empty() const {}
		// Return the number of <K,V> pairs
		size_t size() const {}
		// Clear the BTree
		void clear() {}
		// Return the value refer to the Key(key)
		Value at(const Key& key)
		{
			idxNode root;
			getRoot(root);
			if (root.isEmpty)
				throw container_is_empty();//throw "do not exist";
			return find(key, 0);//need modify
			//return NULL;
		}
		Value find(const Key& key, size_t offset_t)
		{
			idxNode t;
			readFile(&t, offset_t, sizeof(idxNode));

			int i;
			for (i = 0; i < t.len - 1; ++i)
				if (key < t.key[i]) break;

			if (t.type == 0) return find(key, t.idx[i]);
			else return findData(key, t.idx[i]);
		}
		Value findData(const Key& key, size_t offset_t)
		{
			dataNode t;
			readFile(&t, offset_t, sizeof(dataNode));
			for (int i = 0; i < t.len; ++i)
				if (key == t.record[i].first)
					return t.record[i].second;
			throw container_is_empty();
			//throw "do not exist";
		}
		/**
			 * Returns the number of elements with key
			 *   that compares equivalent to the specified argument,
			 * The default method of check the equivalence is !(a < b || b > a)
			 */
		size_t count(const Key& key) const {}
		/**
			 * Finds an element with key equivalent to key.
			 * key value of the element to search for.
			 * Iterator to an element with key equivalent to key.
			 *   If no such element is found, past-the-end (see end()) iterator is
			 * returned.
			 */
		iterator find(const Key& key) {}
		const_iterator find(const Key& key) const {}
	};
} // namespace sjtu
